#include "CPack.h"
#include "../aplib/aplib.h"
#pragma comment(lib, "../aplib/aplib.lib")
#include<string>
using namespace std;


VOID CPack::ReadPackFile(
  _In_ PCHAR pPackPath)
{
  // 1 ���ļ�
  HANDLE hFile = CreateFileA(
    pPackPath, GENERIC_READ, FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, NULL);
  
  // 2 ��ȡ�ļ���С
  m_dwFileSize = GetFileSize(hFile, NULL);
  m_dwNewFileSize = m_dwFileSize;

  // 3 ������ô��Ŀռ�
  m_pBuf = new CHAR[m_dwFileSize];
  m_pNewBuf = m_pBuf;
  memset(m_pBuf, 0, m_dwFileSize);

  // 4 ���ļ����ݶ�ȡ��������Ŀռ���
  DWORD dwRealSize = 0;
  if (!ReadFile(hFile, m_pBuf, m_dwFileSize, &dwRealSize, NULL))
  {
    MessageBoxA(0, "Ŀ������ȡʧ��", "ERROR", MB_ICONWARNING | MB_OK);
    ExitProcess(-1);
  }
   
  // 5 ��ȡDOSͷָ�� NTͷָ�� OEP��RVA .text�������Ϣ rsrc��Դ����Ϣ tls��Ϣ
  m_pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  m_pNt = (PIMAGE_NT_HEADERS)(m_pDos->e_lfanew + m_pNewBuf);
  // ��ȡ��һ�����ε�λ��
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(m_pNt); 
  // ����ԭʼ������
  m_OriSectionNumber = m_pNt->FileHeader.NumberOfSections;
  // ��ȡOEP
  DWORD dwOEP = m_pNt->OptionalHeader.AddressOfEntryPoint;
  // �����Դ�ε���Ϣ
  m_pResRva = m_pNt->OptionalHeader.DataDirectory[2].VirtualAddress;
  m_pResSectionRva = 0;
  m_ResSectionIndex = -1;
  m_ResPointerToRawData = 0;
  m_ResSizeOfRawData = 0;
  // ��ȡtls����Ϣ
  m_pTlsSectionRva = 0;
  m_TlsSectionIndex = -1;
  m_TlsPointerToRawData = 0;
  m_TlsSizeOfRawData = 0;
  if (m_pNt->OptionalHeader.DataDirectory[9].VirtualAddress)
  {
    // ���tls��ָ��
    PIMAGE_TLS_DIRECTORY32 g_lpTlsDir =
      (PIMAGE_TLS_DIRECTORY32)(RvaToFoa(m_pNt->OptionalHeader.DataDirectory[9].VirtualAddress) + m_pNewBuf);
    // ���tls������ʼrva
    m_pTlsDataRva = g_lpTlsDir->StartAddressOfRawData - m_pNt->OptionalHeader.ImageBase;
  }
  // ѭ����ȡ����� tls�� ��Դ���������Ǹ����Ρ�index��
  for (int i = 0; i < m_pNt->FileHeader.NumberOfSections; i++)
  {
    // ���oep���������,���ж���������Ǵ����
    if (dwOEP >= pSection->VirtualAddress &&
      dwOEP <= pSection->VirtualAddress + pSection->Misc.VirtualSize)
    {
      // ��ȡ����������������[ͨ��oep�ж�]
      m_codeIndex = i;
    }
    if (m_pResRva >= pSection->VirtualAddress &&
      m_pResRva <= pSection->VirtualAddress + pSection->Misc.VirtualSize)
    {
      // ��ȡrsrc�ε���Ϣ��rsrc���������˵ڼ�������
      m_pResSectionRva = pSection->VirtualAddress;
      m_ResPointerToRawData = pSection->PointerToRawData;
      m_ResSizeOfRawData = pSection->SizeOfRawData;
      m_ResSectionIndex = i;
    }
    // ��ȡtls��Ϣ
    if (m_pNt->OptionalHeader.DataDirectory[9].VirtualAddress)
    {
      // tls�����������˵ڼ�������
      if (m_pTlsDataRva >= pSection->VirtualAddress &&
        m_pTlsDataRva <= pSection->VirtualAddress + pSection->Misc.VirtualSize)
      {
        m_pTlsSectionRva = pSection->VirtualAddress;
        m_TlsSectionIndex = i;
        m_TlsPointerToRawData = pSection->PointerToRawData;
        m_TlsSizeOfRawData = pSection->SizeOfRawData;
      }
    }
    // ���û���ҵ�������һ������
    pSection = pSection + 1;
  }

  // 6 �ر��ļ�
  CloseHandle(hFile);
}


DWORD CPack::RvaToFoa(
  _In_ DWORD Rva)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
  {
    // �������ε���ʼλ�� С�����εĽ���λ�� - ���������������м�
    if (Rva >= pSection->VirtualAddress &&
      Rva <= pSection->VirtualAddress + pSection->Misc.VirtualSize)
    {
      // ����ļ���ַΪ0,���޷����ļ����ҵ���Ӧ������
      if (pSection->PointerToRawData == 0)
      {
        return -1;
      }
      return Rva - pSection->VirtualAddress + pSection->PointerToRawData;
    }
    pSection = pSection + 1;
  }
  return -1;
}


BOOL CPack::DealwithTLS(
  _Out_ PPACKINFO& pPackInfo)
  // 0. tls�����rdata��,pe�ļ��е�tls��ᱻѹ��,����ʹ��stub���е�tls��(stub���ں�֮��tls�δ���.text����)
  // 1. �������̴߳���֮ʱ���ȶ�ȡtls�ε����ݵ�һ��ռ�,�ռ��ַ������FS:[0x2C],֮��Ҳ����ʹ�����ռ�
  //    ���Բ�Ҫ��ͼ�ڿǴ������޸�tls��,������ʹ�����ṩ���ڴ�ռ�,Ӧ���ڼӿ�ʱ��Ӧ�ô�����ⷽ�������
  // 2. index������FS:[0x2C]�´��ָ���ҵ�tls��ʹ�õ��ڴ�ռ�ָ��
  // 3. ����ֻ���Լ�ѭ�����ü���
  // ����:
  // 0. ��peĿ¼��9ָ��stub��tls��
  // 1. ��ѹ��tls���ݶ�[tls���ݶε�Ѱ�ҷ�ʽ:ͨ��tls���е�StartAddressOfRawData��������Ѱ��]
  // 2. ��index���빲����Ϣ�ṹ��,�������������rva(��FixRloc֮������Ϊrva-0x1000+allensection_rva+pe_imagebase)
  // 3. stub��tls��ǰ����ͬpe��tls��,��ֵ����Ҫת��(��FixRloc֮������Ϊ��pe��tls������ͬ����)
  // 4. stub��addressOfFuncͬpe��tls��,��ֵ����Ҫת��(��FixRloc֮������Ϊ��pe��tls������ͬ����)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  DWORD dwImageBase = pNt->OptionalHeader.ImageBase;
  // ���ж�tls�Ƿ����
  if (pNt->OptionalHeader.DataDirectory[9].VirtualAddress == 0)
  {
    pPackInfo->bIsTlsUseful = FALSE;
    return FALSE;
  }
  else
  {
    //����Ϊ������tls
    pPackInfo->bIsTlsUseful = TRUE;

    PIMAGE_TLS_DIRECTORY32 g_lpTlsDir =
      (PIMAGE_TLS_DIRECTORY32)(RvaToFoa(pNt->OptionalHeader.DataDirectory[9].VirtualAddress) + m_pNewBuf);
    // ��ȡtlsIndex(������ַ����Offset 
    DWORD indexOffset = RvaToFoa(g_lpTlsDir->AddressOfIndex - dwImageBase);
    // ��ȡ����tlsIndex��ֵ
    pPackInfo->TlsIndex = 0;//indexһ��Ĭ��ֵΪ0
    // �������ļ��е�
    if (indexOffset != -1)
    {
      //ȡ����
      pPackInfo->TlsIndex = *(DWORD*)(indexOffset + m_pNewBuf);
    }
    // ����tls���е���Ϣ 
    m_StartOfDataAddress = g_lpTlsDir->StartAddressOfRawData;
    m_EndOfDataAddress = g_lpTlsDir->EndAddressOfRawData;
    m_CallBackFuncAddress = g_lpTlsDir->AddressOfCallBacks;

    // ��tls�ص�����rva���õ�������Ϣ�ṹ��
    pPackInfo->TlsCallbackFuncRva = m_CallBackFuncAddress;
    return TRUE;
  }

  return FALSE;
}


VOID CPack::EnCode(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  // ��λ�������,����ÿ���μ���
  pSection = pSection + m_codeIndex;
  PCHAR pStart = pSection->PointerToRawData + m_pNewBuf;
  for (size_t i = 0; i < (pSection->Misc.VirtualSize); i++)
  {
    pStart[i] ^= 0x20;
  }
}


PCHAR CPack::Compress(
  _In_  PVOID pSource,
  _In_  long lInLength,
  _In_  long& lOutLenght)
{
  //packed����ѹ�����ݵĿռ䣬workmemΪ���ѹ����Ҫʹ�õĿռ�
  PCHAR packed, workmem;
  if ((packed = (PCHAR)malloc(aP_max_packed_size(lInLength))) == NULL ||
    (workmem = (PCHAR)malloc(aP_workmem_size(lInLength))) == NULL)
  {
    return NULL;
  }
  //����aP_packѹ������
  lOutLenght = aPsafe_pack(pSource, packed, lInLength, workmem, NULL, NULL);

  if (lOutLenght == APLIB_ERROR)
  {
    return NULL;
  }
  if (NULL != workmem)
  {
    free(workmem);
    workmem = NULL;
  }

  return packed;//���ر����ַ
}


DWORD CPack::CalcAlignment(
  _In_ DWORD dwSize,
  _In_ DWORD dwAlignment)
{
  if (dwSize % dwAlignment == 0)
  {
    return dwSize;
  }
  else
  {
    return (dwSize / dwAlignment + 1) * dwAlignment;
  }
}


DWORD CPack::AddSection(
  _In_opt_ PCHAR szName,
  _In_opt_ PCHAR pSectionBuf,
  _In_opt_ DWORD dwSectionSize,
  _In_opt_ DWORD dwAttribute
)
{
  //1 ���ݸղŶ�ȡ��exe�ļ������ݣ��õ���������κ��µ�exe�ļ��Ĵ�С
  m_dwNewFileSize = m_dwFileSize + CalcAlignment(dwSectionSize, 0x200);
  //2 ����ռ�
  m_pNewBuf = nullptr;  //���ÿ�
  m_pNewBuf = new CHAR[m_dwNewFileSize];
  memset(m_pNewBuf, 0, m_dwNewFileSize);
  //3 ��ԭ����PE���ݿ�����������Ŀռ���
  memcpy(m_pNewBuf, m_pBuf, m_dwFileSize);
  //4 �������ο�����PE�ļ��ĺ���
  memcpy(m_pNewBuf + m_dwFileSize, pSectionBuf, dwSectionSize);
  //5 �޸����α�
  m_pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  m_pNt = (PIMAGE_NT_HEADERS)(m_pDos->e_lfanew + m_pNewBuf);
  m_pSection = IMAGE_FIRST_SECTION(m_pNt);
  //�õ����α�����һ��
  PIMAGE_SECTION_HEADER pLastSection =
    m_pSection + m_pNt->FileHeader.NumberOfSections - 1;
  //�õ����α�����һ��ĺ���
  PIMAGE_SECTION_HEADER pNewSection = pLastSection + 1;
  pNewSection->Characteristics = dwAttribute;    //����
  strcpy_s((char*)pNewSection->Name, 8, szName);//������--->�˴�������,����㲻����Ϊ֮����ռ�,������ӽ���ͷʱ���ܻ�Խ��.

                           // �����ڴ�ƫ�ƺ��ڴ��С
  pNewSection->Misc.VirtualSize = dwSectionSize; //�ڴ��еĴ�С������Ҫ���룩
  pNewSection->VirtualAddress = pLastSection->VirtualAddress +
    CalcAlignment(pLastSection->Misc.VirtualSize, 0x1000);
  pNewSection->SizeOfRawData = CalcAlignment(dwSectionSize, 0x200);

  // �����ļ�ƫ�ƺ��ļ���С
  while (TRUE)
  {
    if (pLastSection->PointerToRawData)
    {
      // �ҵ�ǰһ����0������
      pNewSection->PointerToRawData = pLastSection->PointerToRawData +
        pLastSection->SizeOfRawData;
      break;
    }
    pLastSection = pLastSection - 1;
  }

  //6 �޸����������;����С
  m_pNt->FileHeader.NumberOfSections++;
  m_pNt->OptionalHeader.SizeOfImage = pNewSection->VirtualAddress + dwSectionSize;



  // ����һ�ݵ�ǰ�Ĵ�С
  m_dwFileSize = m_dwNewFileSize;
  // �ͷ�֮ǰ���ڴ�,�����浱ǰ�ĵ�����

  delete[] m_pBuf;
  m_pBuf = m_pNewBuf;

  // ������������ε�rva
  return pNewSection->VirtualAddress;
}


DWORD CPack::GetOepRva(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pBuf);
  return pNt->OptionalHeader.AddressOfEntryPoint;
}


DWORD CPack::GetImageBase(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pBuf);
  return pNt->OptionalHeader.ImageBase;
}


DWORD CPack::GetImportTableRva(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pBuf);
  return pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
}


DWORD CPack::GetRelocRva(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pBuf);
  return pNt->OptionalHeader.DataDirectory[0].VirtualAddress;
}


VOID CPack::FixDllRloc(
  _In_ PCHAR pBuf,
  _In_ PCHAR pOri)
{
  // �����ض�λ��Ϣ�ṹ��
  typedef struct _TYPE
  {
    unsigned short offset : 12;
    unsigned short type : 4;
  }TYPE, * PTYPE;

  //��λ����һ���ض�λ��
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pBuf);
  PIMAGE_DATA_DIRECTORY pRelocDir = (pNt->OptionalHeader.DataDirectory + 5);
  PIMAGE_BASE_RELOCATION pReloc =
    (PIMAGE_BASE_RELOCATION)(pRelocDir->VirtualAddress + pBuf);

  // ��ʼ�޸��ض�λ
  while (pReloc->SizeOfBlock != 0)
  {
    // �ض�λ�ʼ����
    DWORD BeginLoc = (DWORD)(pReloc->VirtualAddress + pBuf);
    // �ض�λ��ĸ���
    DWORD dwCount = (pReloc->SizeOfBlock - 8) / 2;
    // �ض�λ����
    PTYPE pType = (PTYPE)(pReloc + 1);
    // �޸�ÿһ���ض�λ��
    for (size_t i = 0; i < dwCount; i++)
    {
      // ���������3
      if (pType->type == 3)
      {
        // ��ȡ�ض�λ��ַ
        PDWORD pReloction = (PDWORD)(pReloc->VirtualAddress + pType->offset + pBuf);
        // ��ȡ���ض�λ��ַ���ض�λ�������ͷ��ƫ��
        DWORD Chazhi = *pReloction - (DWORD)pOri - 0x1000;
        // ��ƫ�Ƽ����½�����rva��ø��ض�λ���rva,�ڼ��ϵ�ǰĬ�ϼ��ػ�ַ�����޸��ض�λ
        *pReloction = Chazhi + GetNewSectionRva() + GetImageBase();
      }
      //��λ����һ���ض�λ��
      pType++;
    }
    // ��λ����һ���ض�λ��
    pReloc = (PIMAGE_BASE_RELOCATION)((PCHAR)pReloc + pReloc->SizeOfBlock);
  }
}


DWORD CPack::GetNewSectionRva(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  PIMAGE_SECTION_HEADER pLastSection = pSection + pNt->FileHeader.NumberOfSections - 1;

  return pLastSection->VirtualAddress +
    CalcAlignment(pLastSection->Misc.VirtualSize, 0x1000);
}


DWORD CPack::GetLastSectionRva(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  PIMAGE_SECTION_HEADER pLastSection = pSection + pNt->FileHeader.NumberOfSections - 1;

  return (DWORD)pLastSection;
}


VOID CPack::ChangeReloc(
  _In_ PCHAR pBuf)
{
  // ��λ����һ���ض�λ��
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  PIMAGE_DATA_DIRECTORY pRelocDir = (pNt->OptionalHeader.DataDirectory + 5);
  PIMAGE_BASE_RELOCATION pReloc =
    (PIMAGE_BASE_RELOCATION)(pRelocDir->VirtualAddress + pBuf);

  // ��ʼ�����ض�λ
  while (pReloc->SizeOfBlock != 0)
  {

    // �ض�λ�ʼ����,���䶨λ���ڴ�֮ǰ���allen��(Ҫ��ԭ����RVA ��Ϊ�Ǵ����е�RVA)
    //ԭ��:��λ���ÿһ���� pReloc->VirtualAddress Ϊÿһҳ����ʼ��ַ(Ҳ����0x1000 0x2000...)������ҳ�ֿ�
    //ԭ���Ĵ����������ڼ��ػ�ַ������0x400000)�ĵ�2ҳҲ����0x1000(�ڶ�ҳ����ʼ��ַ����ʾPE�е�ͷ������Ŀ¼��ȵ�
    // ������ض�λ��dll ��Ҳ������û��ͷ�� Ҫ��ȥ0x1000�����еģ�  ͬʱ���˽ڵ�RVA,Ҳ�������һ�ڿǴ���Ŀ�ʼ��RVA)  �൱�ڴ���δ����￪ʼ
    //����Ҫ�ҵ���Ҫ�ض�λ���Ǹ���ַ��va  va=���ػ�ַ +ҳ��ַ(pReloc->VirtualAddress)+ҳ�ڵ�ַ���ض�λ���е�һ�����鱣����ǣ�
    pReloc->VirtualAddress = (DWORD)(pReloc->VirtualAddress - 0x1000 + GetLastSectionRva());
    // ��λ����һ���ض�λ��
    pReloc = (PIMAGE_BASE_RELOCATION)((PCHAR)pReloc + pReloc->SizeOfBlock);
  }
  DWORD dwRelocRva = 0;
  DWORD dwRelocSize = 0;
  DWORD dwSectionAttribute = 0;
  // ��ȡ�ض�λ������
  while (TRUE)
  {
    if (!strcmp((char*)pSection->Name, ".reloc"))
    {
      dwRelocRva = pSection->VirtualAddress;
      dwRelocSize = pSection->SizeOfRawData;
      dwSectionAttribute = pSection->Characteristics;
      break;
    }
    pSection = pSection + 1;
  }

  // ��stubdll��.reloc��ӵ�PE�ļ������,����Ϊ.nreloc,���ظ����ε�Rva
  DWORD RelocRva = AddSection((PCHAR)".nreloc", dwRelocRva + pBuf, dwRelocSize, dwSectionAttribute);

  // ���ض�λ��Ϣָ������ӵ�����
  PIMAGE_DOS_HEADER pExeDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pExeNt = (PIMAGE_NT_HEADERS)(pExeDos->e_lfanew + m_pNewBuf);
  pExeNt->OptionalHeader.DataDirectory[5].VirtualAddress = RelocRva;
  pExeNt->OptionalHeader.DataDirectory[5].Size = dwRelocSize;
}


VOID CPack::CompressPE(
  _In_ PPACKINFO& pPackInfo)
{

  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);

  // ���ڼ�¼ѹ�����εĸ���
  pPackInfo->PackSectionNumber = 0;

  // 1.1 ��ȡ�ļ�ͷ�Ĵ�С,����ȡ����Դ��,tls֮��ε��ļ��е��ܴ�С
  DWORD SecSizeWithOutResAndTls = 0;
  PIMAGE_SECTION_HEADER pSectionTmp1 = pSection;
  BOOL isFirstNoEmptySec = TRUE;
  DWORD dwHeaderSize = 0;
  //�ȼ���Ҫ�������ܴ�С 
  for (SIZE_T i = 0; i < pNt->FileHeader.NumberOfSections; i++)
  {
    // ���ڻ�õ�һ���ǿ����ε��ļ�ƫ��,Ҳ�����ļ�ͷ��С
    if (isFirstNoEmptySec && pSectionTmp1->SizeOfRawData != 0)
    {
      dwHeaderSize = pSectionTmp1->PointerToRawData;
      isFirstNoEmptySec = FALSE;
    }
    // ���ڻ�ȡ �� rsrc/tls�ε��ܴ�С
    if (pSectionTmp1->VirtualAddress != m_pResSectionRva &&
      pSectionTmp1->VirtualAddress != m_pTlsSectionRva)
    {
      SecSizeWithOutResAndTls += pSectionTmp1->SizeOfRawData;
    }
    pSectionTmp1 = pSectionTmp1 + 1;
  }
  // 1.2 ��ȡҪѹ���Ķε��ڴ�
  // �����ڴ�
  PCHAR memWorked = new CHAR[SecSizeWithOutResAndTls]{ 0 };
  // �Ѿ��������ڴ��С
  DWORD dwCopySize = 0;
  // ������Щ���ε��ڴ�
  PIMAGE_SECTION_HEADER pSectionTmp2 = pSection;
  //�ٴ�ѭ��
  for (SIZE_T i = 0; i < pNt->FileHeader.NumberOfSections; i++)
  {
    //�ж��Ƿ�Ϊtls����Դ��
    if (pSectionTmp2->VirtualAddress != m_pResSectionRva &&
      pSectionTmp2->VirtualAddress != m_pTlsSectionRva)
    {
      //��ʼ����
      memcpy_s(memWorked + dwCopySize, pSectionTmp2->SizeOfRawData,
        m_pNewBuf + pSectionTmp2->PointerToRawData, pSectionTmp2->SizeOfRawData);
      dwCopySize += pSectionTmp2->SizeOfRawData;
    }
    pSectionTmp2 = pSectionTmp2 + 1;
  }
  // 1.3 ѹ��,����ȡѹ����Ĵ�С(�ļ���������������н���)
  LONG blen;
  PCHAR packBuf = Compress(memWorked, SecSizeWithOutResAndTls, blen);

  // 1.4 ������Դ�� ���ڴ�ռ�
  PCHAR resBuffer = new CHAR[m_ResSizeOfRawData];
  PCHAR tlsBuffer = new CHAR[m_TlsSizeOfRawData];
  //ԭ��
  memcpy_s(resBuffer, m_ResSizeOfRawData, m_ResPointerToRawData + m_pNewBuf, m_ResSizeOfRawData);
  memcpy_s(tlsBuffer, m_TlsSizeOfRawData, m_TlsPointerToRawData + m_pNewBuf, m_TlsSizeOfRawData);

  // 1.6 ����ѹ����Ϣ����Ϣ�ṹ����
  //     ���ҽ�m_pBuf�еķ���Դ�κͷ�tls�ε������ļ�ƫ�ƺʹ�С��Ϊ0
  PIMAGE_DOS_HEADER pOriDos = (PIMAGE_DOS_HEADER)m_pBuf;
  PIMAGE_NT_HEADERS pOriNt = (PIMAGE_NT_HEADERS)(pOriDos->e_lfanew + m_pBuf);
  PIMAGE_SECTION_HEADER pOriSection = IMAGE_FIRST_SECTION(pOriNt);

  for (int i = 0; i < pOriNt->FileHeader.NumberOfSections; i++)
  {
    if (pOriSection->VirtualAddress != m_pResSectionRva &&
      pOriSection->VirtualAddress != m_pTlsSectionRva)
    {
      // ���ڻ�ȡѹ������������
      pPackInfo->PackSectionNumber++;
      // ���õ�i���ڵ�ѹ������index
      pPackInfo->PackInfomation[pPackInfo->PackSectionNumber][0] = i;
      // ����ѹ���������ļ���С
      pPackInfo->PackInfomation[pPackInfo->PackSectionNumber][1] = pOriSection->SizeOfRawData;

      // ����ԭ���Ľ������ļ���ƫ�ƺʹ�СΪ0
      pOriSection->SizeOfRawData = 0;
      pOriSection->PointerToRawData = 0;
    }

    pOriSection = pOriSection + 1;
  }


  // 1.6 �����¿ռ�,ʹm_pNewBufָ��֮,��m_pBuf�ļ�ͷ����
  m_dwFileSize = dwHeaderSize + m_ResSizeOfRawData + m_TlsSizeOfRawData;
  //��ʼ����
  m_pNewBuf = nullptr;
  m_pNewBuf = new CHAR[m_dwFileSize];

  // 1.6 �޸�res�ε�����ͷ,������
  pOriSection = IMAGE_FIRST_SECTION(pOriNt);
  //��ʼ�޸���Դ��tls�ε�ͷ  ���ж�������λ��
  if (m_ResSectionIndex < m_TlsSectionIndex)
  {
    pOriSection[m_ResSectionIndex].PointerToRawData = dwHeaderSize;
    pOriSection[m_TlsSectionIndex].PointerToRawData = dwHeaderSize + m_ResSizeOfRawData;
    memcpy_s(m_pNewBuf, dwHeaderSize, m_pBuf, dwHeaderSize);
    memcpy_s(m_pNewBuf + dwHeaderSize, m_ResSizeOfRawData, resBuffer, m_ResSizeOfRawData);
    memcpy_s(m_pNewBuf + dwHeaderSize + m_ResSizeOfRawData
      , m_TlsSizeOfRawData, tlsBuffer, m_TlsSizeOfRawData);
  }
  else if (m_ResSectionIndex > m_TlsSectionIndex)
  {
    pOriSection[m_TlsSectionIndex].PointerToRawData = dwHeaderSize;
    pOriSection[m_ResSectionIndex].PointerToRawData = dwHeaderSize + m_TlsSizeOfRawData;
    memcpy_s(m_pNewBuf, dwHeaderSize, m_pBuf, dwHeaderSize);
    memcpy_s(m_pNewBuf + dwHeaderSize, m_TlsSizeOfRawData, tlsBuffer, m_TlsSizeOfRawData);
    memcpy_s(m_pNewBuf + dwHeaderSize + m_TlsSizeOfRawData
      , m_ResSizeOfRawData, resBuffer, m_ResSizeOfRawData);
  }
  else
  {
    memcpy_s(m_pNewBuf, dwHeaderSize, m_pBuf, dwHeaderSize);
  }
  /*else if(m_ResSectionIndex == -1 && m_TlsSectionIndex == -1)
  {
  }*/

  delete[] m_pBuf;
  m_pBuf = m_pNewBuf;

  // ������Σ�ѹ������������ݣ�
  pPackInfo->packSectionRva = AddSection((PCHAR)".pack", packBuf, blen, 0xC0000040);
  pPackInfo->packSectionSize = CalcAlignment(blen, 0x200);
  // 1.7 ���.pack��
  delete[] memWorked;
  free(packBuf);
  delete[] resBuffer;
}


VOID CPack::SetTls(
  _In_ DWORD NewSectionRva,
  _In_ PCHAR pStubBuf,
  _In_ PPACKINFO pPackInfo)
{
  PIMAGE_DOS_HEADER pStubDos = (PIMAGE_DOS_HEADER)pStubBuf;
  PIMAGE_NT_HEADERS pStubNt = (PIMAGE_NT_HEADERS)(pStubDos->e_lfanew + pStubBuf);

  PIMAGE_DOS_HEADER pPeDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pPeNt = (PIMAGE_NT_HEADERS)(pPeDos->e_lfanew + m_pNewBuf);

  //0 ��peĿ¼��9ָ��stub��tls��
  pPeNt->OptionalHeader.DataDirectory[9].VirtualAddress =
    (pStubNt->OptionalHeader.DataDirectory[9].VirtualAddress - 0x1000) + NewSectionRva;
  pPeNt->OptionalHeader.DataDirectory[9].Size =
    pStubNt->OptionalHeader.DataDirectory[9].Size;

  PIMAGE_TLS_DIRECTORY32  pITD =
    (PIMAGE_TLS_DIRECTORY32)(RvaToFoa(pPeNt->OptionalHeader.DataDirectory[9].VirtualAddress) + m_pNewBuf);
  // ��ȡ�����ṹ����tlsIndex��va
  //������Ϣ�ṹ����׵�ַ-stubdll���ڴ��ָ��+4(�ڽṹ�����ƫ�ƣ�-stubͷ��+NewSectionRva+���ػ�ַ   
  DWORD indexva = ((DWORD)pPackInfo - (DWORD)pStubBuf + 4) - 0x1000 + NewSectionRva + pPeNt->OptionalHeader.ImageBase;
  pITD->AddressOfIndex = indexva;
  pITD->StartAddressOfRawData = m_StartOfDataAddress;
  pITD->EndAddressOfRawData = m_EndOfDataAddress;

  // ������ȡ��tls�Ļص�����,������Ϣ�ṹ���д���tls�ص�����ָ��,��stub��ǵĹ������ֶ�����tls,����tls�ص�����ָ�����û�ȥ
  pITD->AddressOfCallBacks = 0;

  m_pBuf = m_pNewBuf;
}


VOID CPack::SetNewOep(
  _In_ DWORD dwNewOep)
{
  m_pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  m_pNt = (PIMAGE_NT_HEADERS)(m_pDos->e_lfanew + m_pNewBuf);
  m_pNt->OptionalHeader.AddressOfEntryPoint = dwNewOep;
}


VOID CPack::SetMemWritable(VOID)
{
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  DWORD SectionNumber = pNt->FileHeader.NumberOfSections;

  for (size_t i = 0; i < SectionNumber; i++)
  {
    pSection[i].Characteristics |= 0x80000000;
  }
}


VOID CPack::ChangeImportTable(VOID)
{
  // ��Ŀ¼��ĵ��������0
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pNewBuf;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pNewBuf);
  pNt->OptionalHeader.DataDirectory[1].VirtualAddress = 0;
  pNt->OptionalHeader.DataDirectory[1].Size = 0;

  pNt->OptionalHeader.DataDirectory[12].VirtualAddress = 0;
  pNt->OptionalHeader.DataDirectory[12].Size = 0;
}


VOID CPack::SavePackFile(
  _In_ PCHAR pNewFileName)
{
  // 1 ���ļ�
  DWORD dwRealSize = 0;
  HANDLE hFile = CreateFileA(
    pNewFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
    NULL,
    OPEN_ALWAYS,
    FILE_ATTRIBUTE_NORMAL, NULL
  );
  // 2 ���ڴ��е�����д�뵽�ļ���
  WriteFile(hFile,
    m_pNewBuf, m_dwNewFileSize, &dwRealSize, NULL);
  // 3 �ر��ļ������
  CloseHandle(hFile);
}


BOOL CPack::Pack(
  _In_opt_ BOOL  bIsCompression,
  _In_opt_ BOOL  bIsNormalEncryption,
  _In_opt_ BOOL  bIsRegisteredProtection,
  _In_opt_ BOOL  bIsDynamicEncryption,
  _In_opt_ BOOL  bIsVerificationProtection,
  _In_opt_ BOOL  bIsAntiDebugging,
  _In_opt_ BOOL  bIsApiRedirect,
  _In_opt_ BOOL  bIsAntiDump,
  _In_     PCHAR pPath)
{
  BOOL bRet = FALSE;
  
  PCHAR lpPath = (PCHAR)"C:\\Users\\nSky\\Desktop\\nSkyPack32\\Release\\nSkyStub.dll";
  // 1 ��stub.dll���뵽�ڴ�
  // HMODULE hStub = LoadLibraryA(lpPath);
  HMODULE hStub = LoadLibraryExA((LPCSTR)lpPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
  if (!hStub)
  {
    MessageBoxA(0, "Stubģ�����ʧ��", "ERROR", MB_ICONWARNING | MB_OK);
    ExitProcess(-1);
  }

  // 2 ���ڴ����ҵ���stub.dllͨѶ�� g_PackInfo
  PPACKINFO pPackInfo = (PPACKINFO)GetProcAddress(hStub, "g_PackInfo");
  
  // 3 ��ȡĿ���ļ���Ҫ�ӿǵĳ���
  ReadPackFile(pPath);

  // ��ȡtls��Ϣ
  BOOL bTlsUseful = DealwithTLS(pPackInfo);

  // �Դ���ν��м���
  if (bIsNormalEncryption)
  {
    EnCode();
  }

  // �Ը����ν���ѹ��
  if (bIsCompression)
  {
    CompressPE(pPackInfo);
  }

  // 4 ��ȡstub.dll���ڴ��С�ͽ���ͷ(Ҳ����Ҫ������ͷ��)
  PIMAGE_DOS_HEADER pStubDos = (PIMAGE_DOS_HEADER)hStub;
  PIMAGE_NT_HEADERS pStubNt = (PIMAGE_NT_HEADERS)(pStubDos->e_lfanew + (PCHAR)hStub);
  DWORD dwImageSize = pStubNt->OptionalHeader.SizeOfImage;
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pStubNt);
  // �ҵ���֮�����ú���ת��OEP
  pPackInfo->TargetOepRva = GetOepRva();
  // ����Iamgebase
  pPackInfo->ImageBase = GetImageBase();
  
  // ����Packѡ��
  pPackInfo->bIsCompression = bIsCompression;                      // ѹ��
  pPackInfo->bIsNormalEncryption = bIsNormalEncryption;            // ����
  pPackInfo->bIsRegisteredProtection = bIsRegisteredProtection;    // ע�ᱣ��
  pPackInfo->bIsDynamicEncryption = bIsDynamicEncryption;          // ��̬�ӽ���
  pPackInfo->bIsVerificationProtection = bIsVerificationProtection;// У��ͱ���
  pPackInfo->bIsAntiDebugging = bIsAntiDebugging;                  // ������
  pPackInfo->bIsApiRedirect = bIsApiRedirect;                      // api�ض���
  pPackInfo->bIsAntiDump = bIsAntiDump;                            // ��dump

  // ���ú��ض�λ��rva�͵�����rva
  pPackInfo->ImportTableRva = GetImportTableRva();
  pPackInfo->RelocRva = GetRelocRva();

  // ���Start������Rva
  DWORD dwStartRva = (DWORD)pPackInfo->StartAddress - (DWORD)hStub;
  // ---���޸�������ͨѶ�ṹ�������֮���ٶ�dll�����ڴ濽��---
  // 6 ����ֱ���ڱ��������޸Ļ�Ӱ�����,���Խ�dll����һ�ݵ�pStubBuf
  PCHAR pStubBuf = new CHAR[dwImageSize];
  memcpy_s(pStubBuf, dwImageSize, (PCHAR)hStub, dwImageSize);

  // 7 �޸�dll�ļ��ض�λ,����ڶ�������Ӧ�ô���hStub,��Ϊ����dll����ʱ�ض�λ������
  FixDllRloc(pStubBuf, (PCHAR)hStub);
  
  //8 ��stub���ֵĴ�������ΪĿ������������

  DWORD NewSectionRva = AddSection(
    (PCHAR)".stub",
    pSection->VirtualAddress + pStubBuf,
    pSection->SizeOfRawData,
    pSection->Characteristics
  );
  SetTls(NewSectionRva, (PCHAR)hStub, pPackInfo);

  //=================�ض�λ���====================
  // ���߽�stub���ض�λ����ճ�������,���ض�λ��ָ��֮,������֮ǰҲ����FixDllRloc,ʹ����Ӧ�µ�PE�ļ�
  ChangeReloc(pStubBuf);

  // 9 ��Ŀ������OEP����Ϊstub�е�start����

  DWORD dwChazhi = (dwStartRva - pSection->VirtualAddress);
  DWORD dwNewOep = (dwChazhi + NewSectionRva);
  SetNewOep(dwNewOep);

  // ����ÿ�����ο�д
  SetMemWritable();
  // �ж�IAT����
  ChangeImportTable();

  FreeLibrary(hStub);
  // 10 ������ļ�
  string savePath = pPath;
  savePath = savePath + "_pack.exe";
  SavePackFile((PCHAR)savePath.c_str());
  return 0;
}
#include "pch.h"
#include "CPE.h"
// ���ļ����ڴ���
BOOL CPE::ReadFileToMem(CString strFile)
{
  if (m_pImage != 0)
    delete[] m_pImage;
  // 0x1�����ļ�
  HANDLE hFile = 0;
  hFile = CreateFileW(strFile, GENERIC_READ, NULL, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ::MessageBoxW(0, L"���ļ�ʧ��\r\n�ļ����ܱ�ռ��", L"Error", MB_ICONERROR);
    return 0;
  }

  // 0x2����ȡ�ļ���С
  DWORD dwFileSize = GetFileSize(hFile, NULL);

  // 0x3�����ڴ�������ռ�洢
  m_pImage = new char[dwFileSize] {0};
  if (0 == m_pImage)
  {
    ::MessageBoxW(0, L"�ڴ�����ʧ��", L"Error", MB_ICONERROR);
    return 0;
  }

  // 0x4����ȡ�ļ�
  DWORD dwReadSize = 0;
  BOOL bRet = ReadFile(hFile, m_pImage, dwFileSize, &dwReadSize, 0);
  // �Ƿ��ȡ�ɹ�
  if (!bRet)
    return 0;
  // ��ȡ��С�ȶ�
  if (dwReadSize != dwFileSize)
    return 0;

  // �ͷ���Դ
  if (hFile)
    CloseHandle(hFile);
  return 1;
}

// ����Ƿ���PE�ļ�
BOOL CPE::CheckPE()
{
  // ���� DOSͷ
  m_pDOS = (PIMAGE_DOS_HEADER)m_pImage;
  // ���� NTͷ
  m_pNT32 = (PIMAGE_NT_HEADERS32)(m_pImage + m_pDOS->e_lfanew);

  // ����Ƿ�λPE
  if (m_pDOS->e_magic != 'ZM' || m_pNT32->Signature != 'EP')
  {
    return 0;
  }
  return 1;
}

// �����64����32 /ROM
INT CPE::Check64or32()
{
  INT ret = -1;
  switch (m_pNT32->OptionalHeader.Magic)
  {
  case 0x020B://64
    ret = 1;
    break;
  case 0x010B://32
    ret = 0;
    break;
  case 0x0107://ROM
    ret = 2;
    break;
  default:
    break;
  }
  return ret;
}

// ����64λPE
void CPE::AnalysisPE64()
{
  m_is32 = 0;
  // DOS
  m_pDOS = (PIMAGE_DOS_HEADER)m_pImage;
  
  // NT64
  m_pNT64 = (PIMAGE_NT_HEADERS64)(m_pDOS->e_lfanew + m_pImage);

  // FileHeader
  m_pFileHeader = &m_pNT64->FileHeader;

  // OpHeader
  m_pOp64 = &m_pNT64->OptionalHeader;
}

// ����32λPE
void CPE::AnalysisPE32()
{
  m_is32 = 1;
  // DOS
  m_pDOS = (PIMAGE_DOS_HEADER)m_pImage;

  // NT32
  m_pNT32 = (PIMAGE_NT_HEADERS32)(m_pDOS->e_lfanew + m_pImage);

  // FileHeader
  m_pFileHeader = &m_pNT32->FileHeader;

  // OpHeader
  m_pOp32 = &m_pNT32->OptionalHeader;
}

// ����Ŀ¼
void CPE::AnalysisDataDir()
{
  if (m_is32)
  {
    m_pDataDir = m_pOp32->DataDirectory;
  }
  else
  {
    m_pDataDir = m_pOp64->DataDirectory;
  }
}

// ����ͷ��
void CPE::AnalysisSection()
{
  if (m_is32)
  {
    m_pSectionHeader = (PIMAGE_SECTION_HEADER)(m_pOp32 + 1);
  }
  else
  {
    m_pSectionHeader = (PIMAGE_SECTION_HEADER)(m_pOp64 + 1);
  }
}

// ����������
DWORD CPE::AnalysisExport()
{
  if (m_is32)
  {
    DWORD dwExportRVA = m_pOp32->DataDirectory[0].VirtualAddress;
    if (dwExportRVA == 0)
      return 0;
    DWORD dwExportFOA = RvaToFoa(dwExportRVA);
    m_pExportDir = (PIMAGE_EXPORT_DIRECTORY)(m_pImage + dwExportFOA);
  }
  else
  {
    DWORD dwExportRVA = m_pOp64->DataDirectory[0].VirtualAddress;
    if (dwExportRVA == 0)
      return 0;
    DWORD dwExportFOA = RvaToFoa(dwExportRVA);
    m_pExportDir = (PIMAGE_EXPORT_DIRECTORY)(m_pImage + dwExportFOA);
  }
  return 1;
}

// ���������
DWORD CPE::AnalysisImport()
{
  if (m_is32)
  {
    DWORD dwImportRVA = m_pOp32->DataDirectory[1].VirtualAddress;
    if (dwImportRVA == 0)
      return 0;
    DWORD dwImportFOA = RvaToFoa(dwImportRVA);
    m_pImportDes = (PIMAGE_IMPORT_DESCRIPTOR)(m_pImage + dwImportFOA);
  }
  else
  {
    DWORD dwImportRVA = m_pOp64->DataDirectory[1].VirtualAddress;
    if (dwImportRVA == 0)
      return 0;
    DWORD dwImportFOA = RvaToFoa(dwImportRVA);
    m_pImportDes = (PIMAGE_IMPORT_DESCRIPTOR)(m_pImage + dwImportFOA);
  }
  return 1;
}

// �����ض�λ
DWORD CPE::AnalysisRelocation()
{
  if (m_is32)
  {
    DWORD dwRelocationRVA = m_pOp32->DataDirectory[5].VirtualAddress;
    if (dwRelocationRVA == 0)
      return 0;
    DWORD dwRelocationFOA = RvaToFoa(dwRelocationRVA);
    m_pBaseRelocation = (PIMAGE_BASE_RELOCATION)(m_pImage + dwRelocationFOA);
  }
  else
  {
    DWORD dwRelocationRVA = m_pOp64->DataDirectory[5].VirtualAddress;
    if (dwRelocationRVA == 0)
      return 0;
    DWORD dwRelocationFOA = RvaToFoa(dwRelocationRVA);
    m_pBaseRelocation = (PIMAGE_BASE_RELOCATION)(m_pImage + dwRelocationFOA);
  }
  return 1;
}

// ������Դ
DWORD CPE::AnalysisRes()
{
  if (m_is32)
  {
    DWORD dwResRVA = m_pOp32->DataDirectory[2].VirtualAddress;
    if (dwResRVA == 0)
      return 0;
    DWORD dwResFOA = RvaToFoa(dwResRVA);
    m_ResDir = (PIMAGE_RESOURCE_DIRECTORY)(m_pImage + dwResFOA);
  }
  else
  {
    DWORD dwResRVA = m_pOp64->DataDirectory[2].VirtualAddress;
    if (dwResRVA == 0)
      return 0;
    DWORD dwResFOA = RvaToFoa(dwResRVA);
    m_ResDir = (PIMAGE_RESOURCE_DIRECTORY)(m_pImage + dwResFOA);
  }
  return 1;
}

// ����TLS
DWORD CPE::AnalysisTLS()
{
  if (m_is32)
  {
    DWORD dwTLSRVA = m_pOp32->DataDirectory[9].VirtualAddress;
    if (dwTLSRVA == 0)
      return 0;
    DWORD dwTLSFOA = RvaToFoa(dwTLSRVA);
    m_pTLS32 = (PIMAGE_TLS_DIRECTORY32)(m_pImage + dwTLSFOA);
  }
  else
  {
    DWORD dwTLSRVA = m_pOp64->DataDirectory[9].VirtualAddress;
    if (dwTLSRVA == 0)
      return 0;
    DWORD dwTLSFOA = RvaToFoa(dwTLSRVA);
    m_pTLS64 = (PIMAGE_TLS_DIRECTORY64)(m_pImage + dwTLSFOA);
  }
  return 1;
}

// �����ӳټ��ر�
DWORD CPE::AnalysisDelayImport()
{
  DWORD dwDelayLoadRva = m_pOp32->DataDirectory[13].VirtualAddress;
  if (dwDelayLoadRva == 0)
    return 0;
  DWORD dwDelayLoadFOA = RvaToFoa(dwDelayLoadRva);
  m_pDelayLoad = (PIMAGE_DELAYLOAD_DESCRIPTOR)(m_pImage + dwDelayLoadFOA);

  return 1;
}

// FOAתRVA
DWORD CPE::FoaToRva(DWORD dwFoa)
{
  DWORD dwRva = 0;

  if (dwFoa < m_pOp32->SizeOfHeaders)
  {
    return dwFoa;
  }

  PIMAGE_SECTION_HEADER pSection = m_pSectionHeader;
  for (size_t i = 0; i < m_pFileHeader->NumberOfSections; i++)
  {
    DWORD dwFoaSecionStart = pSection->PointerToRawData;
    DWORD dwFoaSecionEnd   = dwFoaSecionStart + pSection->SizeOfRawData;
    DWORD dwRvaSecion      = pSection->VirtualAddress;
    if (dwFoa >= dwFoaSecionStart && dwFoa < dwFoaSecionEnd)
    {
      dwRva = dwFoa - dwFoaSecionStart + dwRvaSecion;
      break;
    }
    pSection++;
  }
  return dwRva;
}

// RVAתFOA
DWORD CPE::RvaToFoa(DWORD dwRva)
{
  DWORD dwFoa = 0;

  if (dwRva < m_pOp32->SizeOfHeaders)
  {
    return dwRva;
  }

  PIMAGE_SECTION_HEADER pSection = m_pSectionHeader;
  for (size_t i = 0; i < m_pFileHeader->NumberOfSections; i++)
  {
    DWORD dwRvaSecionStart = pSection->VirtualAddress;
    DWORD dwRvaSecionEnd = dwRvaSecionStart + pSection->SizeOfRawData;
    DWORD dwFoaSecion = pSection->PointerToRawData;
    if (dwRva >= dwRvaSecionStart && dwRva < dwRvaSecionEnd)
    {
      dwFoa = dwRva - dwRvaSecionStart + dwFoaSecion;
      break;
    }
    pSection++;
  }
  return dwFoa;
}
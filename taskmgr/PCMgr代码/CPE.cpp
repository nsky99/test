#include "pch.h"
#include "CPE.h"
// 读文件到内存中
BOOL CPE::ReadFileToMem(CString strFile)
{
  if (m_pImage != 0)
    delete[] m_pImage;
  // 0x1、打开文件
  HANDLE hFile = 0;
  hFile = CreateFileW(strFile, GENERIC_READ, NULL, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ::MessageBoxW(0, L"打开文件失败\r\n文件可能被占用", L"Error", MB_ICONERROR);
    return 0;
  }

  // 0x2、获取文件大小
  DWORD dwFileSize = GetFileSize(hFile, NULL);

  // 0x3、在内存中申请空间存储
  m_pImage = new char[dwFileSize] {0};
  if (0 == m_pImage)
  {
    ::MessageBoxW(0, L"内存申请失败", L"Error", MB_ICONERROR);
    return 0;
  }

  // 0x4、读取文件
  DWORD dwReadSize = 0;
  BOOL bRet = ReadFile(hFile, m_pImage, dwFileSize, &dwReadSize, 0);
  // 是否读取成功
  if (!bRet)
    return 0;
  // 读取大小比对
  if (dwReadSize != dwFileSize)
    return 0;

  // 释放资源
  if (hFile)
    CloseHandle(hFile);
  return 1;
}

// 检查是否是PE文件
BOOL CPE::CheckPE()
{
  // 解析 DOS头
  m_pDOS = (PIMAGE_DOS_HEADER)m_pImage;
  // 解析 NT头
  m_pNT32 = (PIMAGE_NT_HEADERS32)(m_pImage + m_pDOS->e_lfanew);

  // 检查是否位PE
  if (m_pDOS->e_magic != 'ZM' || m_pNT32->Signature != 'EP')
  {
    return 0;
  }
  return 1;
}

// 检查是64还是32 /ROM
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

// 解析64位PE
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

// 解析32位PE
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

// 数据目录
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

// 区段头表
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

// 解析导出表
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

// 解析导入表
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

// 解析重定位
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

// 解析资源
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

// 解析TLS
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

// 解析延迟加载表
DWORD CPE::AnalysisDelayImport()
{
  DWORD dwDelayLoadRva = m_pOp32->DataDirectory[13].VirtualAddress;
  if (dwDelayLoadRva == 0)
    return 0;
  DWORD dwDelayLoadFOA = RvaToFoa(dwDelayLoadRva);
  m_pDelayLoad = (PIMAGE_DELAYLOAD_DESCRIPTOR)(m_pImage + dwDelayLoadFOA);

  return 1;
}

// FOA转RVA
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

// RVA转FOA
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
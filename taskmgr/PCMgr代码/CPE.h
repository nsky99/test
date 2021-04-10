#pragma once
class CPE
{
public:
  BOOL  m_is32 = 1;
  char* m_pImage = 0;
  PIMAGE_DOS_HEADER m_pDOS;

  PIMAGE_NT_HEADERS32 m_pNT32;
  PIMAGE_NT_HEADERS64 m_pNT64;

  PIMAGE_FILE_HEADER  m_pFileHeader;

  PIMAGE_OPTIONAL_HEADER32 m_pOp32;
  PIMAGE_OPTIONAL_HEADER64 m_pOp64;

  PIMAGE_DATA_DIRECTORY    m_pDataDir;

  PIMAGE_SECTION_HEADER    m_pSectionHeader;

  PIMAGE_EXPORT_DIRECTORY  m_pExportDir;

  PIMAGE_IMPORT_DESCRIPTOR m_pImportDes;

  PIMAGE_BASE_RELOCATION   m_pBaseRelocation;

  PIMAGE_RESOURCE_DIRECTORY m_ResDir;

  PIMAGE_TLS_DIRECTORY32   m_pTLS32;

  PIMAGE_TLS_DIRECTORY64   m_pTLS64;

  PIMAGE_DELAYLOAD_DESCRIPTOR m_pDelayLoad;
public:
  // 读文件到内存中
  BOOL ReadFileToMem(CString strFile);
  // 检查是否是PE文件
  BOOL CheckPE();
  // 检查是64还是32/ROM
  INT Check64or32();

  // 32/64 DOS - 文件头一致

  // 解析64位PE
  void AnalysisPE64();

  // 解析32位PE
  void AnalysisPE32();

  // 数据目录
  void AnalysisDataDir();

  // 区段头表
  void AnalysisSection();

  // 解析导出表
  DWORD AnalysisExport();

  // 解析导入表
  DWORD AnalysisImport();

  // 解析重定位
  DWORD AnalysisRelocation();

  // 解析资源
  DWORD AnalysisRes();

  // 解析TLS
  DWORD AnalysisTLS();

  // 解析延迟加载表
  DWORD AnalysisDelayImport();

  // FOA转RVA
  DWORD FoaToRva(DWORD dwFoa);

  // RVA转FOA
  DWORD RvaToFoa(DWORD dwRva);
};


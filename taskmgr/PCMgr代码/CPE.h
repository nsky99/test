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
  // ���ļ����ڴ���
  BOOL ReadFileToMem(CString strFile);
  // ����Ƿ���PE�ļ�
  BOOL CheckPE();
  // �����64����32/ROM
  INT Check64or32();

  // 32/64 DOS - �ļ�ͷһ��

  // ����64λPE
  void AnalysisPE64();

  // ����32λPE
  void AnalysisPE32();

  // ����Ŀ¼
  void AnalysisDataDir();

  // ����ͷ��
  void AnalysisSection();

  // ����������
  DWORD AnalysisExport();

  // ���������
  DWORD AnalysisImport();

  // �����ض�λ
  DWORD AnalysisRelocation();

  // ������Դ
  DWORD AnalysisRes();

  // ����TLS
  DWORD AnalysisTLS();

  // �����ӳټ��ر�
  DWORD AnalysisDelayImport();

  // FOAתRVA
  DWORD FoaToRva(DWORD dwFoa);

  // RVAתFOA
  DWORD RvaToFoa(DWORD dwRva);
};


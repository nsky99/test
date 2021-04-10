#pragma once
#include <Windows.h>
#include "../nSkyStub/nSkyStub.h"

enum PackSetting
{
	SUPPORT = 1,   // ֧��
	UNSUPPORT = 0 // ��֧��
};
class CPack
{
private:
	// �ϵ�buf�е�
	PCHAR m_pBuf;
	DWORD m_dwFileSize;
  
	// �µ�buf�е�
	PCHAR m_pNewBuf;
	DWORD m_dwNewFileSize;
	
	// PE��ؽṹ��
	PIMAGE_DOS_HEADER m_pDos;
	PIMAGE_NT_HEADERS m_pNt;
	PIMAGE_SECTION_HEADER m_pSection;
	DWORD m_OriSectionNumber;					// ԭʼ������

	// �������������
	DWORD m_codeIndex;

	// �洢��Դ���ݵ����� .rsrc
	DWORD m_pResRva;
	DWORD m_pResSectionRva;
	DWORD m_ResSectionIndex;
	DWORD m_ResPointerToRawData;
	DWORD m_ResSizeOfRawData;

	// �洢tls���ݵ�����,Ҳ����.tls����
	DWORD m_pTlsDataRva;
	DWORD m_pTlsSectionRva;
	DWORD m_TlsSectionIndex;
	DWORD m_TlsPointerToRawData;
	DWORD m_TlsSizeOfRawData;

	//tls���е���Ϣ
	DWORD m_StartOfDataAddress;
	DWORD m_EndOfDataAddress;
	DWORD m_CallBackFuncAddress;
private:
	// ��ȡҪ�ӿǵĳ����ڴ���
	// ����1: Ҫ�ӿǵĳ���·��
	VOID ReadPackFile(
		_In_ PCHAR	pPackPath);

	// ��PE�ļ���rvaתΪ�ļ�ƫ��(Foa)
	// ����1: PE�ļ���RVA
	DWORD RvaToFoa(
		_In_ DWORD Rva);

	// ����TLS��Ϣ
	// ����1: ���ڱ�����stub�����TLS��Ϣ
	BOOL DealwithTLS(
		_Out_ PPACKINFO& pPackInfo);

	// �Դ���ν��м���
	// �޲�
	VOID EnCode(VOID);

	// ��ÿ�����ν���ѹ��
	// ����1: ���ڱ�����stub�����ѹ����Ϣ
	VOID CompressPE(
		_In_ PPACKINFO& pPackInfo);

	// ��ָ�����ν���ѹ��������ѹ����������׵�ַ
	// ����1: ָ�����ε��׵�ַ
	// ����2: ָ�����εĴ�С
	// ����3: ѹ����Ĵ�С
	PCHAR Compress(
		_In_ PVOID pSource, 
		_In_ long lInLength, 
		_In_ long& lOutLenght);

	// �����Ƿ����
	// ����1: ���δ�С
	// ����2: ��������
	DWORD CalcAlignment(
		_In_ DWORD dwSize, 
		_In_ DWORD dwAlignment);

	// ����µ�����
	// ����1: ����������
	// ����2: �����ε�����
	// ����3: �����εĴ�С
	// ����4: �����ε�����
	DWORD AddSection(
		_In_opt_ PCHAR szName,        
		_In_opt_ PCHAR pSectionBuf,   
		_In_opt_ DWORD dwSectionSize, 
		_In_opt_ DWORD dwAttribute    
	);

	// ��ȡҪ��ת��OEP - RVA
	// �޲���
	DWORD GetOepRva(VOID);

	// ��ȡPE�ļ����ػ�ַ
	// �޲���
	DWORD GetImageBase(VOID);

	// ��ȡ�����Rva
	// �޲���
	DWORD GetImportTableRva(VOID);

	// ��ȡ�ض�λ���Rva
	// �޲���
	DWORD GetRelocRva(VOID);

	// ���������εĵ�ַ�޸�dll���ض�λ[dll�Ǽ��ص��ڴ��,�������Ĭ�ϼ��ػ�ַ,����ӵĽ�����rva�Լ���ԭ������ʼ�Ĳ�ֵ����������.text���ض�λ]
	// ����1: stub�ڴ�ָ��
	// ����2: stub�ľֲ�
	VOID FixDllRloc(
		_In_ PCHAR pBuf, 
		_In_ PCHAR pOri);

	// ���Ҫ���һ��������,�����������ε�rva
	// �޲���
	DWORD GetNewSectionRva(VOID);

	// ���ڶ�̬���ػ�ַ,��Ҫ��stub���ض�λ����(.reloc)�޸ĺ󱣴�,��PE�ض�λ��Ϣָ��ָ��õ�ַ
	// ����1: �޸��ض�λ
	VOID ChangeReloc(
		_In_ PCHAR pBuf);

	// ��ȡ���һ�����ε�RVA
	// �޲���
	DWORD GetLastSectionRva(VOID);

	// ����TLS
	// ����1: TLS���ε�RVA
	// ����2: ָ��Stub���ε�ָ��
	// ����3: ����ṹ����Ϣ
	VOID SetTls(
		_In_ DWORD NewSectionRva, 
		_In_ PCHAR pStubBuf, 
		_In_ PPACKINFO pPackInfo);

	// �����µĳ�����ڵ�
	// ����1: �µ�OEP
	VOID SetNewOep(
		_In_ DWORD dwNewOep);

	// ����ÿ������Ϊ��д״̬
  // �޲���
	VOID SetMemWritable(VOID);

	// �Ե������и��� ȫ����Ϊ0
	// �޲���
	VOID ChangeImportTable(VOID);

	// ���Ӻÿǵĳ��򱣴浽�ļ�
	// ����1: ·��\\�ļ�����
	VOID SavePackFile(
		_In_ PCHAR pNewFileName);
	//------------ �ⲿ�ӿ� - �ṩ���ⲿ���õĺ��� ------------// 
public:
	// ��ָ������ӿ�
	// ����1: �Ƿ�֧��ѹ��
	// ����2: �Ƿ�֧�ּ���
	// ����3: �Ƿ�֧��ע�ᱣ��
	// ����4: �Ƿ�֧�ֶ�̬�ӽ���
	// ����5: �Ƿ�֧��У���ܺͱ���
	// ����6: �Ƿ�֧�ַ�����
	// ����7: �Ƿ�֧��API�ض���
	// ����8: �Ƿ�֧�ַ�dump
	// ����9: Ҫ�ӿǳ����·��
	BOOL Pack(
		_In_opt_ BOOL  bIsCompression,
		_In_opt_ BOOL  bIsNormalEncryption,
		_In_opt_ BOOL  bIsRegisteredProtection,
		_In_opt_ BOOL  bIsDynamicEncryption,
		_In_opt_ BOOL  bIsVerificationProtection,
		_In_opt_ BOOL  bIsAntiDebugging,
		_In_opt_ BOOL  bIsApiRedirect,
		_In_opt_ BOOL  bIsAntiDump,
		_In_		 PCHAR pPath);
};


#pragma once
#include <Windows.h>
typedef struct _PACKINFO
{
	DWORD StartAddress;					// ���洢��ʼ������ַ
	DWORD TlsIndex;							// tls���
	DWORD TlsCallbackFuncRva;		// tls�ص�����ָ������
	DWORD TargetOepRva;					// �����洢Ŀ������OEP��
	DWORD ImageBase;						// ������ػ�ַ
	DWORD ImportTableRva;				// iat��rva
	DWORD RelocRva;							// �ض�λ��rva


	DWORD PackSectionNumber;		// ѹ����������
	DWORD packSectionRva;				// ѹ�����ε�rva
	DWORD packSectionSize;			// ѹ�����εĴ�С
	DWORD PackInfomation[50][2];// ѹ��������ÿ�����ε�index�ʹ�С
	BOOL	bIsTlsUseful;					
	BOOL	bIsCompression;				 
	BOOL	bIsNormalEncryption;
	BOOL	bIsRegisteredProtection;
	BOOL	bIsDynamicEncryption;
	BOOL	bIsVerificationProtection;
	BOOL	bIsAntiDebugging;
	BOOL	bIsApiRedirect;
	BOOL	bIsAntiDump;
}PACKINFO, * PPACKINFO;

// ��C���Եķ�ʽ�������Stub��Pack����������
extern "C" _declspec(dllexport) PACKINFO g_PackInfo;
#include <stdio.h>
#include <string>
#include "RemoteThreadInject.h"
using std::string;
int main(int argc,char* argv[])
{
	// ������� == 1û�д������
	if (argc == 1)
	{
		MessageBoxW(0, L"��������ȷ!", L"Error", MB_ICONERROR);
		return 0;
	}

	DWORD dwTaskmgrPid = 0;
	DWORD dwProtectPid = 0;
	char  strDllPath[MAX_PATH];
	memset(strDllPath, 0, MAX_PATH);
	//argv[1] Ϊһ���������������������ID
	sscanf_s(argv[1], "%d", &dwTaskmgrPid);
	//argv[2] Ϊ�����������������Ľ���ID
	sscanf_s(argv[2], "%d", &dwProtectPid); 
	//argv[3] Ϊ��������������Hook��Dll
	memcpy(strDllPath, argv[3], strlen(argv[3]) + 1);
	// �����ں˶�������ҳ								��������ҳ�����������ֵ��ں˶�������˾�ֱ�ӷ��ؾ��																
	HANDLE hMapFile = CreateFileMapping(0, 0, PAGE_READWRITE, 0, 0x1000, L"Ҫ�����Ľ���ID");

	// ������ҳ�����Ե�ַ(�����ڴ�)����ӳ������￪ʼӳ��																								
	LPSTR lpBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0x1000);
	// ������ҳ��д����					
	*PDWORD(lpBuf) = dwProtectPid;

	Inject(dwTaskmgrPid, strDllPath);

	// �ر�ӳ�� �Ͽ�����ҳ�������ڴ��ӳ��												
	UnmapViewOfFile(lpBuf);

	// �ر��ں˾��												
	CloseHandle(hMapFile);

	return 0;
}
#include <windows.h>
#include <stdio.h>
#include <WtsApi32.h>
#pragma  comment (lib,"Wtsapi32.lib")
#pragma warning(disable : 4996)
// ���޸��Ķ������ļ��ͽű��ļ�����
unsigned int FixBinaryFileNumber = 0;
unsigned int FixScriptFileNumber = 0;
// ɾ����Desktop_.INI�ļ�����
unsigned int DelININUmber = 0;
// �����ļ��Ĵ�С
unsigned int SizeOfVirusFile = 0;
char  path[MAX_PATH];


// �������Ʋ��ҽ���ID
unsigned int SearchPidByName(const char* pProcessName)
{
	// 1. ��ָ��Զ������Ự�����������ľ��
	char szServerName[] = "127.0.0.1";    // �򿪱����ķ���
	HANDLE WtsServerHandle = WTSOpenServer(szServerName);


	// ��ʼ����Ҫ�õ��ı���
	PWTS_PROCESS_INFO pWtspi = NULL;
	DWORD dwCount = 0;
	// 2. ö�ٽ���
	if (!WTSEnumerateProcesses(WtsServerHandle, 0, 1, &pWtspi, &dwCount))
	{
		WTSCloseServer(WtsServerHandle);
		return -1;
	}
	WTSCloseServer(WtsServerHandle);


	// 3. ����ָ���Ľ���id
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (strcmp(pWtspi[i].pProcessName, pProcessName) == 0)
			return pWtspi[i].ProcessId;
	}


	// 4. û���ҵ�
	return -1;
}

// ɱ����è�������
void TerminatePandaVir()
{
	DWORD dwPid = SearchPidByName("spo0lsv.exe");
	HANDLE hP = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
	if (hP == NULL)
	{
		printf("����è�������ʧ��\n");
		return;
	}
	TerminateProcess(hP, 0);
	CloseHandle(hP);
	printf("�ս���è������̳ɹ�\n");
}

// �޸�ע���
void FixReg()
{
	// ɾ��svcshare
	char RegRun[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKeyHKCU = NULL;
	LONG lSize = MAXBYTE;
	char cData[MAXBYTE] = { 0 };
	long lRet = RegOpenKey(HKEY_CURRENT_USER, RegRun, &hKeyHKCU);
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegQueryValueEx(hKeyHKCU, "svcshare", NULL, NULL, (unsigned char*)cData, (unsigned long*)&lSize);
		if (lRet == ERROR_SUCCESS)
		{
			if (strcmp(cData, "C:\\WINDOWS\\system32\\drivers\\spo0lsv.exe") == 0)
			{
				printf("Find virus AutorunRegInfo!\n\n");
			}
			lRet = RegDeleteValue(hKeyHKCU, "svcshare");
			if (lRet == ERROR_SUCCESS)
			{
				printf("Panda's RegItem has beed deleted!\n\n");
			}
			else
			{
				printf("Panda's RegItem is still alive or is gone!\n\n");
			}
		}
		else
		{
			printf("Reg is clear!\n\n");
		}
		RegCloseKey(hKeyHKCU);
	}
	else
	{
		printf("Open Reg failed!\n\n");
	}
	// �޸��ļ���������ʾ����CheckedValue��ֵ����Ϊ1
	char RegHide[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\Folder\\Hidden\\SHOWALL";
	HKEY hKeyHKLM = NULL;
	DWORD dwFlag = 1;
	long lRetHide = RegOpenKey(HKEY_LOCAL_MACHINE, RegHide, &hKeyHKLM);
	if (lRetHide == ERROR_SUCCESS)
	{
		if (ERROR_SUCCESS == RegSetValueEx(
			hKeyHKLM,             //subkey handle  
			"CheckedValue",       //value name  
			0,                    //must be zero  
			REG_DWORD,            //value type  
			(CONST BYTE*) & dwFlag, //pointer to value data  
			4))                   //length of value data
		{
			printf("Reg fixed!\n\n");
		}
		else
		{
			printf("Can't fix RegHiddenItem or it's clear!\n\n");
		}
	}

}

// ɾ��ָ���ļ�
unsigned int DelSPacificFile(const char* FileName)
{
	// ȥ���ļ������ء�ϵͳ�Լ�ֻ������
	DWORD dwFileAttributes = GetFileAttributes(FileName);						// ��ȡ�ļ�����
	dwFileAttributes &= FILE_ATTRIBUTE_HIDDEN;									
	dwFileAttributes &= FILE_ATTRIBUTE_SYSTEM;
	dwFileAttributes &= FILE_ATTRIBUTE_READONLY;
	SetFileAttributes(FileName, dwFileAttributes);

	int delRet = DeleteFile(FileName);											// ɾ���ļ�
	if (delRet)
	{
		printf("File %s has been Deleted!\n\n", FileName);
		return TRUE;
	}
	else
	{
		printf("File %s is still alive! MayBe it has been deleted!\n\n", FileName);
		return FALSE;
	}
}

// �ж��ļ��Ƿ�Ϊָ���������ļ�
bool IsEXE(const char* pFileName)
{
	const char* pTemp = pFileName;								// �ӵ�һ���ַ���ʼ�����ϱȶ�ʣ�µ��ַ���
	while (*pTemp != 0x00)										// ע��PIFΪ��д
	{
		if (!strcmp(pTemp, ".exe") || !strcmp(pTemp, ".PIF") || !strcmp(pTemp, ".com") || !strcmp(pTemp, ".src"))
		{
			return true;
		}
		++pTemp;
	}
	return false;
}

// �ж��ļ��Ƿ�Ϊָ���ű�
bool IsScript(const char* pFileName)
{
	const char* pTemp = pFileName;
	while (*pTemp != 0x00)
	{
		if (!strcmp(pTemp, ".html") || !strcmp(pTemp, ".htm") || !strcmp(pTemp, ".asp") || !strcmp(pTemp, ".php") || !strcmp(pTemp, ".jsp") || !strcmp(pTemp, ".aspx"))
		{
			return true;
		}
		++pTemp;
	}
	return false;
}

// ����·���������ļ���
char* GetFilename(char* p)
{
	int x = strlen(p);
	char ch = '\\';
	char* q = strrchr(p, ch) + 1;
	return q;
}

// �޸���Ⱦ�������ļ�
unsigned int FixBinaryFile(char* pStrFilePath)
{
	CHAR* pFilebuf = NULL;
	HANDLE hFile = CreateFile(pStrFilePath,									// ���ܸ�Ⱦ�ļ�
		GENERIC_READ | GENERIC_WRITE,
		FALSE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "Open infected file failed!", "oh no!", NULL);
		return 0;
	}

	DWORD FileSize = GetFileSize(hFile, NULL);								// ��ȡ��Ⱦ���ļ���С
	pFilebuf = new CHAR[FileSize]{};										// �����������������
	DWORD dwCount = 1;
	BOOL bRet = ReadFile(hFile, pFilebuf, FileSize, &dwCount, NULL);		// ����Ⱦ�ļ������ڴ�
	if (!bRet)																// ��ȡ���ִ���
	{
		CloseHandle(hFile);
		delete pFilebuf;
		return FALSE;
	}
	char* pFileOffset = pFilebuf + SizeOfVirusFile;									// ����Ⱦ�ļ���ǰ�棬0x7531Ϊ����Դ�ļ���С��1ffff�ֽ�Ϊ�����ѿǺ���ļ�
	char* p = pStrFilePath;
	int FileNameLength = strlen(GetFilename(p));							// ��ȡ�ļ�������

	SetFilePointer(hFile, 0, 0, FILE_BEGIN);								// 0x7531�ǲ����Ĵ�С
	WriteFile(hFile, pFileOffset, FileSize - SizeOfVirusFile - FileNameLength - 2, &dwCount, NULL);	// ��Ⱦ��־�ĳ���Ӱ������ΪĿ���ļ����ļ�������
	SetEndOfFile(hFile);
	FixBinaryFileNumber++;													// ������ͳ���޸�����
	CloseHandle(hFile);

	delete[] pFilebuf;
	return TRUE;
}

// �޸���Ⱦ�ű��ļ�
bool FixScriptFile(const char* pstrFilePath)
{
	CHAR* pFilebuf = NULL;
	HANDLE hFile = CreateFile(pstrFilePath,
		GENERIC_READ | GENERIC_WRITE,
		FALSE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "Open file failed!", "oh no", NULL);
		return 0;
	}
	DWORD FileSize = GetFileSize(hFile, NULL);
	pFilebuf = new CHAR[FileSize]{};
	DWORD dwCount = 1;
	BOOL bRet = ReadFile(hFile, pFilebuf, FileSize, &dwCount, NULL);		// �ļ������ڴ�
	if (!bRet)
	{
		CloseHandle(hFile);
		delete pFilebuf;
		return FALSE;
	}
	char* pFileOffset = pFilebuf;
	SetFilePointer(hFile, 0, 0, FILE_BEGIN);
	WriteFile(hFile, pFilebuf, FileSize - 76, &dwCount, NULL);				// ɾ�����75���ֽ�
	SetEndOfFile(hFile);
	FixScriptFileNumber++;													// ������ͳ���޸�����
	CloseHandle(hFile);
	delete[] pFilebuf;
	return TRUE;
}

// ���ļ������ڴ沢��ȡ��С
char* GetFileBuf(char* pstrFilePath, DWORD* FileSize)
{
	char* pFilebuf = NULL;
	//���ļ���ȡ���
	HANDLE hFile = CreateFile(pstrFilePath,
		GENERIC_READ,
		FALSE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("File Open Faild!\n\n");
		return 0;
	}

	//��ȡ�ļ���С
	*FileSize = GetFileSize(hFile, NULL);
	pFilebuf = new char[*FileSize]{};
	//���ļ�
	DWORD dwCount = 1;
	BOOL bRet = ReadFile(hFile, pFilebuf, *FileSize, &dwCount, NULL);

	if (bRet)
	{
		CloseHandle(hFile);
		return pFilebuf;
	}
	//�ͷ���Դ
	CloseHandle(hFile);
	delete pFilebuf;
	return 0;

}

// �Ƿ��Ǳ���Ⱦ�Ķ������ļ�,����Ⱦ�ļ����һ���ֽ�Ϊ01,��ǰ�ҵ�00�ĺ�����ֽ���WhBoy
bool IsInfectedBinaryFile(char* pstrFilePath)
{
	CHAR* pFileBuf = NULL;
	DWORD dwFileSize = 0;
	pFileBuf = GetFileBuf(pstrFilePath, &dwFileSize);

	if (pFileBuf == 0)
	{
		return false;
	}
	BYTE* pFileOffset = (BYTE*)pFileBuf;
	pFileOffset += (dwFileSize - 1);

	if (*pFileOffset != 0x01)									// �ж��Ƿ�Ϊ0x01�����ǵĻ���û��Ⱦ
	{
		delete[] pFileBuf;
		return  false;
	}
	while (*pFileOffset != 0x00)
	{
		--pFileOffset;
	}
	pFileOffset++;
	CHAR temp[6] = { 0 };
	memcpy_s(temp, 5, pFileOffset, 5);
	if (!strcmp(temp, "WhBoy"))
	{
		delete[] pFileBuf;
		return  true;
	}
	delete[] pFileBuf;
	return  false;
}

// �Ƿ��Ǳ���Ⱦ�Ľű��ļ�
bool IsInfectedScriptFIle(char* pstrFilePath)
{
	CHAR* pFileBuf = NULL;
	DWORD dwFileSize = 0;
	pFileBuf = GetFileBuf(pstrFilePath, &dwFileSize);
	if (pFileBuf == 0)
	{
		return 0;
	}
	BYTE* pFileOffset = (BYTE*)pFileBuf;
	*pFileOffset;
	pFileOffset += (dwFileSize - 64);

	CHAR temp[32] = { 0 };
	memcpy_s(temp, 31, pFileOffset, 31);
	if (!lstrcmp(temp, "http://www.ac86.cn/66/index.htm"))
	{
		delete[] pFileBuf;
		return  TRUE;
	}
	delete[] pFileBuf;
	return  FALSE;
}

// ����ȫ���޸��ļ�
unsigned int DelIniFixInfectedFiles(const char* lpszPath)
{

	WIN32_FIND_DATA stFindFile;
	HANDLE hFindFile;

	char szPath[MAX_PATH];
	char szFindFile[MAX_PATH];
	char szSearch[MAX_PATH];
	const char* szFilter;
	int len;


	szFilter = "*.*";
	strcpy(szPath, lpszPath);
	len = lstrlen(szPath);
	if (szPath[len - 1] != '\\')
	{
		szPath[len] = '\\';
		szPath[len + 1] = '\0';
	}
	strcpy(szSearch, szPath);
	strcat(szSearch, szFilter);

	hFindFile = FindFirstFile(szSearch, &stFindFile);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			strcpy(szFindFile, szPath);
			strcat(szFindFile, stFindFile.cFileName);

			// ��Ŀ¼���Ҳ��ǵ�ǰĿ¼����һ��Ŀ¼
			if (stFindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (stFindFile.cFileName[0] != '.')
				{
					DelIniFixInfectedFiles(szFindFile);
				}
			}
			else
			{
				// ɾ��Desktop_.ini
				if (!strcmp(stFindFile.cFileName, "Desktop_.ini"))						
				{

					DWORD dwFileAttributes = GetFileAttributes(szFindFile);				// ȥ���ļ������ء�ϵͳ�Լ�ֻ������
					dwFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
					dwFileAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
					dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
					SetFileAttributes(szFindFile, dwFileAttributes);

					BOOL bRet = DeleteFile(szFindFile);
					if (bRet)
					{
						printf("\"%s\"_____deleted!\n", szFindFile);
						DelININUmber++;										// ������ͳ�Ƹ���
					}
					else
					{
						printf("Deleted \"%s\" failed!\n", szFindFile);
					}
				}
				//�ж��Ƿ��Ƕ������ļ�
				else if (IsEXE(stFindFile.cFileName))									
				{
					if (IsInfectedBinaryFile(szFindFile))
					{
						// printf("%s infected!\n", szFindFile);
						if (FixBinaryFile(szFindFile))
						{
							printf("\"%s\"_____fixed!\n", szFindFile);
						}
						else
						{
							printf("Fix \"%s\" failed!\n", szFindFile);
						}
					}
					else
					{
						printf("\"%s\"_____healthy!\n\n", szFindFile);
					}
				}
				//�ж��Ƿ��ǽű��ļ�
				else if (IsScript(stFindFile.cFileName))									
				{
					if (IsInfectedScriptFIle(szFindFile))
					{
						// printf("%s infected!\n", szFindFile);
						if (FixScriptFile(szFindFile))
						{
							printf("\"%s\"_____fixed!\n", szFindFile);
						}
						else
						{
							printf("Fix \"%s\" failed!!!!!\n", szFindFile);
						}
					}
					else
					{
						printf("\"%s\"_____healthy!\n", szFindFile);
					}
				}
			}
		} while (FindNextFile(hFindFile, &stFindFile));
	}

	FindClose(hFindFile);
	return 0;
}

// ѡ�񲡶�Դ�ļ�����ȡ����Դ�ļ���С
void SelectVirFileGetVirSize()
{
	// ѡ��Virus�ļ�
	OPENFILENAMEA ofn;
	memset(path, 0, MAX_PATH);
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = path;																			// path
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "*.exe\0*.exe\0";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileName(&ofn)) {															// ������ļ�����
		MessageBox(NULL, "Open file failed!", NULL, MB_OK);
		exit(0);																								// �˳����н���
	}
	// ��ȡ�ļ����,ӳ�䵽�ڴ�
	HANDLE hFile = CreateFileA(path, GENERIC_ALL, 3u, NULL, OPEN_EXISTING, 0x80u, 0);		// path����һ��3u��ʾ�����д
	DWORD dwFileSize = GetFileSize(hFile, NULL);											// ��ȡ�ļ���С
	SizeOfVirusFile = dwFileSize;
}

int main()
{
	SelectVirFileGetVirSize();											// ѡ�񲡶��ļ�����ȡ���С
	FixReg();																				// �޸�ע���
	TerminatePandaVir();														// ������������

	DelSPacificFile("C:\\autorun.inf");						  // ɾ��C�̸�Ŀ¼�µ��ļ�
	DelSPacificFile("C:\\setup.exe");
	DelSPacificFile("C:\\Windows\\System32\\drivers\\spo0lsv.exe");

	DelIniFixInfectedFiles("C:");											// ִ��C�̸�Ⱦ�޸�
	DelIniFixInfectedFiles("D:");											// ִ��D�̸�Ⱦ�޸�
																			

	printf("\n==================���������� !=====================\n");
	printf("\n*********************** ���� **************************\n");// �޸�����
	printf("�����ļ���С %d bytes\n", SizeOfVirusFile);
	printf("�޸���ִ���ļ����� :%d \n", FixBinaryFileNumber);
	printf("�޸��ű��ļ����� :%d \n", FixScriptFileNumber);
	printf("ɾ��Desktop_.ini���� :%d \n", DelININUmber);
	printf("************************* ���� ************************\n\n");// �޸�����
	MessageBox(0, "�޸����", "Success", MB_OK);

  return 0;
}
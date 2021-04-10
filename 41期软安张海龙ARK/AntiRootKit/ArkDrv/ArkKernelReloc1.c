#include "ArkKernelReloc1.h"
#include <ntifs.h>
#include <ntimage.h>
#include "ArkDrv.h"
extern PDRIVER_OBJECT g_pArkDrvObj;


#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG* ServiceTableBase;        // ������ַ
	ULONG* ServiceCounterTableBase; // �������ַ
	ULONG NumberOfServices;         // ������ĸ���
	UCHAR* ParamTableBase;          // �������ַ
}SSDTEntry, * PSSDTEntry;
#pragma pack()

// ����SSDT
NTSYSAPI SSDTEntry KeServiceDescriptorTable;

PSSDTEntry g_pNewSSDT;//�µ�SSDT
ULONG g_JmpPoint;
PUCHAR pHookPoint;

// ���ļ�
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile, // �ļ�·����������
	IN BOOLEAN         bIsDir)   // �Ƿ�Ϊ�ļ���
{
	HANDLE          hFile = NULL;
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess =
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG           ulCreateOpt =
		FILE_SYNCHRONOUS_IO_NONALERT;
	// 1. ��ʼ��OBJECT_ATTRIBUTES������
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // ���س�ʼ����ϵĽṹ��
		pstrFile,      // �ļ���������
		ulAttributes,  // ��������
		NULL, NULL);   // һ��ΪNULL
	// 2. �����ļ�����
	ulCreateOpt |= bIsDir ?
		FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	Status = ZwCreateFile(
		&hFile,                // �����ļ����
		GENERIC_ALL,           // �ļ���������
		&objAttrib,            // OBJECT_ATTRIBUTES
		&StatusBlock,          // ���ܺ����Ĳ������
		0,                     // ��ʼ�ļ���С
		FILE_ATTRIBUTE_NORMAL, // �½��ļ�������
		ulShareAccess,         // �ļ�����ʽ
		FILE_OPEN_IF,          // �ļ�������򿪲������򴴽�
		ulCreateOpt,           // �򿪲����ĸ��ӱ�־λ
		NULL,                  // ��չ������
		0);                   // ��չ����������
	if (!NT_SUCCESS(Status))
		return (HANDLE)-1;
	return hFile;
}

// ��ȡ�ļ���С
ULONG64 KernelGetFileSize(IN HANDLE hfile)
{
	// ��ѯ�ļ�״̬
	IO_STATUS_BLOCK           StatusBlock = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = ZwQueryInformationFile(
		hfile,        // �ļ����
		&StatusBlock, // ���ܺ����Ĳ������
		&fsi,         // �������һ��������������������Ϣ
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(Status))
		return 0;
	return fsi.EndOfFile.QuadPart;
}
// ��ȡ�ļ�
ULONG64 KernelReadFile(
	IN  HANDLE         hfile,    // �ļ����
	IN  PLARGE_INTEGER Offset,   // �����￪ʼ��ȡ
	IN  ULONG          ulLength, // ��ȡ�����ֽ�
	OUT PVOID          pBuffer)  // �������ݵĻ���
{
	// 1. ��ȡ�ļ�
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwReadFile(
		hfile,        // �ļ����
		NULL,         // �ź�״̬(һ��ΪNULL)
		NULL, NULL,   // ����
		&StatusBlock, // ���ܺ����Ĳ������
		pBuffer,      // �����ȡ���ݵĻ���
		ulLength,     // ��Ҫ��ȡ�ĳ���
		Offset,       // ��ȡ����ʼƫ��
		NULL);        // һ��ΪNULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. ����ʵ�ʶ�ȡ�ĳ���
	return StatusBlock.Information;
}
typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;    //˫������
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

// �����ڴ�����
void* SearchMemory(char* buf, int BufLenth, char* Mem, int MaxLenth)
{
	int MemIndex = 0;
	int BufIndex = 0;
	for (MemIndex = 0; MemIndex < MaxLenth; MemIndex++)
	{
		BufIndex = 0;
		if (Mem[MemIndex] == buf[BufIndex] || buf[BufIndex] == '?')
		{
			int MemIndexTemp = MemIndex;
			do
			{
				MemIndexTemp++;
				BufIndex++;
			} while ((Mem[MemIndexTemp] == buf[BufIndex] || buf[BufIndex] == '?') && BufIndex < BufLenth);
			if (BufIndex == BufLenth)
			{
				return Mem + MemIndex;
			}

		}
	}
	return 0;
}

// �ر�ҳ����
void OffProtected()
{
	__asm { //�ر��ڴ汣��
		cli;
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}
}

// ����ҳ����
void OnProtected()
{
	__asm { //�ָ��ڴ汣��
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
		sti;
	}

}

// ͨ�����ƻ�ȡģ���ַ
ULONG32 MyGetModuleHandle(PUNICODE_STRING pModuleName)
{
	PLDR_DATA_TABLE_ENTRY pLdr =
		(PLDR_DATA_TABLE_ENTRY)g_pArkDrvObj->DriverSection;
	LIST_ENTRY* pTemp = &pLdr->InLoadOrderLinks;
	do
	{
		PLDR_DATA_TABLE_ENTRY pDriverInfo =
			(PLDR_DATA_TABLE_ENTRY)pTemp;
		if (RtlCompareUnicodeString(pModuleName, &pDriverInfo->BaseDllName, FALSE) == 0)
		{
			return pDriverInfo->DllBase;
		}
		pTemp = pTemp->Blink;
	} while (pTemp != &pLdr->InLoadOrderLinks);
	return 0;
}

//windows���ݲ�ͬ�Ļ���������ز�ͬ���ں��ļ�
//���ˣ�����PAE  
//���ˣ�û��PAE
//��ˣ�����PAE
//��ˣ�û��PAE

// ��ȡ�ں�ģ�鵽�ڴ���
void ReadKernelToBuf(PWCHAR pPath, PUCHAR* pBuf)
{
	//-----------------------------------------
	UNICODE_STRING pKernelPath;  //�ں��ļ�·��
	HANDLE hFile = 0;            //�ں��ļ����
	LARGE_INTEGER Offset = { 0 };//��ȡ��ƫ��ֵ
	//-----------------------------------------
	//1 ���ļ�
	RtlInitUnicodeString(
		&pKernelPath,
		pPath);
	hFile = KernelCreateFile(&pKernelPath, FALSE);
	//2 ��ȡ�ļ���С
	ULONG64 ulFileSize = KernelGetFileSize(hFile);
	*pBuf = ExAllocatePool(NonPagedPool, ulFileSize);
	RtlZeroMemory(*pBuf, ulFileSize);
	//3 ��ȡ�ļ����ڴ�
	KernelReadFile(hFile, &Offset, ulFileSize, *pBuf);

	ZwClose(hFile);
}

// չ���ں�PE�ļ�
void ZKKernel(PUCHAR* pZkBUf, PUCHAR buf)
{
	//1 ���DOSͷ���̶����NTͷ���ٻ����չͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)buf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + buf);
	ULONG uZkSize = pNt->OptionalHeader.SizeOfImage;

	//2 ����ռ�
	*pZkBUf = ExAllocatePool(NonPagedPool, uZkSize);
	RtlZeroMemory(*pZkBUf, uZkSize);
	//3 ��ʼչ��
	//3.1 �ȿ���ͷ��
	memcpy(*pZkBUf, buf, pNt->OptionalHeader.SizeOfHeaders);
	//3.2�ٿ�������
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		memcpy(
			*pZkBUf + pSection[i].VirtualAddress,//�������ڴ��е���ʼλ��
			buf + pSection[i].PointerToRawData,  //���������ļ��е�λ��
			pSection[i].Misc.VirtualSize         //�����εĴ�С
		);
	}
}

// �޸����ں��ض�λ
void FixReloc(PUCHAR ZkBuf, PUCHAR OldBase)
{
	typedef struct _TYPE {
		USHORT Offset : 12;
		USHORT Type : 4;
	}TYPE, * PTYPE;

	//1 ���DOSͷ���̶����NTͷ���ٻ����չͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ZkBuf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + ZkBuf);
	//2 ����ض�λ��
	PIMAGE_DATA_DIRECTORY pRelocDir = (pNt->OptionalHeader.DataDirectory + 5);
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)
		(pRelocDir->VirtualAddress + ZkBuf);

	//2.5 �õ�һ�����ں���Ĭ�ϻ�ַ���һ����ֵ
	ULONG uOffset = (ULONG)OldBase - pNt->OptionalHeader.ImageBase;

	//3 ��ʼ�޸��ض�λ
	while (pReloc->SizeOfBlock != 0)
	{
		ULONG uCount = (pReloc->SizeOfBlock - 8) / 2;//��0x1000�ڣ��ж�����Ҫ�ض�λ�ĵط�
		ULONG uBaseRva = pReloc->VirtualAddress;     //��0x1000����ʼλ��
		PTYPE pType = (PTYPE)(pReloc + 1);
		for (int i = 0; i < uCount; i++)
		{
			if (pType->Type == 3)
			{
				PULONG pRelocPoint = (uBaseRva + pType->Offset + ZkBuf);
				//�ض�λ��ĵ�ַ - �»�ַ = û�ض�λ�ĵ�ַ - Ĭ�ϻ�ַ
				//���ԣ��ض�λ��ĵ�ַ = �»�ַ - Ĭ�ϻ�ַ + û�ض�λ�ĵ�ַ
				*pRelocPoint = uOffset + *pRelocPoint;
			}
			pType++;
		}
		pReloc = (PIMAGE_BASE_RELOCATION)((ULONG)pReloc + pReloc->SizeOfBlock);
	}

}

// �޸���SSDT��
void FixSSDT(PUCHAR pZKBuf, PUCHAR OldBase)
{

	//���ں�ĳλ��1 - ���ں˻�ַ = ���ں�ĳλ��1 - ���ں˻�ַ��
	//���ں�ĳλ��1 = ���ں˻�ַ - ���ں˻�ַ + ���ں�ĳλ��1;
	DbgBreakPoint();
	LONG Offset = (ULONG)pZKBuf - (ULONG)OldBase;
	//1 �õ����ں��е�SSDT
	g_pNewSSDT = (PSSDTEntry)((LONG)&KeServiceDescriptorTable + Offset);

	//2 ���ϵͳ�������
	g_pNewSSDT->NumberOfServices = KeServiceDescriptorTable.NumberOfServices;

	//3 ���SSDT��
	g_pNewSSDT->ServiceTableBase = (ULONG*)((PUCHAR)KeServiceDescriptorTable.ServiceTableBase + Offset);
	//�����е�SSDT�б���ĺ�����ַ����ָ�����ں�
	for (int i = 0; i < g_pNewSSDT->NumberOfServices; i++)
	{
		g_pNewSSDT->ServiceTableBase[i] = g_pNewSSDT->ServiceTableBase[i] + Offset;
	}
	//4 ��������
	g_pNewSSDT->ParamTableBase = (PULONG)((PUCHAR)KeServiceDescriptorTable.ParamTableBase + Offset);
	memcpy(g_pNewSSDT->ParamTableBase,
		KeServiceDescriptorTable.ParamTableBase,
		g_pNewSSDT->NumberOfServices
	);
}

// ��ȡKiFastCallEntry����
ULONG GetKiFastCallEntry()
{
	ULONG uAddress = 0;
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		rdmsr;
		mov uAddress, eax;
		pop ecx;
		pop eax;
	}
	return uAddress;
}

// hook�й��˺���
ULONG   FilterFun(ULONG SSdtBase, PULONG OldFun, ULONG Id)
{
	//�����ȣ�˵�����õ���SSDT�еĺ���
	if (SSdtBase == (ULONG)KeServiceDescriptorTable.ServiceTableBase)
	{
		// ʹ��˼·��
		// ���������OD�����Һ���������190�ţ��������ں��еĺ���������ͨ��hookOpenProcess���޷���סOD�ˡ�
		return g_pNewSSDT->ServiceTableBase[Id];
	}
	return OldFun;

}

// inline Hook�Ļص�����
_declspec(naked)void MyHookFun()
{
	//eax �����ǵ��úţ�edx�������Ϻ�����ַ��edi������SSDT��ַ
	_asm {
		pushad;
		pushfd;
		push eax;		//���ú�
		push edx;		//ԭʼ������ַ
		push edi;		//SSDT��ַ

		call FilterFun;	// �Լ��Ĺ��˺�������ȡ����ʵ������ַ  ����ǰ��ѹ��3��������pushfd,pushad ebx�պ���ջ�еġ�esp+0x18]
		mov dword ptr ds : [esp + 0x18] , eax; // ��esp+0x18�� ebx��ֵ���滻��������ַ
		popfd;
		popad;			  // �ָ�ͨ�üĴ�����ebx���滻���µĺ�����ַ��
							// ִ��ԭʼhook��5���ֽ�
		sub     esp, ecx;
		shr     ecx, 2;
		jmp     g_JmpPoint;	// ��ת��ԭ��������ַ
	}
}

// hook Ŀ���5���ֽ�
UCHAR Old_Code[5] = { 0 };

// hookKiFastCallEntry����
void OnHookKiFastCallEntry()
{
	// KiFastCallEntry������ֵ
	char buf[] = { 0x2b, 0xe1, 0xc1, 0xe9, 0x02 };
	// ��ȡKiFastCallEntry������ַ
	ULONG KiFastCallEntryAdd = GetKiFastCallEntry();
	// �ҵ�hook��
	pHookPoint = SearchMemory(buf, 5, (char*)KiFastCallEntryAdd, 0x200);
	// �ƹ�ǰ5���ֽڣ�ӦΪ��hook�滻��
	g_JmpPoint = (ULONG)(pHookPoint + 5);
	// ���ݾɵ�5���ֽ�
	memcpy(Old_Code, pHookPoint, 5);
	// �ر�ҳ����
	OffProtected();
	// jmp xxxxxx
	// д����תĿ���ַ   Ŀ���ַ-ָ������-5
	pHookPoint[0] = 0xE9;
	*(ULONG*)(&pHookPoint[1]) = (ULONG)MyHookFun - (ULONG)pHookPoint - 5;
	// ����ҳ����
	OnProtected();
}


//ntoskrnl - ������������֧��PAE
//ntkrnlpa - ����������֧��PAE

//ntkrnlmp - �ദ��������֧��PAE
//ntkrpamp - �ദ������֧��PAE
// ��windows vista ��ʼ�� ���е��ں�Ĭ�ϰ�װ��˴�������ʽ��ntkrnlmp ���� ntkrpamp��
// Ȼ�󿽱���System32Ŀ¼�£����ĺ͵��˴�������һ��



// �ں����� ��ʼ
void KernelReload()
{
	PUCHAR pBuf = NULL;
	PUCHAR pZKBuf = NULL;
	UNICODE_STRING KernelName;

	//1 ���Ȱ��ں��ļ���ȡ���ڴ��� (Ĭ�Ͽ���PAE)
	ReadKernelToBuf(L"\\??\\C:\\Windows\\System32\\ntkrnlpa.exe", &pBuf);

	//2 �Ѷ����ڴ��е��ں˸�չ����0x1000����
	ZKKernel(&pZKBuf, pBuf);
	ExFreePool(pBuf);

	//3 �޸����ں˵��ض�λ ����Ȼ����PAE��ntkrnlpa.exe��,������ʾ������ ntoskrnl.exe
	RtlInitUnicodeString(&KernelName, L"ntoskrnl.exe");
	ULONG32 uBase = MyGetModuleHandle(&KernelName);

	FixReloc(pZKBuf, (PUCHAR)uBase);

	//4 �޸��µ�SSDT��
	FixSSDT(pZKBuf, (PUCHAR)uBase);

	//5 Hook��KiFastCallEntry�����Լ���Hook�������ж�Ӧ�������ں˻������ں�
	OnHookKiFastCallEntry();

}


// ж���ں˹���
void UnKernelHook()
{
	OffProtected();
	memcpy(pHookPoint, Old_Code, 5);
	OnProtected();
}
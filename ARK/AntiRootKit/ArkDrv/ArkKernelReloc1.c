#include "ArkKernelReloc1.h"
#include <ntifs.h>
#include <ntimage.h>
#include "ArkDrv.h"
extern PDRIVER_OBJECT g_pArkDrvObj;


#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG* ServiceTableBase;        // 服务表基址
	ULONG* ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR* ParamTableBase;          // 参数表基址
}SSDTEntry, * PSSDTEntry;
#pragma pack()

// 导入SSDT
NTSYSAPI SSDTEntry KeServiceDescriptorTable;

PSSDTEntry g_pNewSSDT;//新的SSDT
ULONG g_JmpPoint;
PUCHAR pHookPoint;

// 打开文件
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile, // 文件路径符号链接
	IN BOOLEAN         bIsDir)   // 是否为文件夹
{
	HANDLE          hFile = NULL;
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess =
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG           ulCreateOpt =
		FILE_SYNCHRONOUS_IO_NONALERT;
	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // 返回初始化完毕的结构体
		pstrFile,      // 文件对象名称
		ulAttributes,  // 对象属性
		NULL, NULL);   // 一般为NULL
	// 2. 创建文件对象
	ulCreateOpt |= bIsDir ?
		FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	Status = ZwCreateFile(
		&hFile,                // 返回文件句柄
		GENERIC_ALL,           // 文件操作描述
		&objAttrib,            // OBJECT_ATTRIBUTES
		&StatusBlock,          // 接受函数的操作结果
		0,                     // 初始文件大小
		FILE_ATTRIBUTE_NORMAL, // 新建文件的属性
		ulShareAccess,         // 文件共享方式
		FILE_OPEN_IF,          // 文件存在则打开不存在则创建
		ulCreateOpt,           // 打开操作的附加标志位
		NULL,                  // 扩展属性区
		0);                   // 扩展属性区长度
	if (!NT_SUCCESS(Status))
		return (HANDLE)-1;
	return hFile;
}

// 获取文件大小
ULONG64 KernelGetFileSize(IN HANDLE hfile)
{
	// 查询文件状态
	IO_STATUS_BLOCK           StatusBlock = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = ZwQueryInformationFile(
		hfile,        // 文件句柄
		&StatusBlock, // 接受函数的操作结果
		&fsi,         // 根据最后一个参数的类型输出相关信息
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(Status))
		return 0;
	return fsi.EndOfFile.QuadPart;
}
// 读取文件
ULONG64 KernelReadFile(
	IN  HANDLE         hfile,    // 文件句柄
	IN  PLARGE_INTEGER Offset,   // 从哪里开始读取
	IN  ULONG          ulLength, // 读取多少字节
	OUT PVOID          pBuffer)  // 保存数据的缓存
{
	// 1. 读取文件
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwReadFile(
		hfile,        // 文件句柄
		NULL,         // 信号状态(一般为NULL)
		NULL, NULL,   // 保留
		&StatusBlock, // 接受函数的操作结果
		pBuffer,      // 保存读取数据的缓存
		ulLength,     // 想要读取的长度
		Offset,       // 读取的起始偏移
		NULL);        // 一般为NULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. 返回实际读取的长度
	return StatusBlock.Information;
}
typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;    //双向链表
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

// 搜索内存特征
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

// 关闭页保护
void OffProtected()
{
	__asm { //关闭内存保护
		cli;
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}
}

// 开启页保护
void OnProtected()
{
	__asm { //恢复内存保护
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
		sti;
	}

}

// 通过名称获取模块地址
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

//windows根据不同的环境，会加载不同的内核文件
//单核，开了PAE  
//单核，没开PAE
//多核，开了PAE
//多核，没开PAE

// 读取内核模块到内存中
void ReadKernelToBuf(PWCHAR pPath, PUCHAR* pBuf)
{
	//-----------------------------------------
	UNICODE_STRING pKernelPath;  //内核文件路径
	HANDLE hFile = 0;            //内核文件句柄
	LARGE_INTEGER Offset = { 0 };//读取的偏移值
	//-----------------------------------------
	//1 打开文件
	RtlInitUnicodeString(
		&pKernelPath,
		pPath);
	hFile = KernelCreateFile(&pKernelPath, FALSE);
	//2 获取文件大小
	ULONG64 ulFileSize = KernelGetFileSize(hFile);
	*pBuf = ExAllocatePool(NonPagedPool, ulFileSize);
	RtlZeroMemory(*pBuf, ulFileSize);
	//3 读取文件到内存
	KernelReadFile(hFile, &Offset, ulFileSize, *pBuf);

	ZwClose(hFile);
}

// 展开内核PE文件
void ZKKernel(PUCHAR* pZkBUf, PUCHAR buf)
{
	//1 获得DOS头，继而获得NT头，再获得扩展头
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)buf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + buf);
	ULONG uZkSize = pNt->OptionalHeader.SizeOfImage;

	//2 申请空间
	*pZkBUf = ExAllocatePool(NonPagedPool, uZkSize);
	RtlZeroMemory(*pZkBUf, uZkSize);
	//3 开始展开
	//3.1 先拷贝头部
	memcpy(*pZkBUf, buf, pNt->OptionalHeader.SizeOfHeaders);
	//3.2再拷贝区段
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		memcpy(
			*pZkBUf + pSection[i].VirtualAddress,//本区段内存中的起始位置
			buf + pSection[i].PointerToRawData,  //本区段在文件中的位置
			pSection[i].Misc.VirtualSize         //本区段的大小
		);
	}
}

// 修复新内核重定位
void FixReloc(PUCHAR ZkBuf, PUCHAR OldBase)
{
	typedef struct _TYPE {
		USHORT Offset : 12;
		USHORT Type : 4;
	}TYPE, * PTYPE;

	//1 获得DOS头，继而获得NT头，再获得扩展头
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ZkBuf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + ZkBuf);
	//2 获得重定位表
	PIMAGE_DATA_DIRECTORY pRelocDir = (pNt->OptionalHeader.DataDirectory + 5);
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)
		(pRelocDir->VirtualAddress + ZkBuf);

	//2.5 得到一个老内核与默认基址间的一个差值
	ULONG uOffset = (ULONG)OldBase - pNt->OptionalHeader.ImageBase;

	//3 开始修复重定位
	while (pReloc->SizeOfBlock != 0)
	{
		ULONG uCount = (pReloc->SizeOfBlock - 8) / 2;//本0x1000内，有多少需要重定位的地方
		ULONG uBaseRva = pReloc->VirtualAddress;     //本0x1000的起始位置
		PTYPE pType = (PTYPE)(pReloc + 1);
		for (int i = 0; i < uCount; i++)
		{
			if (pType->Type == 3)
			{
				PULONG pRelocPoint = (uBaseRva + pType->Offset + ZkBuf);
				//重定位后的地址 - 新基址 = 没重定位的地址 - 默认基址
				//所以：重定位后的地址 = 新基址 - 默认基址 + 没重定位的地址
				*pRelocPoint = uOffset + *pRelocPoint;
			}
			pType++;
		}
		pReloc = (PIMAGE_BASE_RELOCATION)((ULONG)pReloc + pReloc->SizeOfBlock);
	}

}

// 修复旧SSDT表
void FixSSDT(PUCHAR pZKBuf, PUCHAR OldBase)
{

	//新内核某位置1 - 新内核基址 = 老内核某位置1 - 老内核基址；
	//新内核某位置1 = 新内核基址 - 老内核基址 + 老内核某位置1;
	DbgBreakPoint();
	LONG Offset = (ULONG)pZKBuf - (ULONG)OldBase;
	//1 得到新内核中的SSDT
	g_pNewSSDT = (PSSDTEntry)((LONG)&KeServiceDescriptorTable + Offset);

	//2 填充系统服务个数
	g_pNewSSDT->NumberOfServices = KeServiceDescriptorTable.NumberOfServices;

	//3 填充SSDT表
	g_pNewSSDT->ServiceTableBase = (ULONG*)((PUCHAR)KeServiceDescriptorTable.ServiceTableBase + Offset);
	//让所有的SSDT中保存的函数地址，都指向新内核
	for (int i = 0; i < g_pNewSSDT->NumberOfServices; i++)
	{
		g_pNewSSDT->ServiceTableBase[i] = g_pNewSSDT->ServiceTableBase[i] + Offset;
	}
	//4 填充参数表
	g_pNewSSDT->ParamTableBase = (PULONG)((PUCHAR)KeServiceDescriptorTable.ParamTableBase + Offset);
	memcpy(g_pNewSSDT->ParamTableBase,
		KeServiceDescriptorTable.ParamTableBase,
		g_pNewSSDT->NumberOfServices
	);
}

// 获取KiFastCallEntry函数
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

// hook中过滤函数
ULONG   FilterFun(ULONG SSdtBase, PULONG OldFun, ULONG Id)
{
	//如果相等，说明调用的是SSDT中的函数
	if (SSdtBase == (ULONG)KeServiceDescriptorTable.ServiceTableBase)
	{
		// 使用思路：
		// 假如进程是OD，并且函数调用是190号，就走新内核中的函数，这样通过hookOpenProcess就无法拦住OD了。
		return g_pNewSSDT->ServiceTableBase[Id];
	}
	return OldFun;

}

// inline Hook的回调函数
_declspec(naked)void MyHookFun()
{
	//eax 里面是调用号，edx里面是老函数地址，edi里面是SSDT基址
	_asm {
		pushad;
		pushfd;
		push eax;		//调用号
		push edx;		//原始函数地址
		push edi;		//SSDT基址

		call FilterFun;	// 自己的过滤函数，获取最真实函数地址  由于前面压入3个参数和pushfd,pushad ebx刚好在栈中的【esp+0x18]
		mov dword ptr ds : [esp + 0x18] , eax; // 【esp+0x18】 ebx的值，替换服务函数地址
		popfd;
		popad;			  // 恢复通用寄存器，ebx被替换成新的函数地址了
							// 执行原始hook的5个字节
		sub     esp, ecx;
		shr     ecx, 2;
		jmp     g_JmpPoint;	// 跳转回原来函数地址
	}
}

// hook 目标的5个字节
UCHAR Old_Code[5] = { 0 };

// hookKiFastCallEntry函数
void OnHookKiFastCallEntry()
{
	// KiFastCallEntry中特征值
	char buf[] = { 0x2b, 0xe1, 0xc1, 0xe9, 0x02 };
	// 获取KiFastCallEntry函数地址
	ULONG KiFastCallEntryAdd = GetKiFastCallEntry();
	// 找到hook点
	pHookPoint = SearchMemory(buf, 5, (char*)KiFastCallEntryAdd, 0x200);
	// 绕过前5个字节，应为被hook替换了
	g_JmpPoint = (ULONG)(pHookPoint + 5);
	// 备份旧的5个字节
	memcpy(Old_Code, pHookPoint, 5);
	// 关闭页保护
	OffProtected();
	// jmp xxxxxx
	// 写入跳转目标地址   目标地址-指令所在-5
	pHookPoint[0] = 0xE9;
	*(ULONG*)(&pHookPoint[1]) = (ULONG)MyHookFun - (ULONG)pHookPoint - 5;
	// 开启页保护
	OnProtected();
}


//ntoskrnl - 单处理器，不支持PAE
//ntkrnlpa - 单处理器，支持PAE

//ntkrnlmp - 多处理器，不支持PAE
//ntkrpamp - 多处理器，支持PAE
// 在windows vista 开始后 所有的内核默认安装多核处理器方式（ntkrnlmp 或者 ntkrpamp）
// 然后拷贝到System32目录下，更改和单核处理名称一样



// 内核重载 开始
void KernelReload()
{
	PUCHAR pBuf = NULL;
	PUCHAR pZKBuf = NULL;
	UNICODE_STRING KernelName;

	//1 首先把内核文件读取到内存里 (默认开启PAE)
	ReadKernelToBuf(L"\\??\\C:\\Windows\\System32\\ntkrnlpa.exe", &pBuf);

	//2 把读到内存中的内核给展开成0x1000对齐
	ZKKernel(&pZKBuf, pBuf);
	ExFreePool(pBuf);

	//3 修复新内核的重定位 ，虽然开启PAE（ntkrnlpa.exe）,但是显示的名称 ntoskrnl.exe
	RtlInitUnicodeString(&KernelName, L"ntoskrnl.exe");
	ULONG32 uBase = MyGetModuleHandle(&KernelName);

	FixReloc(pZKBuf, (PUCHAR)uBase);

	//4 修复新的SSDT表
	FixSSDT(pZKBuf, (PUCHAR)uBase);

	//5 Hook掉KiFastCallEntry，在自己的Hook函数中判断应该走新内核还是老内核
	OnHookKiFastCallEntry();

}


// 卸载内核钩子
void UnKernelHook()
{
	OffProtected();
	memcpy(pHookPoint, Old_Code, 5);
	OnProtected();
}
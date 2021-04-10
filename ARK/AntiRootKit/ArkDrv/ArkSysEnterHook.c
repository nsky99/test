#include "ArkSysEnterHook.h"

// 原始函数
ULONG g_OldKiFastCallEntry = 0;
// 要保护的进程PID
ULONG g_pid = 0;

// 过滤函数
void _declspec(naked) MyKiFastCallEntry()
{
	_asm
	{
		pushad;										// 保存寄存器
		cmp eax, 0xBE;						// 是否是ZwOpenProcess 函数
		jne CallEnd;							// 结束过滤
		mov eax, [edx + 0x14];		// 获取第四个参数 ClientId
		mov eax, [eax];						// ClientId->ProcessId
		cmp eax, g_pid;						// 判断是否要保存的进程
		jne CallEnd;
		mov[edx + 0x0c], 0;				// 将权限改为0，无法访问

	CallEnd:
		popad;										//恢复寄存器
		jmp g_OldKiFastCallEntry;	//调用原始函数
	}
}

// 安装钩子
void InstallHook()
{
	// 获取原始函数
	_asm
	{
		// 从 MSR 0x176 的位置读取原有的 KiFastCallEntry 函数进行保存
		mov ecx, 0x176;
		rdmsr;												 //将msr176寄存器的内容保存eax
		mov g_OldKiFastCallEntry, eax; //保存原始函数地址
	}

	// 设置钩子
	_asm
	{
		mov ecx, 0x176;
		mov eax, MyKiFastCallEntry; // 过滤函数
		wrmsr;											// 写入到msr0x176寄存器
	}

}

// 卸载钩子
void UnInstallHook()
{
	// 设置钩子
	_asm
	{
		mov ecx, 0x176;
		mov eax, g_OldKiFastCallEntry;  // 过滤函数
		wrmsr;							//写入到msr0x176寄存器
	}
}

// 响应Io SysHook
NTSTATUS OnHookSysEnter(PIRP pIrp)
{
	// hook的进程ID
	g_pid = *(PULONG)pIrp->AssociatedIrp.SystemBuffer;
	InstallHook();

	// ret
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;
}

// 响应Io UnSysHook
NTSTATUS OnUnHookSysEnter(PIRP pIrp)
{
	DbgBreakPoint();

  UnInstallHook();

	// ret
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;
}

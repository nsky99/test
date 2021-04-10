#pragma once

#include <ntifs.h>

// 获取指定进程的线程数量
ULONG ArkCountThreadByEProcess(PEPROCESS pEProcess);

// 响应Io计数指定进程线程数量
NTSTATUS OnCountThread(PIRP pIrp);

// 响应Io查询指定进程线程信息
NTSTATUS OnQueryThread(PIRP pIrp);

// 响应Io暂停指定线程
NTSTATUS OnSuSpendThread(PIRP pIrp);

// 响应Io恢复指定线程
NTSTATUS OnResumeThread(PIRP pIrp);

// 响应Io结束指定线程
NTSTATUS OnKillThread(PIRP pIrp);



// ntosk未导出未声明函数 - 暂停线程
typedef NTSTATUS(__fastcall* FPsSuspendThread)(PETHREAD pEThread, PULONG PreviousSuspendCount);
// ntosk未导出未声明函数 - 恢复线程
typedef NTSTATUS(__fastcall*FZwResumeThread)(HANDLE ThreadHandle,PULONG PreviousSuspendCount);
// ntosk未导出未声明函数 - 结束线程
typedef NTSTATUS(__fastcall* FZwTerminateThread)(HANDLE ThreadHandle,NTSTATUS ExitStatus);
// notskrl未导出未声明函数 - 打开线程
typedef NTSTATUS(__fastcall* FZwOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PCLIENT_ID ClientId);
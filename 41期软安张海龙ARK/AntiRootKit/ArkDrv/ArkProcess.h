#pragma once

#include <ntifs.h>

// 获取进程个数
ULONG ArkGetProcessNum();

// 响应Io统计进程个数
NTSTATUS OnCountProcess(PIRP pIrp);

// 响应Io查询进程
NTSTATUS OnQueryProcess(PIRP pIrp);

// 响应Io暂停进程
NTSTATUS OnSuspendProcess(PIRP pIrp);

// 响应Io恢复进程
NTSTATUS OnResumeProcess(PIRP pIrp);

// 响应Io结束进程
NTSTATUS OnTerminateProcess(PIRP pIrp);

// 响应Io隐藏进程
NTSTATUS OnHideProcess(PIRP pIrp);

// 结束进程
NTSTATUS ArkTerminateProcess(ULONG ulPid);


// 暂停进程
NTKERNELAPI NTSTATUS PsSuspendProcess(PEPROCESS Process);

// 恢复进程
NTKERNELAPI NTSTATUS PsResumeProcess(PEPROCESS Process);

//// 结束进程 未导出，未声明
//NTKERNELAPI NTSTATUS PsTerminateProcess(PEPROCESS Process, NTSTATUS ExitStatus);

// 获取进程名称
NTKERNELAPI UCHAR* PsGetProcessImageFileName(PEPROCESS Process);

// 获取父进程ID
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(PEPROCESS Process);
#pragma once

#include <ntifs.h>

// ��ȡָ�����̵��߳�����
ULONG ArkCountThreadByEProcess(PEPROCESS pEProcess);

// ��ӦIo����ָ�������߳�����
NTSTATUS OnCountThread(PIRP pIrp);

// ��ӦIo��ѯָ�������߳���Ϣ
NTSTATUS OnQueryThread(PIRP pIrp);

// ��ӦIo��ָͣ���߳�
NTSTATUS OnSuSpendThread(PIRP pIrp);

// ��ӦIo�ָ�ָ���߳�
NTSTATUS OnResumeThread(PIRP pIrp);

// ��ӦIo����ָ���߳�
NTSTATUS OnKillThread(PIRP pIrp);



// ntoskδ����δ�������� - ��ͣ�߳�
typedef NTSTATUS(__fastcall* FPsSuspendThread)(PETHREAD pEThread, PULONG PreviousSuspendCount);
// ntoskδ����δ�������� - �ָ��߳�
typedef NTSTATUS(__fastcall*FZwResumeThread)(HANDLE ThreadHandle,PULONG PreviousSuspendCount);
// ntoskδ����δ�������� - �����߳�
typedef NTSTATUS(__fastcall* FZwTerminateThread)(HANDLE ThreadHandle,NTSTATUS ExitStatus);
// notskrlδ����δ�������� - ���߳�
typedef NTSTATUS(__fastcall* FZwOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PCLIENT_ID ClientId);
#pragma once

#include <ntifs.h>

// ��ȡ���̸���
ULONG ArkGetProcessNum();

// ��ӦIoͳ�ƽ��̸���
NTSTATUS OnCountProcess(PIRP pIrp);

// ��ӦIo��ѯ����
NTSTATUS OnQueryProcess(PIRP pIrp);

// ��ӦIo��ͣ����
NTSTATUS OnSuspendProcess(PIRP pIrp);

// ��ӦIo�ָ�����
NTSTATUS OnResumeProcess(PIRP pIrp);

// ��ӦIo��������
NTSTATUS OnTerminateProcess(PIRP pIrp);

// ��ӦIo���ؽ���
NTSTATUS OnHideProcess(PIRP pIrp);

// ��������
NTSTATUS ArkTerminateProcess(ULONG ulPid);


// ��ͣ����
NTKERNELAPI NTSTATUS PsSuspendProcess(PEPROCESS Process);

// �ָ�����
NTKERNELAPI NTSTATUS PsResumeProcess(PEPROCESS Process);

//// �������� δ������δ����
//NTKERNELAPI NTSTATUS PsTerminateProcess(PEPROCESS Process, NTSTATUS ExitStatus);

// ��ȡ��������
NTKERNELAPI UCHAR* PsGetProcessImageFileName(PEPROCESS Process);

// ��ȡ������ID
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(PEPROCESS Process);
#pragma once


#include <ntifs.h>

// ������������
ULONG ArkCountDriverNum();

// ��ӦIo ��ѯ��������
NTSTATUS OnCountDriver(PIRP pIrp);


// ��ӦIo ��ѯ������Ϣ
NTSTATUS OnQueryDriver(PIRP pIrp);

// ��ӦIo ��������
NTSTATUS OnHideDriver(PIRP pIrp);

NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
  __in PUNICODE_STRING ObjectName,
  __in ULONG Attributes,
  __in_opt PACCESS_STATE AccessState,
  __in_opt ACCESS_MASK DesiredAccess,
  __in POBJECT_TYPE ObjectType,
  __in KPROCESSOR_MODE AccessMode,
  __inout_opt PVOID ParseContext,
  __out PVOID* Object
);

//ȫ�ֵ�һ����������
extern POBJECT_TYPE* IoDriverObjectType;

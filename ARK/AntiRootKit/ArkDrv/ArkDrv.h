#pragma once


#include <ntifs.h>

// 计算驱动个数
ULONG ArkCountDriverNum();

// 响应Io 查询驱动个数
NTSTATUS OnCountDriver(PIRP pIrp);


// 响应Io 查询驱动信息
NTSTATUS OnQueryDriver(PIRP pIrp);

// 响应Io 隐藏驱动
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

//全局的一个对象类型
extern POBJECT_TYPE* IoDriverObjectType;

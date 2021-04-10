#pragma once
#include <ntifs.h>

typedef struct _ServiceDescriptorTable
{
  //System Service Dispatch Table的基地址
  PVOID ServiceTableBase;
  //SSDT中每个服务被调用次数的计数器。这个计数器一般由sysenter 更新。
  PVOID ServiceCounterTable;
  //由 ServiceTableBase 描述的服务的数目。
  unsigned int NumberOfServices;
  //每个系统服务参数字节数表的基地址-系统服务参数表SSPT 
  PVOID ParamTableBase;
}ServiceDescriptorTable,*PServiceDescriptorTable;

// 响应Io 查看SSDT数量
NTSTATUS OnCountSSDT(PIRP pIrp);

// 响应Io 查看SSDT信息
NTSTATUS OnQuerySSDT(PIRP pIrp);


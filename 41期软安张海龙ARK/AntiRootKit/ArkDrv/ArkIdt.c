#include "ArkIdt.h"
#include "ArkCommon.h"
#define MAKE_LONG(a,b) ((a) + (b<<16))

ULONG ArkCountIdt()
{
  // IDT table
  IDTR stcIDT = { 0 };
  __asm sidt stcIDT;
  return (ULONG)(stcIDT.uLimit / 0x8);
}

NTSTATUS OnCountIdt(PIRP pIrp)
{ 
  // 获取idt数量
  ULONG ulIdtNum = ArkCountIdt();

  // 设置返回缓冲区
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulIdtNum;
  
  
  // 请求完成
  pIrp->IoStatus.Information = sizeof(ULONG);        // 返回内存大小
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // 返回处理状态

  return STATUS_SUCCESS;
}

NTSTATUS OnQueryIdt(PIRP pIrp)
{ 
  // 1. 获取数量
  ULONG ulIdtNum = ArkCountIdt();

  // 2. 获取idt base
  IDTR stcIDT = { 0 };
  __asm sidt stcIDT;
  ULONG ulMemSize = sizeof(IDT_INFO) * ulIdtNum;
  PIDT_INFO pIdtEntry = (PIDT_INFO)MAKE_LONG(stcIDT.uLowBase, stcIDT.uHighBase);

  // 3. 拷贝到缓冲区返回
  if (pIdtEntry)
  {
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pIdtEntry, ulMemSize);
  }


  // 4. 设置返回状态
  pIrp->IoStatus.Information = ulMemSize;        // 返回内存大小
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // 返回处理状态

  return STATUS_SUCCESS;
}

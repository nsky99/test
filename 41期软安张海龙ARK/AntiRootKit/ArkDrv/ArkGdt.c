#include "ArkGdt.h"
#include "ArkCommon.h"
#include <ntifs.h>


ULONG ArkCountGdt()
{
  GDTR gdt = { 0 };
  _asm sgdt gdt;

  return gdt.uLimit / 8;
}


NTSTATUS OnCountGDT(PIRP pIrp)
{
  // 统计GDT数量
  ULONG ulNum = ArkCountGdt();

  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulNum;
  // 返回R3 
  pIrp->IoStatus.Information = sizeof(ULONG);
  pIrp->IoStatus.Status = STATUS_SUCCESS;

  return STATUS_SUCCESS;
}

NTSTATUS OnQueryGDT(PIRP pIrp)
{
  // 统计GDT数量
  ULONG ulNum = ArkCountGdt();

  // 拷贝内存给R3
  ULONG ulMemSize = ulNum * sizeof(GDT_INFO);

  // 获取GDT信息
  GDTR gdt = { 0 };
  PGDT_INFO pGdtInfo = NULL;
  _asm sgdt gdt;
  pGdtInfo = (PGDT_INFO)((ULONG)(gdt.uHighBase << 16) + gdt.uLowBase);
  if (pGdtInfo)
  {
    KdPrint(("%p\n", pGdtInfo));
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pGdtInfo, ulMemSize);
  }


  // 返回R3 
  pIrp->IoStatus.Information = ulMemSize;
  pIrp->IoStatus.Status = STATUS_SUCCESS;

  return STATUS_SUCCESS;
}

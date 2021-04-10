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
  // ͳ��GDT����
  ULONG ulNum = ArkCountGdt();

  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulNum;
  // ����R3 
  pIrp->IoStatus.Information = sizeof(ULONG);
  pIrp->IoStatus.Status = STATUS_SUCCESS;

  return STATUS_SUCCESS;
}

NTSTATUS OnQueryGDT(PIRP pIrp)
{
  // ͳ��GDT����
  ULONG ulNum = ArkCountGdt();

  // �����ڴ��R3
  ULONG ulMemSize = ulNum * sizeof(GDT_INFO);

  // ��ȡGDT��Ϣ
  GDTR gdt = { 0 };
  PGDT_INFO pGdtInfo = NULL;
  _asm sgdt gdt;
  pGdtInfo = (PGDT_INFO)((ULONG)(gdt.uHighBase << 16) + gdt.uLowBase);
  if (pGdtInfo)
  {
    KdPrint(("%p\n", pGdtInfo));
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pGdtInfo, ulMemSize);
  }


  // ����R3 
  pIrp->IoStatus.Information = ulMemSize;
  pIrp->IoStatus.Status = STATUS_SUCCESS;

  return STATUS_SUCCESS;
}

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
  // ��ȡidt����
  ULONG ulIdtNum = ArkCountIdt();

  // ���÷��ػ�����
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulIdtNum;
  
  
  // �������
  pIrp->IoStatus.Information = sizeof(ULONG);        // �����ڴ��С
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // ���ش���״̬

  return STATUS_SUCCESS;
}

NTSTATUS OnQueryIdt(PIRP pIrp)
{ 
  // 1. ��ȡ����
  ULONG ulIdtNum = ArkCountIdt();

  // 2. ��ȡidt base
  IDTR stcIDT = { 0 };
  __asm sidt stcIDT;
  ULONG ulMemSize = sizeof(IDT_INFO) * ulIdtNum;
  PIDT_INFO pIdtEntry = (PIDT_INFO)MAKE_LONG(stcIDT.uLowBase, stcIDT.uHighBase);

  // 3. ����������������
  if (pIdtEntry)
  {
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pIdtEntry, ulMemSize);
  }


  // 4. ���÷���״̬
  pIrp->IoStatus.Information = ulMemSize;        // �����ڴ��С
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // ���ش���״̬

  return STATUS_SUCCESS;
}

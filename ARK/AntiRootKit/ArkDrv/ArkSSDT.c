#include "ArkSSDT.h"
#include "ArkCommon.h"


//����ϵͳ������������SSDT��ָ��
extern  PServiceDescriptorTable  KeServiceDescriptorTable;


// ��ӦIo �鿴SSDT����
NTSTATUS OnCountSSDT(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;

  ULONG ulNum = (ULONG)KeServiceDescriptorTable->NumberOfServices;
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulNum;
  // ����
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = sizeof(ULONG);
  return status;
}

// ��ӦIo �鿴SSDT��Ϣ
NTSTATUS OnQuerySSDT(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  ULONG ulNum = (ULONG)KeServiceDescriptorTable->NumberOfServices;


  // �����ڴ�
  ULONG ulMemSize = sizeof(SSDT_INFO) * ulNum;
  PSSDT_INFO pSSDTInfo = (PSSDT_INFO)ExAllocatePoolWithTag(PagedPool, ulMemSize, 'ssdt');
  do
  {
    if (!pSSDTInfo)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    RtlZeroMemory(pSSDTInfo, ulMemSize);
    PULONG pFunAddr = (PULONG)KeServiceDescriptorTable->ServiceTableBase;
    PUINT8 pArgsNum = (PUINT8)KeServiceDescriptorTable->ParamTableBase;

    for (ULONG i = 0; i < ulNum; i++)
    {
      (pSSDTInfo + i)->ulServerNum = i;
      (pSSDTInfo + i)->ulFunAddr = pFunAddr[i];
      (pSSDTInfo + i)->ulCountArgs = pArgsNum[i] / 4;
    }

    // �����ڴ浽systembuf
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pSSDTInfo, ulMemSize);

    // �ͷ��ڴ�
    if (pSSDTInfo)
    {
      ExFreePoolWithTag(pSSDTInfo, 'ssdt');
      pSSDTInfo = NULL;
    }

  } while (FALSE);
  

  // ����
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = ulMemSize;
  return status;
}

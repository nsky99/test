#include "ArkModule.h"
#include "ArkCommon.h"

#include <ntifs.h>


// ����ģ������
ULONG ArkCountModuleByEProcess(PEPROCESS pEProcess)
{

  ULONG ulNum = 0;
  // ��ȡָ������PEB
  ULONG Peb = *(PULONG)((ULONG)pEProcess + 0x1a8);
  // ��ǰ�߳��л����µĽ��̶���
  KeAttachProcess(pEProcess);


  // ��ȡLDR��
  LIST_ENTRY pLdrHead = ((PEB_LDR_DATA*)(*(PULONG)(Peb + 0xC)))->InLoadOrderModuleList;

  // ��ȡ����ʼ
  PLIST_ENTRY pTmp = pLdrHead.Flink;
  PLIST_ENTRY pNext = pLdrHead.Flink;


  do
  {
    // ��ȡģ����Ϣ
    LDR_DATA_TABLE_ENTRY pLdrTable = *(PLDR_DATA_TABLE_ENTRY)pNext;
    if (pLdrTable.DllBase)
    {
      // KdPrint(("Name:%ws\n", pLdrTable.BaseDllName.Buffer));
      // ͳ��
      ulNum++;
    }
    pNext = pNext->Flink;
  } while (pTmp != pNext);
  
  // ���߳�ת��ԭ���Ľ���
  KeDetachProcess();
  // ���ü���--
  ObDereferenceObject(pEProcess);
  return ulNum;
}

// ��ӦIoͳ��ָ������ģ������
NTSTATUS OnCountModule(PIRP pIrp)
{
  // ȡ��EPROCESS
  PEPROCESS pEProcess = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;

  // ��ȡָ�����̵��߳�����
  ULONG ulModuleNum = ArkCountModuleByEProcess(pEProcess);

  // R0 - out
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulModuleNum;
  // KdPrint(("�߳�ģ������: %u\n", ulThreadNum));
  // �������
  pIrp->IoStatus.Information = sizeof(ULONG);        // �����ڴ��С
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // ���ش���״̬

  return STATUS_SUCCESS;
}

// ��ӦIo��ѯָ������ģ����Ϣ
NTSTATUS OnQueryModule(PIRP pIrp)
{
  PEPROCESS pEProcess = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
  
  do
  {
    // ����ģ������
    ULONG ulModuleNum = ArkCountModuleByEProcess(pEProcess);

    // Ϊ�洢��Ϣ�����ڴ�
    ULONG ulAllocateMemSize = ulModuleNum * sizeof(MODULE_INFO);
    PMODULE_INFO pModuleInfo = ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.ark');
    if (!pModuleInfo)
    {
      break;
    }

    // ��ȡָ������PEB
    ULONG Peb = *(PULONG)((ULONG)pEProcess + 0x1a8);
    // ��ǰ�߳��л����µĽ��̶���
    KeAttachProcess(pEProcess);


    // ��ȡLDR��
    LIST_ENTRY pLdrHead = ((PEB_LDR_DATA*)(*(PULONG)(Peb + 0xC)))->InLoadOrderModuleList;

    // ��ȡ����ʼ
    PLIST_ENTRY pTmp = pLdrHead.Flink;
    PLIST_ENTRY pNext = pLdrHead.Flink;

    ULONG ulIndex = 0;
    do
    {
      // ��ȡģ����Ϣ
      LDR_DATA_TABLE_ENTRY pLdrTable = *(PLDR_DATA_TABLE_ENTRY)pNext;
      if (pLdrTable.DllBase)
      {
        // KdPrint(("Name:%ws\n", pLdrTable.BaseDllName.Buffer));
        // ��ѯģ����Ϣ
        RtlCopyMemory((pModuleInfo + ulIndex)->szName, 
          pLdrTable.BaseDllName.Buffer, pLdrTable.BaseDllName.MaximumLength + 1);
        (pModuleInfo + ulIndex)->ulBase = (ULONG)pLdrTable.DllBase;
        (pModuleInfo + ulIndex)->ulSize = (ULONG)pLdrTable.SizeOfImage;
        RtlCopyMemory((pModuleInfo + ulIndex)->szPath, 
          pLdrTable.FullDllName.Buffer, pLdrTable.FullDllName.MaximumLength + 1);


        // ��ѯ��һ��
        ulIndex++;
      }
      pNext = pNext->Flink;
    } while (pTmp != pNext);

    // ���߳�ת��ԭ���Ľ���
    KeDetachProcess();
    // ���ü���--
    ObDereferenceObject(pEProcess);

    // �����ڴ浽irp������
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pModuleInfo, ulAllocateMemSize);

    // ���÷�����Ϣ
    pIrp->IoStatus.Information = ulAllocateMemSize;
    pIrp->IoStatus.Status = STATUS_SUCCESS;

    // �ͷ��ڴ�
    if (pModuleInfo)
    {
      ExFreePoolWithTag(pModuleInfo, '.ark');
      pModuleInfo = NULL;
    }
  } while (FALSE);

  return STATUS_SUCCESS;
}

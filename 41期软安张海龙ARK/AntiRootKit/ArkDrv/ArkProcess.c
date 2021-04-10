#include "ArkProcess.h"
#include "ArkCommon.h"

// ��ȡ��������
ULONG ArkGetProcessNum()
{
  NTSTATUS status = STATUS_SUCCESS;
  // ���̿�
  PEPROCESS pProc = NULL;
  // ��ȡ���̸���
  ULONG uProcessNum = 0;       // ���̸���
  for (ULONG i = 90000; i > 0; i -= 4)
  {
    // ���Ի�ȡ����EPROCESS
    status = PsLookupProcessByProcessId((HANDLE)i, &pProc);
    if (!NT_SUCCESS(status))
    {
      continue;
    }

    uProcessNum++;
    // ʹ���˶���ͻ����ü���+1,
    ObDereferenceObject(pProc);
  }
  return uProcessNum;
}

// ��������
NTSTATUS ArkTerminateProcess(ULONG ulPid)
{
  HANDLE hProcess = NULL;
  CLIENT_ID ClientId = { 0 };
  OBJECT_ATTRIBUTES objAttr = { sizeof(OBJECT_ATTRIBUTES) };
  ClientId.UniqueProcess = (HANDLE)ulPid;
  ClientId.UniqueThread = 0;

  // �򿪽��̣���������Ч�����������
  NTSTATUS status = ZwOpenProcess(&hProcess, 1, &objAttr, &ClientId);
  if (hProcess)
  {
    ZwTerminateProcess(hProcess, 0);
    ZwClose(hProcess);
  }
  return status;
}

// ��Ӧͳ�ƽ���Io
NTSTATUS OnCountProcess(PIRP pIrp)
{
  // ��ȡ���̸���
  ULONG uProcessNum = ArkGetProcessNum();

  // ���ؽ��̸���
  RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, &uProcessNum, sizeof(ULONG));

  // �������
  pIrp->IoStatus.Information = sizeof(ULONG);        // �����ڴ��С
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // ���ش���״̬

  return STATUS_SUCCESS;
}

// ��Ӧ��ѯ����Io
NTSTATUS OnQueryProcess(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  // ���̿�
  PEPROCESS pProc = NULL;

  // ��ȡ���̸���
  ULONG ulProcessNum = ArkGetProcessNum();
  
  // Ϊ�洢��Ϣ�����ڴ�
  ULONG ulAllocateMemSize = sizeof(PROCESS_INFO) * ulProcessNum;
  PPROCESS_INFO pProcInfo = ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.ark');
  if (NULL == pProcInfo)
  {
    return STATUS_SUCCESS;// �����ڴ�ʧ��
  }
  RtlFillMemory(pProcInfo, ulAllocateMemSize, 0);

  // ��ѯ���洢������Ϣ
  ULONG ulIndex = 0;
  for (ULONG i = 90000; i > 0; i -= 4)
  {
    // ���Ի�ȡ����EPROCESS
    status = PsLookupProcessByProcessId((HANDLE)i, &pProc);
    if (!NT_SUCCESS(status))
    {
      continue;
    }

    // name
    CANSI_STRING aZProcName = { 0 };
    UNICODE_STRING wZProcName = { 0 };
    RtlInitAnsiString(&aZProcName, (PCSZ)PsGetProcessImageFileName(pProc));
    RtlAnsiStringToUnicodeString(&wZProcName, &aZProcName, TRUE);

    RtlCopyMemory((pProcInfo + ulIndex)->szName, wZProcName.Buffer, wZProcName.MaximumLength);

    // pid
    (pProcInfo + ulIndex)->uPid = (ULONG)PsGetProcessId(pProc);

    // Ppid
    (pProcInfo + ulIndex)->uParentPid = (ULONG)PsGetProcessInheritedFromUniqueProcessId(pProc);

    // ����·��


    // EPROCESS
    (pProcInfo + ulIndex)->uEprocess = (ULONG)pProc;

    // ����������, �±�++
    ulIndex++;
    // ʹ���˶���ͻ����ü���+1,
    ObDereferenceObject(pProc);
  }


  // ����ѯ�õ�R0���ݿ�����R3��������
  RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pProcInfo, ulAllocateMemSize);

  // �ͷ��ڴ�
  if (pProcInfo)
  {
    ExFreePoolWithTag(pProcInfo, '.ark');
    pProcInfo = NULL;
  }

  // ����������÷�����Ϣ
  pIrp->IoStatus.Information = ulAllocateMemSize;
  pIrp->IoStatus.Status = STATUS_SUCCESS;


  return STATUS_SUCCESS;
}

// ��Ӧ��ͣ����Io
NTSTATUS OnSuspendProcess(PIRP pIrp)
{
  PEPROCESS pProc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
  
  // ��ͣ����
  NTSTATUS status = PsSuspendProcess(pProc);

  // ����������÷�����Ϣ
  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = status;
  return status;
}

// ��Ӧ�ָ�����Io
NTSTATUS OnResumeProcess(PIRP pIrp)
{
  PEPROCESS pProc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = PsResumeProcess(pProc);

  // ����������÷�����Ϣ
  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = status;
  return status;
}

// ��Ӧ��������Io
NTSTATUS OnTerminateProcess(PIRP pIrp)
{
  ULONG ulPid = *(PULONG)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = ArkTerminateProcess(ulPid);

  // ����������÷�����Ϣ
  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = status;
  return status;
}

// ��Ӧ���ؽ���Io
NTSTATUS OnHideProcess(PIRP pIrp)
{
  // Ҫ���صĽ���EPROCESS
  PEPROCESS pHideProc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
  
  // ��ȡ��ǰ����EPROCESS
  PEPROCESS pCurProc = PsGetCurrentProcess();
  
  // ��ȡ���̶����ڵĵ�ǰ���������pCurProc->ActiveProcessLinks 
  LIST_ENTRY* pNowList = (LIST_ENTRY*)((ULONG)pCurProc + 0xb8);

  // ��ʱ����
  LIST_ENTRY* pTempList = pNowList;
  // ��������
  while (pNowList != pTempList->Flink)
  {
    // ��ǰ��Խ��̶���ƫ����0xb8����Ҫ��ȥ
    // �Ա��Ƿ�ΪĿ�����			
    // EPROCESS���
    if (pHideProc == (PEPROCESS)((ULONG)pTempList - 0xb8))
    {
      //���ҵ��Ľ��̴�������ɾ��
      (pTempList->Blink)->Flink = pTempList->Flink;
      (pTempList->Flink)->Blink = pTempList->Blink;

      pTempList->Flink = pTempList->Blink = NULL;
      return STATUS_SUCCESS;
      break;
    }
    pTempList = pTempList->Flink;
  }
  return 1; // û���ҵ�
}


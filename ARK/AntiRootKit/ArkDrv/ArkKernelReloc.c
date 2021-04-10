#include "ArkKernelReloc.h"
#include "FileOp.h"
#include "ArkCommon.h"
#include "ArkDrvEntry.h"
#include <ntifs.h>
#include <ntimage.h>
#include "ArkSSDT.h"

//����ϵͳ������������SSDT��ָ��
extern  PServiceDescriptorTable  KeServiceDescriptorTable;
PServiceDescriptorTable g_NewSSDT;

// �����ں��ļ����ڴ�
PVOID ArkLoadKernelFileToMemory()
{
  // �ں��ļ�·��
  UNICODE_STRING ustrPath = { 0 };
  RtlInitUnicodeString(&ustrPath, L"\\??\\C:\\Windows\\System32\\ntkrnlpa.exe");

  // 1. ��ֻ���ķ�ʽ���ļ�
  HANDLE hKernelFile = CreateFile(&ustrPath, GENERIC_READ, TRUE);

  // 2. ��ȡ�ļ���С
  ULONG ulFileSize = GetFileSize(hKernelFile);

  // 3. �����ڴ�
  PVOID pKernelMem = ExAllocatePoolWithTag(NonPagedPool, ulFileSize, 'reke');

  // 4. ��ȡ�ļ�����д�뻺����
  NTSTATUS status = ReadFile(hKernelFile, pKernelMem, ulFileSize, 0);
  if (!NT_SUCCESS(status))
  {
    return 0;
  }
  return pKernelMem;
}

// ���ڴ�������Ƚ���ӳ���ļ�
PVOID ArkMappingKernel(PUCHAR pFile)
{
  // 1. ��ȡ�ں��ļ�����չͷ
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFile;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pFile);

  // 2. ͨ����չͷ�ҵ� SizeOfImage( ӳ��װ���ڴ���ܴ�С ) ������ռ�
  ULONG ulMapedSize = pNt->OptionalHeader.SizeOfImage;
  PUCHAR pMapedBuffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, ulMapedSize, 'reke');
  if (!pMapedBuffer)
  {
    return 0;
  }

  // 3. �Ƚ� PE ͷ�������ڴ��� - PEͷ����Ҫչ����ֱ�ӿ�������
  RtlCopyMemory(pMapedBuffer, pFile, pNt->OptionalHeader.SizeOfHeaders);

  // 4. ��ȡÿ�����β�չ��������ָ����λ�� - IMAGE_FIRST_SECTION ����ntͷ��ȡ��һ������ͷ�ĵ�ַ
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  for (UCHAR i = 0; i < pNt->FileHeader.NumberOfSections; i++)
  {
    RtlCopyMemory(
      pMapedBuffer + pSection[i].VirtualAddress, // �ڴ���ʼ��ַ + �ڴ�RVA
      pFile + pSection[i].PointerToRawData,      // �ļ���ʼ��ַ + �ļ�FOA
      pSection[i].Misc.VirtualSize               // ʵ�ʴ�С
    );
  }

  // 5. �ͷ�֮ǰֱ�Ӷ�ȡ�ں˵��ڴ�
  ExFreePoolWithTag(pFile, 'reke');
  pFile = NULL;

  return pMapedBuffer;
}

// �޸����غ���ں��ض�λ
VOID ArkFixKernelReloc(PUCHAR pFile)
{
  // 1. ��ȡ�ɵ��ں˻�ַ
  PVOID OldKernelBase = GetNtImageBase(g_pArkDrvObj);

  // 2. ��ȡPE��Ϣ dosͷ��ntͷ���ض�λ����Ŀ¼���ض�λ��
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFile;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pFile);
  PIMAGE_DATA_DIRECTORY pRelocDir = &pNt->OptionalHeader.DataDirectory[5]; // ���������ض�λ
  PIMAGE_BASE_RELOCATION pRelocTable = (PIMAGE_BASE_RELOCATION)(pRelocDir->VirtualAddress + pFile);
  
  // 4. �����ض�λ���޸��ض�λ
  while (!pRelocTable->SizeOfBlock)
  {
    ULONG ulItemCount = (pRelocTable->SizeOfBlock - 8) / 2;
    PTYPEOFFSET TypeOffset = (PTYPEOFFSET)(pRelocTable + 1);
    // ������ǰ��ҳ�ڵ������ض�λ��
    for (ULONG i = 0; i < ulItemCount; i++)
    {
      if (TypeOffset->Type == 3)
      {
        // ��Ҫ�ض�λ�������ڵ�λ�� = ����base + ��ҳbase + ҳ��offse
        PULONG pRelocPoint = (PULONG)(pFile +
          pRelocTable->VirtualAddress + TypeOffset->Offset);

        // �ض�λ��ĵ�ַ = �»�ַ - Ĭ�ϻ�ַ + û�ض�λ�ĵ�ַ
        *pRelocPoint = *pRelocPoint - (ULONG)OldKernelBase + (ULONG)pFile;
      }
      TypeOffset++;
    }
    pRelocTable = (PIMAGE_BASE_RELOCATION)((ULONG)pRelocTable + pRelocTable->SizeOfBlock);
  }
}

// �����µ�SSDT���޸�����䣩
VOID ArkCreateAndFixNewSSDT(PUCHAR pBase)
{
  // newSSDT = OldSSDT - oldKernelBase + newKernelBase
  KdBreakPoint();


  // 1. ��ȡold�ں˻�ַ �� new�ں˻�ַ
  PVOID pOldKerBase = GetNtImageBase(g_pArkDrvObj);
  PVOID pNewKerBase = (PVOID)pBase;

  // 2.offset = newKernelBase - oldKernelBase�������ҵ����ں˵� SSDT
  LONG ulOffset = (ULONG)pNewKerBase - (ULONG)pOldKerBase;
  KdPrint(("Offset: %x\n", ulOffset));
  g_NewSSDT = (PServiceDescriptorTable)((ULONG)&KeServiceDescriptorTable + ulOffset);

  // 3. ϵͳ�������
  g_NewSSDT->NumberOfServices = KeServiceDescriptorTable->NumberOfServices;

  // 4. ϵͳ������ַ����亯��
  g_NewSSDT->ServiceTableBase = (PVOID)((ULONG)KeServiceDescriptorTable->ServiceTableBase + ulOffset);
	for (ULONG i = 0; i < g_NewSSDT->NumberOfServices; i++)
	{
    (&(ULONG)g_NewSSDT->ServiceTableBase)[i] = (&(ULONG)g_NewSSDT->ServiceTableBase)[i] + ulOffset;
	}

  // 5. ���ϵͳ����������ַ�����ֵ
  g_NewSSDT->ParamTableBase = (PVOID)((ULONG)KeServiceDescriptorTable->ParamTableBase + ulOffset);
  RtlCopyMemory(
    g_NewSSDT->ParamTableBase,
    KeServiceDescriptorTable->ParamTableBase,
    g_NewSSDT->NumberOfServices
  );

}

// �ں�����
VOID ArkKernelReloc()
{
  // 1. ��ȡ�ں˵��ڴ�
  PVOID pKernelMem = ArkLoadKernelFileToMemory();

  // 2. ���ڴ�������Ƚ���ӳ���ļ�
  pKernelMem = ArkMappingKernel(pKernelMem);

  // 3. �޸����ص��ں��ض�λ
  ArkFixKernelReloc(pKernelMem);

  // 4. �����µ�SSDT���޸�����䣩
  ArkCreateAndFixNewSSDT(pKernelMem);
}
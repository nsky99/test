#include "ArkKernelReloc.h"
#include "FileOp.h"
#include "ArkCommon.h"
#include "ArkDrvEntry.h"
#include <ntifs.h>
#include <ntimage.h>
#include "ArkSSDT.h"

//导出系统服务描述符表SSDT的指针
extern  PServiceDescriptorTable  KeServiceDescriptorTable;
PServiceDescriptorTable g_NewSSDT;

// 加载内核文件到内存
PVOID ArkLoadKernelFileToMemory()
{
  // 内核文件路径
  UNICODE_STRING ustrPath = { 0 };
  RtlInitUnicodeString(&ustrPath, L"\\??\\C:\\Windows\\System32\\ntkrnlpa.exe");

  // 1. 以只读的方式打开文件
  HANDLE hKernelFile = CreateFile(&ustrPath, GENERIC_READ, TRUE);

  // 2. 获取文件大小
  ULONG ulFileSize = GetFileSize(hKernelFile);

  // 3. 申请内存
  PVOID pKernelMem = ExAllocatePoolWithTag(NonPagedPool, ulFileSize, 'reke');

  // 4. 读取文件内容写入缓冲区
  NTSTATUS status = ReadFile(hKernelFile, pKernelMem, ulFileSize, 0);
  if (!NT_SUCCESS(status))
  {
    return 0;
  }
  return pKernelMem;
}

// 按内存对其粒度进行映射文件
PVOID ArkMappingKernel(PUCHAR pFile)
{
  // 1. 获取内核文件的扩展头
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFile;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pFile);

  // 2. 通过扩展头找到 SizeOfImage( 映像装入内存的总大小 ) 并申请空间
  ULONG ulMapedSize = pNt->OptionalHeader.SizeOfImage;
  PUCHAR pMapedBuffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, ulMapedSize, 'reke');
  if (!pMapedBuffer)
  {
    return 0;
  }

  // 3. 先将 PE 头拷贝到内存中 - PE头不需要展开，直接拷贝即可
  RtlCopyMemory(pMapedBuffer, pFile, pNt->OptionalHeader.SizeOfHeaders);

  // 4. 获取每个区段并展开拷贝到指定的位置 - IMAGE_FIRST_SECTION 根据nt头获取第一个区段头的地址
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
  for (UCHAR i = 0; i < pNt->FileHeader.NumberOfSections; i++)
  {
    RtlCopyMemory(
      pMapedBuffer + pSection[i].VirtualAddress, // 内存起始地址 + 内存RVA
      pFile + pSection[i].PointerToRawData,      // 文件起始地址 + 文件FOA
      pSection[i].Misc.VirtualSize               // 实际大小
    );
  }

  // 5. 释放之前直接读取内核的内存
  ExFreePoolWithTag(pFile, 'reke');
  pFile = NULL;

  return pMapedBuffer;
}

// 修复重载后的内核重定位
VOID ArkFixKernelReloc(PUCHAR pFile)
{
  // 1. 获取旧的内核基址
  PVOID OldKernelBase = GetNtImageBase(g_pArkDrvObj);

  // 2. 获取PE信息 dos头、nt头、重定位数据目录、重定位表
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFile;
  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pFile);
  PIMAGE_DATA_DIRECTORY pRelocDir = &pNt->OptionalHeader.DataDirectory[5]; // 第五项是重定位
  PIMAGE_BASE_RELOCATION pRelocTable = (PIMAGE_BASE_RELOCATION)(pRelocDir->VirtualAddress + pFile);
  
  // 4. 遍历重定位表，修复重定位
  while (!pRelocTable->SizeOfBlock)
  {
    ULONG ulItemCount = (pRelocTable->SizeOfBlock - 8) / 2;
    PTYPEOFFSET TypeOffset = (PTYPEOFFSET)(pRelocTable + 1);
    // 遍历当前分页内的所有重定位项
    for (ULONG i = 0; i < ulItemCount; i++)
    {
      if (TypeOffset->Type == 3)
      {
        // 需要重定位的项所在的位置 = 加载base + 分页base + 页内offse
        PULONG pRelocPoint = (PULONG)(pFile +
          pRelocTable->VirtualAddress + TypeOffset->Offset);

        // 重定位后的地址 = 新基址 - 默认基址 + 没重定位的地址
        *pRelocPoint = *pRelocPoint - (ULONG)OldKernelBase + (ULONG)pFile;
      }
      TypeOffset++;
    }
    pRelocTable = (PIMAGE_BASE_RELOCATION)((ULONG)pRelocTable + pRelocTable->SizeOfBlock);
  }
}

// 创建新的SSDT并修复（填充）
VOID ArkCreateAndFixNewSSDT(PUCHAR pBase)
{
  // newSSDT = OldSSDT - oldKernelBase + newKernelBase
  KdBreakPoint();


  // 1. 获取old内核基址 和 new内核基址
  PVOID pOldKerBase = GetNtImageBase(g_pArkDrvObj);
  PVOID pNewKerBase = (PVOID)pBase;

  // 2.offset = newKernelBase - oldKernelBase，用于找到新内核的 SSDT
  LONG ulOffset = (ULONG)pNewKerBase - (ULONG)pOldKerBase;
  KdPrint(("Offset: %x\n", ulOffset));
  g_NewSSDT = (PServiceDescriptorTable)((ULONG)&KeServiceDescriptorTable + ulOffset);

  // 3. 系统服务个数
  g_NewSSDT->NumberOfServices = KeServiceDescriptorTable->NumberOfServices;

  // 4. 系统服务表基址并填充函数
  g_NewSSDT->ServiceTableBase = (PVOID)((ULONG)KeServiceDescriptorTable->ServiceTableBase + ulOffset);
	for (ULONG i = 0; i < g_NewSSDT->NumberOfServices; i++)
	{
    (&(ULONG)g_NewSSDT->ServiceTableBase)[i] = (&(ULONG)g_NewSSDT->ServiceTableBase)[i] + ulOffset;
	}

  // 5. 填充系统服务参数表基址并填充值
  g_NewSSDT->ParamTableBase = (PVOID)((ULONG)KeServiceDescriptorTable->ParamTableBase + ulOffset);
  RtlCopyMemory(
    g_NewSSDT->ParamTableBase,
    KeServiceDescriptorTable->ParamTableBase,
    g_NewSSDT->NumberOfServices
  );

}

// 内核重载
VOID ArkKernelReloc()
{
  // 1. 读取内核到内存
  PVOID pKernelMem = ArkLoadKernelFileToMemory();

  // 2. 按内存对其粒度进行映射文件
  pKernelMem = ArkMappingKernel(pKernelMem);

  // 3. 修复重载的内核重定位
  ArkFixKernelReloc(pKernelMem);

  // 4. 创建新的SSDT并修复（填充）
  ArkCreateAndFixNewSSDT(pKernelMem);
}
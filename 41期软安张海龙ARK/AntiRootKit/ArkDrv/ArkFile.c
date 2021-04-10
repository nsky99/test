#include "ArkFile.h"
#include <ntifs.h>
#include "ArkCommon.h"

// 根据文件句柄查找第一个文件
BOOLEAN ArkFindFirstFile(
  IN HANDLE hFile, 
  IN ULONG ulLen, 
  OUT PFILE_BOTH_DIR_INFORMATION pDir, 
  IN ULONG uFirstLen,
  OUT PFILE_BOTH_DIR_INFORMATION pFirstDir)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  IO_STATUS_BLOCK statusBlock = { 0 };
  // 1. 获取第一个文件信息
  status = ZwQueryDirectoryFile(
    hFile, NULL, NULL, NULL,
    &statusBlock,// 接收函数操作结构
    pFirstDir,    // 文件信息
    uFirstLen,    // 文件信息的时间长度
    FileBothDirectoryInformation,// 查询模式
    TRUE,         // 是否返回一条起始信息
    NULL,         // 文件句柄指向的文件（一般为NULL）
    FALSE         // 是否从目录开始的第一项扫描
  );

  // 2. 若成功，则获取文件列表
  if (!NT_SUCCESS(status))
  {
    return FALSE;
  }
  status = ZwQueryDirectoryFile(
    hFile, NULL, NULL, NULL,
    &statusBlock,// 接收函数操作结构
    pDir,        // 文件信息
    ulLen,       // 文件信息的时间长度
    FileBothDirectoryInformation,// 查询模式
    FALSE,       // 是否返回一条起始信息
    NULL,        // 文件句柄指向的文件（一般为NULL）
    FALSE        // 是否从目录开始的第一项扫描
  );
  return NT_SUCCESS(status);
}


// 根据文件句柄查找下一个文件
BOOLEAN ArkFindNextFile(
  IN PFILE_BOTH_DIR_INFORMATION pDirList, 
  OUT PFILE_BOTH_DIR_INFORMATION pDirInfo, 
  IN OUT LONG* Loc)
{
  // 如果有下一项，则移动指针指向下一项
  PFILE_BOTH_DIR_INFORMATION pDir = (PFILE_BOTH_DIR_INFORMATION)((PCHAR)pDirList + *Loc);
  LONG structLen = 0;
  if (pDir->FileName[0] != 0 && pDir->FileNameLength < 0xFF)
  {
    structLen = sizeof(FILE_BOTH_DIR_INFORMATION);
    RtlCopyMemory(pDirInfo, pDir, structLen + pDir->FileNameLength);
    *Loc = *Loc + pDir->NextEntryOffset;
    if (pDir->NextEntryOffset == 0)
    {
      *Loc = *Loc + structLen + pDir->FileNameLength;
    }
    return TRUE;
  }
  return FALSE;
}

// 根据文件路径符号链接创建文件
HANDLE ArkCreateFileF(
  IN PUNICODE_STRING pstrFile,// 文件路径符号链接
  IN BOOLEAN bIsDir            // 是否为文件夹
)
{
  HANDLE hFile = NULL;
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  IO_STATUS_BLOCK statusBlock = { 0 };
  // 文件共享属性
  ULONG ulShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
  ULONG ulCreateOpt = FILE_SYNCHRONOUS_IO_NONALERT;
  
  // 1. 初始化OBJ_ATT内容
  OBJECT_ATTRIBUTES objAttr = { 0 };
  ULONG ulAttrs = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
  InitializeObjectAttributes(
    &objAttr, // 返回初始化完毕的结构体
    pstrFile, // 文件对象名称 
    ulAttrs,  // 对象属性
    NULL, NULL);

  // 2. 创建文件对象
  ulCreateOpt |= bIsDir ?
    FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
  status = ZwCreateFile(
    &hFile,             // 返回文件句柄
    GENERIC_ALL,        // 文件操作描述
    &objAttr,           // OBJ_ATTR
    &statusBlock,       // 接收函数操作结构
    0,                  // 初始化文件大小
    FILE_ATTRIBUTE_NORMAL,// 新建文件属性
    ulShareAccess,        // 文件共享方式
    FILE_OPEN_IF,         // 文件存在打开，不存在则创建
    ulCreateOpt,          // 打开操作的附加标志位
    NULL,// 扩展属性区
    0// 扩展属性区长度
  );
  KdPrint(("%08X", (ULONG)status));
  if (!NT_SUCCESS(status))
  {
    return (HANDLE)-1;
  }

  return hFile;
}


NTSTATUS ArkEnumFile(PWCHAR szPath, PFILE_INFO pFileInfo)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  PFILE_BOTH_DIR_INFORMATION pFileTmp = NULL;
  PFILE_BOTH_DIR_INFORMATION pFileList = NULL;
  // 文件夹字符串
  UNICODE_STRING ustrPath = { 0 };
  RtlInitUnicodeString(&ustrPath, szPath);

  HANDLE hFile = NULL;
  SIZE_T nFileInfoSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 270 * sizeof(WCHAR);

  SIZE_T nSize = nFileInfoSize * 0x256;// 假设文件最大0x256
  WCHAR  szFileName[0xFF] = { 0 };
  do
  {
    pFileList = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.lst');
    if (!pFileList)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    pFileTmp = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.tmp');
    if (!pFileTmp)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    // 将路径组装为链接符号名称，并打开文件

    hFile = ArkCreateFileF(&ustrPath, TRUE);


    if (ArkFindFirstFile(hFile, nSize, pFileList, nFileInfoSize, pFileTmp))
    {
      LONG loc = 0;
      ULONG ulIndex = 0;
      do
      {
        RtlZeroMemory(szFileName, 0xFF);
        RtlCopyMemory(szFileName, pFileTmp->FileName, pFileTmp->FileNameLength);

        // 当前目录或下一层目录
        if (0 == wcscmp(szFileName, L"..")
          || 0 == wcscmp(szFileName, L"."))
        {
          continue;
        }

        if (pFileTmp->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          RtlCopyMemory((pFileInfo + ulIndex)->szName, szFileName, 0xFF);
          RtlCopyMemory((pFileInfo + ulIndex)->szType, L"目录", sizeof L"目录");
          (pFileInfo + ulIndex)->ulSize = pFileTmp->EndOfFile.QuadPart;
        }
        else
        {
          RtlCopyMemory((pFileInfo + ulIndex)->szName, szFileName, 0xFF);
          RtlCopyMemory((pFileInfo + ulIndex)->szType, L"文件", sizeof L"文件");
          (pFileInfo + ulIndex)->ulSize = pFileTmp->EndOfFile.QuadPart;
        }
        RtlZeroMemory(pFileTmp, nFileInfoSize);
        ulIndex++;
      } while (ArkFindNextFile(pFileList, pFileTmp, &loc));

    }
  } while (FALSE);

  if (hFile)
  {
    ZwClose(hFile);
    hFile = NULL;
  }

  // 释放内存
  if (pFileTmp)
  {
    ExFreePoolWithTag(pFileTmp, '.tmp');
    pFileTmp = NULL;
  }
  if (pFileList)
  {
    ExFreePoolWithTag(pFileList, '.lst');
    pFileList = NULL;
  }

  return status;
}

// 统计文件数量
ULONG ArkCountFileF(PWCHAR szPath)
{
  ULONG ulFileNum = 0;
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  PFILE_BOTH_DIR_INFORMATION pFileTmp = NULL;
  PFILE_BOTH_DIR_INFORMATION pFileList = NULL;
  // 文件夹字符串
  UNICODE_STRING ustrPath = { 0 };
  RtlInitUnicodeString(&ustrPath, szPath);
  HANDLE hFile = NULL;
  SIZE_T nFileInfoSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 270 * sizeof(WCHAR);

  SIZE_T nSize = nFileInfoSize * 0x256;// 假设文件最大0x256
  WCHAR  szFileName[0xFF] = { 0 };
  do
  {
    pFileList = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.lst');
    if (!pFileList)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    pFileTmp = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.tmp');
    if (!pFileTmp)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    // 将路径组装为链接符号名称，并打开文件
    hFile = ArkCreateFileF(&ustrPath, TRUE);
    if (ArkFindFirstFile(hFile, nSize, pFileList, nFileInfoSize, pFileTmp))
    {
      LONG loc = 0;
      do
      {
        RtlZeroMemory(szFileName, 0xFF);
        RtlCopyMemory(szFileName, pFileTmp->FileName, pFileTmp->FileNameLength);

        // 当前目录或下一层目录
        if (0 == wcscmp(szFileName, L"..")
          || 0 == wcscmp(szFileName, L"."))
        {
          continue;
        }
        ulFileNum++;
      } while (ArkFindNextFile(pFileList, pFileTmp, &loc));

    }
  } while (FALSE);

  if (hFile)
  {
    ZwClose(hFile);
    hFile = NULL;
  }

  // 释放内存
  if (pFileTmp)
  {
    ExFreePoolWithTag(pFileTmp, '.tmp');
    pFileTmp = NULL;
  }
  if (pFileList)
  {
    ExFreePoolWithTag(pFileList, '.lst');
    pFileList = NULL;
  }
  return ulFileNum;
}

// 响应Io 统计指定目录下的文件数量
NTSTATUS OnCountFile(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  // 获取要统计的符号链接目录
  WCHAR szPath[0xff] = { 0 };
  RtlCopyMemory(szPath, pIrp->AssociatedIrp.SystemBuffer, 0xFF);
  // 统计文件个数
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ArkCountFileF(szPath);
  // 返回
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = sizeof(ULONG);
  return status;
}


// 响应Io 查询指定目录下的文件信息
NTSTATUS OnQueryFile(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  PFILE_INFO pFileInfo = NULL;
  // 获取要统计的符号链接目录
  WCHAR szPath[0xff] = { 0 };
  RtlCopyMemory(szPath, pIrp->AssociatedIrp.SystemBuffer, 0xFF);
  // 统计文件个数
  ULONG ulNum = ArkCountFileF(szPath);
  ULONG ulMemSize = sizeof(FILE_INFO) * ulNum;
  do
  {
    // 为存储文件信息申请内存
    pFileInfo = (PFILE_INFO)ExAllocatePoolWithTag(PagedPool, ulMemSize, '.fil');
    if (!pFileInfo)
    {
      // status = STATUS_UNSUCCESSFUL;
      pFileInfo = NULL;
      break;
    }

    ArkEnumFile(szPath, pFileInfo);

    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pFileInfo, ulMemSize);
  } while (FALSE);

  // 释放内存
  if (pFileInfo)
  {
    ExFreePoolWithTag(pFileInfo, '.fil');
    pFileInfo = NULL;
  }

  // 返回
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = ulMemSize;
  return status;
}


// 响应Io 查询指定目录下的文件信息
NTSTATUS OnDelFile(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  // 获取要统计的符号链接目录
  WCHAR szPath[0xff] = { 0 };
  RtlCopyMemory(szPath, pIrp->AssociatedIrp.SystemBuffer, 0xFF);

  // 要删除的文件符号链接 unicode string
  UNICODE_STRING StrDelFile = { 0 };
  RtlInitUnicodeString(&StrDelFile, szPath);

  OBJECT_ATTRIBUTES objAttr = { 0 };
  ULONG ulAttrs = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
  InitializeObjectAttributes(&objAttr, &StrDelFile, ulAttrs, NULL, NULL);

  ZwDeleteFile(&objAttr);

  // 返回
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = 0;
  return status;
}


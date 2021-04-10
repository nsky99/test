#pragma once
#include <ntifs.h>

/// <summary>
/// 创建或打开文件
/// </summary>
/// <param name="pustrPath">文件路径</param>
/// <param name="access">访问属性</param>
/// <param name="bIsFile">是否是文件</param>
/// <returns>文件句柄，返回-1失败</returns>
HANDLE CreateFile(PUNICODE_STRING pustrPath, ACCESS_MASK access, BOOLEAN bIsFile)
{
  DbgBreakPoint();
  HANDLE hFile = NULL;

  // 目标文件信息
  OBJECT_ATTRIBUTES objAttr = { 0 };
  InitializeObjectAttributes(
    &objAttr, // 文件属性对象
    pustrPath,// 文件路径
    OBJ_CASE_INSENSITIVE,// 文件属性，区分大小写
    NULL, NULL           // 对象根目录和安全属性字段
  );

  // 接收ZwCreateFile返回信息
  IO_STATUS_BLOCK statusBlock = { 0 };

  // 根据用户传入的布尔值设置 CreateOption 的值是目录\文件
  ULONG CreateOption = bIsFile ? FILE_NON_DIRECTORY_FILE
    : FILE_DIRECTORY_FILE;

  // 创建或打开文件
  NTSTATUS status = ZwCreateFile(
    &hFile, // 返回文件句柄
    access,// 访问属性
    &objAttr,     // 目标文件属性
    &statusBlock, // 接收返回状态
    0,            // 如果是创建新的文件 - 文件大小
    FILE_ATTRIBUTE_NORMAL,// 创建、打开的文件具有什么样的属性
    FILE_SHARE_VALID_FLAGS,// 共享属性文件允许的共享方式，和 R3 保持一致
    FILE_OPEN_IF,          // 如果存在则打开，不存在则创建
    CreateOption,          // 创建或打开的设置属性
    NULL, 0                // 扩展缓冲区
  );
  if (!NT_SUCCESS(status))
  {
    return (HANDLE)-1;
  }
  return hFile;
}


/// <summary>
/// 获取文件大小
/// </summary>
/// <param name="hFile">文件句柄</param>
/// <returns>文件大小，如果是-1标识获取失败</returns>
ULONG GetFileSize(HANDLE hFile)
{
  // 要获取的文件信息
  FILE_STANDARD_INFORMATION FileInfo = { 0 };

  // 接收ZwQueryInformationFile返回信息
  IO_STATUS_BLOCK statusBlock = { 0 };

  // 执行查询文件信息 - 获取文件大小
  NTSTATUS status = ZwQueryInformationFile(
    hFile,    // 要查询信息的文件句柄
    &statusBlock,   // 接收函数执行信息
    &FileInfo,      // 接收文件信息
    sizeof(FileInfo),// 接收文件信息的结构体大小
    FileStandardInformation // 要查询什么信息
  );
  if (!NT_SUCCESS(status))
  {
    return (ULONG)-1;
  }
  return FileInfo.EndOfFile.LowPart;
}


/// <summary>
/// 读取文件
/// </summary>
/// <param name="hFile">文件句柄</param>
/// <param name="pBuffer">保存到哪</param>
/// <param name="ulReadSize">读多少</param>
/// <param name="pOffset">起始偏移</param>
/// <returns></returns>
NTSTATUS ReadFile(HANDLE hFile ,PVOID pBuffer,ULONG ulReadSize, ULONG pOffset)
{
  // 接收函数执行结果
  IO_STATUS_BLOCK statusBlock = { 0 };
  LARGE_INTEGER Offset = { pOffset, 0 };

  // 执行读取
  NTSTATUS status = ZwReadFile(
    hFile,                // 要读取谁的文件
    NULL, NULL, NULL,			// 异步 IO 使用的三个参数，几乎不用
    &statusBlock,         // 接收函数执行结果
    pBuffer, ulReadSize,  // 读到哪里，读多少
    &Offset, NULL         // 该变量指定了将开始读取操作的文件中的起始偏移
  );
  return status;
}
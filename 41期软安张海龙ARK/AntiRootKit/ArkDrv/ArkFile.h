#pragma once

#include <ntifs.h>

// 根据文件句柄查找第一个文件
BOOLEAN ArkFindFirstFile(
  IN HANDLE hFile,   // 文件句柄
  IN ULONG ulLen,    // 信息长度
  OUT PFILE_BOTH_DIR_INFORMATION pDir,// 文件信息
  IN ULONG uFirstLen,// 信息长度
  OUT PFILE_BOTH_DIR_INFORMATION pFirstDir// 第一个文件信息
  );


// 根据文件句柄查找下一个文件
BOOLEAN ArkFindNextFile(
  IN PFILE_BOTH_DIR_INFORMATION pDirList,
  OUT PFILE_BOTH_DIR_INFORMATION pDirInfo,
  IN OUT LONG* Loc
);

// 根据文件路径符号链接创建文件
HANDLE ArkCreateFileF(
  IN PUNICODE_STRING pstrFile,// 文件路径符号链接
  IN BOOLEAN bIsDir            // 是否为文件夹
);

// 响应Io 统计指定目录下的文件数量
NTSTATUS OnCountFile(PIRP pIrp);

// 响应Io 查询指定目录下的文件信息
NTSTATUS OnQueryFile(PIRP pIrp);


// 响应Io 查询指定目录下的文件信息
NTSTATUS OnDelFile(PIRP pIrp);
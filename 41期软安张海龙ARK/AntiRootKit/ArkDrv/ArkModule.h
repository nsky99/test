#pragma once

#include <ntifs.h>

// 响应Io统计指定进程模块数量
NTSTATUS OnCountModule(PIRP pIrp);

// 响应Io查询进程模块信息
NTSTATUS OnQueryModule(PIRP pIrp);
#pragma once
#include <ntifs.h>

// 响应Io 统计IDT
NTSTATUS OnCountIdt(PIRP pIrp);

// 响应Io 查询IDT
NTSTATUS OnQueryIdt(PIRP pIrp);
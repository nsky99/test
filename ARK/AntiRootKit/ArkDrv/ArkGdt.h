#pragma once
#include <ntifs.h>

// 响应IO 统计GDT数量
NTSTATUS OnCountGDT(PIRP pIrp);

// 响应IO 查询GDT信息
NTSTATUS OnQueryGDT(PIRP pIrp);
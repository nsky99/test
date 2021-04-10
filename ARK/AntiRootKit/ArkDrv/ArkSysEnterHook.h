#pragma once

#include <ntifs.h>
// 安装钩子
void InstallHook();


// 卸载钩子
void UnInstallHook();



// 响应Io SysHook
NTSTATUS OnHookSysEnter(PIRP pIrp);


// 响应Io SysUnHook
NTSTATUS OnUnHookSysEnter(PIRP pIrp);
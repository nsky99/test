#ifndef _CPLUGIN_H_
#define _CPLUGIN_H_
#include <Windows.h>
#include <vector>
using std::vector;
// 插件信息
struct PLUGIN_INFO {
  HMODULE hMod;               // 插件模块
  char plugin_name[200];      // 插件名称
  char plugin_version[200];   // 版本
};


// 1. 如何找插件
//          遍历指定目下，plugin
// 2. 过滤插件名称 
//          dll后缀名
// 3. 插件是否有导出指定函数
//              init_plugin
//              loadmenu_plugin
//              exit_plugin
// 4. 检查插件是否符合我们的版本
// 5. 保存插件，在程序运行中分别调用


// 定义函数指针
typedef  bool(*INIT_PLUGIN)(WORD, PLUGIN_INFO*);
using LOADMENU_PLUGIN = const char* (*)();
using EXIT_PLUGIN = void(*)();

void OnPluginInit();

void OnPlguinLoadMenu();

void OnPluginExit();

#endif // !_CPLUGIN_H_


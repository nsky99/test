#include "CPlugin.h"

// 保存所有插件
vector<PLUGIN_INFO> g_vecPlugin;

void  OnPluginInit()
{
  WIN32_FIND_DATAA Data;
  // 遍历目录找到插件
  HANDLE hFile = FindFirstFileA(".\\plugin\\*.dll", &Data);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return;
  }
  do
  {
    // 拼接字符串，合并完成路径
    char szFileName[200] = {};
    sprintf_s(szFileName, ".\\plugin\\%s", Data.cFileName);

    // 加载dll
    HMODULE hmod = LoadLibraryA(szFileName);
    // 加载成功
    if (hmod != NULL)
    {
      // 获取指定函数
      auto pFnInit = (INIT_PLUGIN)GetProcAddress(hmod, "init_plugin");
      // 判断是否函数是否获取成功
      if (pFnInit != NULL)
      {
        PLUGIN_INFO info;
        // 调用插件初始化
        if (pFnInit(MAKEWORD(2, 2), &info))
        {
          // 保存模块，方便后面使用
          info.hMod = hmod;
          // 保存到插件列表中
          g_vecPlugin.push_back(info);
          printf("%s 加载成功\n", info.plugin_name);
          continue;
        }
      }
      // 加载失败卸载模块
      FreeLibrary(hmod);
    }
    //遍历下一个文件
  } while (FindNextFileA(hFile, &Data));
}

void OnPlguinLoadMenu()
{
  // 遍历所有插件，调用相应函数
  for (auto& plugin : g_vecPlugin)
  {
    // 获取加载菜单的函数
    auto FnLoadMenu = (LOADMENU_PLUGIN)GetProcAddress(plugin.hMod, "loadmenu_plugin");
    // 模仿加载到菜单中
    printf("菜单 ： %s 被加载了\n", FnLoadMenu());
  }

}

void OnPluginExit()
{
  // 遍历所有插件，调用相应函数
  for (auto& plugin : g_vecPlugin)
  {
    // 获取加载菜单的函数
    auto FnExitPlugin = (EXIT_PLUGIN)GetProcAddress(plugin.hMod, "exit_plugin");
    // 模仿加载到菜单中
    FnExitPlugin();
    // 释放dll
    FreeLibrary(plugin.hMod);
  }
}
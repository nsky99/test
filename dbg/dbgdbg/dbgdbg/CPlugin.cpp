#include "CPlugin.h"

// �������в��
vector<PLUGIN_INFO> g_vecPlugin;

void  OnPluginInit()
{
  WIN32_FIND_DATAA Data;
  // ����Ŀ¼�ҵ����
  HANDLE hFile = FindFirstFileA(".\\plugin\\*.dll", &Data);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return;
  }
  do
  {
    // ƴ���ַ������ϲ����·��
    char szFileName[200] = {};
    sprintf_s(szFileName, ".\\plugin\\%s", Data.cFileName);

    // ����dll
    HMODULE hmod = LoadLibraryA(szFileName);
    // ���سɹ�
    if (hmod != NULL)
    {
      // ��ȡָ������
      auto pFnInit = (INIT_PLUGIN)GetProcAddress(hmod, "init_plugin");
      // �ж��Ƿ����Ƿ��ȡ�ɹ�
      if (pFnInit != NULL)
      {
        PLUGIN_INFO info;
        // ���ò����ʼ��
        if (pFnInit(MAKEWORD(2, 2), &info))
        {
          // ����ģ�飬�������ʹ��
          info.hMod = hmod;
          // ���浽����б���
          g_vecPlugin.push_back(info);
          printf("%s ���سɹ�\n", info.plugin_name);
          continue;
        }
      }
      // ����ʧ��ж��ģ��
      FreeLibrary(hmod);
    }
    //������һ���ļ�
  } while (FindNextFileA(hFile, &Data));
}

void OnPlguinLoadMenu()
{
  // �������в����������Ӧ����
  for (auto& plugin : g_vecPlugin)
  {
    // ��ȡ���ز˵��ĺ���
    auto FnLoadMenu = (LOADMENU_PLUGIN)GetProcAddress(plugin.hMod, "loadmenu_plugin");
    // ģ�¼��ص��˵���
    printf("�˵� �� %s ��������\n", FnLoadMenu());
  }

}

void OnPluginExit()
{
  // �������в����������Ӧ����
  for (auto& plugin : g_vecPlugin)
  {
    // ��ȡ���ز˵��ĺ���
    auto FnExitPlugin = (EXIT_PLUGIN)GetProcAddress(plugin.hMod, "exit_plugin");
    // ģ�¼��ص��˵���
    FnExitPlugin();
    // �ͷ�dll
    FreeLibrary(plugin.hMod);
  }
}
#ifndef _CPLUGIN_H_
#define _CPLUGIN_H_
#include <Windows.h>
#include <vector>
using std::vector;
// �����Ϣ
struct PLUGIN_INFO {
  HMODULE hMod;               // ���ģ��
  char plugin_name[200];      // �������
  char plugin_version[200];   // �汾
};


// 1. ����Ҳ��
//          ����ָ��Ŀ�£�plugin
// 2. ���˲������ 
//          dll��׺��
// 3. ����Ƿ��е���ָ������
//              init_plugin
//              loadmenu_plugin
//              exit_plugin
// 4. ������Ƿ�������ǵİ汾
// 5. ���������ڳ��������зֱ����


// ���庯��ָ��
typedef  bool(*INIT_PLUGIN)(WORD, PLUGIN_INFO*);
using LOADMENU_PLUGIN = const char* (*)();
using EXIT_PLUGIN = void(*)();

void OnPluginInit();

void OnPlguinLoadMenu();

void OnPluginExit();

#endif // !_CPLUGIN_H_


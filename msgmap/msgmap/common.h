#pragma once
#include <Windows.h>

// 绑定消息和消息处理函数的结构体
typedef struct _MSGMAP_ENTRY
{
  UINT uMsg;
  LRESULT(* pfn)(HWND, UINT, WPARAM, LPARAM);
}MSGMAP_ENTRY, * PMSGMAP_ENTRY;

typedef struct _MSGMAP_ENTRYDLG
{
  UINT uMsg;
  INT_PTR(* pfn)(HWND, UINT, WPARAM, LPARAM);
}MSGMAP_ENTRYDLG, * PMSGMAP_ENTRYDLG;

// 求数组的大小
#define dim(x) (sizeof(x)/sizeof(x[0]))
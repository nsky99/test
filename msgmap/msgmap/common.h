#pragma once
#include <Windows.h>

// ����Ϣ����Ϣ�������Ľṹ��
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

// ������Ĵ�С
#define dim(x) (sizeof(x)/sizeof(x[0]))
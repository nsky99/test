// CheckDebug.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>


int main()
{
  getchar();
  if (IsDebuggerPresent())
  {
    printf("有调试器");
  }
  else
  {
    printf("无调试器");
  }
  system("pause");
}



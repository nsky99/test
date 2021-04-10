#pragma once
#include <Windows.h>
#include <winsvc.h>


class ArkOption
{
private:
  // 服务名称和路径
  WCHAR m_SerName[MAX_PATH];
  WCHAR m_SerPath[MAX_PATH];

  // 服务管理器句柄和服务句柄
  SC_HANDLE m_hSCM;
  SC_HANDLE m_hSer;

public:
  /// <summary>
  /// 控制ARK服务的默认构造
  /// 需要初始化服务名称和路径
  /// </summary>
  ArkOption();

  /// <summary>
  /// 创建ARK服务
  /// </summary>
  /// <returns> ARK服务是否创建成功 </returns>
  BOOL CreateARK();

  /// <summary>
  /// 启动ARK服务
  /// </summary>
  /// <returns> ARK服务是否启动成功 </returns>
  BOOL StartARK();

  /// <summary>
  /// 停止ARK服务
  /// </summary>
  /// <returns> ARK服务是否停止成功 </returns>
  BOOL StopARK();

  /// <summary>
  /// 删除ARK服务
  /// </summary>
  /// <returns> ARK服务是否删除成功 </returns>
  BOOL DelARK();
};


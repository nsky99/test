#pragma once
#include <Windows.h>
#include <winsvc.h>


class ArkOption
{
private:
  // �������ƺ�·��
  WCHAR m_SerName[MAX_PATH];
  WCHAR m_SerPath[MAX_PATH];

  // �������������ͷ�����
  SC_HANDLE m_hSCM;
  SC_HANDLE m_hSer;

public:
  /// <summary>
  /// ����ARK�����Ĭ�Ϲ���
  /// ��Ҫ��ʼ���������ƺ�·��
  /// </summary>
  ArkOption();

  /// <summary>
  /// ����ARK����
  /// </summary>
  /// <returns> ARK�����Ƿ񴴽��ɹ� </returns>
  BOOL CreateARK();

  /// <summary>
  /// ����ARK����
  /// </summary>
  /// <returns> ARK�����Ƿ������ɹ� </returns>
  BOOL StartARK();

  /// <summary>
  /// ֹͣARK����
  /// </summary>
  /// <returns> ARK�����Ƿ�ֹͣ�ɹ� </returns>
  BOOL StopARK();

  /// <summary>
  /// ɾ��ARK����
  /// </summary>
  /// <returns> ARK�����Ƿ�ɾ���ɹ� </returns>
  BOOL DelARK();
};


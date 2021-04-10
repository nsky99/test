#include "ArkSysEnterHook.h"

// ԭʼ����
ULONG g_OldKiFastCallEntry = 0;
// Ҫ�����Ľ���PID
ULONG g_pid = 0;

// ���˺���
void _declspec(naked) MyKiFastCallEntry()
{
	_asm
	{
		pushad;										// ����Ĵ���
		cmp eax, 0xBE;						// �Ƿ���ZwOpenProcess ����
		jne CallEnd;							// ��������
		mov eax, [edx + 0x14];		// ��ȡ���ĸ����� ClientId
		mov eax, [eax];						// ClientId->ProcessId
		cmp eax, g_pid;						// �ж��Ƿ�Ҫ����Ľ���
		jne CallEnd;
		mov[edx + 0x0c], 0;				// ��Ȩ�޸�Ϊ0���޷�����

	CallEnd:
		popad;										//�ָ��Ĵ���
		jmp g_OldKiFastCallEntry;	//����ԭʼ����
	}
}

// ��װ����
void InstallHook()
{
	// ��ȡԭʼ����
	_asm
	{
		// �� MSR 0x176 ��λ�ö�ȡԭ�е� KiFastCallEntry �������б���
		mov ecx, 0x176;
		rdmsr;												 //��msr176�Ĵ��������ݱ���eax
		mov g_OldKiFastCallEntry, eax; //����ԭʼ������ַ
	}

	// ���ù���
	_asm
	{
		mov ecx, 0x176;
		mov eax, MyKiFastCallEntry; // ���˺���
		wrmsr;											// д�뵽msr0x176�Ĵ���
	}

}

// ж�ع���
void UnInstallHook()
{
	// ���ù���
	_asm
	{
		mov ecx, 0x176;
		mov eax, g_OldKiFastCallEntry;  // ���˺���
		wrmsr;							//д�뵽msr0x176�Ĵ���
	}
}

// ��ӦIo SysHook
NTSTATUS OnHookSysEnter(PIRP pIrp)
{
	// hook�Ľ���ID
	g_pid = *(PULONG)pIrp->AssociatedIrp.SystemBuffer;
	InstallHook();

	// ret
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;
}

// ��ӦIo UnSysHook
NTSTATUS OnUnHookSysEnter(PIRP pIrp)
{
	DbgBreakPoint();

  UnInstallHook();

	// ret
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;
}

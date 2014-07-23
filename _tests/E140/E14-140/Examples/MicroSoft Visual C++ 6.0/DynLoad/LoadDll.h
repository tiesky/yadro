//---------------------------------------------------------------------------
#ifndef __LoadDllH
#define __LoadDllH
//---------------------------------------------------------------------------
#include <windows.h>
#include <winioctl.h>

typedef int (WINAPI *pGetDllVersion)(void);
typedef LPVOID (WINAPI *pCreateInstance)(char *);

class TLoadDll
{
	public:
		// ����������
		TLoadDll(PCHAR DllName);
		// ����������
		~TLoadDll();

		// ��������� ������ ������� ���������� ������
		LPVOID WINAPI CallCreateLInstance(void);
		// ��������� ������ ������� ������ DLL
		LPVOID WINAPI CallGetDllVersion(void);
		// ��������� �������������� ����������
		HINSTANCE WINAPI GetDllHinstance(void);


	private:
		// ������������� ������ DLL
		HINSTANCE hDll;
};

#endif


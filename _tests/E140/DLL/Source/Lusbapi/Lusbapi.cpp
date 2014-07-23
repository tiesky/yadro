#include "e140.h"
#include "e154.h"
#include "e310.h"
#include "e440.h"
#include "e2010.h"

// ������������� ������������ ������
static HINSTANCE hInstance;

//------------------------------------------------------------------------
// ����� ����� � ����������
//------------------------------------------------------------------------
#if defined (__BORLANDC__)
BOOL WINAPI DllEntryPoint(HINSTANCE hInst, DWORD fdwReason, LPVOID /*lpvReserved*/)
#else
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID /*lpvReserved*/)
#endif
{
	if(fdwReason == DLL_PROCESS_ATTACH)
		// �������� ������������� ������������ ������
		hInstance = hInst;
	return TRUE;
}

//------------------------------------------------------------------------
// ���������� ������� ������ ����������
//------------------------------------------------------------------------
DWORD WINAPI GetDllVersion(void)
{
	return CURRENT_VERSION_LUSBAPI;
}

//------------------------------------------------------------------------
// �������� ���������� ������������ ������
//------------------------------------------------------------------------
LPVOID WINAPI CreateLInstance(PCHAR const DeviceName)
{
	LPVOID pDevice;

	// �������� �������� ����������
	if(!DeviceName) return NULL;

	// ������ ��������� �������� ��������� �� ������
	if(!strcmpi(DeviceName, "LUSBBASE")) pDevice = static_cast<ILUSBBASE *>(new TLUSBBASE(hInstance));
	else if(!strcmpi(DeviceName, "E140")) pDevice = static_cast<ILE140 *>(new TLE140(hInstance));
	else if(!strcmpi(DeviceName, "E154")) pDevice = static_cast<ILE154 *>(new TLE154(hInstance));
	else if(!strcmpi(DeviceName, "E310")) pDevice = static_cast<ILE310 *>(new TLE310(hInstance));
	else if(!strcmpi(DeviceName, "E440")) pDevice = static_cast<ILE440 *>(new TLE440(hInstance));
	else if(!strcmpi(DeviceName, "E2010")) pDevice = static_cast<ILE2010 *>(new TLE2010(hInstance));
	else pDevice = NULL;
	// ���������� ��������� �� ��, ��� ����������
	return pDevice;
}


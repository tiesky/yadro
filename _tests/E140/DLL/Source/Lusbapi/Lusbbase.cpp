//------------------------------------------------------------------------------
#include <stdio.h>
#include <algorithm>
#include "LUsbBase.h"
#include "ErrorBaseIds.h"
#include "Lusbapi.h"
//------------------------------------------------------------------------------

#define 	LAST_ERROR_NUMBER(ErrorNumber)	LastErrorNumber = LUSBBASE_BASE_ERROR_ID + ErrorNumber;

//------------------------------------------------------------------------------
//  ����������
//------------------------------------------------------------------------------
TLUSBBASE::TLUSBBASE(HINSTANCE hInst)
{
	// ����� ��������� ����� ���� float
	FloatEps = 1.0E-7;
	// ����� ��������� ����� ���� double
	DoubleEps = 1.0E-14;

	// ������������� ������ DLL
	hInstance = hInst;
	// ������������� ����������
	hDevice = INVALID_HANDLE_VALUE;
	// ������� �������� ������ ������ �� ���� USB
	UsbSpeed = INVALID_USB_SPEED_LUSBAPI;
	// ������� ����� ��������� ������
	LAST_ERROR_NUMBER(0x0);
	// ������� ������� � ��������� ������
	ZeroMemory(ModuleName, sizeof(ModuleName));
	// ������� �������� �������� � ��
	TimeOut = 5000;

	// ������������� ������� �������������� ID USB ���������
	DEVICES_ID_ARRAY[E154_INDEX]		= E154_ID;
	DEVICES_ID_ARRAY[E14_140_INDEX]	= E14_140_ID;
	DEVICES_ID_ARRAY[E14_440_INDEX]	= E14_440_ID;
	DEVICES_ID_ARRAY[E2010_INDEX]		= E2010_ID;
	DEVICES_ID_ARRAY[E270_INDEX]		= E270_ID;
	DEVICES_ID_ARRAY[LTR010_INDEX] 	= LTR010_ID;
	DEVICES_ID_ARRAY[LTR021_INDEX] 	= LTR021_ID;
	DEVICES_ID_ARRAY[E2010B_INDEX]	= E2010B_ID;
	DEVICES_ID_ARRAY[E_310_INDEX]	= E310_ID;

	// ������ ���������� ������ ��� ������ ������
	InitializeCriticalSection(&cs);
}

//------------------------------------------------------------------------------
//  ����������
//------------------------------------------------------------------------------
TLUSBBASE::~TLUSBBASE() { }

//------------------------------------------------------------------------------
//  ������� ����������� ���� ��� ������� � USB ������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::OpenLDevice(WORD VirtualSlot)
{
	// ������� ������� ���� �����-������ USB ����������
	if(!OpenLDeviceByID(VirtualSlot, ENUM_ALL_USB_DEVICE_ID)) { return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
//  ��������� ������� ����������� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::CloseLDevice(void)
{
	BOOL Status = TRUE;

	// ��������� ����������
	if(hDevice != INVALID_HANDLE_VALUE)
	{
		if(!CloseHandle(hDevice)) { LAST_ERROR_NUMBER(6); Status = FALSE; }
		hDevice = INVALID_HANDLE_VALUE;
	}

	return Status;
}

//------------------------------------------------------------------------------
//  ��������� ��������� �� ��������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::ReleaseLInstance(void)
{
	BOOL Status = TRUE;

	// ��������� ������ ����������� ������
	DeleteCriticalSection(&cs);
	// ��������� ������������� ������������ �����
	if(!CloseLDevice()) Status = FALSE;
	// ��������� ��������� �� ����������
	delete this;

	return Status;
}

//------------------------------------------------------------------------------
//  ������������ ���������� ����������
//------------------------------------------------------------------------------
HANDLE WINAPI TLUSBBASE::GetModuleHandle(void) { return hDevice; }

//------------------------------------------------------------------------------
//  ������ �������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::GetModuleName(PCHAR const ModuleName)
{
	// ������� �������� �������� ������
	if(!GetModuleNameByLength(ModuleName)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ��������� ������� �������� ������ ���� USB
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// ����������� ���� ��������?
	if(hDevice == INVALID_HANDLE_VALUE) { LAST_ERROR_NUMBER(7); return FALSE; }
	// ��� ������� E14-140 � E14-440 ������������� ����� FULL-Speed (12 �bit/s)
	else if(!strcmp(ModuleName, "E140") || !strcmp(ModuleName, "E440") || !strcmp(ModuleName, "E154")) *UsbSpeed = USB11_LUSBAPI;
	// ��� ������ E-310 � E20-10 ������� ������ ������� �������� ������ USB ����
	else if(!strcmp(ModuleName, "E-310") ||!strcmp(ModuleName, "E20-10"))
	{
		// ��������� ������ �� ��������� �������� ������ ������ �� ���� USB
		WORD InBuf[4] = { 0x01, V_GET_USB_SPEED, 0x00, 0x00 };

		// �������� ������ � ������
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), UsbSpeed, 0x1/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(9); return FALSE; }
	}
	// ������������ �������� ������
	else { LAST_ERROR_NUMBER(10); return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ������� ������� ������������������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::LowPowerMode(BOOL LowPowerFlag)
{
	// ��� ��� ����� - ����������, � ���� ���������� ������
	LAST_ERROR_NUMBER(14);
   return FALSE;
}

//------------------------------------------------------------------------------
// ������� ������ ���������� � ��������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	// �������� ��������� ��������� ������
	if(!LastErrorInfo) { return FALSE; }
	// ����� ��������� ������
	LastErrorInfo->ErrorNumber = LastErrorNumber;
	// ��������� �������� ������ � ��������� �������
	if(!LoadString(hInstance, LastErrorNumber, (LPTSTR)LastErrorInfo->ErrorString, sizeof(LastErrorInfo->ErrorString)))
	{
		// ������������� ����� ������
		LastErrorInfo->ErrorNumber = LUSBBASE_BASE_ERROR_ID + 99;
		strcpy((char *)LastErrorInfo->ErrorString, "������ TLUSBBASE::GetLastErrorInfo(). ���������� �������� ������ � ��������� ������� ��������� �� ����������� WinAPI ������� LoadString().");
		return FALSE;
	}
	// ��� ������ :)))))
	else return TRUE;
}





//==============================================================================
// ��������������� ������� ��� ������ � �������
//==============================================================================
BOOL WINAPI TLUSBBASE::OpenLDeviceByID(WORD VirtualSlot, DWORD DeviceID)
{
	char DeviceName[18];
	WORD i;
	DWORD WindowsVersion;

	// ������� ����� ��������� ������
	LAST_ERROR_NUMBER(0x0);

	// ����������� ���� ��������?
	if(hDevice != INVALID_HANDLE_VALUE) { CloseHandle(hDevice); hDevice = INVALID_HANDLE_VALUE; }

	// ����������� � ������� ������������ Windows
	WindowsVersion = GetWindowsVersion();
	if((WindowsVersion == UNKNOWN_WINDOWS_VERSION) ||
		(WindowsVersion == WINDOWS_32S) || (WindowsVersion == WINDOWS_95) ||
		(WindowsVersion == WINDOWS_NT)) { LAST_ERROR_NUMBER(1); return FALSE; }

	// ��������� �������� �������� USB
	wsprintf(DeviceName, "\\\\.\\LDev%d", VirtualSlot);
	// ��������� ������� ������������� ��� ������
	hDevice = CreateFile(DeviceName, GENERIC_READ|GENERIC_WRITE, 0x0, NULL,
												OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	// ��������: ���������� �� ������� ����������?
	if(hDevice == INVALID_HANDLE_VALUE) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��������� �������� ��������� ���������� �� �������� ���������
	if(!GetDeviceInitialInfo()) { LAST_ERROR_NUMBER(11); CloseLDevice(); return FALSE; }
	// ���� ��������� �������� �� ���� ID, �� �������� ���
	else if(DeviceID == ENUM_ALL_USB_DEVICE_ID)
	{
		for(i = 0x0; i < SUPPORTED_USB_DEVICES_QUANTITY; i++)
			if(DeviceInitialInfo.DeviceID == DEVICES_ID_ARRAY[i]) break;
		if(i == SUPPORTED_USB_DEVICES_QUANTITY) { LAST_ERROR_NUMBER(12); return FALSE; }
	}
	// �������� ���������� ID ���������� � ���������
	else if(DeviceInitialInfo.DeviceID != DeviceID)
	{
		if(DeviceID == E2010_ID)
		{
			if(DeviceInitialInfo.DeviceID != E2010B_ID) { LAST_ERROR_NUMBER(13); return FALSE; }
		}
		else { LAST_ERROR_NUMBER(13); return FALSE; }
	}

	// ��������� ��������� �������� ������
	if(!GetModuleName(ModuleName)) { LAST_ERROR_NUMBER(3); CloseLDevice(); return FALSE; }
	// ��������� �� ����� �������� �������� ������
	else if(!GetUsbSpeed(&UsbSpeed)) { LAST_ERROR_NUMBER(4); CloseLDevice(); return FALSE; }
	// ����������� ���������� �������� ������ ������ �� ���� USB
	else if(UsbSpeed >= INVALID_USB_SPEED_LUSBAPI) { LAST_ERROR_NUMBER(5); CloseLDevice(); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� �������� ��������� ���������� �� �������� ���������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::GetDeviceInitialInfo(void)
{
	// ����������� ���� ��������?
	if(hDevice == INVALID_HANDLE_VALUE) { LAST_ERROR_NUMBER(7); return FALSE; }
	else if(!LDeviceIoControl(DIOC_GET_PARAMS, NULL, NULL,
							  &DeviceInitialInfo, sizeof(DEVICE_INITIAL_INFO), TimeOut)) { LAST_ERROR_NUMBER(9); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
//  ������ �������� ������ � ������������ ������������� ����� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::GetModuleNameByLength(PCHAR const ModuleName, WORD ModuleNameLength)
{
	// ��������� ������ �� ��������� �������� ������
	WORD InBuf[4] = { 0x01, V_GET_MODULE_NAME, 0x00, 0x00 };

	// ����������� ���� ��������?
	if(hDevice == INVALID_HANDLE_VALUE) { LAST_ERROR_NUMBER(7); return FALSE; }
	// �������� ������
	else if(!ModuleName) { LAST_ERROR_NUMBER(8); return FALSE; }
	// �������� ����� ��������
	if(ModuleNameLength)
	{
		// ������� ������� � ��������� ������
		ZeroMemory(ModuleName, ModuleNameLength);
		// �������� ������ � ������
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), ModuleName, ModuleNameLength, TimeOut)) { LAST_ERROR_NUMBER(9); return FALSE; }
	}
	else
	{
		char LocModuleName[LONG_MODULE_NAME_STRING_LENGTH];

		// ������� ������� � ��������� ������
		ZeroMemory(LocModuleName, LONG_MODULE_NAME_STRING_LENGTH);
		// ��� ������ ��������� ������� ������ ������� �����
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), LocModuleName, LONG_MODULE_NAME_STRING_LENGTH, TimeOut))
		{
			// ������ ����� ����������� ������� ������ ������� �����
			if(!LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), LocModuleName, SHORT_MODULE_NAME_STRING_LENGTH, TimeOut)) { LAST_ERROR_NUMBER(9); return FALSE; }
			// ��������� �������� �������
			strncpy(ModuleName, LocModuleName, SHORT_MODULE_NAME_STRING_LENGTH);
			// �������� ��������� ������ � ������
			ModuleName[SHORT_MODULE_NAME_STRING_LENGTH - 0x1] = 0x0;
		}
		else
		{
			// ��������� ������� �������
			strncpy(ModuleName, LocModuleName, LONG_MODULE_NAME_STRING_LENGTH);
			// �������� ��������� ������ � ������
			ModuleName[LONG_MODULE_NAME_STRING_LENGTH - 0x1] = 0x0;
		}
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ������������� ����������������� ������� � ������
//------------------------------------------------------------------------------
BOOL WINAPI TLUSBBASE::LDeviceIoControl(	DWORD dwIoControlCode,		// control code of operation to perform
														LPVOID lpInBuffer,			// pointer to buffer to supply input data
														DWORD nInBufferSize,			// size of input buffer in bytes
														LPVOID lpOutBuffer,			// pointer to buffer to receive output data
														DWORD nOutBufferSize,		// size of output buffer in bytes
														DWORD TimeOut)					// ������� � ��
{
	DWORD RealBytesTransferred;
	DWORD BytesReturned;
	OVERLAPPED Ov;

	// ����������� ���� ��������?
	if(hDevice == INVALID_HANDLE_VALUE) { LAST_ERROR_NUMBER(7); return FALSE; }

	// �������������� OVERLAPPED ���������
	memset(&Ov, 0x0, sizeof(OVERLAPPED));
	// ������ ������� ��� ������������ �������
	Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
	if(!Ov.hEvent) { return FALSE; }

	// �������� ��������� ������ � AVR
	if(!DeviceIoControl(	hDevice, dwIoControlCode,
								lpInBuffer, nInBufferSize,
								lpOutBuffer, nOutBufferSize,
								&BytesReturned, &Ov))
			{ if(GetLastError() != ERROR_IO_PENDING) { /*LastErrorNumber = 1001;*/ CloseHandle(Ov.hEvent); return FALSE; } }
	// ��� ��������� ���������� �������
	if(WaitForSingleObject(Ov.hEvent, TimeOut) == WAIT_TIMEOUT) { CancelIo(hDevice); CloseHandle(Ov.hEvent); /*LastErrorNumber = 1001;*/ return FALSE; }
	// ��������� �������� ���-�� ������� ���������� ���� ������
	else if(!GetOverlappedResult(hDevice, &Ov, &RealBytesTransferred, TRUE)) { CancelIo(hDevice); CloseHandle(Ov.hEvent); /*LastErrorNumber = 1001;*/ return FALSE; }
	// �������� ������� ������� ���� �������� ���� ������
	if(nOutBufferSize != RealBytesTransferred) { CancelIo(hDevice); CloseHandle(Ov.hEvent); /*LastErrorNumber = 1001;*/ return FALSE; }
	// ������� ������� ������������ �������
	else if(!CloseHandle(Ov.hEvent)) { /*LastErrorNumber = 1001;*/ return FALSE; }
	// ��� ������ :)))))
	return TRUE;
}

//----------------------------------------------------------------
// ������� ��� ���������� CRC16
//----------------------------------------------------------------
WORD WINAPI TLUSBBASE::CalculateCrc16(BYTE *ptr, WORD NBytes)
{
	WORD w = 0x0;
	WORD i, j;

	for(i = 0x0; i < NBytes; i++)
	{
		w ^= (WORD)((WORD)(ptr[i]) << 0x8);
		for(j = 0x0; j < 8; j++)
		{
			if(w & (WORD)0x8000) w = (WORD)((w << 0x1) ^ 0x8005);
			else w <<= 0x1;
		}
	}
	return w;
}

//------------------------------------------------------------------------------
// ��������� �������� ������ ������������ Windows
//------------------------------------------------------------------------------
DWORD TLUSBBASE::GetWindowsVersion(void)
{
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(!GetVersionEx(&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO *)&osvi)) return UNKNOWN_WINDOWS_VERSION;
	}

	switch (osvi.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_NT:
			if(osvi.dwMajorVersion <= 4 ) return WINDOWS_NT; 			// WinNT
			else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0)) return WINDOWS_2000; 			// Win2000
			else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 1)) return WINDOWS_XP; 				// WinXP
			else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 2)) return WINDOWS_2003_SERVER; 	// Win2003 Server
			else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0)) return WINDOWS_VISTA; 	// WinVista ���  WinServer 2008
			else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1)) return WINDOWS_7; 		// Win7     ���  WinServer 2008 R2
			else if(osvi.dwMajorVersion == 6) return WINDOWS_FUTURE;			// �������� ��� ������� Windows
			else return UNKNOWN_WINDOWS_VERSION;

		case VER_PLATFORM_WIN32_WINDOWS:
			if((osvi.dwMajorVersion > 0x4) || ((osvi.dwMajorVersion == 0x4) &&
				(osvi.dwMinorVersion > 0x0))) return WINDOWS_98_OR_LATER; // Win98 or later
			else return WINDOWS_95; 											// Win95

		case VER_PLATFORM_WIN32s: return WINDOWS_32S; 					// Win32s
	}

	return UNKNOWN_WINDOWS_VERSION;
}

//------------------------------------------------------------------------------
#ifndef __LUSBBASEAPIH__
#define __LUSBBASEAPIH__
//------------------------------------------------------------------------------
	#include <windows.h>
	#include <winioctl.h>
	#include "Ioctl.h"
	#include "Lusbapi.h"

	//---------------------------------------------------------------------------
	// ���������� �������� ������ ��� ������ � USB ������������
	//---------------------------------------------------------------------------
	class TLUSBBASE : public ILUSBBASE
	{
		public :
			// ������� ������ ���������� ��� ������ � USB ������������
			virtual BOOL WINAPI OpenLDevice(WORD VirtualSlot);
			virtual BOOL WINAPI CloseLDevice(void);
			virtual BOOL WINAPI ReleaseLInstance(void);
			// ��������� ����������� ���������� USB
			virtual HANDLE WINAPI GetModuleHandle(void);
			// ��������� �������� ������������� ������
			virtual BOOL WINAPI GetModuleName(PCHAR const ModuleName);
			// ��������� ������� �������� ������ ���� USB
			virtual BOOL WINAPI GetUsbSpeed(BYTE * const UsbSpeed);
			// ���������� ������� ������� ������������������
			virtual BOOL WINAPI LowPowerMode(BOOL LowPowerFlag);
			// ������� ������ ������ � ��������� �������
			virtual BOOL WINAPI GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo);

			// ����������/����������
			TLUSBBASE(HINSTANCE hInst);
			virtual ~TLUSBBASE();

		protected:
			// �������� ���������� �� ��� ID
			BOOL WINAPI OpenLDeviceByID(WORD VirtualSlot, DWORD DeviceID = ENUM_ALL_USB_DEVICE_ID);
			// ��������� �������� ������������� ������
			BOOL WINAPI GetModuleNameByLength(PCHAR const ModuleName, WORD ModuleNameLength = 0x0);
			// ��������� ��������� ���������� �� �������� ���������
			BOOL WINAPI GetDeviceInitialInfo(void);
			// ������� ��������� �������� USB
			BOOL WINAPI LDeviceIoControl(DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize,
																LPVOID lpOutBuffer, DWORD nOutBufferSize, DWORD dwTimeOut);
			// ��������������� ������� ��� ���������� CRC16
			WORD WINAPI CalculateCrc16(BYTE *ptr, WORD NBytes);

			// ������������� DLL
			HINSTANCE hInstance;
			// ������������� ����������
			HANDLE hDevice;
			// �������� ������ ������ �� ���� USB (0 -> USB11, 1 -> USB20)
			BYTE UsbSpeed;
			// ������� �������� ��������
			DWORD TimeOut;
			// ����������� ������ ��� ���������������� ������
			CRITICAL_SECTION cs;
			// ����� ��������� ������
			WORD LastErrorNumber;

			// ����� ��������� ����� ���� float
			float FloatEps;
			// ����� ��������� ����� ���� double
			double DoubleEps;

			// ��������� c ��������� ����������� �� ����������
			#pragma pack(1)
			struct DEVICE_INITIAL_INFO
			{
				ULONG		Base;
				ULONG		BaseL;
				ULONG		Base1;
				ULONG		BaseL1;
				ULONG		Mem;
				ULONG		MemL;
				ULONG		Mem1;
				ULONG		MemL1;
				ULONG		Irq;
				ULONG		DeviceID;
				ULONG		DSPType;
				ULONG		Dma;
				ULONG		DmaDac;
				ULONG		DTA_REG;
				ULONG		IDMA_REG;
				ULONG		CMD_REG;
				ULONG		IRQ_RST;
				ULONG		DTA_ARRAY;
				ULONG		RDY_REG;
				ULONG		CFG_REG;
			};
			#pragma pack()
			// ��������� � ��������� ����������� �� �������� ����������
			DEVICE_INITIAL_INFO DeviceInitialInfo;

			// ������� �������������� ����������� Lusbapi USB ���������
			enum 	{
						E154_INDEX, E14_140_INDEX, E14_440_INDEX, E2010_INDEX, E2010B_INDEX,
						E_310_INDEX,
						E270_INDEX,								/*�� ��������������*/
						LTR010_INDEX, LTR021_INDEX,		/*�� ��������������*/
						SUPPORTED_USB_DEVICES_QUANTITY
					};

			// ID �������������� ����������� Lusbapi USB ���������
			enum 	{
						E14_440_ID	= 30,
						E14_140_ID	= 31,
						E2010_ID		= 32,
						E270_ID		= 33,
						LTR010_ID	= 36,
						LTR021_ID	= 37,
						E154_ID		= 38,
						E2010B_ID	= 39,
						E310_ID		= 77,

						ENUM_ALL_USB_DEVICE_ID = 0xFFFE,
						INVALID_USB_DEVICE_ID  = 0xFFFF
					};
			// ������ �������������� ID USB ���������
			DWORD DEVICES_ID_ARRAY[SUPPORTED_USB_DEVICES_QUANTITY];

			// ����� ������ � ��������� ������
			enum
			{
				SHORT_MODULE_NAME_STRING_LENGTH	= 0x7,
				LONG_MODULE_NAME_STRING_LENGTH	= 0x10
			};
         
		private:
			// ��������� ������� ������ Windows
			DWORD GetWindowsVersion(void);

			// �������� ������
			char ModuleName[LONG_MODULE_NAME_STRING_LENGTH];

			// ������ ��������� ���������������� �������� ��� USB ��������� (vendor request)
			enum
			{
				V_GET_USB_SPEED 		= 6,
				V_GET_MODULE_NAME  	= 11
			};

			// ������� ������ Windows
			enum 	{
						UNKNOWN_WINDOWS_VERSION,
						WINDOWS_32S,
						WINDOWS_95, WINDOWS_98_OR_LATER,
						WINDOWS_NT,
						WINDOWS_2000, WINDOWS_XP, WINDOWS_2003_SERVER,
						WINDOWS_VISTA, WINDOWS_7, WINDOWS_FUTURE
					};
	};

#endif


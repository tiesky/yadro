//---------------------------------------------------------------------------
#ifndef __E440APIH
#define __E440APIH
//---------------------------------------------------------------------------
	#include "LUsbBase.h"

	//---------------------------------------------------------------------------
	// ���������� ���������� ��� ������ � ������� E14-440
	//---------------------------------------------------------------------------
	class TLE440 : public ILE440, public TLUSBBASE
	{
		// ������� ������ ����������
		virtual BOOL WINAPI OpenLDevice(WORD VirtualSlot);
		virtual BOOL WINAPI CloseLDevice(void);
		virtual BOOL WINAPI ReleaseLInstance(void);
		// ��������� ����������� ���������� USB
		virtual HANDLE WINAPI GetModuleHandle(void);
		// ��������� �������� ������������� ������
		virtual BOOL WINAPI GetModuleName(PCHAR const ModuleName);
		// ��������� ������� �������� ������ ���� USB
		virtual BOOL WINAPI GetUsbSpeed(BYTE * const UsbSpeed);
		// ���������� ������� ������� ������������������ ������
		virtual BOOL WINAPI LowPowerMode(BOOL LowPowerFlag);
		// ������� ������ ������ � ��������� �������
		virtual BOOL WINAPI GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo);

		// ������� ������ � DSP
		virtual BOOL WINAPI RESET_MODULE(BYTE ResetFlag);
		virtual BOOL WINAPI LOAD_MODULE(PCHAR const FileName);
		virtual BOOL WINAPI TEST_MODULE(void);
		virtual BOOL WINAPI SEND_COMMAND(WORD Command);

		// ������� ��� ������ � ���
		virtual BOOL WINAPI GET_ADC_PARS(ADC_PARS_E440 * const AdcPars);
		virtual BOOL WINAPI SET_ADC_PARS(ADC_PARS_E440 * const AdcPars);
		virtual BOOL WINAPI START_ADC(void);
		virtual BOOL WINAPI STOP_ADC(void);
		virtual BOOL WINAPI ADC_KADR(SHORT * const Data);
		virtual BOOL WINAPI ADC_SAMPLE(SHORT * const AdcData, WORD AdcChannel);
		virtual BOOL WINAPI ReadData(IO_REQUEST_LUSBAPI * const ReadRequest);

		// ������� ��� ������ � ���
		virtual BOOL WINAPI GET_DAC_PARS(DAC_PARS_E440 * const DacPars);
		virtual BOOL WINAPI SET_DAC_PARS(DAC_PARS_E440 * const DacPars);
		virtual BOOL WINAPI START_DAC(void);
		virtual BOOL WINAPI STOP_DAC(void);
		virtual BOOL WINAPI WriteData(IO_REQUEST_LUSBAPI * const WriteRequest);
		virtual BOOL WINAPI DAC_SAMPLE(SHORT * const DacData, WORD DacChannel);

		// ������� ��� ������ � ��� �������
		virtual BOOL WINAPI ENABLE_TTL_OUT(BOOL EnableTtlOut);
		virtual BOOL WINAPI TTL_IN(WORD * const TtlIn);
		virtual BOOL WINAPI TTL_OUT(WORD TtlOut);

		// ������� ��� ������ ����
		virtual BOOL WINAPI ENABLE_FLASH_WRITE(BOOL EnableFlashWrite);
		virtual BOOL WINAPI READ_FLASH_WORD(WORD FlashAddress, SHORT * const FlashWord);
		virtual BOOL WINAPI WRITE_FLASH_WORD(WORD FlashAddress, SHORT FlashWord);

		// ������� ��� ������ �� ��������� ����������� �� ����
		virtual BOOL WINAPI GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E440 * const md);
		virtual BOOL WINAPI SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E440 * const md);

		// ������� ��� ������ � ������� DSP
		virtual BOOL WINAPI PUT_LBIOS_WORD(WORD Address, SHORT Data);
		virtual BOOL WINAPI GET_LBIOS_WORD(WORD Address, SHORT * const Data);
		virtual BOOL WINAPI PUT_DM_WORD(WORD Address, SHORT Data);
		virtual BOOL WINAPI GET_DM_WORD(WORD Address, SHORT * const Data);
		virtual BOOL WINAPI PUT_PM_WORD(WORD Address, long Data);
		virtual BOOL WINAPI GET_PM_WORD(WORD Address, long * const Data);
		virtual BOOL WINAPI PUT_DM_ARRAY(WORD BaseAddress, WORD NPoints, SHORT * const Data);
		virtual BOOL WINAPI GET_DM_ARRAY(WORD BaseAddress, WORD NPoints, SHORT * const Data);
		virtual BOOL WINAPI PUT_PM_ARRAY(WORD BaseAddress, WORD NPoints, long * const Data);
		virtual BOOL WINAPI GET_PM_ARRAY(WORD BaseAddress, WORD NPoints, long * const Data);

		// ������� ��� ������ � ����������� ����
		virtual BOOL WINAPI ERASE_BOOT_FLASH(void);
		virtual BOOL WINAPI PUT_ARRAY_BOOT_FLASH(DWORD BaseAddress, DWORD NBytes, BYTE *Data);
		virtual BOOL WINAPI GET_ARRAY_BOOT_FLASH(DWORD BaseAddress, DWORD NBytes, BYTE *Data);

		public:
			// ����������
			TLE440(HINSTANCE hInst);
			// ����������
			~TLE440();

		private:
			// ������� ��������� DSP ������ �� ������������ ����
			BOOL LoadModuleFromBootFlash(void);
			// ������� ��������� DSP ������ �� ������� ���������� Lusbapi
			BOOL LoadModuleFromResource(void);
			// ������� ��������� DSP ������ �� �����
			BOOL LoadModuleFromFile(PCHAR const FileName);
			// �������������� ������������ �������
			long *Transform_Buffer_E440(WORD PmPoints, long * const Buffer);
			// ������ � ������������ ������ (��������� ����� ���� ������)
			BOOL GetModuleDescriptor(void);
			BOOL PutModuleDescriptor(void);
			// ��������� ���������� � LBIOS
			BOOL GetDspDescriptor(void);
			// ��������� ���������� � �������� ����������������
			BOOL GetMcuDescriptor(void);
			// ��������� �������������� ��������� ������ ����������������
			BOOL SetMcuAttributes(WORD McuAttributes);
			// ������� ����������� �������� ��� ��� ������ � ������� ������ 'C'
			BOOL StopAdcForOldRevision(void);
			// ������� ����������� �������� ��� ��� ������ � ������� ������ 'C'
			BOOL StopDacForOldRevision(void);


			// ������������ ��������
			#pragma pack(1)
			// ��������� � ����������� � ������, ������� �������� � ��������� ����� ���� ������
			struct MODULE_DESCRIPTOR
			{
				BYTE SerialNumber[9];				// �������� �����
				BYTE Name[7];							// �������� ������
				BYTE Revision;							// ������� ������
				BYTE DspType[5];						// ��� �������������� DSP
				BYTE IsDacPresented; 				// ������ ������� ���
				ULONG QuartzFrequency; 				// ������� ������ � ��
				BYTE ReservedWord[13];				// ���������������
				SHORT AdcOffsetCoefs[ADC_CALIBR_COEFS_QUANTITY_E440];	// ��������	���: 4���������
				WORD AdcScaleCoefs[ADC_CALIBR_COEFS_QUANTITY_E440];	// ������� ���	: 4���������
				SHORT DacOffsetCoefs[DAC_CALIBR_COEFS_QUANTITY_E440];	// ��������	���: 2������ x 1��������
				WORD DacScaleCoefs[DAC_CALIBR_COEFS_QUANTITY_E440];	// ������� ���	: 2������ x 1��������
			};

			// ��������� � ����������� � ��������� � DSP (LBIOS)
			struct DSP_DESCRIPTOR
			{
				BYTE DspName[NAME_LINE_LENGTH_LUSBAPI];	// �������� DSP
				BYTE Version[10];									// ������ LBIOS
				BYTE Created[14];									// ���� ������ LBIOS
				BYTE Manufacturer[NAME_LINE_LENGTH_LUSBAPI];	// �����-������������� LBIOS
				BYTE Author[NAME_LINE_LENGTH_LUSBAPI];			// ����� LBIOS
				BYTE Comment[128];								// ������ �����������
			};

			// ��������� � ����������� � ��������� � ���������������� MCU
			struct FIRMWARE_DESCRIPTOR
			{
				BYTE McuName[NAME_LINE_LENGTH_LUSBAPI];	// �������� ����������������
				BYTE Version[10];									// ������ �������� MCU
				BYTE Created[14];									// ���� ������ �������� MCU
				BYTE Manufacturer[NAME_LINE_LENGTH_LUSBAPI];	// �����-������������� �������� MCU
				BYTE Author[NAME_LINE_LENGTH_LUSBAPI];			// ����� �������� MCU
				BYTE Comment[128];								// ������ �����������
			};
			//
			#pragma pack()

			// ������ ���������� ���������� ������ � ����
			BOOL EnableFlashWrite;
			// ������ ���������� ������� � ��������� �������� ���� ������
			BOOL IsServiceFlashWriting;
			// ��������� ��������� ����� ���� ������
			MODULE_DESCRIPTOR ModuleDescriptor;
			// ��������� � ����������� � LBIOS
			DSP_DESCRIPTOR DspDescriptor;
			// ��������� � ����������� � �������� ����������������
			FIRMWARE_DESCRIPTOR FirmwareDescriptor;
			// ��������� � ����������� ���������������� ���
			ADC_PARS_E440 AdcPars;
			// ��������� � ����������� ���������������� ���
			DAC_PARS_E440 DacPars;
			// ����� ������ �������� ���� � LBIOS � ������ ��������
			WORD ProgramBaseAddress;
			// ����� ������ �������� ����� ������ � ������ ��������
			WORD DataBaseAddress;
			// ������ ������� �������� �������� ����� ������
			BOOL EnableTtlOut;
			// �������������� ��������� ������ MCU
			WORD McuAttributes;
			// ��������� ��������� ��� ������� ������
//			IO_REQUEST_LUSBAPI IoReq;

			// ������ ��������� ������ LBIOS
			enum 	{
						C_TEST,
						C_ENABLE_FLASH_WRITE, C_READ_FLASH_WORD, C_WRITE_FLASH_WORD,
						C_START_ADC, C_STOP_ADC, C_ADC_KADR, C_ADC_SAMPLE,
						C_START_DAC, C_STOP_DAC, C_DAC_SAMPLE,
						C_ENABLE_TTL_OUT, C_TTL_IN, C_TTL_OUT,
						C_NALADKA_MODE,
						C_LAST_COMMAND
					};

			// ������ ��������� ���������������� �������� ��� USB (vendor request)
			enum 	{
						V_RESET_MODULE,
						V_PUT_ARRAY, V_GET_ARRAY,
						V_START_ADC, V_START_DAC,
						V_COMMAND_IRQ,
						V_GET_MCU_DESCRIPTOR, V_SET_MCU_ATTRIBUTES,
						V_ERASE_BOOT_FLASH, V_PUT_ARRAY_BOOT_FLASH, V_GET_ARRAY_BOOT_FLASH,
						V_GET_MODULE_NAME,
						V_LOAD_LBIOS_FROM_FLASH
					};
			// �������������� ��������� ��� ���������������� MCU
			enum 	{
						NO_MCU_ATTRIBUTES				= 0x0,
						ENABLE_RESET_BULK_PIPES		= (0x1 << 0x0)
					};
			// ����� ������� � ������ DSP
			enum 	{
						DSP_DM							= 0x4000,		// � ������ ������
						DSP_PM							= 0x0000			// � ������ ��������
					};
			// ������������ ������ ����� ������, ������� ����� ����������
			// ���������� �� ����������� ������ USB (control pipe)
			enum 	{	MAX_USB_CONTROL_PIPE_BLOCK	= 4096 };
			// ������� ����� ������ ���������� ��� ������ � ������� E14-440
//			enum 	{	ERROR_ID_BASE					= 100 };
	};

	#define	MAKE_PM_BUFFER_E440(PmPoints, Buffer)		(WORD)(PmPoints), Transform_Buffer_E440((WORD)(PmPoints), (long *)(Buffer))

#endif


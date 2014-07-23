//---------------------------------------------------------------------------
#ifndef __E154API__
#define __E154API__
//---------------------------------------------------------------------------
	#include "LUsbBase.h"

	//---------------------------------------------------------------------------
	// ���������� ���������� ��� ������ � ������� E14-154
	//---------------------------------------------------------------------------
	class TLE154 : public ILE154, public TLUSBBASE
	{
		// ----------------- ������� ������ ���������� --------------------------
		// ��������� ����������� ���� ��� ������� � USB ������
		virtual BOOL WINAPI OpenLDevice(WORD VirtualSlot);
		// ����������� ������� ����������� ����
		virtual BOOL WINAPI CloseLDevice(void);
		// ����������� ��������� �� ��������� ����������
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

		// ---------------- ������� ��� ������ � ��� ----------------------------
		// ��������� ������� ���������� ������ ���
		virtual BOOL WINAPI GET_ADC_PARS(ADC_PARS_E154 * const AdcPars);
		// ���������� ��������� ���������� ������ ���
		virtual BOOL WINAPI SET_ADC_PARS(ADC_PARS_E154 * const AdcPars);
		// ����� ������ ���
		virtual BOOL WINAPI START_ADC(void);
		// ���� ������ ���
		virtual BOOL WINAPI STOP_ADC(void);
		// ���� ����� �������� � ��� ������
		virtual BOOL WINAPI ADC_KADR(SHORT * const Data);
		// ����������� ���� � ��������� ����������� ������ ��� ������
		virtual BOOL WINAPI ADC_SAMPLE(SHORT * const Data, WORD Channel);
		// ��������� ������ ������ � ��� ������
		virtual BOOL WINAPI ReadData(IO_REQUEST_LUSBAPI * const ReadRequest);
      virtual BOOL WINAPI ProcessArray(SHORT *src, double *dest, DWORD size, BOOL calibr, BOOL volt);
      virtual BOOL WINAPI ProcessOnePoint(SHORT src, double *dest, DWORD channel, BOOL calibr, BOOL volt);
      virtual BOOL WINAPI FIFO_STATUS(DWORD *FifoOverflowFlag, double *FifoMaxPercentLoad, DWORD *FifoSize, DWORD *MaxFifoBytesUsed);

		// ------------------- ������� ��� ������ � ��� --------------------------
		// ����������� ����� �� �������� ����� ���
		virtual BOOL WINAPI DAC_SAMPLE(SHORT * const DacData, WORD DacChannel);
  		virtual BOOL WINAPI DAC_SAMPLE_VOLT(double  const DacData, BOOL calibr);

		// --------------- ������� ��� ������ � ��� ������� ----------------------
		// ������� ���������� �������� ����� �������� ��������� �������
		virtual BOOL WINAPI ENABLE_TTL_OUT(BOOL EnableTtlOut);
		// ������� ������ ������� ����� �������� ��������� �������
		virtual BOOL WINAPI TTL_IN(WORD * const TtlIn);
		// ������� ������ �� �������� ����� �������� ��������� �������
		virtual BOOL WINAPI TTL_OUT(WORD TtlOut);

		// ------- ������� ��� ������ � ���������������� ����������� ����  -------
		// ����������/���������� ������ ������ � ���������������� ������� ����
		virtual BOOL WINAPI ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled);
		// ������� ����� �� ���������������� ������� ����
		virtual BOOL WINAPI READ_FLASH_ARRAY(BYTE * const UserFlash);
		// ������ ����� � ���������������� ������� ����
		virtual BOOL WINAPI WRITE_FLASH_ARRAY(BYTE * const UserFlash);

		// ------- ������� ��� ������ �� ��������� ����������� �� ���� -----------
		// ������� ��������� ���������� � ������ �� ����
		virtual BOOL WINAPI GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E154 * const md);
		// ������� ��������� ���������� � ������ � ����
		virtual BOOL WINAPI SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E154 * const md);

		// ----------- ������� ��� ������� ������ � ���������������� -------------
		virtual 	BOOL WINAPI GetArray(BYTE * const Buffer, WORD Size, WORD Address);
		virtual 	BOOL WINAPI PutArray(BYTE * const Buffer, WORD Size, WORD Address);


		//
		public:
			// ����������
			TLE154(HINSTANCE hInst);
			// ����������
			~TLE154();

		//
		private :
			// ������������ ��������
			#pragma pack(1)
			// ���������, ���������� ���������� � �����, ������� �������� � ����. ����
			struct MODULE_DESCRIPTOR
			{
				BYTE Name[11];				    		// �������� ������
				BYTE SerialNumber[9];    			// �������� �����
				BYTE Revision;					    	// ������� �����
				BYTE CpuType[11];						// ��� �������������� MCU
				long QuartzFrequency;				// ������� ������ � ��
				BYTE IsDacPresented;					// ������ ������� ���
				double AdcOffsetCoefs[ADC_CALIBR_COEFS_QUANTITY_E154];	// ��������	���: 4���������
				double AdcScaleCoefs[ADC_CALIBR_COEFS_QUANTITY_E154];		// ������� ���	: 4���������
				double DacOffsetCoefs[DAC_CALIBR_COEFS_QUANTITY_E154];	// ��������	���: 2������ x 1��������
				double DacScaleCoefs[DAC_CALIBR_COEFS_QUANTITY_E154];		// ������� ���	: 2������ x 1��������
			};
			//
			#pragma pack()

			// ������ ���������� ���������� ������ � ����
			BOOL IsUserFlashWriteEnabled;
			// ������ ���������� ������� � ��������� �������� ���� ������
			BOOL EnableSystemFlashWrite;
			// ��������� ���������� ���� ��� ������ E-154
			MODULE_DESCRIPTOR ModuleDescriptor;
			// ��������� ��������� � ����������� ���������������� ���
			ADC_PARS_E154 AdcPars;
			// ������ ����� ������
			BOOL IsDataAcquisitionInProgress;
			// ����� ������ Firmware ����������������
			double FirmwareVersionNumber;
         WORD	RateScale;
         WORD	DescriptorReadFlag;
         MODULE_DESCRIPTION_E154 E154DescriptionStruct;
         ADC_PARS_E154 AdcConfigStruct;

			// ������� ����� ������ ���������� ��� ������ � ������� E14-154
			enum 	{	ERROR_ID_BASE				= 300 };
			// ������������ ������ ����� ������, ������� ����� ����������
			// ���������� �� ����������� ������ USB (control pipe)
			enum 	{	MAX_USB_CONTROL_PIPE_BLOCK	= 4096 };
			// ���������
			enum
			{
				ADC_PARS_BASE						= 0x0060,
				ADC_ONCE_FLAG						= (ADC_PARS_BASE + 136),
				ENABLE_FLASH_WRITE_FLAG	 		= (ADC_PARS_BASE + 137),
				DOUT_REGISTER						= 0x0400,
				DIN_REGISTER						= 0x0400,
				DOUT_ENABLE_REGISTER		 		= 0x0402,
				ADC_DATA_REGISTER			 		= 0x0410,
				ADC_CHANNEL_SELECT_REGISTER	= 0x0412,
				ADC_START_REGISTER				= 0x0413,
				DAC_DATA_REGISTER					= 0x0420,
				SUSPEND_MODE_FLAG					= 0x0430,
				DATA_FLASH_BASE					= 0x0800,
				CODE_FLASH_BASE					= 0x1000,
				LBIOS_VERSION						= 0x1080,
				FIFO_STATUS_ADDRESS				= 0x1090,
				DESCRIPTOR_BASE					= 0x2780,
				RAM									= 0x8000
			};

			// ������ ��������� ���������������� �������� ��� USB (vendor request)
			enum
			{
					V_PUT_ARRAY      	= 0x0001,
					V_GET_ARRAY			= 0x0002,
					V_START_ADC	 		= 0x0003,
					V_STOP_ADC			= 0x0004,
					V_START_ADC_ONCE	= 0x0005,
					V_GET_MODULE_NAME	= 0x000B
			};

			// ������� ������/������ ��������� ������������ ������
			// � ��� ����� �/�� ������ MCU
//			BOOL GetArray (BYTE * const Buffer, WORD Size, WORD Address);
//			BOOL PutArray(BYTE * const Buffer, WORD Size, WORD Address);
			// --------------------------------------------------------------------

			// �������������� �������
			BOOL SetSuspendModeFlag(BOOL SuspendModeFlag);
			// ������� �������� ����������� �����
			BYTE CRC8ADD(BYTE a, BYTE b);
			BYTE CRC8CALC(BYTE *Buffer, WORD Size);
			// ������� ������� � ���������� ����������� ������         
			BOOL PackModuleDescriptor(BYTE * const Buffer, MODULE_DESCRIPTOR * const ModuleDescriptor);
			BOOL UnpackModuleDescriptor(BYTE * const Buffer, MODULE_DESCRIPTOR * const ModuleDescriptor);
};
#endif


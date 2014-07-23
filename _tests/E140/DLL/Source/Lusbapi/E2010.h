//------------------------------------------------------------------------------
#ifndef __E2010H__
#define __E2010H__

	#include "LUsbBase.h"

	//---------------------------------------------------------------------------
	// ���������� ���������� ��� ������ � ������� E20-10
	//---------------------------------------------------------------------------
	class TLE2010 : public ILE2010, public TLUSBBASE
	{
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
		// ���������� ������� ������� ������������������ ������
		virtual BOOL WINAPI LowPowerMode(BOOL LowPowerFlag);
		// ������� ������ ������ � ��������� �������
		virtual BOOL WINAPI GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo);

		// �������� � �������� ����
		virtual BOOL WINAPI LOAD_MODULE(PCHAR const FileName);
		virtual BOOL WINAPI TEST_MODULE(WORD TestModeMask);

		// ������ � ���
		virtual BOOL WINAPI GET_ADC_PARS(ADC_PARS_E2010 * const AdcPars);
		virtual BOOL WINAPI SET_ADC_PARS(ADC_PARS_E2010 * const AdcPars);
		virtual BOOL WINAPI START_ADC(void);
		virtual BOOL WINAPI STOP_ADC(void);
		virtual BOOL WINAPI GET_DATA_STATE(DATA_STATE_E2010 * const DataState);
		virtual BOOL WINAPI ReadData(IO_REQUEST_LUSBAPI * const ReadRequest);

		// ������ � ���
		virtual BOOL WINAPI DAC_SAMPLE(SHORT * const DacData, WORD DacChannel);

		// ������ � ��������� �������
		virtual BOOL WINAPI ENABLE_TTL_OUT(BOOL EnableTtlOut);
		virtual BOOL WINAPI TTL_IN (WORD * const TtlIn);
		virtual BOOL WINAPI TTL_OUT(WORD TtlOut);

		// ������� ��� ������ � ���������������� ����������� ����
		virtual BOOL WINAPI ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled);
		virtual BOOL WINAPI READ_FLASH_ARRAY(USER_FLASH_E2010 * const UserFlash);
		virtual BOOL WINAPI WRITE_FLASH_ARRAY(USER_FLASH_E2010 * const UserFlash);

		// ���������� � ������
		virtual BOOL WINAPI GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E2010 * const ModuleDescription);
		virtual BOOL WINAPI SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E2010 * const ModuleDescription);

		// ��������� ���������� ����������
//		virtual BOOL WINAPI GET_DEBUG_INFO(WORD * const DebugInfo);

		public :
			// �����������/����������
			TLE2010(HINSTANCE hInst);
			virtual ~TLE2010();

		private:
			#pragma pack(1)
			// ��������� ��������� � ��������� ������
			struct MODULE_DESCRIPTOR
			{
				BYTE ModuleName[16];	 		// �������� ������
				BYTE SerialNumber[16];		// �������� ����� ������
				BYTE McuType[16];				// ��� ������������� ����������������
				DWORD ClockRate;				// �������� ������� ������ ���������������� � ��
				BYTE Revision;					// ������� ������ (��������� ������)
				BYTE IsDacPresented;			// ������� ������� ��� �� �����
				float AdcOffsetCoefs[ADC_CALIBR_COEFS_QUANTITY_E2010];	// ��������	���: 4������ x 3���������
				float AdcScaleCoefs[ADC_CALIBR_COEFS_QUANTITY_E2010];		// �������  ���: 4������ x 3���������
				float DacOffsetCoefs[DAC_CALIBR_COEFS_QUANTITY_E2010];	// ��������	���: 2������ x 1��������
				float DacScaleCoefs[DAC_CALIBR_COEFS_QUANTITY_E2010];		// �������  ���: 2������ x 1��������
				BYTE Modification;			// ���������� ������ (�����);
				BYTE Reserved[254-167];		// ���������������
				WORD CRC16;						// ����������� ����� ���������
			};
			// ��������� � ����������� �� �������� ����������������
			struct FIRMWARE_DESCRIPTOR
			{
				BYTE McuName[NAME_LINE_LENGTH_LUSBAPI]; 		// �������� ����������������
				BYTE Version[10];										// ������ �������� ����������������
				BYTE Created[14];										// ���� ������ �������� ����������������
				BYTE Manufacturer[NAME_LINE_LENGTH_LUSBAPI];	// ������������� �������� ����������������
				BYTE Author[NAME_LINE_LENGTH_LUSBAPI];		 	// ����� �������� ����������������
				BYTE Comment[128];									// ������ �����������
			};
			// ��������� � ����������� �� ���������� ����������������
			struct BOOT_LOADER_DESCRIPTOR
			{
				BYTE McuName[20]; 							 		// �������� ����������������
				BYTE Version[10];										// ������ ���������� ����������������
				BYTE Created[14];										// ���� ������ ���������� ����������������
				BYTE Manufacturer[15];								// ������������� ���������� ����������������
				BYTE Author[20];		 								// ����� ���������� ����������������
//				BYTE Comment[128];									// ������ �����������
			};
			// ��������� � ��������������� ����������� ������������� ����� ������
			struct EXTRA_SYNCHRO_PARS
			{
				DWORD StartDelay; 					// �������� ������ ����� ������ � ������ �������� ���
				DWORD StopAfterNKadrs;				// ������� ����� ������ ����� ����������� ����� ���-�� ��������� ������ �������� ���
				WORD	SynchroAdMode;					// ��������� ���������� ������������: ����������, �����, ������� ��� �������
				SHORT SynchroAdPorog;				// ����� ������������ ��� ��� ���������� �������������
				BYTE IsBlockDataMarkerEnabled;	// ������������ ����� ������ (��������, ��� ���������� ������������� ����� �� ������)
			};
			// ��������� � ����������� � ������� ��������� �������� ����� ������
			struct DATA_STATUS
			{
				BYTE Status;
				DWORD CurBufferFilling;
				DWORD MaxOfBufferFilling;
				DWORD BufferSize;
			};
			#pragma pack()

			// ���������� ����
			BOOL WINAPI ResetPld(void);
			BOOL WINAPI StartPld(void);
			BOOL WINAPI LoadPld(BYTE * const PldData, DWORD NBytes);
			// ������ �������� ������ ������ ������         
			void WINAPI GetSynchroParsForRevA(BYTE ModeParam, ADC_PARS_E2010 * const AdcPars);
			void WINAPI GetSynchroParsForRevBandAbove(BYTE ModeParam, ADC_PARS_E2010 * const AdcPars);
			// ������ �������������� ���������� ������������� ����� ������
			BOOL WINAPI GetExtraSynchroParsForRevBandAbove(ADC_PARS_E2010 * const AdcPars);
			// ������������ ������ ������ ������
			void WINAPI MakeSynchroParsForRevA(BYTE &ModeParam, ADC_PARS_E2010 * const AdcPars);
			void WINAPI MakeSynchroParsForRevBandAbove(BYTE &ModeParam, ADC_PARS_E2010 * const AdcPars);
			// �������������� ��������� ������������� ����� ������
			BOOL WINAPI SetExtraSynchroParsForRevBandAbove(ADC_PARS_E2010 * const AdcPars);
			// ������ ����������� ������
			BOOL WINAPI GetModuleDescriptor(MODULE_DESCRIPTOR const * ModuleDescriptor);
			// ������� ������� � ������ ����������������
			BOOL WINAPI GetArray(DWORD Address, BYTE * const Data, DWORD Size);
			BOOL WINAPI PutArray(DWORD Address, BYTE * const Data, DWORD Size);
			// ���������� ����������� ������
			BOOL WINAPI SaveModuleDescriptor(void);
			// ������� ������ � �������������� �������� ������
			BOOL WINAPI SetTestMode(WORD TestModeMask);
			// ������� ������� ������ ������ ����������������
			BOOL WINAPI RunMcuApplication(DWORD Address, WORD BitParam = NO_SWITCH_PARAMS);
			//         
			BOOL WINAPI GET_DEBUG_INFO(WORD * const DebugInfo);

			// ������� ���������� �������� � ��������
			enum
			{
				PLD_INFO_SIZE					= (2*256),
				ADC_PARAM_SIZE					= (3+2+2+256),
				ADC_CALIBR_COEFS_QUANTITY	= (2*ADC_CALIBR_COEFS_QUANTITY_E2010)
			};

			// ��������� c ��������� ����������� ������
			MODULE_DESCRIPTOR ModuleDescriptor;
			// ��������� � ����������� � �������� ����������������
			FIRMWARE_DESCRIPTOR FirmwareDescriptor;
			// ��������� � ����������� �� ���������� ����������������
			BOOT_LOADER_DESCRIPTOR BootLoaderDescriptor;
			// ��������� � ��������������� ����������� ������������� ����� ������
			EXTRA_SYNCHRO_PARS ExtraSynchroPars;
			// ��������� � ����������� � ������� ��������� �������� ����� ������
			DATA_STATUS LocDataStatus;
			// ���������� ��������� � �������
			BYTE PldInfo[PLD_INFO_SIZE];  			// ���������� �� ������������ ����
			BYTE AdcParamsArray[ADC_PARAM_SIZE];	// ��������� ��������� ��� ������ � ����������� ���
			WORD AdcCalibrCoefsArray[ADC_CALIBR_COEFS_QUANTITY];	// ��������� ��������� ��� ������ ����������������� �������������� ���
			BOOL IsMcuApplicationActive;				// ����� ������ ����������������
			BOOL IsDataAcquisitionInProgress;		// ������ ����� ������
			BOOL IsUserFlashWriteEnabled;				// ������ ���������� ������ � ��������������� ����
			double FirmwareVersionNumber;				// ����� ������ Firmware ����������������

			// ���������������� ������� USB
			enum
			{
				V_RESET_PLD        	= 0,
				V_PUT_ARRAY         	= 1,
				V_GET_ARRAY         	= 2,
				V_START_PLD         	= 3,
				V_START_ADC         	= 4,
				V_STOP_ADC          	= 5,
				V_TEST_PLD         	= 7,
				V_CALL_APPLICATION  	= 15
			};
			// ������ ������������ ���������� ����
			enum	{	EP1K10_SIZE		= ( 22*1024 + 100) };
			// ������������ ������ ����� ������, ������� ����� ����������
			// ���������� �� ����������� ������ USB (control pipe)
			enum 	{	MAX_USB_CONTROL_PIPE_BLOCK	= 4096 };
			// ��������� ������������ �������
			enum
			{
				MAX_START_DELAY			= (16777214),
				MAX_STOP_AFTER_NKADRS	= (16777215)
			};

			// ����������� ������ ��� ������� � ��������� �������� ������
			enum
			{
				SEL_TEST_MODE			= (0x81000000L),
				SEL_AVR_DM				= (0x82000000L),
				SEL_AVR_PM				= (0x83000000L),
				SEL_DIO_PARAM			= (0x84000000L),
				SEL_DIO_DATA		  	= (0x85000000L),
				SEL_ADC_PARAM		  	= (0x86000000L),
//				SEL_ADC_DATA		  	= (0x87000000L),
				SEL_BULK_REQ_SIZE	  	= (0x88000000L),
				SEL_DAC_DATA		  	= (0x89000000L),
				SEL_ADC_CALIBR_KOEFS	= (0x8A000000L),
				SEL_PLD_DATA		  	= (0x8B000000L),
				SEL_DEBUG_INFO		  	= (0x8F000000L)
			};

			// ��������� ������ ������ �������� ����������������
			enum
			{
				FIRMWARE_START_ADDRESS				= (SEL_AVR_PM | 0x0000L),
				USER_FLASH_ADDRESS					= (SEL_AVR_PM | 0x2D00L),
				FIRMWARE_DESCRIPTOR_ADDRESS		= (SEL_AVR_PM | 0x2F00L),
				MODULE_DESCRIPTOR_ADDRESS			= (SEL_AVR_PM | 0x3000L),
				BOOT_LOADER_START_ADDRESS 			= (SEL_AVR_PM | 0x3C00L),
				BOOT_LOADER_DESCRIPTOR_ADDRESS	= (SEL_AVR_PM | 0x3FB0L)
			};
			// ��������� ������ ���������� � ������ ������ ����������������
			enum
			{
				DATA_STATE_ADDRESS					= (SEL_AVR_DM | (0x0150L + 0x00L)),
				EXTRA_SYNCHRO_PARS_ADDRESS			= (SEL_AVR_DM | (0x0150L + sizeof(DATA_STATUS))),
				ADC_CORRECTION_ADDRESS				= (SEL_AVR_DM | (0x0150L + sizeof(DATA_STATUS) + sizeof(EXTRA_SYNCHRO_PARS))),
				LUSBAPI_OR_LCOMP_ADDRESS			= (SEL_AVR_DM | (0x0150L + sizeof(DATA_STATUS) + sizeof(EXTRA_SYNCHRO_PARS) + 0x1))
			};
			// ��������� ������� ���������, ������������ ��� ������������ ������
			// ������ ��������� ����������������: "���������" ��� "����������'
			enum
			{
				NO_SWITCH_PARAMS						= 0x0,	// ��� ����������
				REINIT_SWITCH_PARAM					= 0x1		// ��� �������� � ����� "����������" ����������
																		// ������ ����������������� ������ E20-10
			};
			// ��������� ���������
			enum
			{
				INVALID_PLD_FIRMWARE_VERSION = 0xFFFFFFFF,
				MASTER_QUARTZ = 60000							// � ���					            
			};
			// ������� ����� ������ ���������� �������������
			enum
			{
				AD_CH0, AD_CH1,  
				AD_M0, AD_M1,
				AD_ENA = 7
			};
	};

#endif


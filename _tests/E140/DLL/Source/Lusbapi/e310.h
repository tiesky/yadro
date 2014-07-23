//---------------------------------------------------------------------------
#ifndef __E310API__
#define __E310API__
//---------------------------------------------------------------------------
	#include "LUsbBase.h"

	//---------------------------------------------------------------------------
	// ���������� ���������� ��� ������ � ������� E-310
	//---------------------------------------------------------------------------
	class TLE310 : public ILE310, public TLUSBBASE
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

		// ---------------- ������� ��� ������ � ����������� ----------------------------
		// ������� ������ ���������� ������ ����������
		virtual BOOL WINAPI GET_GENERATOR_PARS(GENERATOR_PARS_E310 * const GenPars);
		// ������� ��������� g��������� ������ DDS ������
		virtual BOOL WINAPI SET_GENERATOR_PARS(GENERATOR_PARS_E310 * const GenPars);
		// ����� ������ ����������
		virtual BOOL WINAPI START_GENERATOR(void);
		// ������� ������ ����������
		virtual BOOL WINAPI STOP_GENERATOR(void);

		// ---------- ������� ��� ������ � ������������� (FM) -------------------
		// ��������� ������� ���������� ������ ����������� (FM)
		virtual BOOL WINAPI GET_FM_PARS(FM_PARS_E310 * const FmPars);
		// ��������� ��������� ���������� ������ ����������� (FM)
		virtual BOOL WINAPI SET_FM_PARS(FM_PARS_E310 * const FmPars);
		// ����� ������ ����������� (FM)
		virtual BOOL WINAPI START_FM(void);
		// ������� ������ ����������� (FM)
		virtual BOOL WINAPI STOP_FM(void);
		// ���������� ������� ��������� �������
		virtual BOOL WINAPI FM_SAMPLE(FM_SAMPLE_E310 * const FmSample);

		// ------------------ ������� ��� ������ ��� ---------------------------
		// ��������� ������� ���������� ������ ���
		virtual BOOL WINAPI GET_ADC_PARS(ADC_PARS_E310 * const AdcPars);
		// ��������� ��������� ���������� ������ ���
		virtual BOOL WINAPI SET_ADC_PARS(ADC_PARS_E310 * const AdcPars);
		// ���������� ������� �������� � ���
		virtual BOOL WINAPI GET_ADC_DATA(ADC_DATA_E310 * const AdcData);

		// ---------- ������� ��� ������ � ��������� ������� -------------------
		// ���������������� �������� �����: ���� ��� �����
		virtual BOOL WINAPI CONFIG_TTL_LINES(WORD Pattern, BOOL AddTtlLinesEna);
		// ������ ��������� �������� ������� �����
		virtual BOOL WINAPI TTL_IN (WORD * const TtlIn);
		// ��������� ��������� �������� �������� �����
		virtual BOOL WINAPI TTL_OUT(WORD * const TtlOut);

		// ------- ������� ��� ������ � ���������������� ����������� ����  -------
		// ����������/���������� ������ ������ � ���������������� ������� ����
		virtual BOOL WINAPI ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled);
		// ������� ���������������� ������� ����
		virtual BOOL WINAPI READ_FLASH_ARRAY(USER_FLASH_E310 * const UserFlash);
		// ������ ���������������� ������� ����
		virtual BOOL WINAPI WRITE_FLASH_ARRAY(USER_FLASH_E310 * const UserFlash);

		// ------- ������� ��� ������ �� ��������� ����������� �� ���� -----------
		// ������� ��������� ���������� � ������ �� ����
		virtual BOOL WINAPI GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E310 * const md);
		// ������� ��������� ���������� � ������ � ����
		virtual BOOL WINAPI SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E310 * const md);

		// ----------- ������� ��� ������� ������ � ���������������� -------------
/*		virtual BOOL WINAPI GetArray(BYTE * const Data, WORD Size, WORD Address);
		virtual BOOL WINAPI PutArray(BYTE * const Data, WORD Size, WORD Address);*/

		//
		public:
			// ����������
			TLE310(HINSTANCE hInst);
			// ����������
			~TLE310();

		//
		private :
			// ������������ ��������
			#pragma pack(1)

			// ��������� � ���������� � ������, ������� �������� � ��������������� ����
			struct MODULE_DESCRIPTOR
			{
				BYTE ModuleName[16];    		// �������� ������
				BYTE SerialNumber[SERIAL_NUMBER_STRING_LENGTH_LUSBAPI];	// �������� ����� ������
				BYTE McuName[NAME_LINE_LENGTH_LUSBAPI];						// �������� �������������� MCU
				BYTE Revision;					    									// ������� �����
				DWORD ClockRate;														// �������� ������� MCU � ��
				BYTE  Reserved[252-60];												// ���������������
				WORD CRC16;																// ����������� ����� ���������
			};
			// ��������� � ����������� �� �������� ����������������
			struct FIRMWARE_DESCRIPTOR
			{
				BYTE Version[10];										// ������ �������� ����������������
				BYTE Created[14];										// ���� ������ �������� ����������������
				BYTE Manufacturer[NAME_LINE_LENGTH_LUSBAPI];	// ������������� �������� ����������������
				BYTE Author[NAME_LINE_LENGTH_LUSBAPI];		 	// ����� �������� ����������������
				BYTE Comment[128];									// ������ �����������
			};
			// ��������� � ����������� �� ���������� ����������������
			struct BOOT_LOADER_DESCRIPTOR
			{
				BYTE Version[10];										// ������ ���������� ����������������
				BYTE Created[14];										// ���� ������ ���������� ����������������
				BYTE Manufacturer[NAME_LINE_LENGTH_LUSBAPI];	// ������������� ���������� ����������������
				BYTE Author[NAME_LINE_LENGTH_LUSBAPI];// ����� ���������� ����������������
//				BYTE Comment[128];									// ������ �����������
			};

			// ��������� ��������� � ����������� ���������������� 'DDS'
			struct DDS_PARS
			{
				union CONTROL_REG
				{
					struct
					{
						WORD Reserved0					: 2;
						WORD SYNCOUTEN					: 1;
						WORD SYNCSEL					: 1;
						WORD Reserved1					: 1;
						WORD INT_OR_EXT_INCR			: 1;
						WORD Reserved2					: 2;
						WORD MSBOUTEN					: 1;
						WORD SIN_OR_TRI				: 1;
						WORD DAC_ENA					: 1;
						WORD B24 						: 1;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} ControlReg;

				union NUMBER_OF_INCREMENTS_REG
				{
					struct
					{
						WORD Value12bit				: 12;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} NumberOfIncrementsReg;

				union LOWER_DELTA_FREQUENCY_REG
				{
					struct
					{
						WORD Value12bit				: 12;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} LowerDeltaFrequencyReg;

				union HIGHER_DELTA_FREQUENCY_REG
				{
					struct
					{
						WORD Value11bit				: 11;
						WORD Sign						: 1;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} HigherDeltaFrequencyReg;

				union INCREMENT_INTERVAL_REG
				{
					struct
					{
						WORD Value11bit				: 11;
						WORD Multiplier				: 2;
						WORD ADDR 						: 3;
					} BitFields;
					WORD Value;
				} IncrementIntervalReg;

				union LOWER_START_FREQUENCY_REG
				{
					struct
					{
						WORD Value12bit				: 12;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} LowerStartFrequencyReg;

				union HIGHER_START_FREQUENCY_REG
				{
					struct
					{
						WORD Value12bit				: 12;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} HigherStartFrequencyReg;

				union LOWER_STOP_FREQUENCY_REG
				{
					struct
					{
						WORD Value12bit				: 12;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} LowerStopFrequencyReg;

				union HIGHER_STOP_FREQUENCY_REG
				{
					struct
					{
						WORD Value12bit				: 12;
						WORD ADDR 						: 4;
					} BitFields;
					WORD Value;
				} HigherStopFrequencyReg;

				// ��� ���������������� �������: ����, ����������� � �.�.
				BYTE AutoScanType;
				// �������� ������������ ������� ���������� � ���
				double MasterClock;
				//
				BYTE Reserved[15];
			};

			// ��������� ����������� � ����������� '������������ P�������'
			union CONTROL_REG
			{
				struct
				{
					BYTE DDS_CTRL_SOURCE 					: 1;
					BYTE DDS_INTERRUPT_SOURCE				: 1;
					BYTE DDS_MCLK_SOURCE						: 1;
					BYTE DDS_10OHM_SIGNAL_OFFSET_SOURCE	: 1;
					BYTE DDS_SIGNAL_GAIN1					: 2;
					BYTE DDS_SIGNAL_GAIN2					: 2;
				} BitFields;
				BYTE Value;
			};

			// ��������� ��������� '�������� ��������������' ������ E-310
			struct DIGITAL_POTENTIOMETERS_PARS
			{
				// ��������� �������� ������� � 10 �� ������ ����������: �� -4B �� + 4�
				union OUTPUT_10OHM_OFFSET
				{
					struct
					{
						WORD Value8bit				: 8;
						WORD ADDR					: 1;
					} BitFields;
					WORD Value;
				} Output10OhmOffset;

				// �������� ���������� ������ �����������: �� -4B �� + 4�
				union FREQUENCY_METER_POROG
				{
					struct
					{
						WORD Value8bit				: 8;
						WORD ADDR					: 1;
					} BitFields;
					WORD Value;
				} FrequencyMeterPorog;
			};

			// ��������� � ����������� ������ ���������� ������� (FM)
			struct FM_PARS
			{
				BYTE	FmEna;					   // ������� ��������� ������ ���������� �������: �������� ��� ���
				BYTE  Mode;							// ����� ������ ���������� �������
				BYTE  InputDivider;				// ���������� ������� ��������� 1/8
				BYTE	BaseClockRateDivIndex;	// ������ �������� �������� �������
				DWORD ClockRate;					// �������� ������� �������� � ��
				DWORD BaseClockRate;				// ������� �������� ������� �������� � ��
				BYTE  Reserved[21];				// ���������������
			};

			// ��������� � ����������� ������ ������
			struct MODULE_PARS
			{
				DDS_PARS Dds;
				CONTROL_REG ControlReg;
				DIGITAL_POTENTIOMETERS_PARS DigitalPotentiometers;
				FM_PARS FrequencyMeasurement;
			};

			// ��������� � �������� ��������� ������� �������� �������
			struct FM_DATA
			{
				BYTE	IsActual;					// [out]	������� ���������������� ��������� ������
				DWORD PeriodCode;					// [out]	������ ����������� ������� � ������ ��������
				DWORD DutyCycleCode;				// [out]	���������� ����������� ������� � ������ ��������
				BYTE  Reserved[23];				// ���������������
			};

			// ����������� � ����������� ������������ �������� �����
			union TTL_CONFIG
			{
				struct
				{
					WORD CONFIG_PATTERN				: 11;
					WORD RESERVED						: 4;
					WORD ADD_TTL_LINES_ENA			: 1;
				} BitFields;
				WORD Value;
			};

			// ��������� � ����������� �������� �����
			struct TTL_PARS
			{
				BYTE 			Mode;					// ��� �������� ��������: ������������, ������ ��� ������
				TTL_CONFIG	TtlConfig;		  	// ������������ �������� �����: ���� ��� �����
				WORD			TtlIn;				// ������� ��������� �������� ������� �����
				WORD			TtlOut;				// ������� ��������� �������� �������� �����
				BYTE			Reserved[0x9];		// ���������������
			};

			// ��������� c ����������� ������ ���
			struct ADC_PARS
			{
				BYTE	AdcStartSource;	  		// �������� ������� ������� ���: ���������� ��� �������
				BYTE	ChannelsMask;				// ������� ����� �������� ������� (������� 4 ����)
				BYTE	Reserved[0x5];				// ���������������
			};

			// ��������� c ������� ���
			struct ADC_DATA
			{
				SHORT DataInCode[ADC_CHANNEL_QUANTITY_E310];	// ������ ������ � ��� � �����
			};
			#pragma pack()

			// -=== ��������� ������� ===-
			BOOL SetSuspendModeFlag(BOOL SuspendModeFlag);
			// ������� ��� ������� ������ � ���������������� -------------
			BOOL GetArray(BYTE * const Data, DWORD Size, DWORD Address);
			BOOL PutArray(BYTE * const Data, DWORD Size, DWORD Address);
			// ������ ����������� ������
			BOOL GetModuleDescriptor(void);
			// ���������� ����������� ������
			BOOL SaveModuleDescriptor(void);
			// ������� ������� ������ ������ ����������������
			BOOL RunMcuApplication(DWORD Address, WORD BitParam = NO_SWITCH_PARAMS);
			// -=========================-


			// -=== ��������� ���������� ===-
			// ����� ������ ����������������: BootLoader ��� Application
			BOOL IsMcuApplicationActive;
			// ������ ���������� ���������� ������ � ����
			BOOL IsUserFlashWriteEnabled;
			// ��������� c ��������� ����������� ������
			MODULE_DESCRIPTOR ModuleDescriptor;
			// ��������� � ����������� � �������� ����������������
			FIRMWARE_DESCRIPTOR FirmwareDescriptor;
			// ��������� � ����������� �� ���������� ����������������
			BOOT_LOADER_DESCRIPTOR BootLoaderDescriptor;
			// ��������� ��������� ���� ���������� ������ ������ E-310
			MODULE_PARS ModulePars;
			// ��������� ��������� � �������� ��������� ������� �������� �������
			FM_DATA LocFmSample;
			// ��������� � ����������� ������ ������
			TTL_PARS TtlPars;
			// ��������� c ����������� ������ ���
			ADC_PARS LocAdcPars;
			// ��������� c ������� ���
			ADC_DATA LocAdcData;
			// ������ ��������� ���������� � ���������� ������
			BOOL IsGeneratorBusy, IsFmBusy;
			// ����� ������ Firmware ����������������
			double FirmwareVersionNumber;
			// �������� ����������� ������������ ������� ���������� � ���
			double INTERNAL_DDS_MASTER_CLOCK;
			// ������� �������� ��� � �
			double ADC_INPUT_RANGE;
			// ������ �������� ��������� ������ ���������� � ��
			double GeneratorGaindBArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
			// ������ ��������� ������� �� 10 �� ������ � �
			double Output10OhmVArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
			// ������ ��������� �������� �� ������ 10 �� � ��
			double Output10OhmdBArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
			// ������ ��������� ������� �� 50 �� ������ � �
			double Output50OhmVArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
			// ������ ��������� �������� �� ������ 50 �� � ��
			double Output50OhmdBArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
			// ������ ����������� ���������� ���������� ��� ���������� DDS
			double DdsMultiplierArray[0x4];
			// ������������ ��� ������� DDS
			DWORD MAX_FREQ_DDS_CODE;
			// ������ ����������� �������� �������� �����
			// DDS_SIGNAL_GAIN1 � DDS_SIGNAL_GAIN1 � CONTROL_REG
			BYTE GeneratorGainIndexes[0x4][0x4];
			// ������� ��������
			BYTE Gain1Index[ANALOG_OUTPUT_GAINS_QUANTITY_E310], Gain2Index[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
			// -============================-

			// ������ ��������� DDS
			enum DDS_REGS_ADDR
			{
				CONTROL_BITS_ADDR_REG					= 0x0,
				NUMBER_OF_INCREMENTS_ADDR_REG			= 0x1,
				LOWER_DELTA_FREQUENCY_ADDR_REG		= 0x2,
				HIGHER_DELTA_FREQUENCY_ADDR_REG		= 0x3,
				CYCLE_INCREMENT_INTERVAL_ADDR_REG	= 0x4 >> 1,
				CLOCK_INCREMENT_INTERVAL_ADDR_REG	= 0x6 >> 1,
				LOWER_START_FREQUENCY_ADDR_REG		= 0xC,
				HIGHER_START_FREQUENCY_ADDR_REG		= 0xD
			};

			// ������� �������������� ����� ������ ��� ������� ������
			enum MODULE_START_MODE_INDEXES
			{
				GENERATOR_START_INDEX, FM_START_INDEX, INVALID_START_INDEX
			};
			// ������ ������ �� ���������� TTL_PARS
			enum
			{
				NO_TLL_LINES_MODE, TLL_LINES_CONFIG, TTL_LINES_IN, TTL_LINES_OUT, INVALID_TTL_LINES_MODE
			};

			// ��������� ������ � ������ ����������������
			enum
			{
				//
				BOOT_LOADER_START_ADDRESS 			= (0x00000000UL),		// �������� ����� ������ '����������' (BootLoader)
				BOOT_LOADER_DESCRIPTOR_ADDRESS	= (0x00102E00UL),
				MODULE_DESCRIPTOR_ADDRESS			= (0x00103000UL),
				USER_FLASH_ADDRESS					= (0x00103100UL),
				FIRMWARE_DESCRIPTOR_ADDRESS		= (0x00103300UL),
				FIRMWARE_START_ADDRESS				= (0x00103500UL),
				//
				MODULE_PARS_ADDRESS					= (0x00201400UL),
				FM_SAMPLE_ADDRESS						= (0x00201480UL),
				TTL_PARS_ADDRESS						= (0x002014A0UL),
				ADC_PARS_ADDRESS						= (0x002014B0UL),
				ADC_DATA_ADDRESS						= (0x002014B8UL)
			};

			// ������ ��������� ���������������� �������� ��� USB (vendor request)
			enum USB_VENDOR_REQUEST
			{
					//
					V_PUT_ARRAY      		= 0x01,
					V_GET_ARRAY				= 0x02,
					//
					V_START_ADC	 			= 0x04,
					V_STOP_ADC				= 0x05,
					//
					V_CALL_APPLICATION  	= 0x0F
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
				MCU_MASTER_CLOCK = 48000						// � ���
			};
			// ������������ ������ ����� ������, ������� ����� ����������
			// ���������� �� ����������� ������ USB (control pipe)
			enum 	{	MAX_USB_CONTROL_PIPE_BLOCK	= 4096 };
			// --------------------------------------------------------------------
};
#endif


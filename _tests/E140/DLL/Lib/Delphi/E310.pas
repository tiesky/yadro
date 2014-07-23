// *****************************************************************************
// *********************  � � � � � �    E - 3 1 0  ****************************
// *****************************************************************************

// ���������
const
	// -============= ��������� ==============-
	// ���� ������������ ���������������� �� ������� ��� ����������
	NO_CYCLIC_AUTOSCAN_E310							=	($00);
	CYCLIC_PILA_AUTO_SCAN_E310						=	($01);
	CYCLIC_TRIANGLE_AUTO_SCAN_E310				=	($02);
	INVALID_CYCLIC_AUTOSCAN_TYPE_E310			=	($03);

	// ���� ���������� ������� ����������
	WAVEFORM_CYCLE_INCREMENT_INTERVAL_E310		=	($00);
	CLOCK_PERIOD_INCREMENT_INTERVAL_E310		=	($01);
	INVALID_INCREMENT_INTERVAL_TYPE_E310		=	($02);

	// ��������� ������� ���������� ���������� ������� ����������
	INCREMENT_INTERVAL_MULTIPLIERS_001_E310	=	($00);
	INCREMENT_INTERVAL_MULTIPLIERS_005_E310	=	($01);
	INCREMENT_INTERVAL_MULTIPLIERS_100_E310	=	($02);
	INCREMENT_INTERVAL_MULTIPLIERS_500_E310	=	($03);
	INVALID_INCREMENT_INTERVAL_MULTIPLIER_E310=	($04);

	// ��������� ������� ��������� ����������� ��������� ��� ����������
	INTERNAL_MASTER_CLOCK_E310						=	($00);
	EXTERNAL_MASTER_CLOCK_E310						=	($01);
	INVALID_MASTER_CLOCK_SOURCE_E310				=	($02);

	// ��������� ���� ������������� ������� ����������
	AUTO_INCREMENT_E310								=	($00);
	CTRL_LINE_INCREMENT_E310						=	($01);
	INVALID_INCREMENT_TYPE_E310					=	($02);

	// ��������� ���� ����� CTRL
	INTERNAL_CTRL_LINE_E310							=	($00);
	EXTERNAL_CTRL_LINE_E310							=	($01);
	INVALID_CTRL_LINE_TYPE_E310					=	($02);

	// ��������� ���� ����� INTERRUPT
	INTERNAL_INTERRUPT_LINE_E310					=	($00);
	EXTERNAL_INTERRUPT_LINE_E310					=	($01);
	INVALID_INTERRUPT_LINE_TYPE_E310				=	($02);

	// ��������� ���� ������������ ��������� ������������� �� ����� "SYNCOUT"
	SYNCOUT_ON_EACH_INCREMENT_E310				=	($00);
	SYNCOUT_AT_END_OF_SCAN_E310					=	($01);
	INVALID_SYNCOUT_TYPES_E310						=	($02);

	// ��������� ���� ���������� �������� ��������
	TRIANGULAR_ANALOG_OUTPUT_E310					=	($00);
	SINUSOIDAL_ANALOG_OUTPUT_E310					=	($01);
	INVALID_ANALOG_OUTPUT_TYPE_E310				=	($02);

	// ��������� ������� �������� ��������� ������ ����������
	ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310			=	($00);
	ANALOG_OUTPUT_GAIN_PLUS_06_DB_E310			=	($01);
	ANALOG_OUTPUT_GAIN_PLUS_04_DB_E310			=	($02);
	ANALOG_OUTPUT_GAIN_PLUS_03_DB_E310			=	($03);
	ANALOG_OUTPUT_GAIN_00_DB_E310					=	($04);
	ANALOG_OUTPUT_GAIN_MINUS_02_DB_E310			=	($05);
	ANALOG_OUTPUT_GAIN_MINUS_03_DB_E310			=	($06);
	ANALOG_OUTPUT_GAIN_MINUS_06_DB_E310			=	($07);
	ANALOG_OUTPUT_GAIN_MINUS_09_DB_E310			=	($08);
	ANALOG_OUTPUT_GAIN_MINUS_12_DB_E310			=	($09);
	ANALOG_OUTPUT_GAIN_MINUS_14_DB_E310			=	($0A);
	ANALOG_OUTPUT_GAIN_MINUS_18_DB_E310			=	($0B);
	ANALOG_OUTPUT_GAIN_MINUS_21_DB_E310			=	($0C);
	ANALOG_OUTPUT_GAIN_MINUS_24_DB_E310			=	($0D);
	INVALID_ANALOG_OUTPUT_GAINS_INDEX_E310		=	($0E);
	ANALOG_OUTPUT_GAINS_QUANTITY_E310 = INVALID_ANALOG_OUTPUT_GAINS_INDEX_E310;

	// ��������� ������� �������� �� ������ 10 ��
	INTERNAL_OUTPUT_10_OHM_OFFSET_E310			=	($00);
	EXTERNAL_OUTPUT_10_OHM_OFFSET_E310			=	($01);
	INVALID_OUTPUT_10_OHM_OFFSET_SOURCE_E310	=	($02);
	// -======================================-

	// -============= ���������� FM ==============-
	// ��������� ������� �������� �������� ������� FM
	PERIOD_MODE_FM_E310								=	($00);
	GATE_MODE_FM_E310									=	($01);
	CAPTURE_MODE_FM_E310								=	($02);
	INVALID_FM_MODE_E310								=	($03);

	// ��������� ������� �������� �������� ������� ���������� ������� FM
	INPUT_DIVIDER_8_FM_E310							=	($00);
	INPUT_DIVIDER_1_FM_E310							=	($01);
	INVALID_INPUT_DIVIDER_FM_E310					=	($02);

	// ��������� ������� �������� ������� ������� FM
	BASE_CLOCK_DIV_01_INDEX_FM_E310				=	($00);
	BASE_CLOCK_DIV_02_INDEX_FM_E310				=	($01);
	BASE_CLOCK_DIV_04_INDEX_FM_E310				=	($02);
	BASE_CLOCK_DIV_08_INDEX_FM_E310				=	($03);
	BASE_CLOCK_DIV_16_INDEX_FM_E310				=	($04);
	BASE_CLOCK_DIV_32_INDEX_FM_E310				=	($05);
	BASE_CLOCK_DIV_64_INDEX_FM_E310				=	($06);
	INVALID_BASE_CLOCK_DIV_INDEX_FM_E310		=	($07);
	// -==========================================-

	// -============= � � � ==============-
	// ��������� ��������� ������� ���
	INTERNAL_ADC_START_E310							=	($00);
	EXTERNAL_ADC_START_E310							=	($01);
	INVALID_ADC_START_SOURCES_E310				=	($02);

	// ��������� ������ ��� ��� ������� ����� ������� ���
	ADC_CHANNEL_1_E310								=	($00);
	ADC_CHANNEL_2_E310								=	($01);
	ADC_CHANNEL_3_E310								=	($02);
	ADC_CHANNEL_4_E310								=	($03);
	INVALID_ADC_CHANNEL_BIT_NUMBER_E310			=	($04);
	ADC_CHANNEL_QUANTITY_E310 = INVALID_ADC_CHANNEL_BIT_NUMBER_E310;
	// -==================================-

	// ������� ��������� ������� ������ E-310
	REVISION_A_E310 									=	($00);
	INVALID_REVISION_E310							=	($01);
	// ���-�� ������� ������
	REVISIONS_QUANTITY_E310 						=	(INVALID_REVISION_E310);	

	// ������ ������� ����������������� ���� � ������
	USER_FLASH_SIZE_E310 							=	($200);							

	// ��������� ������� ������
	REVISIONS_E310 : array [0..(REVISIONS_QUANTITY_E310-1)] of char = ( 'A' );


// ����
type
	// ��������� � ����������� �� ������ E-310
	MODULE_DESCRIPTION_E310 = packed record
		Module 		: MODULE_INFO_LUSBAPI;		  	// ����� ���������� � ������
		DevInterface	: INTERFACE_INFO_LUSBAPI;	// ���������� �� ������������ ����������� ����������
		Mcu			: MCU1_INFO_LUSBAPI;				// ���������� � ���������������� (������� '���������')
		Adc			: ADC_INFO_LUSBAPI;				// ���������� � ���
		Dac			: DAC_INFO_LUSBAPI;				// ���������� � ���
		DigitalIo	: DIGITAL_IO_INFO_LUSBAPI;		// ���������� � �������� �����-������
	end;
	pMODULE_DESCRIPTION_E310 = ^MODULE_DESCRIPTION_E310;

	// ��������� ����������������� ���� ������
	USER_FLASH_E310 = packed record
		Buffer : array [0..(USER_FLASH_SIZE_E310-1)] of BYTE;
	end;
	pUSER_FLASH_E310 = ^USER_FLASH_E310;

	// ��������� � ����������� ���������� ����������
	INCREMENT_INTRERVAL_PARS_E310 = packed record
		BaseIntervalType	: BYTE;			// [in-out]	��� �������� ��������� ����������, ������� ����� ���� ������: ������� MCLK ��� ������� ��������� �������
		MultiplierIndex	: BYTE;			// [in-out]	������ ���������� ��� �������� ��������� ����������
		MultiplierValue	: double;		// [out]		�������� ���������� ��� �������� ��������� ����������: 1, 5, 100 ��� 500
		BaseIntervalsNumber : WORD;		// [in-out]	���-�� ������� ���������� � ��������� ����������
		Duration				: double;		// [out] 	����� ������������ ���������� � �� (������ ��� ��������� ���������� �� MCLK), ����� 0.0
	end;

	// ��������� � ����������� ���������� ������� ����������
	ANALOG_OUTPUTS_PARS_E310 = packed record
		SignalType 			: BYTE;			// [in-out]	��� ����������� ������� �� ������� 10 � 50 ��: �������������� ��� �����������
		GainIndex 			: BYTE;			// [in-out]	������ �������� ��������� ������ ����������
		GaindB 				: double; 		// [out]		�������� ��������� ������ ���������� � ��
		Output10OhmInV		: double; 		// [out]		��������� ������� �� ������ 10 �� � �
		Output10OhmIndB	: double; 		// [out]		��������� ������� �� ������ 10 �� � ��
		Output10OhmOffset : double;		// [in-out]	�������� ����������� �������� �� ������ 10 �� � �
		Output10OhmOffsetSource : BYTE;	//[in-out]	��� �������� �� ������ 10 ��: ���������� ��� �������
		Output50OhmInV		: double; 		// [out]		��������� ������� �� ������ 50 �� � �
		Output50OhmIndB	: double; 		// [out]		��������� ������� �� ������ 50 �� � ��
	end;

	// ��������� � ����������� ������ ����������
	GENERATOR_PARS_E310 = packed record
		GeneratorEna		: BOOL;			// [in]		������� ��������� ������ ����������
		StartFrequency		: double;		// [in-out]	��������� ������� � ���
		FinalFrequency		: double;		// [out]		�������� ������� � ���
		FrequencyIncrements 	: double;	// [in-out]	������� ���������� � ���
		NumberOfIncrements	: WORD;		// [in-out]	���-�� ���������� ������� ������������
		IncrementIntervalPars: INCREMENT_INTRERVAL_PARS_E310;	// [in-out] ��������� ���������� ����������
		MasterClock			: double;		// [in-out]	������� ������������ ������� ���������� � ���
		MasterClockSource : BYTE;			// [in-out]	�������� ������������ ������� ����������: ���������� ��� �������
		CyclicAutoScanType: BYTE;			// [in-out]	��� ������������ ���������������� ��������� �������: ��� ������������ ������������, '����' ��� '�����������'
		IncrementType		: BYTE;			// [in-out]	��� ������������� ������� ����������: ���������� (��������������) ��� � ������� ����������� ����� "CTRL"
		CtrlLineType		: BYTE;			// [in-out]	��� ����� "CTRL" ��� (���������� �������������� �������)/������ ����������: ���������� (�� MCU) ��� �������
		InterrupLineType	: BYTE;			// [in-out]	��� ����� "INTERRUPT" ��� �������� ������ ����������: ���������� (�� MCU) ��� �������
		SquareWaveOutputEna	: BOOL;		// [in-out]	���������� ������� ���������� �� �������� ������ "������"
		SynchroOutEna		: BOOL;			// [in-out]	���������� ������������� ���������� �� �������� ����� "SYNCOUT"
		SynchroOutType		: BYTE;			// [in-out]	��� ������������ ������������� ����������: ��� ������ ���������� ������� ��� ������ �� ��������� ������������
		AnalogOutputsPars	: ANALOG_OUTPUTS_PARS_E310;	//	[in-out] ��������� ������ ���������� �������
	end;
	pGENERATOR_PARS_E310 = ^GENERATOR_PARS_E310;

	// ��������� � ����������� ������ ���������� ������� (FM)
	FM_PARS_E310 = packed record
		FmEna					: BOOL;			// [out]		������� ��������� ������ ���������� �������
		Mode					: BYTE;			// [in-out]	����� ������ ���������� �������
		InputDivider		: BYTE;			// [in-out]	���������� ������� ������� ��������� 1/8: 0x0 ��� 0x1
		BaseClockRateDivIndex : BYTE;		// [in-out]	������ �������� ������� �������� �������
		ClockRate			: DWORD;			// [out]		������� �������� ������� �������� FM � ��
		BaseClockRate		: DWORD;			// [const]	������� �������� ������� �������� FM 25 000 000 ��
		Offset				: double;		// [in-out]	�������� ������ ���������� ������� � �
	end;
	pFM_PARS_E310 = ^FM_PARS_E310;

	// ��������� � ����������� � ������� ����������� �������
	FM_SAMPLE_E310 = packed record
		IsActual				: BOOL;			// [out]	������� ���������������� �����e���� ������
		Frequency			: double;		// [out]	������� ����������� ������� � ���
		Period				: double;		// [out]	������ ����������� ������� � ��
		DutyCycle			: double;		// [out]	���������� ����������� ������� � ��
	end;
	pFM_SAMPLE_E310 = ^FM_SAMPLE_E310;

	// ��������� ��� ������ � ���
	ADC_PARS_E310 = packed record
		AdcStartSource		: BYTE; 			// [in-out]	�������� ������� ������� ���: ���������� ��� �������
		ChannelsMask		: BYTE;			// [in-out]	������� ����� �������� ������� (������� 4 ����)
		InputRange			: double;		// [out]		������� �������� ��� � �
	end;
	pADC_PARS_E310 = ^ADC_PARS_E310;

	// ��������� � ��������� ���
	ADC_DATA_E310 = packed record
		DataInCode	: array [0..(ADC_CHANNEL_QUANTITY_E310-1)] of SHORT;		// [out] ������ ������ � ��� � �����
		DataInV		: array [0..(ADC_CHANNEL_QUANTITY_E310-1)] of double;		// [out] ������ ������ � ��� � �
	end;
	pADC_DATA_E310 = ^ADC_DATA_E310;

// ��������� ������ E-310
ILE310 = class(Lusbbase)
  public
		// ---------------- ������� ��� ������ � ����������� ----------------------------
		Function GET_GENERATOR_PARS(GenPars : pGENERATOR_PARS_E310) : BOOL; virtual; stdcall; abstract;
		Function SET_GENERATOR_PARS(GenPars : pGENERATOR_PARS_E310) : BOOL; virtual; stdcall; abstract;
		Function START_GENERATOR : BOOL; virtual; stdcall; abstract;
		Function STOP_GENERATOR : BOOL; virtual; stdcall; abstract;

		// ---------- ������� ��� ������ � ������������� (FM) -------------------
		Function GET_FM_PARS(FmPars : pFM_PARS_E310) : BOOL; virtual; stdcall; abstract;
		Function SET_FM_PARS(FmPars : pFM_PARS_E310) : BOOL; virtual; stdcall; abstract;
		Function START_FM : BOOL; virtual; stdcall; abstract;
		Function STOP_FM : BOOL; virtual; stdcall; abstract;
		Function FM_SAMPLE(FmSample : pFM_SAMPLE_E310) : BOOL; virtual; stdcall; abstract;

		// ------------------ ������� ��� ������ ��� ---------------------------
		Function GET_ADC_PARS(AdcPars : pADC_PARS_E310) : BOOL; virtual; stdcall; abstract;
		Function SET_ADC_PARS(AdcPars : pADC_PARS_E310) : BOOL; virtual; stdcall; abstract;
		Function GET_ADC_DATA(AdcData : pADC_DATA_E310) : BOOL; virtual; stdcall; abstract;

		// ---------- ������� ��� ������ � ��������� ������� -------------------
		Function CONFIG_TTL_LINES(Pattern : WORD; AddTtlLinesEna : BOOL = FALSE) : BOOL; virtual; stdcall; abstract;
		Function TTL_IN(TtlIn 	: pWORD) : BOOL; virtual; stdcall; abstract;
		Function TTL_OUT(TtlOut : pWORD) : BOOL; virtual; stdcall; abstract;

		// ------- ������� ��� ������ � ���������������� ����������� ����  -------
		Function ENABLE_FLASH_WRITE(IsFlashWriteEnabled : BOOL) : BOOL; virtual; stdcall; abstract;
		Function READ_FLASH_ARRAY(UserFlash : pUSER_FLASH_E310) : BOOL; virtual; stdcall; abstract;
		Function WRITE_FLASH_ARRAY(UserFlash : pUSER_FLASH_E310) : BOOL; virtual; stdcall; abstract;

		// ------- ������� ��� ������ �� ��������� ����������� �� ���� -----------
		Function GET_MODULE_DESCRIPTION(ModuleDescription : pMODULE_DESCRIPTION_E310) : BOOL; virtual; stdcall; abstract;
		Function SAVE_MODULE_DESCRIPTION(ModuleDescription : pMODULE_DESCRIPTION_E310) : BOOL; virtual; stdcall; abstract;

	end;
	pILE310 = ^ILE310;


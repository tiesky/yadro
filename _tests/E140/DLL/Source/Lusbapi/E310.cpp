#include <algorithm>
#include <stdio.h>
#include "ErrorBaseIds.h"
#include "e310.h"

#define 	LAST_ERROR_NUMBER(ErrorNumber)	LastErrorNumber = E310_BASE_ERROR_ID + ErrorNumber

// ----------------------------------------------------------------------------------
//  ����������
// ----------------------------------------------------------------------------------
TLE310::TLE310(HINSTANCE hInst) : TLUSBBASE(hInst)
{
	WORD i;
	double Ampl = sqrt(2.0) * 0.77459666924148;		// ��������� ������ 0 �� �� �������� 600 ��

	// ������������� ���� ��������� �������� ������
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));
	ZeroMemory(&ModulePars, 		sizeof(MODULE_PARS));
	ZeroMemory(&TtlPars,				sizeof(TTL_PARS));
	ZeroMemory(&LocAdcPars, 		sizeof(ADC_PARS));
	// ������� ������ ������ ���������������� � ������ '����������' (Application)
	IsMcuApplicationActive = FALSE;
	// ������� ������ ���������� ������ � ��������������� ����
	IsUserFlashWriteEnabled = FALSE;
	// ������� ����� ������ Firmware ����������������
	FirmwareVersionNumber = 0.0;
	// �������� ����������� ������������ ������� ���������� � ���
	INTERNAL_DDS_MASTER_CLOCK = 50000.0;
	// ������� �������� ��� � �
	ADC_INPUT_RANGE = 3.3;

	// ������� ������ ��������� ���������� � ���������� ������
	IsGeneratorBusy = IsFmBusy = FALSE;

	// ������������ ��� ������� DDS
	MAX_FREQ_DDS_CODE = 0x1 << 24;

	// ������ ����������� ����� DDS_SIGNAL_GAIN1 � DDS_SIGNAL_GAIN1 � CONTROL_REG
	// �������� �������� ����������
	GeneratorGainIndexes[0x3][0x0] = ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310;
	GeneratorGainIndexes[0x2][0x0] = ANALOG_OUTPUT_GAIN_PLUS_06_DB_E310;
	GeneratorGainIndexes[0x1][0x0] = ANALOG_OUTPUT_GAIN_PLUS_03_DB_E310;
	GeneratorGainIndexes[0x0][0x0] = ANALOG_OUTPUT_GAIN_00_DB_E310;
	GeneratorGainIndexes[0x3][0x1] = ANALOG_OUTPUT_GAIN_PLUS_04_DB_E310;
	GeneratorGainIndexes[0x2][0x1] = INVALID_ANALOG_OUTPUT_GAINS_INDEX_E310;
	GeneratorGainIndexes[0x1][0x1] = ANALOG_OUTPUT_GAIN_MINUS_03_DB_E310;
	GeneratorGainIndexes[0x0][0x1] = INVALID_ANALOG_OUTPUT_GAINS_INDEX_E310;
	GeneratorGainIndexes[0x3][0x2] = ANALOG_OUTPUT_GAIN_MINUS_02_DB_E310;
	GeneratorGainIndexes[0x2][0x2] = ANALOG_OUTPUT_GAIN_MINUS_06_DB_E310;
	GeneratorGainIndexes[0x1][0x2] = ANALOG_OUTPUT_GAIN_MINUS_09_DB_E310;
	GeneratorGainIndexes[0x0][0x2] = ANALOG_OUTPUT_GAIN_MINUS_12_DB_E310;
	GeneratorGainIndexes[0x3][0x3] = ANALOG_OUTPUT_GAIN_MINUS_14_DB_E310;
	GeneratorGainIndexes[0x2][0x3] = ANALOG_OUTPUT_GAIN_MINUS_18_DB_E310;
	GeneratorGainIndexes[0x1][0x3] = ANALOG_OUTPUT_GAIN_MINUS_21_DB_E310;
	GeneratorGainIndexes[0x0][0x3] = ANALOG_OUTPUT_GAIN_MINUS_24_DB_E310;

	// ������� ����������� �������� �������� �����
	// DDS_SIGNAL_GAIN1 � DDS_SIGNAL_GAIN1 � CONTROL_REG
	Gain1Index[ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310]	= 0x3;	Gain2Index[ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310]	= 0x0;
	Gain1Index[ANALOG_OUTPUT_GAIN_PLUS_06_DB_E310]	= 0x2;	Gain2Index[ANALOG_OUTPUT_GAIN_PLUS_06_DB_E310]	= 0x0;
	Gain1Index[ANALOG_OUTPUT_GAIN_PLUS_04_DB_E310]	= 0x3;	Gain2Index[ANALOG_OUTPUT_GAIN_PLUS_04_DB_E310]	= 0x1;
	Gain1Index[ANALOG_OUTPUT_GAIN_PLUS_03_DB_E310]	= 0x1;	Gain2Index[ANALOG_OUTPUT_GAIN_PLUS_03_DB_E310]	= 0x0;
	Gain1Index[ANALOG_OUTPUT_GAIN_00_DB_E310] 		= 0x0;	Gain2Index[ANALOG_OUTPUT_GAIN_00_DB_E310] 		= 0x0;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_02_DB_E310]	= 0x3;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_02_DB_E310]	= 0x2;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_03_DB_E310]	= 0x1;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_03_DB_E310]	= 0x1;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_06_DB_E310]	= 0x2;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_06_DB_E310]	= 0x2;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_09_DB_E310]	= 0x1;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_09_DB_E310]	= 0x2;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_12_DB_E310]	= 0x0;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_12_DB_E310]	= 0x2;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_14_DB_E310]	= 0x3;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_14_DB_E310]	= 0x3;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_18_DB_E310]	= 0x2;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_18_DB_E310]	= 0x3;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_21_DB_E310]	= 0x1;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_21_DB_E310]	= 0x3;
	Gain1Index[ANALOG_OUTPUT_GAIN_MINUS_24_DB_E310]	= 0x0;	Gain2Index[ANALOG_OUTPUT_GAIN_MINUS_24_DB_E310]	= 0x3;

	// �������� ������ �������� ��������� ������ ���������� � ��
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310]		= 10.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_PLUS_06_DB_E310]		= 6.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_PLUS_04_DB_E310]		= 4.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_PLUS_03_DB_E310]		= 3.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_00_DB_E310]			= 0.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_02_DB_E310]	= -2.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_03_DB_E310]	= -3.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_06_DB_E310]	= -6.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_09_DB_E310]	= -9.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_12_DB_E310]	= -12.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_14_DB_E310]	= -14.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_18_DB_E310]	= -18.;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_21_DB_E310]	= -21.0;
	GeneratorGaindBArray[ANALOG_OUTPUT_GAIN_MINUS_24_DB_E310]	= -24.0;

	// ���������� ������� ��������� �������� �� ������ 10 � 50 �� � ��
	for(i = 0x0; i < ANALOG_OUTPUT_GAINS_QUANTITY_E310; i++)
	{
		Output10OhmdBArray[i] = GeneratorGaindBArray[i];
		Output50OhmdBArray[i] = GeneratorGaindBArray[i] - 6.0;
	}

	// ���������� ������� ��������� �������� �� ������ 10 � 50 �� � �
	for(i = 0x0; i < ANALOG_OUTPUT_GAINS_QUANTITY_E310; i++)
	{
		Output10OhmVArray[i] = Ampl * pow(10.0, Output10OhmdBArray[i]/20.0);
		Output50OhmVArray[i] = Ampl * pow(10.0, Output50OhmdBArray[i]/20.0);
	}

	// ������������� ���c��� ����������� ���������� ���������� ��� ���������� DDS
	DdsMultiplierArray[0x0] = 1.0;		DdsMultiplierArray[0x1] = 5.0;
	DdsMultiplierArray[0x2] = 100.0;		DdsMultiplierArray[0x3] = 500.0;
}

// ----------------------------------------------------------------------------------
//  ����������
// ----------------------------------------------------------------------------------
TLE310::~TLE310() { }



// ==================================================================================
// ������� ������ ���������� ��� ������ � ������� �-310
// ==================================================================================
// ----------------------------------------------------------------------------------
//  ������� ����������� ���� ��� ������� � USB ������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::OpenLDevice (WORD VirtualSlot)
{
	char ModuleName[LONG_MODULE_NAME_STRING_LENGTH];

	//  ��������� ������� ����������� ���� ��� ������� � USB ������
	if(!TLUSBBASE::OpenLDeviceByID(VirtualSlot, E310_ID)) { return FALSE; }
	// ��������� ��������� �������� ������
	else if(!TLUSBBASE::GetModuleName(ModuleName)) { TLUSBBASE::CloseLDevice(); return FALSE; }
	// ��������, ��� ��� ������ E-310
	else if(strcmp(ModuleName, "E-310")) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(0); return FALSE; }
	// ��������� ��������� ��������������� � ������ '����������' (BootLoader)
	else if(!RunMcuApplication(BOOT_LOADER_START_ADDRESS)) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(1); return FALSE; }
	// � ������ �������� ��������������� � ������ '����������' (Application)
	else if(!RunMcuApplication(FIRMWARE_START_ADDRESS, REINIT_SWITCH_PARAM)) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(2); return FALSE; }
	// ������ ����� ��������� ��������� � ����������� �������� �����,
	// ����� ������ ��������� �� �������������� �������� �����,
	// ������� �������� �� ��������� � ����������
	else if(!GetArray((BYTE *)&TtlPars, sizeof(TTL_PARS), TTL_PARS_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }
	// �� ������
	else return TRUE;
}

//-----------------------------------------------------------------------------------
// ��������� ������� ����������� ����
//-----------------------------------------------------------------------------------
BOOL WINAPI TLE310::CloseLDevice(void)
{
	BOOL Status = TRUE;

	// ������� ������ ����������
	if(IsGeneratorBusy)
		{ if(!STOP_GENERATOR()) Status = FALSE; }
	// ������� ������ ����������� (FM)
	if(IsFmBusy)
		{ if(!STOP_FM()) Status = FALSE; }
	// ������� ����������
	if(!TLUSBBASE::CloseLDevice()) Status = FALSE;
	// ���������� ������ ���������� �������
	return Status;
}

//-----------------------------------------------------------------------------------
// ��������� ��������� �� ����������
//-----------------------------------------------------------------------------------
BOOL WINAPI TLE310::ReleaseLInstance(void)
{
	BOOL Status = TRUE;

	// ��������� ������������� ������������ �����
	if(!CloseLDevice()) Status = FALSE;
	// ��������� ��������� �� ����������
	delete this;
	// ���������� ������ ���������� �������
	return Status;
}

// ----------------------------------------------------------------------------------
// ������������ ���������� ����������
// ----------------------------------------------------------------------------------
HANDLE WINAPI TLE310::GetModuleHandle(void) { return hDevice; }

// ----------------------------------------------------------------------------------
//  ������ �������� ������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GetModuleName(PCHAR const ModuleName)
{
	// �������� �����
	if(!ModuleName) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������ ��������� ������ �������� ������
	else if(!TLUSBBASE::GetModuleNameByLength(ModuleName, LONG_MODULE_NAME_STRING_LENGTH)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

// ----------------------------------------------------------------------------------
//  ��������� ������� �������� ������ ���� USB
//-----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// �������� ���������
	if(!UsbSpeed) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ������ ��������� ������ �������� ������ USB ����
	else if(!TLUSBBASE::GetUsbSpeed(UsbSpeed)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ���������� ������� ������� ������������������ ������ E-310
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::LowPowerMode(BOOL LowPowerFlag)
{
/*	BYTE Flag = (BYTE)(LowPowerFlag ? 0x1 : 0x0);

	// ��������� ��������� � ������ ������� ���������� ������ �������������������
	if(!PutArray(&Flag, 0x1, SUSPEND_MODE_FLAG)) { return FALSE; }
	// �� ������
	else*/ return TRUE;
}

// ----------------------------------------------------------------------------------
// ������� ������ ������ � ��������� �������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	return TLUSBBASE::GetLastErrorInfo(LastErrorInfo);
}



// ==================================================================================
// -------------------- ������� ��� ������ � ����������� ----------------------------
// ==================================================================================
// ----------------------------------------------------------------------------------
// ������� ������ ������� ���������� ������ ����������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GET_GENERATOR_PARS(GENERATOR_PARS_E310 * const GenPars)
{
	WORD i;
	DWORD StartFreqCode, DeltaFreqCode;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!GenPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E310; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E310[i]) break;
	if(i == REVISIONS_QUANTITY_E310) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ����������, ����������� � ����������
	ZeroMemory(&ModulePars.Dds, sizeof(DDS_PARS));

	// ������ ����� ��������� ������� ��������� ������ �� ������
	if(!GetArray((BYTE *)&ModulePars, sizeof(MODULE_PARS), MODULE_PARS_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }


	// *** ����������� � ����������� ������ ���������� ***
	// == ���������� ������ ����������
	GenPars->GeneratorEna = ModulePars.Dds.ControlReg.BitFields.DAC_ENA ? TRUE : FALSE;

	// == �������� ������������ ������� ��� ����������: ���������� ��� �������
	GenPars->MasterClockSource = ModulePars.ControlReg.BitFields.DDS_MCLK_SOURCE;
	// == �������� ������������ ������� ���������� � ���
	if(GenPars->MasterClockSource == INTERNAL_MASTER_CLOCK_E310)
		GenPars->MasterClock = INTERNAL_DDS_MASTER_CLOCK;
	else if(GenPars->MasterClockSource == EXTERNAL_MASTER_CLOCK_E310)
		GenPars->MasterClock = ModulePars.Dds.MasterClock;
	else { LAST_ERROR_NUMBER(7); return FALSE; }

	// == ��������� ������� ������������ � ���
	StartFreqCode = (ModulePars.Dds.HigherStartFrequencyReg.BitFields.Value12bit << 12) |
						  ModulePars.Dds.LowerStartFrequencyReg.BitFields.Value12bit;
	GenPars->StartFrequency = StartFreqCode * GenPars->MasterClock / MAX_FREQ_DDS_CODE;
	// == ������� ���������� ��� ������������ � ���
	DeltaFreqCode = (ModulePars.Dds.HigherDeltaFrequencyReg.BitFields.Value11bit << 12) |
						  ModulePars.Dds.LowerDeltaFrequencyReg.BitFields.Value12bit;
	GenPars->FrequencyIncrements = DeltaFreqCode * GenPars->MasterClock / MAX_FREQ_DDS_CODE;
	// == ���-�� ���������� �������
	GenPars->NumberOfIncrements = ModulePars.Dds.NumberOfIncrementsReg.BitFields.Value12bit;
	// == �������� ������� ������������
	GenPars->FinalFrequency = GenPars->StartFrequency + (GenPars->FrequencyIncrements * GenPars->NumberOfIncrements);

	// == ��� �������� ��������� ����������, ������� ����� ����
	//    ������: ������� MCLK ��� ������� ��������� �������
	GenPars->IncrementIntervalPars.BaseIntervalType = (BYTE)(ModulePars.Dds.IncrementIntervalReg.BitFields.ADDR & 0x1);
	// == ������ ���������� ��� �������� ��������� ����������
	GenPars->IncrementIntervalPars.MultiplierIndex = ModulePars.Dds.IncrementIntervalReg.BitFields.Multiplier;
	// == �������� ���������� ��� �������� ��������� ����������
	GenPars->IncrementIntervalPars.MultiplierValue = DdsMultiplierArray[ModulePars.Dds.IncrementIntervalReg.BitFields.Multiplier];
	// == ���-�� ������ (���� MCLK, ���� �������� �������) � ��������� ����������
	GenPars->IncrementIntervalPars.BaseIntervalsNumber = ModulePars.Dds.IncrementIntervalReg.BitFields.Value11bit;

	// == ��� ������������ ���������������� ��������� �������:
	//    ��� ������������ ������������, '����' ��� '�����������'
	GenPars->CyclicAutoScanType = ModulePars.Dds.AutoScanType;
	// == ��� ����� "INTERRUPT" ��� �������� ������ ����������: ���������� (�� MCU) ��� �������
	GenPars->InterrupLineType = ModulePars.ControlReg.BitFields.DDS_INTERRUPT_SOURCE;
	// == ���������� ������� �� ������ "������"
	GenPars->SquareWaveOutputEna = ModulePars.Dds.ControlReg.BitFields.MSBOUTEN ? TRUE : FALSE;
	// ��� CyclicAutoScanType ���������� ��������� ���������
	if(GenPars->CyclicAutoScanType == NO_CYCLIC_AUTOSCAN_E310)
	{
		// == ��� ������������� ������� ����������: ���������� (��������������) ��� � ������� ����������� ����� "CTRL"
		GenPars->IncrementType = ModulePars.Dds.ControlReg.BitFields.INT_OR_EXT_INCR;
		// == ��� ����� "CTRL" ��� (���������� �������������� �������)/������ ����������: ���������� (�� MCU) ��� �������
		GenPars->CtrlLineType = ModulePars.ControlReg.BitFields.DDS_CTRL_SOURCE;
	}
	else
	{
		// == ��� ������������� ������� ����������: ���������� (��������������)
		GenPars->IncrementType = ModulePars.Dds.ControlReg.BitFields.INT_OR_EXT_INCR = AUTO_INCREMENT_E310;
		// == ��� ����� "CTRL" ��� (���������� �������������� �������)/������ ����������: ���������� (�� MCU)
		GenPars->CtrlLineType = ModulePars.ControlReg.BitFields.DDS_CTRL_SOURCE = INTERNAL_CTRL_LINE_E310;
	}

	// == ����� ������������ ���������� � �� (������ ��� ��������� ���������� �� MCLK), ����� 0.0
	if(GenPars->IncrementType == AUTO_INCREMENT_E310)
	{
		if(GenPars->IncrementIntervalPars.BaseIntervalType == CLOCK_PERIOD_INCREMENT_INTERVAL_E310)
			GenPars->IncrementIntervalPars.Duration = GenPars->IncrementIntervalPars.BaseIntervalsNumber * (GenPars->IncrementIntervalPars.MultiplierValue/GenPars->MasterClock);
		else
			GenPars->IncrementIntervalPars.Duration = 0.0;
	}
	else GenPars->IncrementIntervalPars.Duration = 0.0;

	// == ������������ ��������� ������������� �� ����� "SYNCOUT":
	// ��� ������ ���������� ������� ��� ������ �� ���������
	GenPars->SynchroOutEna = ModulePars.Dds.ControlReg.BitFields.SYNCOUTEN;
	GenPars->SynchroOutType = ModulePars.Dds.ControlReg.BitFields.SYNCSEL;

	// == ��� ����������� ������� �� ������� 10 � 50 ��: �������������� (TRUE) ��� ����������� (FALSE)
	GenPars->AnalogOutputsPars.SignalType = ModulePars.Dds.ControlReg.BitFields.SIN_OR_TRI;
	// == ��� �������� �� ������ 10 ��: ���������� ��� �������
	GenPars->AnalogOutputsPars.Output10OhmOffsetSource = ModulePars.ControlReg.BitFields.DDS_10OHM_SIGNAL_OFFSET_SOURCE;
	// == �������� ����������� �������� �� ������ 10 �� � �
	//   ���� 000 --> -4B
	//   ���� 255 --> +4B
	GenPars->AnalogOutputsPars.Output10OhmOffset = 8.0*ModulePars.DigitalPotentiometers.Output10OhmOffset.BitFields.Value8bit/255.0 - 4.0;
	// ������ �������� ��������� ������ ����������
	GenPars->AnalogOutputsPars.GainIndex = GeneratorGainIndexes[ModulePars.ControlReg.BitFields.DDS_SIGNAL_GAIN1][ModulePars.ControlReg.BitFields.DDS_SIGNAL_GAIN2];
	// �������� ��������� ������ ���������� � ��
	GenPars->AnalogOutputsPars.GaindB = GeneratorGaindBArray[GenPars->AnalogOutputsPars.GainIndex];
	// ��������� ������� �� ������ 10 �� � �
	GenPars->AnalogOutputsPars.Output10OhmInV = Output10OhmVArray[GenPars->AnalogOutputsPars.GainIndex];
	// ��������� ������� �� ������ 10 �� � ��
	GenPars->AnalogOutputsPars.Output10OhmIndB = Output10OhmdBArray[GenPars->AnalogOutputsPars.GainIndex];
	// ��������� ������� �� ������ 50 �� � �
	GenPars->AnalogOutputsPars.Output50OhmInV = Output50OhmVArray[GenPars->AnalogOutputsPars.GainIndex];
	// ��������� ������� �� ������ 50 �� � ��
	GenPars->AnalogOutputsPars.Output50OhmIndB = Output50OhmdBArray[GenPars->AnalogOutputsPars.GainIndex];

	// ��� ������ :)))))
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ������� ��������� ������������ ���������� ������ ����������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::SET_GENERATOR_PARS(GENERATOR_PARS_E310 * const GenPars)
{
	WORD i;
	DWORD StartFreqCode, StopFreqCode;
	double StartFreqKHz, StopFreqKHz;
	DWORD FreqIncCode;
	double AbsFreqIncKHz, FreqIncSign;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!GenPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }
	// ���� ��������� ������������ �������������� �������� �����, ������� ����������
	// � SPI MCU ������, �� ���������� �������� � ����������� ������� ����������
	else if(TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA) { LAST_ERROR_NUMBER(8); return FALSE; }
	// �������� ������ ��������� ����������
	else if(IsGeneratorBusy) { LAST_ERROR_NUMBER(9); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E310; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E310[i]) break;
	if(i == REVISIONS_QUANTITY_E310) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ����������, ����������� � ����������
	ZeroMemory(&ModulePars.Dds, sizeof(DDS_PARS));
	ModulePars.ControlReg.Value = 0x0;
	ModulePars.DigitalPotentiometers.Output10OhmOffset.Value = 0x0;

	// ��������� ����������������� ���� ������������ �������� DDS
	ModulePars.Dds.ControlReg.BitFields.Reserved0 = 0x3;
	ModulePars.Dds.ControlReg.BitFields.Reserved1 = 0x1;
	ModulePars.Dds.ControlReg.BitFields.Reserved2 = 0x3;

	// ����� ������������ �������� DDS
	ModulePars.Dds.ControlReg.BitFields.ADDR = CONTROL_BITS_ADDR_REG;

	// *** ����������� � ����������� ������ ���������� ***
	// == ���������� ������ ����������
	GenPars->GeneratorEna = FALSE; 		// GenPars->GeneratorEna = TRUE ������ ����� ���������� �. START_GENERATOR()
	ModulePars.Dds.ControlReg.BitFields.DAC_ENA = (BYTE)(GenPars->GeneratorEna ? 0x1 : 0x0);
	// ������ ��� ���� ������������� � ���. '1'
	ModulePars.Dds.ControlReg.BitFields.B24 = 0x1;

	// == �������� ������������ ������� ��� ����������: ���������� ��� �������
	if(GenPars->MasterClockSource >= INVALID_MASTER_CLOCK_SOURCE_E310)
		GenPars->MasterClockSource = INTERNAL_MASTER_CLOCK_E310;
	ModulePars.ControlReg.BitFields.DDS_MCLK_SOURCE = GenPars->MasterClockSource;

	// == �������� ������������ ������� ���������� � ���
	if(GenPars->MasterClockSource == INTERNAL_MASTER_CLOCK_E310)
		ModulePars.Dds.MasterClock = GenPars->MasterClock = INTERNAL_DDS_MASTER_CLOCK;
	else if(GenPars->MasterClockSource == EXTERNAL_MASTER_CLOCK_E310)
	{
		GenPars->MasterClock = fabs(GenPars->MasterClock);
		if((GenPars->MasterClock < 0.001) || (GenPars->MasterClock > INTERNAL_DDS_MASTER_CLOCK))
		{
			GenPars->MasterClock = INTERNAL_DDS_MASTER_CLOCK;
			ModulePars.ControlReg.BitFields.DDS_MCLK_SOURCE = GenPars->MasterClockSource = INTERNAL_MASTER_CLOCK_E310;;
		}
		ModulePars.Dds.MasterClock = GenPars->MasterClock;
	}

	// == ���-�� ���������� ������� ������������
	if(GenPars->NumberOfIncrements > 4095) GenPars->NumberOfIncrements = 4095;
	ModulePars.Dds.NumberOfIncrementsReg.BitFields.ADDR = NUMBER_OF_INCREMENTS_ADDR_REG;
	ModulePars.Dds.NumberOfIncrementsReg.BitFields.Value12bit = GenPars->NumberOfIncrements;

	// == ������� ���������� � ���
	FreqIncSign = (GenPars->FrequencyIncrements >= 0.0) ? 1.0 : (-1.0);
	AbsFreqIncKHz = fabs(GenPars->FrequencyIncrements);
	// ������ ��� ������� ����������
	FreqIncCode = (AbsFreqIncKHz / GenPars->MasterClock) * MAX_FREQ_DDS_CODE + 0.5;
	// �������� �� ������������ ��������
	if(FreqIncCode > MAX_FREQ_DDS_CODE) FreqIncCode = MAX_FREQ_DDS_CODE;
	// �������� ���������� ������� ����������
	AbsFreqIncKHz = GenPars->MasterClock * FreqIncCode / MAX_FREQ_DDS_CODE;
	GenPars->FrequencyIncrements = AbsFreqIncKHz * FreqIncSign;
	// ������� 12 ��� ���� ������� ����������
	ModulePars.Dds.LowerDeltaFrequencyReg.BitFields.ADDR = LOWER_DELTA_FREQUENCY_ADDR_REG;
	ModulePars.Dds.LowerDeltaFrequencyReg.BitFields.Value12bit = (WORD)(FreqIncCode & 0x0FFF);
	// ������� 12 ��� ���� ������� ����������
	ModulePars.Dds.HigherDeltaFrequencyReg.BitFields.ADDR = HIGHER_DELTA_FREQUENCY_ADDR_REG;
	ModulePars.Dds.HigherDeltaFrequencyReg.BitFields.Value11bit = (WORD)((FreqIncCode >> 12) & 0x07FF);
	ModulePars.Dds.HigherDeltaFrequencyReg.BitFields.Sign = (WORD)((FreqIncSign > 0.0) ? 0x0 : 0x1);

	// == ��������� ������� ������������ � ���
	StartFreqKHz = GenPars->StartFrequency;
	// ������ ��� ��������� �������
	StartFreqCode = (StartFreqKHz / GenPars->MasterClock) * MAX_FREQ_DDS_CODE + 0.5;
	// �������� �� ������������ ��������
	if(StartFreqCode > MAX_FREQ_DDS_CODE) StartFreqCode = MAX_FREQ_DDS_CODE;
	// �������� ���������� ��������� ������� DDS
	GenPars->StartFrequency = StartFreqKHz = GenPars->MasterClock * StartFreqCode / MAX_FREQ_DDS_CODE;
	// ������� 12 ��� ���� ��������� �������
	ModulePars.Dds.LowerStartFrequencyReg.BitFields.ADDR = LOWER_START_FREQUENCY_ADDR_REG;
	ModulePars.Dds.LowerStartFrequencyReg.BitFields.Value12bit = (WORD)(StartFreqCode & 0x0FFF);
	// ������� 12 ��� ���� ��������� �������
	ModulePars.Dds.HigherStartFrequencyReg.BitFields.ADDR = HIGHER_START_FREQUENCY_ADDR_REG;
	ModulePars.Dds.HigherStartFrequencyReg.BitFields.Value12bit = (WORD)((StartFreqCode >> 12) & 0x0FFF);

	// == �������� ������� ������������ � ���
	// ������������ ������������� � ����������� ���������������� ���� '�����������'
	// �������� �������� �������
	StopFreqKHz = StartFreqKHz + ModulePars.Dds.NumberOfIncrementsReg.BitFields.Value12bit * GenPars->FrequencyIncrements;
	// �������� �� ��������������� - ���� �� ������ ����!
	if(StopFreqKHz < 0.0) { LAST_ERROR_NUMBER(10); return FALSE; }
	// �������� �� ��������
	else if(StopFreqKHz > (GenPars->MasterClock/2.0)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �������� ��� �������� �������
	StopFreqCode = (StopFreqKHz / GenPars->MasterClock) * MAX_FREQ_DDS_CODE + 0.5;
	// �������� �� ����������� ��������� �������� - ���� �� ������ ����!
	if(StopFreqCode > MAX_FREQ_DDS_CODE)  { LAST_ERROR_NUMBER(11); return FALSE; }
	// �������� ���������� �������� ������� DDS
	GenPars->FinalFrequency = StopFreqKHz = GenPars->MasterClock * StopFreqCode / MAX_FREQ_DDS_CODE;
	// ������� 12 ��� ���� �������� �������
	ModulePars.Dds.LowerStopFrequencyReg.BitFields.ADDR = LOWER_START_FREQUENCY_ADDR_REG;
	ModulePars.Dds.LowerStopFrequencyReg.BitFields.Value12bit = (WORD)(StopFreqCode & 0x0FFF);
	// ������� 12 ��� ���� �������� �������
	ModulePars.Dds.HigherStopFrequencyReg.BitFields.ADDR = HIGHER_START_FREQUENCY_ADDR_REG;
	ModulePars.Dds.HigherStopFrequencyReg.BitFields.Value12bit = (WORD)((StopFreqCode >> 12) & 0x0FFF);

	// == ��� �������� ��������� ����������, ������� ����� ����
	//    ������: ������� MCLK ��� ������� ��������� �������
	// �������� ��� �������� ��������� ����������
	if(GenPars->IncrementIntervalPars.BaseIntervalType >= INVALID_INCREMENT_INTERVAL_TYPE_E310)
		GenPars->IncrementIntervalPars.BaseIntervalType = CLOCK_INCREMENT_INTERVAL_ADDR_REG;
	if(GenPars->IncrementIntervalPars.BaseIntervalType == WAVEFORM_CYCLE_INCREMENT_INTERVAL_E310)
		ModulePars.Dds.IncrementIntervalReg.BitFields.ADDR = CYCLE_INCREMENT_INTERVAL_ADDR_REG;
	else
		ModulePars.Dds.IncrementIntervalReg.BitFields.ADDR = CLOCK_INCREMENT_INTERVAL_ADDR_REG;

	// == ���������� ��������� ����������
	// �������� ������ ����������
	if(GenPars->IncrementIntervalPars.MultiplierIndex >= INVALID_INCREMENT_INTERVAL_MULTIPLIER_E310)
		GenPars->IncrementIntervalPars.MultiplierIndex = INCREMENT_INTERVAL_MULTIPLIERS_001_E310;
	ModulePars.Dds.IncrementIntervalReg.BitFields.Multiplier = GenPars->IncrementIntervalPars.MultiplierIndex;
	// �������� ����������
	GenPars->IncrementIntervalPars.MultiplierValue = DdsMultiplierArray[GenPars->IncrementIntervalPars.MultiplierIndex];

	// == ���-�� ������� ���������� � ��������� ����������
	if(GenPars->IncrementIntervalPars.BaseIntervalsNumber >= 2047)
		GenPars->IncrementIntervalPars.BaseIntervalsNumber = 2047;
	else if(GenPars->IncrementIntervalPars.BaseIntervalsNumber < 2)
		GenPars->IncrementIntervalPars.BaseIntervalsNumber = 2;
	ModulePars.Dds.IncrementIntervalReg.BitFields.Value11bit = GenPars->IncrementIntervalPars.BaseIntervalsNumber;

	// == ��� ������������ ���������������� ��������� �������:
	//    ��� ������������ ������������, '����' ��� '�����������'
	if(GenPars->CyclicAutoScanType >= INVALID_CYCLIC_AUTOSCAN_TYPE_E310)
		GenPars->CyclicAutoScanType = NO_CYCLIC_AUTOSCAN_E310;
	ModulePars.Dds.AutoScanType = GenPars->CyclicAutoScanType;

	// == ������������ ��������� ������������� �� ����� "SYNCOUT":
	// ��� ������ ���������� ������� ��� ������ �� ��������� ������������
	if(GenPars->SynchroOutType >= INVALID_SYNCOUT_TYPES_E310)
		GenPars->SynchroOutType = SYNCOUT_ON_EACH_INCREMENT_E310;
	ModulePars.Dds.ControlReg.BitFields.SYNCSEL = GenPars->SynchroOutType;
	ModulePars.Dds.ControlReg.BitFields.SYNCOUTEN = (BYTE)(GenPars->SynchroOutEna ? 0x1 : 0x0);

	// == ��� ����� "INTERRUPT" ��� �������� ������ ����������: ���������� (�� MCU) ��� �������
	if(GenPars->InterrupLineType >= INVALID_INTERRUPT_LINE_TYPE_E310)
		GenPars->InterrupLineType = INTERNAL_INTERRUPT_LINE_E310;
	ModulePars.ControlReg.BitFields.DDS_INTERRUPT_SOURCE = GenPars->InterrupLineType;
	// == ���������� ������� �� �������� ������ "������"
	ModulePars.Dds.ControlReg.BitFields.MSBOUTEN = (BYTE)(GenPars->SquareWaveOutputEna ? 0x1 : 0x0);

	// ��� CyclicAutoScanType ���������� ��������� ���������
	if(GenPars->CyclicAutoScanType == NO_CYCLIC_AUTOSCAN_E310)
	{
		// == ��� ������������� ������� ����������: ���������� ��� �� �������� ������� �� ����� "�����"
		if(GenPars->IncrementType >= INVALID_INCREMENT_TYPE_E310)
			GenPars->IncrementType = AUTO_INCREMENT_E310;
		ModulePars.Dds.ControlReg.BitFields.INT_OR_EXT_INCR = GenPars->IncrementType;

		// == ��� ����� "CTRL" ��� (���������� �������������� �������)/������ ����������: ���������� (�� MCU) ��� �������
		if(GenPars->CtrlLineType >= INVALID_CTRL_LINE_TYPE_E310)
			GenPars->CtrlLineType = INTERNAL_CTRL_LINE_E310;
		ModulePars.ControlReg.BitFields.DDS_CTRL_SOURCE = GenPars->CtrlLineType;
	}
	else
	{
		// == ��� ������������� ������� ����������: ���������� (��������������)
		GenPars->IncrementType = ModulePars.Dds.ControlReg.BitFields.INT_OR_EXT_INCR = AUTO_INCREMENT_E310;
		// == ��� ����� "CTRL" ��� (���������� �������������� �������)/������ ����������: ���������� (�� MCU)
		GenPars->CtrlLineType = ModulePars.ControlReg.BitFields.DDS_CTRL_SOURCE = INTERNAL_CTRL_LINE_E310;
		// == ������������ ��������� ������������� �� ����� "SYNCOUT"
		GenPars->SynchroOutEna = TRUE;
		ModulePars.Dds.ControlReg.BitFields.SYNCOUTEN = (BYTE)(GenPars->SynchroOutEna ? 0x1 : 0x0);
		ModulePars.Dds.ControlReg.BitFields.SYNCSEL = GenPars->SynchroOutType = SYNCOUT_AT_END_OF_SCAN_E310;
	}

	// == ����� ������������ ���������� � �� (������ ��� ��������� ���������� �� MCLK, ����� 0
	if(GenPars->IncrementType == AUTO_INCREMENT_E310)
	{
		if(GenPars->IncrementIntervalPars.BaseIntervalType == CLOCK_PERIOD_INCREMENT_INTERVAL_E310)
			GenPars->IncrementIntervalPars.Duration = GenPars->IncrementIntervalPars.BaseIntervalType * (GenPars->IncrementIntervalPars.MultiplierValue/GenPars->MasterClock);
		else
			GenPars->IncrementIntervalPars.Duration = 0.0;
	}
	else GenPars->IncrementIntervalPars.Duration = 0.0;

	// == ��� ����������� ������� �� ������� 10 � 50 ��: �������������� (TRUE) ��� ����������� (FALSE)
	if(GenPars->AnalogOutputsPars.SignalType >= INVALID_ANALOG_OUTPUT_TYPE_E310)
		GenPars->AnalogOutputsPars.SignalType = SINUSOIDAL_ANALOG_OUTPUT_E310;
	ModulePars.Dds.ControlReg.BitFields.SIN_OR_TRI = (WORD)((GenPars->AnalogOutputsPars.SignalType == SINUSOIDAL_ANALOG_OUTPUT_E310) ? 0x1 : 0x0);

	// == ��� �������� �� ������ 10 ��: ���������� ��� �������
	if(GenPars->AnalogOutputsPars.Output10OhmOffsetSource >= INVALID_OUTPUT_10_OHM_OFFSET_SOURCE_E310)
		GenPars->AnalogOutputsPars.Output10OhmOffsetSource = INTERNAL_OUTPUT_10_OHM_OFFSET_E310;
	ModulePars.ControlReg.BitFields.DDS_10OHM_SIGNAL_OFFSET_SOURCE = GenPars->AnalogOutputsPars.Output10OhmOffsetSource;

	// == �������� ����������� �������� �� ������ 10 �� � �
	//     ���� 000 --> -4B
	//     ���� 255 --> +4B
	if((GenPars->AnalogOutputsPars.Output10OhmOffset < -4.0) || (GenPars->AnalogOutputsPars.Output10OhmOffset > 4.0))
		GenPars->AnalogOutputsPars.Output10OhmOffset = 0.0;
	ModulePars.DigitalPotentiometers.Output10OhmOffset.BitFields.Value8bit = 255.0*(GenPars->AnalogOutputsPars.Output10OhmOffset + 4.0)/8.0 + 0.5;

	ModulePars.DigitalPotentiometers.Output10OhmOffset.BitFields.ADDR = 0x0;
	GenPars->AnalogOutputsPars.Output10OhmOffset = 8.0*ModulePars.DigitalPotentiometers.Output10OhmOffset.BitFields.Value8bit/255.0 - 4.0;

	// == ������ �������� ��������� ������ ����������
	if(GenPars->AnalogOutputsPars.GainIndex >= INVALID_ANALOG_OUTPUT_GAINS_INDEX_E310)
		GenPars->AnalogOutputsPars.GainIndex = ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310;
	ModulePars.ControlReg.BitFields.DDS_SIGNAL_GAIN1 = Gain1Index[GenPars->AnalogOutputsPars.GainIndex];
	ModulePars.ControlReg.BitFields.DDS_SIGNAL_GAIN2 = Gain2Index[GenPars->AnalogOutputsPars.GainIndex];
	// == �������� ��������� ������ ���������� � ��
	GenPars->AnalogOutputsPars.GaindB = GeneratorGaindBArray[GenPars->AnalogOutputsPars.GainIndex];
	// == ��������� ������� �� ������ 10 �� � �
	GenPars->AnalogOutputsPars.Output10OhmInV = Output10OhmVArray[GenPars->AnalogOutputsPars.GainIndex];
	// == ��������� ������� �� ������ 10 �� � ��
	GenPars->AnalogOutputsPars.Output10OhmIndB = Output10OhmdBArray[GenPars->AnalogOutputsPars.GainIndex];
	// == ��������� ������� �� ������ 50 �� � �
	GenPars->AnalogOutputsPars.Output50OhmInV = Output50OhmVArray[GenPars->AnalogOutputsPars.GainIndex];
	// == ��������� ������� �� ������ 50 �� � ��
	GenPars->AnalogOutputsPars.Output50OhmIndB = Output50OhmdBArray[GenPars->AnalogOutputsPars.GainIndex];

	// ������ ����� �������� ������� ��������� ������ � ������
	if(!PutArray((BYTE *)&ModulePars, sizeof(MODULE_PARS), MODULE_PARS_ADDRESS)) { LAST_ERROR_NUMBER(12); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ����� ������ ����������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::START_GENERATOR(void)
{
	WORD InBuf[4] = { 0x0, V_START_ADC, 0x1 << GENERATOR_START_INDEX, 0x0 };

	// ���� ��������� ������������ �������������� �������� �����, ������� ����������
	// � SPI MCU ������, �� ���������� �������� � ����������� ������� ����������
	if(TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA) { LAST_ERROR_NUMBER(8); return FALSE; }
	// ��� USB-������ � MCU �� ������ ����������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LAST_ERROR_NUMBER(13); return FALSE; }

	// ��������� ������ ��������� ����������
	IsGeneratorBusy = TRUE;

	// �� ������
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ���� ������ ����������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::STOP_GENERATOR(void)
{
	WORD InBuf[4] = { 0x0, V_STOP_ADC, 0x1 << GENERATOR_START_INDEX, 0x0 };

	// ������� ������ ��������� ����������
	IsGeneratorBusy = FALSE;

	// ��� USB-������ � MCU �� ������ ����������
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LAST_ERROR_NUMBER(13); return FALSE; }
	// �� ������
	else return TRUE;
}






// ==================================================================================
//------------------ ������� ��� ������ � ������������� (FM) ------------------------
// ==================================================================================
// ----------------------------------------------------------------------------------
// ��������� ������� ���������� ������ ����������� (FM)
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GET_FM_PARS(FM_PARS_E310 * const FmPars)
{
	WORD i;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!FmPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E310; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E310[i]) break;
	if(i == REVISIONS_QUANTITY_E310) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ����������, ����������� � ����������
	ZeroMemory(&ModulePars.Dds, sizeof(DDS_PARS));
	// ������ ����� ��������� ������� ��������� ������ �� ������
	if(!GetArray((BYTE *)&ModulePars, sizeof(MODULE_PARS), MODULE_PARS_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }

	// *** ����������� � ����������� ������ FM ***
	// == ������� ��������� FM
	FmPars->FmEna = ModulePars.FrequencyMeasurement.FmEna ? TRUE : FALSE;
	// == ����� ������ ���������� �������
	FmPars->Mode = ModulePars.FrequencyMeasurement.Mode;
	// == ��������� �������� �������� �������: 1:1 ��� 1:8
	FmPars->InputDivider = ModulePars.FrequencyMeasurement.InputDivider;
	// == ������ �������� ������� �������� ������� FM
	FmPars->BaseClockRateDivIndex = ModulePars.FrequencyMeasurement.BaseClockRateDivIndex;
	// == �������� ������ ���������� ������� � �
	FmPars->Offset = 4.0 - 8.0*ModulePars.DigitalPotentiometers.FrequencyMeterPorog.BitFields.Value8bit/255.0;

	// == ������� �������� ������� �������� FM � ��
	FmPars->ClockRate = ModulePars.FrequencyMeasurement.ClockRate;
	// == ������� �������� ������� �������� FM � ��
	FmPars->BaseClockRate = ModulePars.FrequencyMeasurement.BaseClockRate;

	// ��� ������ :)))))
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ��������� ��������� ���������� ������ ����������� (FM)
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::SET_FM_PARS(FM_PARS_E310 * const FmPars)
{
	WORD i;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!FmPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }
	// ���� ��������� ������������ �������������� �������� �����, ������� ����������
	// � SPI MCU ������, �� ���������� �������� � ����������� ������� ����������
	else if(TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA) { LAST_ERROR_NUMBER(8); return FALSE; }
	// �������� ������ ��������� ���������� ������� (FM)
	else if(IsFmBusy) { LAST_ERROR_NUMBER(14); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E310; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E310[i]) break;
	if(i == REVISIONS_QUANTITY_E310) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ����������, ����������� � FM
	ZeroMemory(&ModulePars.FrequencyMeasurement, sizeof(FM_PARS));
	ModulePars.DigitalPotentiometers.FrequencyMeterPorog.Value = 0x0;

	// == ������� ��������� FM
	FmPars->FmEna = FALSE;		// FmPars->FmEna = TRUE ������ ����� ���������� �. START_FM()
	ModulePars.FrequencyMeasurement.FmEna = (BYTE)(FmPars->FmEna ? 0x1 : 0x0);
	// == ����� ������ ���������� ������� - ���� ������ ����� ��������� ������� :(((
	if(FmPars->Mode >= GATE_MODE_FM_E310) FmPars->Mode = PERIOD_MODE_FM_E310;
	ModulePars.FrequencyMeasurement.Mode = FmPars->Mode;
	// == ��������� �������� �������� �������: 1:1 ��� 1:8
	if(FmPars->InputDivider >= INVALID_INPUT_DIVIDER_FM_E310) FmPars->InputDivider = INPUT_DIVIDER_1_FM_E310;
	ModulePars.FrequencyMeasurement.InputDivider = FmPars->InputDivider;
	// == ������ �������� ������� �������� ������� FM
	if(FmPars->BaseClockRateDivIndex >= INVALID_BASE_CLOCK_DIV_INDEX_FM_E310)
		FmPars->BaseClockRateDivIndex = BASE_CLOCK_DIV_01_INDEX_FM_E310;
	ModulePars.FrequencyMeasurement.BaseClockRateDivIndex = FmPars->BaseClockRateDivIndex;
	// == �������� ������ ���������� ������� � �
	//     ���� 000 --> +4B
	//     ���� 255 --> -4B
	if((FmPars->Offset < -4.0) || (FmPars->Offset > 4.0)) FmPars->Offset = 0.0;
	ModulePars.DigitalPotentiometers.FrequencyMeterPorog.BitFields.Value8bit = 255.0*(4.0 - FmPars->Offset)/8.0 + 0.5;
	ModulePars.DigitalPotentiometers.FrequencyMeterPorog.BitFields.ADDR 		 = 0x1;
	FmPars->Offset = 4.0 - 8.0*ModulePars.DigitalPotentiometers.FrequencyMeterPorog.BitFields.Value8bit/255.0;

	// == ������� �������� ������� �������� FM � ��
	ModulePars.FrequencyMeasurement.BaseClockRate = FmPars->BaseClockRate = 25000000;
	// == ������� �������� ������� �������� FM � ��
	ModulePars.FrequencyMeasurement.ClockRate = FmPars->ClockRate = FmPars->BaseClockRate >> FmPars->BaseClockRateDivIndex;

	// ������ ����� �������� ������� ��������� ������ � ������
	if(!PutArray((BYTE *)&ModulePars, sizeof(MODULE_PARS), MODULE_PARS_ADDRESS)) { LAST_ERROR_NUMBER(12); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ����� ������ ����������� (FM)
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::START_FM(void)
{
	WORD InBuf[4] = { 0x0, V_START_ADC, 0x1 << FM_START_INDEX, 0x0 };

	// ���� ��������� ������������ �������������� �������� �����, ������� ����������
	// � SPI MCU ������, �� ���������� �������� � ����������� ������� ����������
	if(TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA) { LAST_ERROR_NUMBER(8); return FALSE; }
	// ��� USB-������ � MCU �� ������ ����������� (FM)
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LAST_ERROR_NUMBER(13); return FALSE; }

	// ��������� ������ ��������� ���������� ������� (FM)
	IsFmBusy = TRUE;

	// �� ������
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ���� ������ ����������� (FM)
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::STOP_FM(void)
{
	WORD InBuf[4] = { 0x0, V_STOP_ADC, 0x1 << FM_START_INDEX, 0x0 };

	// ������� ������ ��������� ���������� ������� (FM)
	IsFmBusy = FALSE;

	// ��� USB-������ � MCU �� ������ ����������� (FM)
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LAST_ERROR_NUMBER(13); return FALSE; }
	// �� ������
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ���������� ������� ��������� �������
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::FM_SAMPLE(FM_SAMPLE_E310 * const FmSample)
{
	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!FmSample) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }
	// ���� ��������� ������������ �������������� �������� �����, ������� ����������
	// � SPI MCU ������, �� ���������� �������� � ����������� ������� ����������
	else if(TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA) { LAST_ERROR_NUMBER(8); return FALSE; }

	// ������ ����� ��������� ��������� ������� � ����������� �������
	if(!GetArray((BYTE *)&LocFmSample, sizeof(FM_DATA), FM_SAMPLE_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }
	// ��� ��� � �������� FM
	if(LocFmSample.IsActual)
	{
		// ��������� ������� ���������������� ������� ����������
		FmSample->IsActual = TRUE;
		// ����� ��������� �������
		if(ModulePars.FrequencyMeasurement.Mode == PERIOD_MODE_FM_E310)
		{
			// ������� �������
			FmSample->Frequency = ModulePars.FrequencyMeasurement.ClockRate/(1000.0 * LocFmSample.PeriodCode);
			// ������ �������
			FmSample->Period = 1000.0 * LocFmSample.PeriodCode / ModulePars.FrequencyMeasurement.ClockRate;
			// ���������� �������
			FmSample->DutyCycle = 1000.0 * LocFmSample.DutyCycleCode / ModulePars.FrequencyMeasurement.ClockRate;
		}
		// ��� ��������� ������� ���� ������ ��� :(((
		else
		{
			FmSample->Frequency = FmSample->Period = FmSample->DutyCycle = 0.0;
		}
	}
	else
	{
		// ������� ��� ���������
		FmSample->IsActual = FALSE;
		FmSample->Frequency = FmSample->Period = FmSample->DutyCycle = 0.0;
	}
	// �� ������
	return TRUE;
}




// ==================================================================================
// ------------------------ ������� ��� ������ ��� ----------------------------------
// ==================================================================================
// ----------------------------------------------------------------------------------
// ��������� ������� ���������� ������ ���
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GET_ADC_PARS(ADC_PARS_E310 * const AdcPars)
{
	WORD i;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!AdcPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E310; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E310[i]) break;
	if(i == REVISIONS_QUANTITY_E310) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ����������, ����������� � ���
	ZeroMemory(&LocAdcPars, sizeof(ADC_PARS));
	// ������ ����� ��������� ������� ��������� ������ �� ������
	if(!GetArray((BYTE *)&LocAdcPars, sizeof(ADC_PARS), ADC_PARS_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }

	// == �������� ����� �������� ������� ������� ���: ���������� ��� �������
	AdcPars->AdcStartSource = LocAdcPars.AdcStartSource;
	// == ������� ����� �������� �������
	AdcPars->ChannelsMask = (BYTE)(LocAdcPars.ChannelsMask >> 0x4);
	// == ������ ������� �������� ��� � �
	AdcPars->InputRange = ADC_INPUT_RANGE;

	// �� ������
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ��������� ��������� ���������� ������ ���
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::SET_ADC_PARS(ADC_PARS_E310 * const AdcPars)
{
	WORD i;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!AdcPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E310; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E310[i]) break;
	if(i == REVISIONS_QUANTITY_E310) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ����������, ����������� � ���
	ZeroMemory(&LocAdcPars, sizeof(ADC_PARS));

	// == �������� ����� �������� ������� ������� ���: ���������� ��� �������
	if(AdcPars->AdcStartSource >= INVALID_ADC_START_SOURCES_E310)
		AdcPars->AdcStartSource = INTERNAL_ADC_START_E310;
	LocAdcPars.AdcStartSource = AdcPars->AdcStartSource;

	// == ��������� � ������� ������ �������� �������
	AdcPars->ChannelsMask &= 0x0F;
	// ��� ������������� � MCU ������� ����� ������� �����
	LocAdcPars.ChannelsMask = (BYTE)(AdcPars->ChannelsMask << 0x4);

	// == ������ ������� �������� ��� � �
	AdcPars->InputRange = ADC_INPUT_RANGE;

	// ������ ����� �������� ������� ��������� ������ � ������
	if(!PutArray((BYTE *)&LocAdcPars, sizeof(ADC_PARS), ADC_PARS_ADDRESS)) { LAST_ERROR_NUMBER(12); return FALSE; }
	// �� ������
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ���������� ������� �������� � ���
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GET_ADC_DATA(ADC_DATA_E310 * const AdcData)
{
	WORD i;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ��������� ���������� ����������
	else if(!AdcData) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// ������� ��������� ����������, ����������� � ���
	ZeroMemory(&LocAdcData, sizeof(ADC_DATA));
	// ������ ����� ��������� ������� �������� � ���
	if(!GetArray((BYTE *)&LocAdcData, sizeof(ADC_DATA), ADC_DATA_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }

	// �������� ������ � ���
	for(i = 0x0; i < ADC_CHANNEL_QUANTITY_E310; i++)
	{
		AdcData->DataInCode[i]	= LocAdcData.DataInCode[i];		// � �����
		if(AdcData->DataInCode[i] != (-1))
			AdcData->DataInV[i] = ADC_INPUT_RANGE * AdcData->DataInCode[i] / 1024.0;	// � �
		else
			AdcData->DataInV[i] = -1.0;	// � �
	}

	// �� ������
	return TRUE;
}




// ==================================================================================
// ---------------- ������� ��� ������ � ��������� ������� --------------------------
// ==================================================================================
// ----------------------------------------------------------------------------------
// ���������������� �������� �����: ���� ��� �����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::CONFIG_TTL_LINES(WORD Pattern, BOOL AddTtlLinesEna)
{
	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// �������� ���������� ������������ �������������� �������� �����, �������
	// ���������� � SPI MCU ������. �� ������������� ��������� �����������
	// ����������� ������ � ����������� � ����������� �������
	TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA 	= (WORD)(AddTtlLinesEna ? 0x1 : 0x0);
	// ����� ���������������� �������� �����
	if(AddTtlLinesEna)
		TtlPars.TtlConfig.BitFields.CONFIG_PATTERN	= (WORD)(Pattern & 0x07FF);
	else
		TtlPars.TtlConfig.BitFields.CONFIG_PATTERN	= (WORD)(Pattern & 0x00FF);

	// �������� ����� ������������ �������� �����
	TtlPars.Mode = TLL_LINES_CONFIG;

	// ������ ����� �������� ������� ������������ �������� ����� � ������
	if(!PutArray((BYTE *)&TtlPars, sizeof(TTL_PARS), TTL_PARS_ADDRESS)) { LAST_ERROR_NUMBER(12); return FALSE; }
	// �� ������
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ������ ��������� �������� ������� �����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::TTL_IN (WORD * const TtlIn)
{
	WORD TtlInPattern;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }

	// ������ ����� ��������� ��������� � ����������� �������� �����
	if(!GetArray((BYTE *)&TtlPars, sizeof(TTL_PARS), TTL_PARS_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }
	// ��������� ������� �������� ����� ��������� ������������
	if(TtlPars.TtlConfig.BitFields.ADD_TTL_LINES_ENA)
		TtlInPattern = (WORD)((~TtlPars.TtlConfig.BitFields.CONFIG_PATTERN) & 0x07FF);
	else
		TtlInPattern = (WORD)((~TtlPars.TtlConfig.BitFields.CONFIG_PATTERN) & 0x00FF);
	// �������� ��������� ������ �������� ������� �����
	*TtlIn = TtlPars.TtlIn & TtlInPattern;
	// �� ������
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ��������� ��������� �������� �������� �����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::TTL_OUT(WORD * const TtlOut)
{
	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E-310")) { LAST_ERROR_NUMBER(0); return FALSE; }
	// ���������� ��������� �������� �������� �����
	*TtlOut &= TtlPars.TtlConfig.BitFields.CONFIG_PATTERN;
	TtlPars.TtlOut = *TtlOut;
	// �������� ����� ������ �������� �����
	TtlPars.Mode = TTL_LINES_OUT;
	// ������ ����� �������� ������� ������������ �������� ����� � ������
	if(!PutArray((BYTE *)&TtlPars, sizeof(TTL_PARS), TTL_PARS_ADDRESS)) { LAST_ERROR_NUMBER(12); return FALSE; }
	// �� ������
	else return TRUE;
}



// ==================================================================================
// ------------ ������� ��� ������ � ���������������� ����������� ����  -------------
// ==================================================================================
// ----------------------------------------------------------------------------------
// ����������/���������� ������ ������ � ���������������� ������� ����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled)
{
	// �������� �������
	this->IsUserFlashWriteEnabled = IsUserFlashWriteEnabled;
	// ��� ������ :)))))
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ������� ���������������� ������� ����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::READ_FLASH_ARRAY(USER_FLASH_E310 * const UserFlash)
{
	// �������� ���������
	if(!UserFlash) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ���� ������ ������
	else if(!GetArray((BYTE *)UserFlash, sizeof(USER_FLASH_E310), USER_FLASH_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

// ----------------------------------------------------------------------------------
// ������ ���������������� ������� ����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::WRITE_FLASH_ARRAY(USER_FLASH_E310 * const UserFlash)
{
	BYTE *ptr;
	DWORD Address, Size;
	DWORD i = 0x0;

	// ��� ������ �������� ���������� �� ������
	if(!IsUserFlashWriteEnabled) { LAST_ERROR_NUMBER(15); return FALSE; }
	// ������ �������� ���������
	else if(!UserFlash) { LAST_ERROR_NUMBER(3); return FALSE; }

	// �������� ������ ����������� ������
	EnterCriticalSection(&cs);
	// ���� ����� ��������� ��������� ��������������� � ����� '����������' (BootLoader)
	if(IsMcuApplicationActive)
		if(!RunMcuApplication(BOOT_LOADER_START_ADDRESS)) { LAST_ERROR_NUMBER(1); LeaveCriticalSection(&cs); return FALSE; }

	Address = USER_FLASH_ADDRESS;
	Size = sizeof(USER_FLASH_E310);
	ptr = (BYTE *)UserFlash;
	while(i < Size)
	{
		DWORD sz = std::min((DWORD)MAX_USB_CONTROL_PIPE_BLOCK, Size - i);
		WORD InBuf[4] = {0x0000, 13/*Special PutArray*/, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10) };

		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), ptr + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(13); return FALSE; }
		//
		Address += sz;
		i += sz;
	 }

	// ��������� ������� ��������������� � ����� '����������' (Application)
	if(!RunMcuApplication(FIRMWARE_START_ADDRESS)) { LAST_ERROR_NUMBER(2); LeaveCriticalSection(&cs); return FALSE; }
	// ��������� ������ ����������� ������
	LeaveCriticalSection(&cs);
	// ��� ������ :)))))
	return TRUE;
}





// ==================================================================================
// ------------- ������� ��� ������ �� ��������� ����������� �� ���� ----------------
// ==================================================================================
// ----------------------------------------------------------------------------------
// ������� ��������� ���������� � ������ �� ����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E310 * const md)
{
	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ���������
	else if(!md) { LAST_ERROR_NUMBER(3); return FALSE; }

	// ������� ��������� ���������� ������
	if(!GetModuleDescriptor()) return FALSE;

	// ������ ��������� �����������
	if(!GetArray((BYTE*)&FirmwareDescriptor, sizeof(FIRMWARE_DESCRIPTOR), FIRMWARE_DESCRIPTOR_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }
	else if(!GetArray((BYTE*)&BootLoaderDescriptor, sizeof(BOOT_LOADER_DESCRIPTOR), BOOT_LOADER_DESCRIPTOR_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }

	// ������� ��� ��������� MODULE_DESCRIPTION_E310
	ZeroMemory(md, sizeof(MODULE_DESCRIPTION_E310));

	// Module Info
	strncpy((char *)md->Module.CompanyName,  "L-Card Ltd.",    sizeof(md->Module.CompanyName));
	strncpy((char *)md->Module.DeviceName,   (char *)ModuleDescriptor.ModuleName,    std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.ModuleName)));
	strncpy((char *)md->Module.SerialNumber, (char *)ModuleDescriptor.SerialNumber,  std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
	strcpy((char *)md->Module.Comment,      			"������� ������ ����������-����������� �� ���� USB 1.1");
	md->Module.Revision = ModuleDescriptor.Revision;
	md->Module.Modification = NO_MODULE_MODIFICATION_LUSBAPI;

	// Interface Info
	md->Interface.Active = TRUE;
	strcpy((char *)md->Interface.Name, "USB 1.1");
	strcpy((char *)md->Interface.Comment, "Full-Speed Mode (12 Mbit/s)");

	// MCU Info
	md->Mcu.Active = TRUE;
	strncpy((char *)md->Mcu.Name,   									(char *)ModuleDescriptor.McuName,    			std::min(sizeof(md->Mcu.Name), sizeof(ModuleDescriptor.McuName)));
	strncpy((char *)md->Mcu.Version.FwVersion.Version,  		(char *)FirmwareDescriptor.Version, 			std::min(sizeof(md->Mcu.Version.FwVersion.Version), sizeof(FirmwareDescriptor.Version)));
	strncpy((char *)md->Mcu.Version.FwVersion.Date,  			(char *)FirmwareDescriptor.Created, 			std::min(sizeof(md->Mcu.Version.FwVersion.Date), sizeof(FirmwareDescriptor.Created)));
	strncpy((char *)md->Mcu.Version.FwVersion.Manufacturer,  (char *)FirmwareDescriptor.Manufacturer, 		std::min(sizeof(md->Mcu.Version.FwVersion.Manufacturer), sizeof(FirmwareDescriptor.Manufacturer)));
	strncpy((char *)md->Mcu.Version.FwVersion.Author,  		(char *)FirmwareDescriptor.Author, 				std::min(sizeof(md->Mcu.Version.FwVersion.Author), sizeof(FirmwareDescriptor.Author)));
	strncpy((char *)md->Mcu.Version.FwVersion.Comment,			(char *)FirmwareDescriptor.Comment, 			std::min(sizeof(md->Mcu.Version.FwVersion.Comment), sizeof(FirmwareDescriptor.Comment)));
	strncpy((char *)md->Mcu.Version.BlVersion.Version,  		(char *)BootLoaderDescriptor.Version, 			std::min(sizeof(md->Mcu.Version.BlVersion.Version), sizeof(BootLoaderDescriptor.Version)));
	strncpy((char *)md->Mcu.Version.BlVersion.Date,  			(char *)BootLoaderDescriptor.Created, 			std::min(sizeof(md->Mcu.Version.BlVersion.Date), sizeof(BootLoaderDescriptor.Created)));
	strncpy((char *)md->Mcu.Version.BlVersion.Manufacturer,  (char *)BootLoaderDescriptor.Manufacturer, 	std::min(sizeof(md->Mcu.Version.BlVersion.Manufacturer), sizeof(BootLoaderDescriptor.Manufacturer)));
	strncpy((char *)md->Mcu.Version.BlVersion.Author,  		(char *)BootLoaderDescriptor.Author, 			std::min(sizeof(md->Mcu.Version.BlVersion.Author), sizeof(BootLoaderDescriptor.Author)));
	strncpy((char *)md->Mcu.Version.BlVersion.Comment,			"������� ��������� ��� �������� USB ������ E-310", sizeof(md->Mcu.Version.BlVersion.Comment));
	strncpy((char *)md->Mcu.Comment, 								"32-bit Microcontroller with 64K Bytes Internal High-speed Flash", sizeof(md->Mcu.Comment));
	md->Mcu.ClockRate = ModuleDescriptor.ClockRate/1000.0;	// � ���
	// ����������� � ������� ������ Firmware ����������������
	FirmwareVersionNumber = atof((char *)md->Mcu.Version.FwVersion.Version);
	if((FirmwareVersionNumber < 0.75) || (FirmwareVersionNumber > 20.0)) { LAST_ERROR_NUMBER(16); return FALSE; }

	// ADC Info
	md->Adc.Active = TRUE;
	strcpy((char *)md->Adc.Name, "Built-in ARM");
	strcpy((char *)md->Adc.Comment, "4 Channels, 10-Bit ADC converter");

	// DAC Info
	md->Dac.Active = TRUE;
	strcpy((char *)md->Dac.Name, "AD5932");
	strcpy((char *)md->Dac.Comment, "Programmable Frequency Scan Waveform Generator (DDS)");

	// IO Info
	md->DigitalIo.Active = TRUE;
	strcpy((char *)md->DigitalIo.Name, "Built-in ARM");
	md->DigitalIo.InLinesQuantity = 11;
	md->DigitalIo.OutLinesQuantity = 11;
	strcpy((char *)md->DigitalIo.Comment, "8/11 TTL/CMOS compatible lines");

	// ��� ������ :)))))
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ������� ��������� ���������� � ������ � ����
// ----------------------------------------------------------------------------------
BOOL WINAPI TLE310::SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E310 * const md)
{
//	WORD i;

	// �������� ��������� �� ���������
	if(!md) { LAST_ERROR_NUMBER(3); return FALSE; }

	// ������� ��� ��������� MODULE_DESCRIPTOR
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));

	// Module Info
	strncpy((char *)ModuleDescriptor.ModuleName,		(char *)md->Module.DeviceName,	 	std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.ModuleName)));
	strncpy((char *)ModuleDescriptor.SerialNumber, 	(char *)md->Module.SerialNumber, 	std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
//	strncpy((char *)ModuleDescriptor.Reserved,     	(char *)md->Module.Comment,      	std::min(sizeof(md->Module.Comment), sizeof(ModuleDescriptor.Reserved)));

	// �������� ��������� ������� ������
	if((	md->Module.Revision > REVISIONS_E310[REVISIONS_QUANTITY_E310 - 0x1] ||
			md->Module.Revision < REVISIONS_E310[REVISION_A_E310])) { LAST_ERROR_NUMBER(17); return FALSE; }
	else ModuleDescriptor.Revision = md->Module.Revision;

	// MCU Info
	strncpy((char *)ModuleDescriptor.McuName, (char *)md->Mcu.Name, std::min(sizeof(md->Mcu.Name), sizeof(ModuleDescriptor.McuName)));
	ModuleDescriptor.ClockRate = 1000*MCU_MASTER_CLOCK;		// � ��

	// ������ �������� CRC16 ������������ ���������
	ModuleDescriptor.CRC16 = 0x0;
	ModuleDescriptor.CRC16 = CalculateCrc16((BYTE *)&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR) - sizeof(WORD));

	// ��������� �������� �������������� ���������� ������
	if(!SaveModuleDescriptor()) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}






// ==================================================================================
//  ���������� ������� ��� ������ � �������
// ==================================================================================
// ----------------------------------------------------------------------------------
// ��������� ����������� ������� ����� ������ ��� �������� ���� USB � ����� suspend
// ----------------------------------------------------------------------------------
BOOL TLE310::SetSuspendModeFlag(BOOL SuspendModeFlag)
{
/*	BYTE Flag = (BYTE)(SuspendModeFlag ? 0x1 : 0x0);

	if(!PutArray(&Flag, 0x1, SUSPEND_MODE_FLAG)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else*/ return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����������� ������
//------------------------------------------------------------------------------
BOOL TLE310::GetModuleDescriptor(void)
{
	// ������ �� ���� �������� ����� ��������� ���� MODULE_DESCRIPTOR
	if(!GetArray((BYTE *)&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR), MODULE_DESCRIPTOR_ADDRESS)) { LAST_ERROR_NUMBER(6); return FALSE; }
	// ������ � CRC ���������� �� ���� ��������� ������
	if(ModuleDescriptor.CRC16 != CalculateCrc16((BYTE *)&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR) - sizeof(WORD))) { LAST_ERROR_NUMBER(19); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����������� ������
//------------------------------------------------------------------------------
BOOL TLE310::SaveModuleDescriptor(void)
{
	BYTE *ptr;
	DWORD Address, MdSize;
	DWORD i = 0x0;

	// �������� ������ ����������� ������
	EnterCriticalSection(&cs);
	// ���� ����� ��������� ��������� ��������������� � ����� '����������' (BootLoader)
	if(IsMcuApplicationActive)
		if(!RunMcuApplication(BOOT_LOADER_START_ADDRESS)) { LAST_ERROR_NUMBER(1); LeaveCriticalSection(&cs); return FALSE; }

	// ��������� ��������� � ����
	Address = MODULE_DESCRIPTOR_ADDRESS;
	MdSize = sizeof(MODULE_DESCRIPTOR);
	ptr = (BYTE *)&ModuleDescriptor;
	while(i < MdSize)
	{
		DWORD sz = std::min((DWORD)MAX_USB_CONTROL_PIPE_BLOCK, MdSize - i);
		WORD InBuf[4] = {0x0000, 13/*Special PutArray*/, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10) };

		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), ptr + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(13); return FALSE; }
		//
		Address += sz;
		i += sz;
	}

	// ��������� ������� ��������������� � ����� '����������' (Application)
	if(!RunMcuApplication(FIRMWARE_START_ADDRESS)) { LAST_ERROR_NUMBER(2); LeaveCriticalSection(&cs); return FALSE; }
	// ��������� ������ ����������� ������
	LeaveCriticalSection(&cs);
	// ��� ������ :)))))
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ������ ������� ������ �� ������ ����������������
// ----------------------------------------------------------------------------------
BOOL TLE310::GetArray(BYTE * const Data, DWORD Size, DWORD Address)
{
	DWORD i = 0x0;

	// ��� ������ �������� ������� ������� ������
	if(!Data || !Size) { return FALSE; }

	// ������ ������ ������ �� ������ ����������������
	while(i < Size)
	{
		DWORD sz = std::min((DWORD)MAX_USB_CONTROL_PIPE_BLOCK, Size - i);
		WORD InBuf[4] = { 0x01, V_GET_ARRAY, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10)};

		// �������� ������ ����������� ������
		EnterCriticalSection(&cs);
		// �������� ������ ������ ���������������� � ������ '����������' (Application)
		if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); LeaveCriticalSection(&cs); return FALSE; }

		// �������� ������ � ������
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(13); return FALSE; }

		// ��������� ������ ����������� ������
		LeaveCriticalSection(&cs);
		//
		Address += sz;
		i += sz;
	}
	// ��� ������ :)))))
	return TRUE;
}

// ----------------------------------------------------------------------------------
// ������ ������� ������ � ������ ����������������
// ----------------------------------------------------------------------------------
BOOL TLE310::PutArray(BYTE * const Data, DWORD Size, DWORD Address)
{
	DWORD i = 0x0;

	// ��� ������ �������� ������� ������� ������
	if(!Data || !Size) { return FALSE; }

	// ����� ������ ������ � ������ ����������������
	while(i < Size)
	{
		DWORD sz = std::min((DWORD)MAX_USB_CONTROL_PIPE_BLOCK, Size - i);
		WORD InBuf[4] = { 0x00, V_PUT_ARRAY, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10) };

		// �������� ������ ����������� ������
		EnterCriticalSection(&cs);

		// �������� ������ ������ ���������������� � ������ '����������' (Application)
		if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); LeaveCriticalSection(&cs); return FALSE; }
		// �������� ������ � ������
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(13); return FALSE; }

		// ��������� ������ ����������� ������
		LeaveCriticalSection(&cs);
		//
		Address += sz;
		i += sz;
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ���������������� � ������ '����������' (Application) ��� '����������' (BootLoader)
//------------------------------------------------------------------------------
BOOL TLE310::RunMcuApplication(DWORD Address, WORD BitParam)
{
	// ��������� ������ ������� ���������� � ����������������
	WORD InBuf[4] = { 0x00, V_CALL_APPLICATION, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10) };

	// �������� �����
	if(Address == (DWORD)FIRMWARE_START_ADDRESS)
	{
		InBuf[0x2] &= 0xFF00;
		InBuf[0x2] |= (WORD)(BitParam ?  0x1 : 0x0);
	}
	else if(Address == (DWORD)BOOT_LOADER_START_ADDRESS)
	{
		InBuf[0x3] = 0x0;
	}
	else { return FALSE; }

	// �������� ������ ����������� ������
	EnterCriticalSection(&cs);
	// ������� ������ ������ ���������������� � ������ '����������' (Application)
	IsMcuApplicationActive = FALSE;
	// �������� ������ � ������
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(13); return FALSE; }
	// �� ��������� ���������� � ����������������?
	IsMcuApplicationActive = (Address == (DWORD)BOOT_LOADER_START_ADDRESS) ? FALSE : TRUE;
	// ��������� ������ ����������� ������
	LeaveCriticalSection(&cs);

	// ��� ������ :)))))
	return TRUE;
}


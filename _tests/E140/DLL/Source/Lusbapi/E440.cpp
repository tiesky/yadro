//------------------------------------------------------------------------------
#include <math.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include "e440.h"
#include "ErrorBaseIds.h"
//------------------------------------------------------------------------------

#define 	LAST_ERROR_NUMBER(ErrorNumber)	LastErrorNumber = E440_BASE_ERROR_ID + ErrorNumber

//------------------------------------------------------------------------------
// ����������
//------------------------------------------------------------------------------
TLE440::TLE440(HINSTANCE hInst) : TLUSBBASE(hInst)
{
	// ������ ������ � ���� ������ ���������
	EnableFlashWrite = FALSE;
	// ������ � ��������� ������� ���� ������ ���������
	IsServiceFlashWriting = FALSE;
	// ����� ������ �������� ���� � LBIOS � ������ ��������
	ProgramBaseAddress = 0x1000;
	// ����� ������ �������� ����� ������ � ������ ��������
	DataBaseAddress = DataBaseAddress_E440;
	// ������� ������ ������� �������� �������� ����� ������
	EnableTtlOut = FALSE;
	// ������� ��� �������������� ��������� ������ MCU
	McuAttributes = NO_MCU_ATTRIBUTES;

	// ������������� ���� ��������� �������� ������
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));
	ZeroMemory(&DspDescriptor, sizeof(DSP_DESCRIPTOR));
	ZeroMemory(&FirmwareDescriptor, sizeof(FIRMWARE_DESCRIPTOR));
	ZeroMemory(&AdcPars, sizeof(ADC_PARS_E440));
	ZeroMemory(&DacPars, sizeof(DAC_PARS_E440));
}

//------------------------------------------------------------------------------
// ����������
//------------------------------------------------------------------------------
TLE440::~TLE440() { }




//==============================================================================
// ������� ������ ���������� ��� ������ � ������� �14-440
//==============================================================================
//------------------------------------------------------------------------------
// ������� ����������� ���� ��� ������� � USB ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::OpenLDevice(WORD VirtualSlot)
{
	char ModuleName[LONG_MODULE_NAME_STRING_LENGTH];

	//  ��������� ������� ����������� ���� ��� ������� � USB ������
	if(!TLUSBBASE::OpenLDeviceByID(VirtualSlot, E14_440_ID)) return FALSE;
	// ��������� ��������� �������� ������
	else if(!TLUSBBASE::GetModuleName(ModuleName)) { TLUSBBASE::CloseLDevice(); return FALSE; }
	// ��������, ��� ��� ������ E14-440
	else if(strcmp(ModuleName, "E440")) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(0); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ������� ����������� ����
//------------------------------------------------------------------------------------
BOOL WINAPI TLE440::CloseLDevice(void)
{
	// ���� ����� - ��������� ��������� �����
//	if(IoReq.Buffer) { delete[] IoReq.Buffer; IoReq.Buffer = NULL; }
	// ������� ����������
	if(!TLUSBBASE::CloseLDevice()) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ��������� �� ����������
//------------------------------------------------------------------------------------
BOOL WINAPI TLE440::ReleaseLInstance(void)
{
	BOOL Status = TRUE;

	// ������� ��� �������������� ��������� ������ ����������������
	if(!SetMcuAttributes(NO_MCU_ATTRIBUTES)) { LAST_ERROR_NUMBER(1); Status = FALSE; }
	// ��������� ������������� ������������ �����
	if(!CloseLDevice()) Status = FALSE;
	// ��������� ��������� �� ����������
	delete this;
	// ���������� ������ ���������� �������
	return Status;
}

//------------------------------------------------------------------------------
// ������������ ���������� ����������
//------------------------------------------------------------------------------
HANDLE WINAPI TLE440::GetModuleHandle(void) { return hDevice; }

//------------------------------------------------------------------------------
//  ������ �������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GetModuleName(PCHAR const ModuleName)
{
	// �������� �����
	if(!ModuleName) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������ ��������� ������ �������� ������
	else if(!TLUSBBASE::GetModuleName(ModuleName)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------------
//  ��������� ������� �������� ������ ���� USB
//------------------------------------------------------------------------------------
BOOL WINAPI TLE440::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// �������� ���������
	if(!UsbSpeed) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������ ��������� ������ �������� ������ USB ����
	else if(!TLUSBBASE::GetUsbSpeed(UsbSpeed)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ������� ������� ������������������ ������ E14-440
// �� �.�. ������ ����� �� �������������� �������, �� ������ ���������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::LowPowerMode(BOOL LowPowerFlag)
{
	if(!TLUSBBASE::LowPowerMode(LowPowerFlag)) return FALSE;
   else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ������ � ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	return TLUSBBASE::GetLastErrorInfo(LastErrorInfo);
}



//==============================================================================
// ������� ��� ������ � DSP ������ �14-440
//==============================================================================
//------------------------------------------------------------------------------
// ����� ������ (DSP, ���� � ��������� �������)
//  0x0 - ����� ����������������� ������, ����� ���� ������ ����� ����� � �������� DSP
//  0x1 - ����� ������� ������ ������ (�.�. DSP ��������� � ���������
//		                                         ������, ��������� ������� ���������)
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::RESET_MODULE(BYTE ResetFlag)
{
	WORD InBuf[4] = { 0x0, V_RESET_MODULE, ResetFlag, 0x0 };

	// �������� ������������ ��������� �������
	if(ResetFlag >= INVALID_RESET_TYPE_E440) { LAST_ERROR_NUMBER(61); return FALSE; }
	// �������� USB-������ V_RESET_MODULE � MCU
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
													&InBuf, sizeof(InBuf),
													NULL, 0x0/*bytes*/,
													TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ������� �������� LBIOS � DSP ������ E14-440
//------------------------------------------------------------------------------------
BOOL WINAPI TLE440::LOAD_MODULE(PCHAR const FileName)
{
	// ��������� ������ ��������� ��������� LBIOS
	if(FileName == NULL)
	{
		// ������� ��������� DSP ������ �� ������� ���������� Lusbapi
		if(!LoadModuleFromResource()) { return FALSE; }
	}
	else
	{
		// ������� ��������� DSP ������ �� ������������ ����
		if(!strcmpi(FileName, "flash"))
		{
			if(!LoadModuleFromBootFlash()) { LAST_ERROR_NUMBER(65); return FALSE; }
		}
		// ������� ��������� DSP ������ �� �����
		else { if(!LoadModuleFromFile(FileName)) { return FALSE; } }
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// �������� ����������������� ������������ ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::TEST_MODULE(void)
{
	WORD TMode1; if(!GET_LBIOS_WORD(L_TMODE1_E440, (SHORT *)&TMode1)) { LAST_ERROR_NUMBER(16); return FALSE; }
	WORD TMode2; if(!GET_LBIOS_WORD(L_TMODE2_E440, (SHORT *)&TMode2)) { LAST_ERROR_NUMBER(16); return FALSE; }
	if((TMode1 != 0x5555) || (TMode2 != 0xAAAA)) { LAST_ERROR_NUMBER(17);  return FALSE; }
	else
	{
		WORD Ready;
		WORD LocTimeOut = 50;

		// ������� � DSP ������ �������� ����������
		if(!PUT_LBIOS_WORD(L_TEST_LOAD_E440, 0x77BB)) { LAST_ERROR_NUMBER(18); return FALSE; }

		// ��� ���� DSP ������ �� �������� ������� ����������
		do
		{
			Sleep(20);
			if(!GET_LBIOS_WORD(L_READY_E440, (SHORT *)&Ready)) { LAST_ERROR_NUMBER(16); return FALSE; }
		} while((!Ready) && (LocTimeOut--));
		if(TimeOut == 0xFFFF)  { LAST_ERROR_NUMBER(19); return FALSE; }

		// ��� � DSP ������ �������� �������
		if(!SEND_COMMAND(C_TEST)) { LAST_ERROR_NUMBER(20); return FALSE; }
		// ����� ���������� �������� ������� � L_TEST_LOAD_E440 ������ ���� 0xAA55
		WORD TestLoadVar; if(!GET_LBIOS_WORD(L_TEST_LOAD_E440, (SHORT *)&TestLoadVar)) { LAST_ERROR_NUMBER(16); return FALSE; }
		if(TestLoadVar != 0xAA55) { LAST_ERROR_NUMBER(21); return FALSE; }
	}
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� � ������ ������ �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::SEND_COMMAND(WORD Command)
{
	WORD InBuf[4] = { 0x0, V_COMMAND_IRQ, 0x0, 0x0 };
	WORD LocCommand, TimeOut = 100;

	// �������� ��������� ����� �������
	if(Command >= C_LAST_COMMAND) { LAST_ERROR_NUMBER(22); return FALSE; }

	// �������� ������ ����������� ������
	EnterCriticalSection(&cs);

	// ������ ����� ������� � DSP ������
	if(!PUT_LBIOS_WORD(L_COMMAND_E440, Command)) { LAST_ERROR_NUMBER(18); LeaveCriticalSection(&cs); return FALSE; }
	// �������� ��������� ���������� � DSP ������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0x0, this->TimeOut)) { LAST_ERROR_NUMBER(3); LeaveCriticalSection(&cs); return FALSE; }
	// ��������, ��� ������� ��������
	do
	{
		if(!GET_LBIOS_WORD(L_COMMAND_E440, (SHORT *)&LocCommand)) { LAST_ERROR_NUMBER(16); LeaveCriticalSection(&cs); return FALSE; }
		else if(LocCommand == 0xFFFF) { LAST_ERROR_NUMBER(58); LeaveCriticalSection(&cs); return FALSE; }
	} while(LocCommand && (TimeOut--));

	// ��������� ������ ����������� ������
	LeaveCriticalSection(&cs);

	// �������� ������� ���������� �������
	if(TimeOut == 0xFFFF) { LAST_ERROR_NUMBER(23); return FALSE; }
	// �� ������
	else return TRUE;
}




//==============================================================================
// ������� ��� ������ � ��� ������
//==============================================================================

//------------------------------------------------------------------------------
// ��������� ������� ���������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI WINAPI TLE440::GET_ADC_PARS(ADC_PARS_E440 * const AdcPars)
{
	WORD i;
	WORD IsAdcEnabled, IsCorrectionEnabled;
	WORD SclockDiv, InterKadrDelay;
	WORD AdcOffsetCoef, AdcScaleCoef;
	double DspClockout;

	// �������� ��������� ���������� ������ ���
	if(!AdcPars) { LAST_ERROR_NUMBER(2); return FALSE; }

	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E440")) { LAST_ERROR_NUMBER(24); return FALSE; }
	else if(!GET_LBIOS_WORD(L_ADC_ENABLED_E440, (SHORT *)&IsAdcEnabled)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(!GET_LBIOS_WORD(L_CORRECTION_ENABLED_E440, (SHORT *)&IsCorrectionEnabled)) { LAST_ERROR_NUMBER(16); return FALSE; }
	AdcPars->IsAdcEnabled = IsAdcEnabled ? TRUE : FALSE;
	AdcPars->IsCorrectionEnabled = IsCorrectionEnabled ? TRUE : FALSE;

	// ��������� ������������� ����� ������ � ���
	if(!GET_LBIOS_WORD(L_INPUT_MODE_E440, (SHORT *)&AdcPars->InputMode)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(!GET_LBIOS_WORD(L_SYNCHRO_AD_TYPE_E440, (SHORT *)&AdcPars->SynchroAdType)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(!GET_LBIOS_WORD(L_SYNCHRO_AD_MODE_E440, (SHORT *)&AdcPars->SynchroAdMode)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(!GET_LBIOS_WORD(L_SYNCHRO_AD_CHANNEL_E440, (SHORT *)&AdcPars->SynchroAdChannel)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(!GET_LBIOS_WORD(L_SYNCHRO_AD_POROG_E440, (SHORT *)&AdcPars->SynchroAdPorog)) { LAST_ERROR_NUMBER(16); return FALSE; }

	// ����������� �������
	if(!GET_LBIOS_WORD(L_CONTROL_TABLE_LENGHT_E440, (SHORT *)&AdcPars->ChannelsQuantity)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(AdcPars->ChannelsQuantity > MAX_CONTROL_TABLE_LENGTH_E440) { LAST_ERROR_NUMBER(25); return FALSE; }
	for(i = 0x0; i < AdcPars->ChannelsQuantity; i++) if(!GET_LBIOS_WORD((WORD)(L_CONTROL_TABLE_E440 + i), (SHORT *)&AdcPars->ControlTable[i])) { LAST_ERROR_NUMBER(16); return FALSE; }

	// �������� �������� ��������� ������� ���
	if(!GET_LBIOS_WORD(L_ADC_CLOCK_SOURCE_E440, (SHORT *)&AdcPars->AdcClockSource)) { LAST_ERROR_NUMBER(18); return FALSE; }

	// ��������� ��������� ������ ���
	DspClockout = 2.0*ModuleDescriptor.QuartzFrequency/1000.0; // �������� ������� DSP � ���
	if(fabs(DspClockout - 48000.0) > 0.00001) { LAST_ERROR_NUMBER(26); return FALSE; }
	if(!GET_LBIOS_WORD(L_ADC_RATE_E440, (SHORT *)&SclockDiv)) { LAST_ERROR_NUMBER(16); return FALSE; }
	AdcPars->AdcRate = DspClockout/(2.0*(WORD)(SclockDiv + 1.0));
	if(!GET_LBIOS_WORD(L_INTER_KADR_DELAY_E440, (SHORT *)&InterKadrDelay)) { LAST_ERROR_NUMBER(16); return FALSE; }
	AdcPars->InterKadrDelay = (WORD)(InterKadrDelay + 1.0)/AdcPars->AdcRate; // ����������� �������� � ��
	AdcPars->KadrRate = 1.0/((AdcPars->ChannelsQuantity - 1.0)/AdcPars->AdcRate + AdcPars->InterKadrDelay);

	// FIFO ����� ���
	if(!GET_LBIOS_WORD(L_ADC_FIFO_BASE_ADDRESS_E440, (SHORT *)&AdcPars->AdcFifoBaseAddress)) { LAST_ERROR_NUMBER(16); return FALSE; }
	if(!GET_LBIOS_WORD(L_CUR_ADC_FIFO_LENGTH_E440, (SHORT *)&AdcPars->AdcFifoLength)) { LAST_ERROR_NUMBER(16); return FALSE; }

	// ������� ������������� ������������ ��� ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E440; i++)
	{
		// �������� ��� ��� �������� i
		if(!GET_LBIOS_WORD((WORD)(L_ZERO_E440 + i), (SHORT *)&AdcOffsetCoef)) { LAST_ERROR_NUMBER(16); return FALSE; }
		AdcPars->AdcOffsetCoefs[i] = AdcOffsetCoef;
		// ������� ��� ��� �������� i
		if(!GET_LBIOS_WORD((WORD)(L_SCALE_E440 + i), (SHORT *)&AdcScaleCoef)) { LAST_ERROR_NUMBER(16); return FALSE; }
		AdcPars->AdcScaleCoefs[i] = (double)AdcScaleCoef/(double)0x8000;
	}
	// �������� ��������� ������ ���
	this->AdcPars = *AdcPars;
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ��������� ���������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::SET_ADC_PARS(ADC_PARS_E440 * const AdcPars)
{
	WORD i;
	WORD AdcOffsetCoef, AdcScaleCoef;
	DWORD SclockDiv, KadrDelay;
	double DspClockOut;

	// �������� ��������� ���������� ������ ���
	if(!AdcPars) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��������� �������� ������
	else if(strcmp((char *)ModuleDescriptor.Name, "E440")) { LAST_ERROR_NUMBER(24); return FALSE; }

	// ��������� ������������� ����� ������ � ���
	if(!PUT_LBIOS_WORD(L_CORRECTION_ENABLED_E440, (SHORT)(AdcPars->IsCorrectionEnabled))) { LAST_ERROR_NUMBER(18); return FALSE; }
	else if(!PUT_LBIOS_WORD(L_INPUT_MODE_E440, AdcPars->InputMode)) { LAST_ERROR_NUMBER(18); return FALSE; }
	else if(AdcPars->InputMode == ANALOG_SYNC_E440)
	{
		if(!PUT_LBIOS_WORD(L_SYNCHRO_AD_TYPE_E440, AdcPars->SynchroAdType)) { LAST_ERROR_NUMBER(18); return FALSE; }
		if(!PUT_LBIOS_WORD(L_SYNCHRO_AD_MODE_E440, AdcPars->SynchroAdMode)) { LAST_ERROR_NUMBER(18); return FALSE; }
		if(!PUT_LBIOS_WORD(L_SYNCHRO_AD_CHANNEL_E440, AdcPars->SynchroAdChannel)) { LAST_ERROR_NUMBER(18); return FALSE; }
		if(!PUT_LBIOS_WORD(L_SYNCHRO_AD_POROG_E440, AdcPars->SynchroAdPorog)) { LAST_ERROR_NUMBER(18); return FALSE; }
	}

	// ����������� �������
	if(!AdcPars->ChannelsQuantity) { LAST_ERROR_NUMBER(62); return FALSE; }
	else if(AdcPars->ChannelsQuantity > MAX_CONTROL_TABLE_LENGTH_E440) AdcPars->ChannelsQuantity = MAX_CONTROL_TABLE_LENGTH_E440;
	else if(!PUT_LBIOS_WORD(L_CONTROL_TABLE_LENGHT_E440, AdcPars->ChannelsQuantity)) { LAST_ERROR_NUMBER(18); return FALSE; }
	for(i = 0x0; i < AdcPars->ChannelsQuantity; i++) if(!PUT_LBIOS_WORD((WORD)(L_CONTROL_TABLE_E440 + i), (SHORT)AdcPars->ControlTable[i])) { LAST_ERROR_NUMBER(18); return FALSE; }

	// ����� ��������� �������� ��������� ������� ��� �������� ������ � �������
	// ������� 'F' � ����
	if((REVISIONS_E440[REVISION_A_E440] < ModuleDescriptor.Revision) &&
		(ModuleDescriptor.Revision < REVISIONS_E440[REVISION_F_E440])) AdcPars->AdcClockSource = INT_ADC_CLOCK_E440;
	// �������� �������� �������� ��������� ���
	else if(AdcPars->AdcClockSource > EXT_ADC_CLOCK_E440) { LAST_ERROR_NUMBER(64); return FALSE; }

	// ������� � LBIOS �������� �������� ��������� ������� ���
	if(!PUT_LBIOS_WORD(L_ADC_CLOCK_SOURCE_E440, AdcPars->AdcClockSource)) { LAST_ERROR_NUMBER(18); return FALSE; }

	// ��������� ��������� ������ ���
	DspClockOut = 2.0*ModuleDescriptor.QuartzFrequency/1000.0; // �������� ������� DSP � ���
	if(fabs(DspClockOut - 48000.0) > 0.00001) { LAST_ERROR_NUMBER(26); return FALSE; }
	AdcPars->AdcRate = fabs(AdcPars->AdcRate);
	if(AdcPars->AdcRate > 400.0) AdcPars->AdcRate = 400.0;
	else if(AdcPars->AdcRate < 0.1) AdcPars->AdcRate = 0.1;
	SclockDiv = DspClockOut/(2.0*AdcPars->AdcRate) - 0.5;
	if(SclockDiv > 65534.0) SclockDiv = 65534.0;
	AdcPars->AdcRate = DspClockOut/(2.0*(SclockDiv + 1.0));
	if(!PUT_LBIOS_WORD(L_FIRST_SAMPLE_DELAY_E440, (SHORT)(DspClockOut/AdcPars->AdcRate + 5.5))) { LAST_ERROR_NUMBER(18); return FALSE; }
	if(!PUT_LBIOS_WORD(L_ADC_RATE_E440, (SHORT)SclockDiv)) { LAST_ERROR_NUMBER(18); return FALSE; }
	AdcPars->InterKadrDelay = fabs(AdcPars->InterKadrDelay);
	if((1.0/(AdcPars->AdcRate)) > (AdcPars->InterKadrDelay)) AdcPars->InterKadrDelay = 1.0/AdcPars->AdcRate;
	KadrDelay = AdcPars->InterKadrDelay*AdcPars->AdcRate - 0.5;
	if(KadrDelay > 65534.0) KadrDelay = 65534.0;
	AdcPars->InterKadrDelay = (KadrDelay + 1.0)/AdcPars->AdcRate;
	if(!PUT_LBIOS_WORD(L_INTER_KADR_DELAY_E440, (SHORT)KadrDelay)) { LAST_ERROR_NUMBER(18); return FALSE; }
	AdcPars->KadrRate = 1.0/((AdcPars->ChannelsQuantity - 1.0)/AdcPars->AdcRate + AdcPars->InterKadrDelay);

	//===== ��������� FIFO ������ ��� =====
	// ������� ����� FIFO ������ ��� ������ ����� 0x0
	AdcPars->AdcFifoBaseAddress = 0x0;
	//
	AdcPars->AdcFifoLength = (WORD)abs(AdcPars->AdcFifoLength);
	// ����� FIFO ������ ��� ������ ���������� � ��������� �� 0x40(64)
	if(AdcPars->AdcFifoLength < 0x40) AdcPars->AdcFifoLength = 0x40;
	// � ������ �� 0x3000 (12288)
	else if(AdcPars->AdcFifoLength > 0x3000) AdcPars->AdcFifoLength = 0x3000;
	// � ����� ����� FIFO ������ ��� ������ ���� ������ 0x40(64)
	AdcPars->AdcFifoLength -= (WORD)(AdcPars->AdcFifoLength%64);
	if(!PUT_LBIOS_WORD(L_ADC_FIFO_LENGTH_E440, AdcPars->AdcFifoLength)) { LAST_ERROR_NUMBER(18); return FALSE; }

	// �������� ������������� ������������ ��� ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E440; i++)
	{
		// �������� ��� ��� �������� i
		if(AdcPars->AdcOffsetCoefs[i] >= 0.0) AdcOffsetCoef = (WORD)(AdcPars->AdcOffsetCoefs[i] + 0.5);
		else AdcOffsetCoef = (WORD)(AdcPars->AdcOffsetCoefs[i] - 0.5);
		if(!PUT_LBIOS_WORD((WORD)(L_ZERO_E440 + i), AdcOffsetCoef)) { LAST_ERROR_NUMBER(18); return FALSE; }

		// ������� ��� ��� �������� i
		AdcScaleCoef = (WORD)(AdcPars->AdcScaleCoefs[i]*(double)0x8000 + 0.5);
		if(!PUT_LBIOS_WORD((WORD)(L_SCALE_E440 + i), AdcScaleCoef)) { LAST_ERROR_NUMBER(18); return FALSE; }
	}
	// �������� ��������� ������ ���
	this->AdcPars = *AdcPars;
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::START_ADC(void)
{
	WORD InBuf[4] = { 0x0, V_START_ADC, 0x0 | DSP_DM, (WORD)(AdcPars.AdcFifoLength/2.0 + 0.5)};

	// ��� USB-������ � MCU �� ������ ���
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ������� � DSP ������ �� ������ ���
	else if(!SEND_COMMAND(C_START_ADC)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::STOP_ADC(void)
{
	// �������� ������� � DSP ������ �� ������� ���
	if(!SEND_COMMAND(C_STOP_ADC)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// ��� ������ ������� ������ ������ ����.��������� �������� ��������� USB-�������� �� ������
	if(('A' <= ModuleDescriptor.Revision) && (ModuleDescriptor.Revision <= 'C'))
		{ if(!StopAdcForOldRevision()) { LAST_ERROR_NUMBER(27); return FALSE; } }

	// ������ ������� �������� Bulk-������ ����� ������ �� USB (�� ������ ������)
	if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3/*reset Read Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3/*reset Read Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ���� ����� �������� � ��� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::ADC_KADR(SHORT * const Data)
{
	WORD ChannelsQuantity;

	// �������� ����� ������
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��������� �������� ������
	else if(strcmp((char *)ModuleDescriptor.Name, "E440")) { LAST_ERROR_NUMBER(24); return FALSE; }
	// ��������� ���
	else if(!STOP_ADC()) { LAST_ERROR_NUMBER(28); return FALSE; }
	// �������� ��������� ������ ���
	else if(!GET_LBIOS_WORD(L_CONTROL_TABLE_LENGHT_E440, (SHORT *)&ChannelsQuantity)) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(ChannelsQuantity != AdcPars.ChannelsQuantity)  { LAST_ERROR_NUMBER(29); return FALSE; }
	else if(ChannelsQuantity > AdcPars.AdcFifoLength)  { LAST_ERROR_NUMBER(30); return FALSE; }
	// ������� ����� ����� �������� � ���
	else if(!SEND_COMMAND(C_ADC_KADR)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// ������ ����� ��������� ���������� ������
	else if(!GET_DM_ARRAY(0x0, AdcPars.ChannelsQuantity, Data)) { LAST_ERROR_NUMBER(31); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ����������� ���� � ��������� ����������� ������ ��� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::ADC_SAMPLE(SHORT * const AdcData, WORD AdcChannel)
{
	// �������� ���������
	if(!AdcData) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������ ����� ���������� ���
	else if(!STOP_ADC()) { LAST_ERROR_NUMBER(28); return FALSE; }
	// ���������� ����� ������ ��� ������������ ����� � ���
	else if(!PUT_LBIOS_WORD(L_ADC_CHANNEL_E440, AdcChannel)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ������� ������������ ����� � ���
	else if(!SEND_COMMAND(C_ADC_SAMPLE)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// ������ ��������� ���������� �������� � ���
	else if(!GET_LBIOS_WORD(L_ADC_SAMPLE_E440, AdcData)) { LAST_ERROR_NUMBER(16); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ��������� ������ ������ � ��� ������:
//   ���� ReadRequest->Overlapped != NULL - ����������� ������ �� ���� ������������ ���-�� ������
//   ���� ReadRequest->Overlapped == NULL - ���������� ���� ������������ ���-�� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::ReadData(IO_REQUEST_LUSBAPI * const ReadRequest)
{
	DWORD NumberOfBytesRead;

	// �������� ����� ������
	if(!ReadRequest->Buffer) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ����� �������� ������ ���������� � ��������� �� 0x20(32)
	else if(ReadRequest->NumberOfWordsToPass < 0x20) ReadRequest->NumberOfWordsToPass = 0x20;
	// � ������ �� 0x100000(1024*1024)
	else if(ReadRequest->NumberOfWordsToPass > (1024*1024)) ReadRequest->NumberOfWordsToPass = 1024*1024;
	// ����� ����� �������� ������ ���� ������ 0x20(32)
	ReadRequest->NumberOfWordsToPass -= ReadRequest->NumberOfWordsToPass%32;
	//
	NumberOfBytesRead = 0x0;

	if(ReadRequest->Overlapped)
	{
		// �������� ����������� ������ �� ���� ������������ ���-�� ������
		if(!ReadFile(hDevice, ReadRequest->Buffer, 2*ReadRequest->NumberOfWordsToPass, &NumberOfBytesRead, ReadRequest->Overlapped))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(59); CancelIo(hDevice); return FALSE; } }
		// ���-�� ���������� �������� (� ���� ������������� ������� ��� �������� ������ ����� ���� ����� 0)
		ReadRequest->NumberOfWordsPassed = NumberOfBytesRead/2;
	}
	else
	{
		OVERLAPPED Ov;

		// �������������� OVERLAPPED ���������
		ZeroMemory(&Ov, sizeof(OVERLAPPED));
		// ������ �������
		Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
		if(!Ov.hEvent) { LAST_ERROR_NUMBER(33); return FALSE; }

		// �������� ����������� ������ �� ���� ������������ ���-�� ������
		NumberOfBytesRead = 0x0;
		if(!ReadFile(hDevice, ReadRequest->Buffer, 2*ReadRequest->NumberOfWordsToPass, &NumberOfBytesRead, &Ov))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(32); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; } }
		// ��� ��������� ���������� �������
		if(WaitForSingleObject(Ov.hEvent, ReadRequest->TimeOut) == WAIT_TIMEOUT) { LAST_ERROR_NUMBER(34); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ��������� �������� ���-�� ������� ���������� ���� ������
		else if(!GetOverlappedResult(hDevice, &Ov, &NumberOfBytesRead, TRUE)) { LAST_ERROR_NUMBER(35); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ������� �������
		else if(!CloseHandle(Ov.hEvent)) { LAST_ERROR_NUMBER(36); return FALSE; }
		// ���-�� ���������� ��������
		ReadRequest->NumberOfWordsPassed = NumberOfBytesRead/2;
		// ������� ������� ���� ������� ��������� ������ (� ������) � ������������� ���-��� ��������
		if(ReadRequest->NumberOfWordsToPass != ReadRequest->NumberOfWordsPassed) { LAST_ERROR_NUMBER(37); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}




//==============================================================================
// ������� ��� ������ � ��� ������
//==============================================================================
//------------------------------------------------------------------------------
// ��������� ������� ���������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_DAC_PARS(DAC_PARS_E440 * const DacPars)
{
	WORD DacEnabled;
	WORD SclkDiv, RfsDiv;
	double DspClockout, SCLK;

	// �������� ��������� ���������� ������ ���
	if(!DacPars) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E440")) { LAST_ERROR_NUMBER(24); return FALSE; }
	else if(!GET_LBIOS_WORD(L_DAC_ENABLED_E440, (SHORT *)&DacEnabled)) { LAST_ERROR_NUMBER(16); return FALSE; }
	DacPars->DacEnabled = DacEnabled ? TRUE : FALSE;

	// ��������� ��������� ������ ���
	DspClockout = 2.0*ModuleDescriptor.QuartzFrequency/1000.0; // �������� ������� DSP � ���
	if(fabs(DspClockout - 48000.0) > 0.00001) { LAST_ERROR_NUMBER(26); return FALSE; }

	if(!GET_LBIOS_WORD(L_DAC_SCLK_DIV_E440, (SHORT *)&SclkDiv)) { LAST_ERROR_NUMBER(16); return FALSE; }
	SCLK = DspClockout/(2.0*(SclkDiv + 1.0));

	if(!GET_LBIOS_WORD(L_DAC_RATE_E440, (SHORT *)&RfsDiv)) { LAST_ERROR_NUMBER(16); return FALSE; }
	DacPars->DacRate = SCLK/(RfsDiv + 1.0);

	// ��������� FIFO ������ ���
	if(!GET_LBIOS_WORD(L_DAC_FIFO_BASE_ADDRESS_E440, (SHORT *)&DacPars->DacFifoBaseAddress)) { LAST_ERROR_NUMBER(16); return FALSE; }
	if(!GET_LBIOS_WORD(L_CUR_DAC_FIFO_LENGTH_E440, (SHORT *)&DacPars->DacFifoLength)) { LAST_ERROR_NUMBER(16); return FALSE; }

	// �������� ��������� ������ ���
	this->DacPars = *DacPars;
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ��������� ���������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::SET_DAC_PARS(DAC_PARS_E440 * const DacPars)
{
	WORD SclkDiv, RfsDiv;
	double DspClockout, SCLK;

	// �������� ��������� ���������� ������ ���
	if(!DacPars) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E440")) { LAST_ERROR_NUMBER(24); return FALSE; }

	// ��������� ��������� ������ ���
	DspClockout = 2.0*ModuleDescriptor.QuartzFrequency/1000.0; // �������� ������� DSP � ���
	if(fabs(DspClockout - 48000.0) > 0.00001) { LAST_ERROR_NUMBER(26); return FALSE; }

	if(!GET_LBIOS_WORD(L_DAC_SCLK_DIV_E440, (SHORT *)&SclkDiv)) { LAST_ERROR_NUMBER(16); return FALSE; }
	SCLK = DspClockout/(2.0*(SclkDiv + 1.0));

	DacPars->DacRate = fabs(DacPars->DacRate);
	if(DacPars->DacRate > 125.0)	DacPars->DacRate = 125.0;
	if(DacPars->DacRate < SCLK/65500.0) DacPars->DacRate = SCLK/65500.0;

	RfsDiv = (SHORT)(SCLK/(DacPars->DacRate) - 0.5);
	DacPars->DacRate = SCLK/(RfsDiv + 1.0);
	if(!PUT_LBIOS_WORD(L_DAC_RATE_E440, RfsDiv)) { LAST_ERROR_NUMBER(18); return FALSE; }

	//===== ��������� FIFO ������ ��� =====
	// ������� ����� FIFO ������ ��� ������ ����� 0x3000
	DacPars->DacFifoBaseAddress = 0x3000;
	//
	DacPars->DacFifoLength = (WORD)abs(DacPars->DacFifoLength);
	// ����� FIFO ������ ��� ������ ���������� � ��������� �� 0x40(64)
	if(DacPars->DacFifoLength < 0x40) DacPars->DacFifoLength = 0x40;
	// � ������ �� 0xFC0(4032)
	else if(DacPars->DacFifoLength > 0xFC0) DacPars->DacFifoLength = 0xFC0;
	// � ����� ����� FIFO ������ ��� ������ ���� ������ 0x40(64)
	DacPars->DacFifoLength -= (WORD)(DacPars->DacFifoLength%64);
	if(!PUT_LBIOS_WORD(L_DAC_FIFO_LENGTH_E440, DacPars->DacFifoLength)) { LAST_ERROR_NUMBER(18); return FALSE; }

	// �������� ��������� ������ ���
	this->DacPars = *DacPars;
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::START_DAC(void)
{
	// � ���� �� ��� �� ������ ���������� ������
	if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E440)  { LAST_ERROR_NUMBER(38); return FALSE; }
	// ��� ������ ������� ������ �������������� ����������
	else if(('A' <= ModuleDescriptor.Revision) && (ModuleDescriptor.Revision <= 'C'))
	{
		WORD InBuf[4] = { 0x0, V_START_DAC, 0x3000 | DSP_DM, (WORD)(DacPars.DacFifoLength/2.0 + 0.5) };

		// ������ ������� �������� Bulk-������ ����� ������ �� USB (�� ������ ������)
		if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		// ��� USB-������ � MCU �� ������ � ���
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	}
	// �������� ������� � DSP ������ �� ������ ���
	if(!SEND_COMMAND(C_START_DAC)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::STOP_DAC(void)
{
	WORD InBuf[4] = { 0x0, V_START_DAC, 0x3000 | DSP_DM, (WORD)(DacPars.DacFifoLength/2.0 + 0.5) };

	// � ���� �� ��� �� ������ ���������� ������
	if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E440)  { LAST_ERROR_NUMBER(38); return FALSE; }
	// �������� ������� � DSP ������ �� ������� ���
	else if(!SEND_COMMAND(C_STOP_DAC)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// ��� ������ ������� ������ ������ ����.��������� �������� ��������
	else if(('A' <= ModuleDescriptor.Revision) && (ModuleDescriptor.Revision <= 'C'))
		{ if(!StopDacForOldRevision()) { LAST_ERROR_NUMBER(39); return FALSE; } }
	else
	{
		// ������ ������� �������� Bulk-������ ����� ������ �� USB (�� ������ ������)
		if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		// ��� USB-������ � MCU �� ������ � ���
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	}
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ��������� �������� ������ ��� � ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::WriteData(IO_REQUEST_LUSBAPI * const WriteRequest)
{
	DWORD NumberOfBytesWritten;

	// �������� ����� ������
	if(!WriteRequest->Buffer) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ����� �������� ������ ���������� � ��������� �� 0x20(32)
	else if(WriteRequest->NumberOfWordsToPass < 0x20) WriteRequest->NumberOfWordsToPass = 0x20;
	// � ������ �� 0x100000(1024*1024)
	else if(WriteRequest->NumberOfWordsToPass > (1024*1024)) WriteRequest->NumberOfWordsToPass = 1024*1024;
	// ����� ����� �������� ������ ���� ������ 0x20(32)
	WriteRequest->NumberOfWordsToPass -= (WriteRequest->NumberOfWordsToPass)%32;

	if(WriteRequest->Overlapped)
	{
		// �������� ����������� ������ �� ����� ������������ ���-�� ������
		NumberOfBytesWritten = 0x0;
		if(!WriteFile(hDevice, WriteRequest->Buffer, 2*WriteRequest->NumberOfWordsToPass, &NumberOfBytesWritten, WriteRequest->Overlapped))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(60); CancelIo(hDevice); return FALSE; } }
		// ���-�� ���������� �������� (� ���� ������������� ������� ��� �������� ������ ����� ���� ����� 0)
		WriteRequest->NumberOfWordsPassed = NumberOfBytesWritten/2;
	}
	else
	{
		OVERLAPPED Ov;

		// �������������� OVERLAPPED ���������
		ZeroMemory(&Ov, sizeof(OVERLAPPED));
		// ������ �������
		Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
		if(!Ov.hEvent) { LAST_ERROR_NUMBER(33); return FALSE; }

		// �������� ����������� ������ �� ���� ������������ ���-�� ������
		NumberOfBytesWritten = 0x0;
		if(!WriteFile(hDevice, WriteRequest->Buffer, 2*WriteRequest->NumberOfWordsToPass, &NumberOfBytesWritten, &Ov))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(40); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; } }
		// ��� ��������� ���������� �������
		if(WaitForSingleObject(Ov.hEvent, WriteRequest->TimeOut) == WAIT_TIMEOUT) { LAST_ERROR_NUMBER(34); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ��������� �������� ���-�� ������� ���������� ���� ������
		else if(!GetOverlappedResult(hDevice, &Ov, &NumberOfBytesWritten, TRUE)) { LAST_ERROR_NUMBER(35); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ������� �������
		else if(!CloseHandle(Ov.hEvent)) { LAST_ERROR_NUMBER(36); return FALSE; }
		// ���-�� ���������� ��������
		WriteRequest->NumberOfWordsPassed = NumberOfBytesWritten/2;
		// ������� ������� ���� ������� �������� ������ (� ������) � ��������� ���-���
		if(WriteRequest->NumberOfWordsToPass != WriteRequest->NumberOfWordsPassed) { LAST_ERROR_NUMBER(37); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ����������� ����� �� �������� ����� ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::DAC_SAMPLE(SHORT * const DacData, WORD DacChannel)
{
	SHORT DacParam;

	// � ���� �� ��� �� ������ ���������� ������
	if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E440)  { LAST_ERROR_NUMBER(38); return FALSE; }
	// �������� ���������
	else if(!DacData) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �������� ����� ������ ���
	else if(DacChannel >= DAC_CHANNELS_QUANTITY_E440) { LAST_ERROR_NUMBER(41); return FALSE; }

	// ���������� �������� ��� ������ �� ���
	if((*DacData) < -2048) *DacData = -2048;
	else if((*DacData) > 2047) *DacData = 2047;
	// ��������� ��������������� ����� ��� �������� � DSP
	DacParam = (SHORT)((*DacData) & 0xFFF);
	DacParam |= (SHORT)(DacChannel << 12);
	if(!PUT_LBIOS_WORD(L_DAC_SAMPLE_E440, DacParam)) { LAST_ERROR_NUMBER(18); return FALSE; }

	// ������� ������������ ������ �� ���
	if(!SEND_COMMAND(C_DAC_SAMPLE)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �� ������
	else return TRUE;
}



//==============================================================================
// ������� ��� ������ � ��������� ������� �� ������� �������� �������
//==============================================================================
//------------------------------------------------------------------------------
// ������� ���������� �������� ����� �������� ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::ENABLE_TTL_OUT(BOOL EnableTtlOut)
{
	// ��������� ������� ����������/���������� �������� �����
	if(!PUT_LBIOS_WORD(L_ENABLE_TTL_OUT_E440, (SHORT)(EnableTtlOut ? 0x1 : 0x0))) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ������ ������� � DSP
	else if(!SEND_COMMAND(C_ENABLE_TTL_OUT)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// ����������� ������ ������� �������� �������� ����� ������
	this->EnableTtlOut = EnableTtlOut;
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ������� ����� �������� ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::TTL_IN(WORD * const TtlIn)
{
	// �������� ���������
	if(!TtlIn) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������ ������� � DSP
	else if(!SEND_COMMAND(C_TTL_IN)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// ������� �������� � �������� ������� ������
	else if(!GET_LBIOS_WORD(L_TTL_IN_E440, (SHORT *)TtlIn)) { LAST_ERROR_NUMBER(16); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ �� �������� ����� �������� ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::TTL_OUT(WORD TtlOut)
{
	// �������� ������ ������� �������� �������� ����� ������
	if(!EnableTtlOut) { LAST_ERROR_NUMBER(42); return FALSE; }
	// ��������� ��������, ������� ������ ���� ���������� �� �������� ������
	else if(!PUT_LBIOS_WORD(L_TTL_OUT_E440, (SHORT)TtlOut)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ������ ������� � DSP
	else if(!SEND_COMMAND(C_TTL_OUT)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �� ������
	else return TRUE;
}




//==============================================================================
// ������� ��� ������ � ���������������� ���� ������
//==============================================================================

//------------------------------------------------------------------------------
// ����������/���������� ������ ������ � ���� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::ENABLE_FLASH_WRITE(BOOL EnableFlashWrite)
{
	// �������� ���� ������
	this->EnableFlashWrite = EnableFlashWrite;
	// ��������� ������� ����������/���������� ������ � ����
	if(!PUT_LBIOS_WORD(L_FLASH_ENABLED_E440, (SHORT)((EnableFlashWrite) ? 0x9800 : 0x8000))) { LAST_ERROR_NUMBER(18); return FALSE; }
	// �������� �������
	else if(!SEND_COMMAND(C_ENABLE_FLASH_WRITE)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ����� �� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::READ_FLASH_WORD(WORD FlashAddress, SHORT * const FlashWord)
{
	// �������� ���������
	if(!FlashWord) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �������� ����� ������ ���� (������ ���� �� ������ 63)
	else if(FlashAddress > 63) { LAST_ERROR_NUMBER(43); return FALSE; }
	// ��������� ����� ������ ����
	else if(!PUT_LBIOS_WORD(L_FLASH_ADDRESS_E440, (SHORT)((FlashAddress << 7) | 0xC000))) { LAST_ERROR_NUMBER(18); return FALSE; }
	// �������� �������
	else if(!SEND_COMMAND(C_READ_FLASH_WORD)) { LAST_ERROR_NUMBER(20);LAST_ERROR_NUMBER(20); return FALSE; }	// ������� ���������� �� ���� ��������
	else if(!GET_LBIOS_WORD(L_FLASH_DATA_E440, FlashWord)) { LAST_ERROR_NUMBER(16); return FALSE; }

	Sleep(1);

	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� � ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::WRITE_FLASH_WORD(WORD FlashAddress, SHORT FlashWord)
{
	// �������� ����� �� ������ � ����
	if(!EnableFlashWrite) { LAST_ERROR_NUMBER(44); return FALSE; }
	else
	{
		if(!IsServiceFlashWriting)
		{
			// �������� ����� ������ ���� (������ ���� �� ����� 32)
			if(FlashAddress < 32) { LAST_ERROR_NUMBER(45); return FALSE; }
			// �������� ����� ������ ���� (������ ���� �� ������ 63)
			else if(FlashAddress > 63) { LAST_ERROR_NUMBER(43); return FALSE; }
		}
	}

	// ��������� ����� ������ ����
	if(!PUT_LBIOS_WORD(L_FLASH_ADDRESS_E440, (SHORT)((FlashAddress << 7) | 0xA000))) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��������� ������
	else if(!PUT_LBIOS_WORD(L_FLASH_DATA_E440, FlashWord)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// �������� �������
	else if(!SEND_COMMAND(C_WRITE_FLASH_WORD)) { LAST_ERROR_NUMBER(20); return FALSE; }

	Sleep(10);

	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ��������� ���������� � ������ �� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E440 * const md)
{
	WORD i;

	// �������� ���������
	if(!md) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������� ����� ��������� ��������� ���� ���� ������
	else if(!GetModuleDescriptor()) { LAST_ERROR_NUMBER(46); return FALSE; }
	// ��������� ���������� � LBIOS
	else if(!GetDspDescriptor()) { LAST_ERROR_NUMBER(47); return FALSE; }
	// ��������� ���������� � �������� ����������������
	else if(!GetMcuDescriptor()) { LAST_ERROR_NUMBER(48); return FALSE; }

	// Module Info
	strncpy((char *)md->Module.CompanyName,  "L-Card Ltd.",    								sizeof(md->Module.CompanyName));
	if(!strcmp((char *)ModuleDescriptor.Name, "E440")) strcpy((char *)md->Module.DeviceName, "E14-440");
	else strncpy((char *)md->Module.DeviceName,		(char *)ModuleDescriptor.Name,	 		std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.Name)));
	strncpy((char *)md->Module.SerialNumber, 			(char *)ModuleDescriptor.SerialNumber,	std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
	strcpy((char *)md->Module.Comment,      			"������� ������ ���/���/��� ������ ���������� �� ���� USB 1.1");
	// ������� ������� ������
	md->Module.Revision = ModuleDescriptor.Revision;
	// ������ E14-440 ���������� � ������������ ���������� (�� ������ ���� ����)
	md->Module.Modification = NO_MODULE_MODIFICATION_LUSBAPI;

	// Interface Info
	md->Interface.Active = TRUE;
	strcpy((char *)md->Interface.Name, "USB 1.1");
	strcpy((char *)md->Interface.Comment, "Full-Speed Mode (12 Mbit/s)");

	// MCU Info
	md->Mcu.Active = TRUE;
	strncpy((char *)md->Mcu.Name, 						(char *)FirmwareDescriptor.McuName, 		std::min(sizeof(md->Mcu.Name), sizeof(FirmwareDescriptor.McuName)));
	strncpy((char *)md->Mcu.Version.Version,  		(char *)FirmwareDescriptor.Version, 		std::min(sizeof(md->Mcu.Version.Version), sizeof(FirmwareDescriptor.Version)));
	strncpy((char *)md->Mcu.Version.Date,  			(char *)FirmwareDescriptor.Created, 		std::min(sizeof(md->Mcu.Version.Date), sizeof(FirmwareDescriptor.Created)));
	strncpy((char *)md->Mcu.Version.Manufacturer,  	(char *)FirmwareDescriptor.Manufacturer, std::min(sizeof(md->Mcu.Version.Manufacturer), sizeof(FirmwareDescriptor.Manufacturer)));
	strncpy((char *)md->Mcu.Version.Author,  			(char *)FirmwareDescriptor.Author, 		std::min(sizeof(md->Mcu.Version.Author), sizeof(FirmwareDescriptor.Author)));
	strncpy((char *)md->Mcu.Version.Comment,			(char *)FirmwareDescriptor.Comment, 		std::min(sizeof(md->Mcu.Version.Comment), sizeof(FirmwareDescriptor.Comment)));
	if(!strcmp((char *)DspDescriptor.Manufacturer, "Unknown"))
		strncpy((char *)md->Mcu.Comment, 					"8-bit Microcontroller with 8K Bytes In-System Programmable Flash", sizeof(md->Mcu.Comment));
	else
		strcpy((char *)md->Mcu.Comment, 					"??????");
	md->Mcu.ClockRate = ModuleDescriptor.QuartzFrequency/3000.0;		// � ���

	// DSP Info
	md->Dsp.Active = TRUE;
	strncpy((char *)md->Dsp.Name, 						(char *)DspDescriptor.DspName, 		std::min(sizeof(md->Dsp.Name), sizeof(DspDescriptor.DspName)));
	strncpy((char *)md->Dsp.Version.Version,  		(char *)DspDescriptor.Version, 		std::min(sizeof(md->Dsp.Version.Version), sizeof(DspDescriptor.Version)));
	strncpy((char *)md->Dsp.Version.Date,  			(char *)DspDescriptor.Created, 		std::min(sizeof(md->Dsp.Version.Date), sizeof(DspDescriptor.Created)));
	strncpy((char *)md->Dsp.Version.Manufacturer,  	(char *)DspDescriptor.Manufacturer, std::min(sizeof(md->Dsp.Version.Manufacturer), sizeof(DspDescriptor.Manufacturer)));
	strncpy((char *)md->Dsp.Version.Author,  			(char *)DspDescriptor.Author, 		std::min(sizeof(md->Dsp.Version.Author), sizeof(DspDescriptor.Author)));
	strncpy((char *)md->Dsp.Version.Comment, 			(char *)DspDescriptor.Comment, 		std::min(sizeof(md->Dsp.Version.Comment), sizeof(DspDescriptor.Comment)));
	strncpy((char *)md->Dsp.Comment,"", sizeof(md->Dsp.Comment));
	md->Dsp.ClockRate = 2.0*ModuleDescriptor.QuartzFrequency/1000.0;		// � ���

	// ADC Info
	md->Adc.Active = TRUE;
	strcpy((char *)md->Adc.Name, "LTC1416");
	// ���������������� ������������ ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E440; i++)
	{
		// �������� ��� �������� i
		md->Adc.OffsetCalibration[i] = (double)ModuleDescriptor.AdcOffsetCoefs[i];
		// ������� ��� �������� i
		md->Adc.ScaleCalibration[i] = (double)ModuleDescriptor.AdcScaleCoefs[i]/(double)0x8000;
	}
	strcpy((char *)md->Adc.Comment, "14-Bit 400Ksps ADC converter");

	// DAC Info
	if(ModuleDescriptor.IsDacPresented)
	{
		md->Dac.Active = TRUE;
		strcpy((char *)md->Dac.Name, "AD7249");
		// ���������������� ������������ ���
		for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E440; i++)
		{
			md->Dac.OffsetCalibration[i] = ModuleDescriptor.DacOffsetCoefs[i]/10000.0;
			md->Dac.ScaleCalibration[i] = ModuleDescriptor.DacScaleCoefs[i]/10000.0;
		}
		strcpy((char *)md->Dac.Comment, "12-Bit 125Ksps DAC converter");
	}
	else md->Dac.Active = FALSE;

	// IO Info
	md->DigitalIo.Active = TRUE;
	if((ModuleDescriptor.Revision >= 'A') && (ModuleDescriptor.Revision < 'E'))  strcpy((char *)md->DigitalIo.Name, "74LVX573/74HTC574");
	else strcpy((char *)md->DigitalIo.Name, "74LVX574MTC/74LCX574MTC");
	md->DigitalIo.InLinesQuantity = 16;
	md->DigitalIo.OutLinesQuantity = 16;
	strcpy((char *)md->DigitalIo.Comment, "TTL/CMOS compatible");

	// ��� ������� ������� 'D' � ���� ������ ��������� � ������� ������ MCU,
	// ��������� �������� ����������� ������������ �� ����� Bulk ������� USB
	McuAttributes |= ENABLE_RESET_BULK_PIPES;
	if(!SetMcuAttributes(McuAttributes)) { LAST_ERROR_NUMBER(49); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ��������� ���������� � ������ � ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E440 * const md)
{
	WORD i;

	// �������� ���������
	if(!md) { LAST_ERROR_NUMBER(2); return FALSE; }

	// ������ ����� ��������� ���������� ������
	strcpy((char *)ModuleDescriptor.Name, 				"E440");
//	strncpy((char *)ModuleDescriptor.Name, 			(char *)md->Module.DeviceName,	std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.Name)));
	strncpy((char *)ModuleDescriptor.SerialNumber,	(char *)md->Module.SerialNumber, std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
	// �������� ��������� ������� ������
	if((	md->Module.Revision < REVISIONS_E440[REVISION_A_E440] ||
			md->Module.Revision > REVISIONS_E440[REVISIONS_QUANTITY_E440 - 0x1])) { LAST_ERROR_NUMBER(63); return FALSE; }
	ModuleDescriptor.Revision = md->Module.Revision;

	if(!strcmp((char *)md->Dsp.Name, "ADSP-2185M")) strcpy((char *)ModuleDescriptor.DspType, "2185");
	else { LAST_ERROR_NUMBER(50); return FALSE; }
	ModuleDescriptor.IsDacPresented = (BYTE)(md->Dac.Active ? 0x1 : 0x0);
	ModuleDescriptor.QuartzFrequency = 1000.0*md->Dsp.ClockRate/2.0 + 0.5;
	// ���������������� ������������ ���
	for(i = 0x0; i < 0x4; i++)
	{
		// �������� ��� ��� �������� i
		if(md->Adc.OffsetCalibration[i + 0x0] >= 0.0) ModuleDescriptor.AdcOffsetCoefs[i] = (SHORT)(md->Adc.OffsetCalibration[i] + 0.5);
		else ModuleDescriptor.AdcOffsetCoefs[i] = (SHORT)(md->Adc.OffsetCalibration[i] - 0.5);
		// ������� ��� ��� �������� i
		ModuleDescriptor.AdcScaleCoefs[i] = (WORD)(md->Adc.ScaleCalibration[i]*(double)0x8000 + 0.5);
	}
	// ���������������� ������������ ���
	for(i = 0x0; i < 0x2; i++)
	{
		// �������� - ���� �� ��� �� �����
		if(md->Dac.Active)
		{
			// �������� ��� ������ i
			if(md->Dac.OffsetCalibration[i] >= 0.0) ModuleDescriptor.DacOffsetCoefs[i] = (SHORT)(10000.0*md->Dac.OffsetCalibration[i] + 0.5);
			else ModuleDescriptor.DacOffsetCoefs[i] = (SHORT)(10000.0*md->Dac.OffsetCalibration[i] - 0.5);
			// ������� ��� ������ i
			ModuleDescriptor.DacScaleCoefs[i] = 10000.0*md->Dac.ScaleCalibration[i] + 0.5;
		}
		else
		{
			// �������� ��� ������ i
			ModuleDescriptor.DacOffsetCoefs[i] = 0.0;
			// ������� ��� ������ i
			ModuleDescriptor.DacScaleCoefs[i] = 1.0;
		}
	}
	for(i = 0x0; i < sizeof(ModuleDescriptor.ReservedWord); i++) ModuleDescriptor.ReservedWord[i] = 0x0;

	// ��������� ��������� ���������� ������
	if(!PutModuleDescriptor()) { LAST_ERROR_NUMBER(51); return FALSE; }
	// �� ������
	else return TRUE;
}




//==============================================================================
// ������� ��� ������ ������� DSP ������
//==============================================================================
//------------------------------------------------------------------------------
// ������ ����� � ������� ������ �������� DSP � ����������� �������� LBIOS
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::PUT_LBIOS_WORD(WORD Address, SHORT Data)
{
	// �������� ��������� ������
	if((Address < DataBaseAddress) || (Address > ProgramBaseAddress)) { LAST_ERROR_NUMBER(52); return FALSE; }
	// ����� ���������� � DSP
	else if(!PUT_PM_WORD(Address, (long)Data << 0x8)) { LAST_ERROR_NUMBER(53); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� �� ������� ������ �������� DSP � ����������� �������� LBIOS
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_LBIOS_WORD(WORD Address, SHORT * const Data)
{
	long PmData;

	// ��� ������ �������� ���������
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������ �������� ��������� ������ � ����
	else if((Address < DataBaseAddress) || (Address > ProgramBaseAddress)) { LAST_ERROR_NUMBER(52); return FALSE; }
	// ������ ���������� �� DSP
	else if(!GET_PM_WORD(Address, &PmData)) { LAST_ERROR_NUMBER(15); return FALSE; }
	// ��������� ��������
	*Data = (SHORT)(PmData >> 0x8);
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� � ������ ������ ������ DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::PUT_DM_WORD(WORD Address, SHORT Data)
{
	WORD InBuf[4] = { 0x0, V_PUT_ARRAY, (WORD)(Address | DSP_DM), 0x0 };

	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
													&InBuf, sizeof(InBuf),
													&Data, 2/*bytes*/,
													TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� �� ������ ������ ������ DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_DM_WORD(WORD Address, SHORT * const Data)
{
	WORD InBuf[4] = { 0x1, V_GET_ARRAY, (WORD)(Address | DSP_DM), 0x0 };

	// ��� ������ �������� ���������
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �������� ��������������� USB-������ � MCU
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
													&InBuf,sizeof(InBuf),
													Data, 2/*bytes*/,
													TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� � ������ ������ �������� DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::PUT_PM_WORD(WORD Address, long Data)
{
	WORD InBuf[4] = { 0x0, V_PUT_ARRAY, (WORD)(Address | DSP_PM), 0x0 };

	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
													&InBuf, sizeof(InBuf),
													&Data, 4/*bytes*/,
													TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� �� ������ ������ �������� DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_PM_WORD(WORD Address, long * const Data)
{
	WORD InBuf[4] = { 0x1, V_GET_ARRAY, (WORD)(Address | DSP_PM), 0x0 };

	// ��� ������ �������� ���������
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �������� ��������������� USB-������ � MCU
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
													&InBuf,sizeof(InBuf),
													Data, 4/*bytes*/,
													TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ������� ���� � ������� ������ ������ DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::PUT_DM_ARRAY(WORD BaseAddress, WORD NPoints, SHORT * const Data)
{
	BOOL Error = FALSE;
	WORD i;
	WORD InBuf[4];
	WORD MaxByteBlock = MAX_USB_CONTROL_PIPE_BLOCK, BlockQuantity, RestDataPoints;

	// ��� ������ �������� �����
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }

	BlockQuantity = (WORD)((sizeof(SHORT)*NPoints)/MaxByteBlock);
	RestDataPoints = (WORD)((sizeof(SHORT)*NPoints)%MaxByteBlock); // � ������

	InBuf[0] = 0x0; InBuf[1] = V_PUT_ARRAY; InBuf[3] = 0x0;
	for(i = 0x0; i < BlockQuantity; i++)
	{
		InBuf[2] = (WORD)((BaseAddress + i*MaxByteBlock/sizeof(SHORT)) | DSP_DM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i*MaxByteBlock/sizeof(SHORT), MaxByteBlock/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; break; }
	}

	if(!Error && RestDataPoints)
	{
		InBuf[2] = (WORD)((BaseAddress + BlockQuantity*MaxByteBlock/sizeof(SHORT)) | DSP_DM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + BlockQuantity*MaxByteBlock/sizeof(SHORT), RestDataPoints/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; }
	}

	return !Error;
}

//------------------------------------------------------------------------------
// ������ ������� ���� �� ������� ������ ������ DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_DM_ARRAY(WORD BaseAddress, WORD NPoints, SHORT * const Data)
{
	BOOL Error = FALSE;
	WORD i;
	WORD InBuf[4];
	WORD MaxByteBlock = MAX_USB_CONTROL_PIPE_BLOCK, BlockQuantity, RestDataPoints;

	// ��� ������ �������� �����
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }

	BlockQuantity = (WORD)((sizeof(SHORT)*NPoints)/MaxByteBlock);
	RestDataPoints = (WORD)((sizeof(SHORT)*NPoints)%MaxByteBlock); // � ������

	InBuf[0] = 1; InBuf[1] = V_GET_ARRAY; InBuf[3] = 0x0;
	for(i = 0x0; i < BlockQuantity; i++)
	{
		InBuf[2] = (WORD)((BaseAddress + i*MaxByteBlock/sizeof(SHORT)) | DSP_DM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf,sizeof(InBuf), Data + i*MaxByteBlock/sizeof(SHORT), MaxByteBlock/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; break; }
	}

	if(!Error && RestDataPoints)
	{
		InBuf[2] = (WORD)((BaseAddress + BlockQuantity*MaxByteBlock/sizeof(SHORT)) | DSP_DM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + BlockQuantity*MaxByteBlock/sizeof(SHORT), RestDataPoints/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; }
	}

	return !Error;
}

//------------------------------------------------------------------------------
// ������ ������� ���� � ������� ������ ������ DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::PUT_PM_ARRAY(WORD BaseAddress, WORD NPoints, long * const Data)
{
	BOOL Error = FALSE;
	WORD i;
	WORD InBuf[4];
	WORD MaxByteBlock = MAX_USB_CONTROL_PIPE_BLOCK, BlockQuantity, RestDataPoints;

	// ��� ������ �������� �����
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }

	BlockQuantity = (WORD)((sizeof(long)*NPoints)/MaxByteBlock);
	RestDataPoints = (WORD)((sizeof(long)*NPoints)%MaxByteBlock); // � ������

	InBuf[0] = 0; InBuf[1] = V_PUT_ARRAY; InBuf[3] = 0x0;
	for(i = 0x0; i < BlockQuantity; i++)
	{
		InBuf[2] = (WORD)((BaseAddress + i*MaxByteBlock/sizeof(long)) | DSP_PM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i*MaxByteBlock/sizeof(long), MaxByteBlock/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; break; }
	}

	if(!Error && RestDataPoints)
	{
		InBuf[2] = (WORD)((BaseAddress + BlockQuantity*MaxByteBlock/sizeof(long)) | DSP_PM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + BlockQuantity*MaxByteBlock/sizeof(long), RestDataPoints/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; }
	}

	return !Error;
}

//------------------------------------------------------------------------------
// ������ ������� ���� �� ������� ������ ������ DSP
//------------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_PM_ARRAY(WORD BaseAddress, WORD NPoints, long * const Data)
{
	BOOL Error = FALSE;
	WORD i;
	WORD InBuf[4];
	WORD MaxByteBlock = MAX_USB_CONTROL_PIPE_BLOCK, BlockQuantity, RestDataPoints;

	// ��� ������ �������� �����
	if(!Data) { LAST_ERROR_NUMBER(2); return FALSE; }

	BlockQuantity = (WORD)((sizeof(long)*NPoints)/MaxByteBlock);
	RestDataPoints = (WORD)((sizeof(long)*NPoints)%MaxByteBlock); // � ������

	InBuf[0] = 1; InBuf[1] = V_GET_ARRAY; InBuf[3] = 0x0;
	for(i = 0x0; i < BlockQuantity; i++)
	{
		InBuf[2] = (WORD)((BaseAddress + i*MaxByteBlock/sizeof(long)) | DSP_PM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i*MaxByteBlock/sizeof(long), MaxByteBlock/*bytes*/, TimeOut))  { LAST_ERROR_NUMBER(3); Error = TRUE; break; }
	}

	if(!Error && RestDataPoints)
	{
		InBuf[2] = (WORD)((BaseAddress + BlockQuantity*MaxByteBlock/sizeof(long)) | DSP_PM);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + BlockQuantity*MaxByteBlock/sizeof(long), RestDataPoints/*bytes*/, TimeOut)) { LAST_ERROR_NUMBER(3); Error = TRUE; }
	}

	return !Error;
}



//************************************************************************
// ������� ��� ������ � ����������� FLASH
//************************************************************************
//------------------------------------------------------------------------
// �������� ������������ FLASH
//------------------------------------------------------------------------
BOOL WINAPI TLE440::ERASE_BOOT_FLASH(void)
{
	BOOL Status;
	WORD InBuf[4];

	// �������� ������ � ������
	InBuf[0] = 0; InBuf[1] = V_ERASE_BOOT_FLASH; InBuf[2] = 0x0; InBuf[3] = 0x0;
	Status = LDeviceIoControl( DIOC_SEND_COMMAND,
										&InBuf, sizeof(InBuf),
										NULL, 0/*bytes*/, 15000);
	// �� ������ :)))))
	if(Status) return TRUE;
	// ������ ���������� LDeviceIoControl()
	else { LastErrorNumber = 2; return FALSE; }
}

//------------------------------------------------------------------------
// ������ ������� ���� � ����������� FLASH
//------------------------------------------------------------------------
BOOL WINAPI TLE440::PUT_ARRAY_BOOT_FLASH(DWORD BaseAddress, DWORD NBytes, BYTE * const Data)
{
	BOOL Error = FALSE;
	WORD i, InBuf[4];
	WORD BlockQuantity, RestDataPoints;

	BlockQuantity = (WORD)(NBytes/MAX_USB_CONTROL_PIPE_BLOCK);
	RestDataPoints = (WORD)(NBytes%MAX_USB_CONTROL_PIPE_BLOCK); // � ������

	InBuf[0] = 0; InBuf[1] = V_PUT_ARRAY_BOOT_FLASH;
	for(i = 0x0; i < BlockQuantity; i++)
	{
		InBuf[2] = (WORD)((BaseAddress + i*MAX_USB_CONTROL_PIPE_BLOCK) & 0x0000FFFFL);
		InBuf[3] = (WORD)(((BaseAddress + i*MAX_USB_CONTROL_PIPE_BLOCK) & 0xFFFF0000L) >> 0x10);
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i*MAX_USB_CONTROL_PIPE_BLOCK, MAX_USB_CONTROL_PIPE_BLOCK/*bytes*/, TimeOut)) { Error = TRUE; break; }
	}

	if(!Error && RestDataPoints)
	{
		InBuf[2] = (WORD)((BaseAddress + BlockQuantity*MAX_USB_CONTROL_PIPE_BLOCK) & 0x0000FFFFL);
		InBuf[3] = (WORD)(((BaseAddress + BlockQuantity*MAX_USB_CONTROL_PIPE_BLOCK) & 0xFFFF0000L) >> 0x10);
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + BlockQuantity*MAX_USB_CONTROL_PIPE_BLOCK, RestDataPoints/*bytes*/, TimeOut)) Error = TRUE;
	}

	return !Error;
}

//------------------------------------------------------------------------
// ������ ������� ���� � ����������� FLASH
//------------------------------------------------------------------------
BOOL WINAPI TLE440::GET_ARRAY_BOOT_FLASH(DWORD BaseAddress, DWORD NBytes, BYTE * const Data)
{
	BOOL Error = FALSE;
	WORD i, InBuf[4];
	WORD BlockQuantity, RestDataPoints;

	BlockQuantity = (WORD)(NBytes/MAX_USB_CONTROL_PIPE_BLOCK);
	RestDataPoints = (WORD)(NBytes%MAX_USB_CONTROL_PIPE_BLOCK); // � ������

	InBuf[0] = 1; InBuf[1] = V_GET_ARRAY_BOOT_FLASH;
	for(i = 0x0; i < BlockQuantity; i++)
	{
		InBuf[2] = (WORD)((BaseAddress + i*MAX_USB_CONTROL_PIPE_BLOCK) & 0x0000FFFFL);
		InBuf[3] = (WORD)(((BaseAddress + i*MAX_USB_CONTROL_PIPE_BLOCK) & 0xFFFF0000L) >> 0x10);
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i*MAX_USB_CONTROL_PIPE_BLOCK, MAX_USB_CONTROL_PIPE_BLOCK/*bytes*/, TimeOut))  { Error = TRUE; break; }
	}

	if(!Error && RestDataPoints)
	{
		InBuf[2] = (WORD)((BaseAddress + BlockQuantity*MAX_USB_CONTROL_PIPE_BLOCK) & 0x0000FFFFL);
		InBuf[3] = (WORD)(((BaseAddress + BlockQuantity*MAX_USB_CONTROL_PIPE_BLOCK) & 0xFFFF0000L) >> 0x10);
		if(!LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + BlockQuantity*MAX_USB_CONTROL_PIPE_BLOCK, RestDataPoints/*bytes*/, TimeOut)) Error = TRUE;
	}

	return !Error;
}



//==============================================================================
// ���������� ��������� ������� ��� ������ � �������
//==============================================================================

//------------------------------------------------------------------------------
// ������� ��������� DSP ������ �� ������������ ����
//------------------------------------------------------------------------------
BOOL TLE440::LoadModuleFromBootFlash(void)
{
	WORD InBuf[4] = { 0x0, V_LOAD_LBIOS_FROM_FLASH, 0x0, 0x0 };

	// �������� USB-������ V_RELOAD_LBIOS_FROM_FLASH
	if(!TLUSBBASE::LDeviceIoControl( DIOC_SEND_COMMAND,
												&InBuf, sizeof(InBuf),
												NULL, 0x0/*bytes*/,
												TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ��������� DSP ������ �� ������� ���������� Lusbapi
//------------------------------------------------------------------------------
BOOL TLE440::LoadModuleFromResource(void)
{
	DWORD	i, NBytes;
	char 	*BiosCodeByte;
	WORD	*BiosCodeWord;
	WORD 	PmWords, DmWords;

	// ���������� ������������� ������ E14-440
	if(!RESET_MODULE(INIT_E440)) { LAST_ERROR_NUMBER(4); return FALSE; }

	// ������ ������ � ����� LBIOS
	HRSRC RsRes = FindResource(hInstance, "LBIOSDATA", RT_RCDATA);
	if(!RsRes) { LAST_ERROR_NUMBER(5); return FALSE; }
	// �������� ���
	HGLOBAL RcResHandle = LoadResource(hInstance, RsRes);
	if(!RcResHandle) { LAST_ERROR_NUMBER(6); return FALSE; }
	// �����������
	char  *pRcData = (char *)LockResource(RcResHandle);
	if(!pRcData) { LAST_ERROR_NUMBER(7); return FALSE; }
	// ��������� ������ �������
	NBytes = SizeofResource(hInstance, RsRes);
	if(!NBytes) { LAST_ERROR_NUMBER(8); return FALSE; }

	// ������� ������ � ������ ���������� ��� LBIOS
	BiosCodeByte = new char[NBytes+2];
	if(!BiosCodeByte)  { LAST_ERROR_NUMBER(9); return FALSE; }

	// ��������� ������ �������
	for(i = 0x0; i < NBytes; i++) BiosCodeByte[i] = pRcData[i];

	// ��������� � WORD
	BiosCodeWord = (WORD *)BiosCodeByte;

	// ���-�� ���� ��� ������ ��������
	PmWords = BiosCodeWord[0];
	// ���-�� ���� ��� ������ ������
	DmWords = BiosCodeWord[PmWords+1];

	// �������� ������ ������ DSP
	if(!PUT_DM_ARRAY((WORD)0x0000, DmWords, (SHORT *)(BiosCodeWord + PmWords + 2))) { delete[] BiosCodeByte;  LAST_ERROR_NUMBER(13); return FALSE; }
	// �������� ������ �������� DSP, ������� � ������ 0�1
	else if(!PUT_PM_ARRAY((WORD)0x1, MAKE_PM_BUFFER_E440((PmWords-2)/2, (BiosCodeWord + 3)))) { delete[] BiosCodeByte;  LAST_ERROR_NUMBER(14); return FALSE; }
	// �������� ������� ������ ������ �������� DSP, �.�. ��������� LBIOS
	else if(!PUT_PM_ARRAY((WORD)0x0, MAKE_PM_BUFFER_E440(1, BiosCodeWord + 1))) { delete[] BiosCodeByte;  LAST_ERROR_NUMBER(14); return FALSE; }

	// ��������� ������
	if(BiosCodeByte) delete[] BiosCodeByte;

	// ������ ������ ������� ����� �������� ���������� LBIOS, �������������� � ������ �������� DSP
	long PBA; if(!GET_PM_WORD(L_PROGRAM_BASE_ADDRESS_E440, &PBA)) { LAST_ERROR_NUMBER(15); return FALSE; }
	ProgramBaseAddress = (WORD)(PBA >> 0x8);
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ��������� DSP ������ �� �����
//------------------------------------------------------------------------------
BOOL TLE440::LoadModuleFromFile(PCHAR const FileName)
{
	int	BiosFile;
	DWORD	NBytes;
	char 	*BiosCodeByte;
	WORD	*BiosCodeWord;
	WORD 	PmWords, DmWords;

	// ���������� ������������� ������ E14-440
	if(!RESET_MODULE(INIT_E440)) { LAST_ERROR_NUMBER(4); return FALSE; }

	// ������� ���� � LBIOS
	BiosFile = open(FileName, O_BINARY | O_RDONLY);
	if(BiosFile == -1) { LAST_ERROR_NUMBER(10); return FALSE; }

	// ��������� ���-�� ���� ��� LBIOS
	NBytes = filelength(BiosFile);
	if((long)NBytes == -1) { LAST_ERROR_NUMBER(11); return FALSE; }

	// ������� ������ ��� ������ LBIOS
	BiosCodeByte = new char[NBytes+2];
	if(!BiosCodeByte)  { LAST_ERROR_NUMBER(9); return FALSE; }

	if(read(BiosFile, BiosCodeByte, NBytes) == -1) { LAST_ERROR_NUMBER(12); return FALSE; }

	// ������� ����
	close(BiosFile);

	// ��������� � WORD
	BiosCodeWord = (WORD *)BiosCodeByte;

	// ���-�� ���� ��� ������ ��������
	PmWords = BiosCodeWord[0];
	// ���-�� ���� ��� ������ ������
	DmWords = BiosCodeWord[PmWords+1];

	// �������� ������ ������ DSP
	if(!PUT_DM_ARRAY((WORD)0x0000, DmWords, (SHORT *)(BiosCodeWord + PmWords + 2))) { delete[] BiosCodeByte;  LAST_ERROR_NUMBER(13); return FALSE; }
	// �������� ������ �������� DSP, ������� � ������ 0�1
	else if(!PUT_PM_ARRAY((WORD)0x1, MAKE_PM_BUFFER_E440((PmWords-2)/2, (BiosCodeWord + 3)))) { delete[] BiosCodeByte;  LAST_ERROR_NUMBER(14); return FALSE; }
	// �������� ������� ������ ������ �������� DSP, �.�. ��������� LBIOS
	else if(!PUT_PM_ARRAY((WORD)0x0, MAKE_PM_BUFFER_E440(1, BiosCodeWord + 1))) { delete[] BiosCodeByte;  LAST_ERROR_NUMBER(14); return FALSE; }

	// ��������� ������
	if(BiosCodeByte) delete[] BiosCodeByte;

	// ������ ������ ������� ����� �������� ���������� LBIOS, �������������� � ������ �������� DSP
	long PBA; if(!GET_PM_WORD(L_PROGRAM_BASE_ADDRESS_E440, &PBA)) { LAST_ERROR_NUMBER(15); return FALSE; }
	ProgramBaseAddress = (WORD)(PBA >> 0x8);
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
long *TLE440::Transform_Buffer_E440(WORD PmPoints, long * const Buffer)
{
	WORD i;

	for(i = 0x0; i < PmPoints; i++)
		Buffer[i] = ((Buffer[i] & 0x0000FFFF) << 0x8) | ((Buffer[i] & 0x00FF0000) >> 0x10);
	return Buffer;
}

//------------------------------------------------------------------------------------
// ������ ����������� ������ (��������� ����� ���� ������)
//------------------------------------------------------------------------------------
BOOL TLE440::GetModuleDescriptor(void)
{
	WORD i;
	SHORT *ptr;

	// ������� ��������� �� ��������� � ���� (SHORT *)
	ptr = (SHORT *)&ModuleDescriptor;
	// ��������� �������� ��������� ������ ���� ������
	for(i = 0x0; i < (sizeof(MODULE_DESCRIPTOR)/2); i++) if(!READ_FLASH_WORD(i, &ptr[i])) { LAST_ERROR_NUMBER(54); return FALSE; }
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------------
// ���������� ����������� ������ (��������� ����� ���� ������)
//------------------------------------------------------------------------------------
BOOL TLE440::PutModuleDescriptor(void)
{
	WORD i;
	SHORT *ptr;

	// ������� ��������� �� ��������� � ���� (SHORT *)
	ptr=(SHORT *)&ModuleDescriptor;
	// �������� ��������� ������ � ����
	if(!ENABLE_FLASH_WRITE(TRUE)) { LAST_ERROR_NUMBER(55); return FALSE; }
	// �������� ������ � ��������� ������ ����
	IsServiceFlashWriting = TRUE;
	for(i = 0x0; i < (sizeof(MODULE_DESCRIPTOR)/2); i++) if(!WRITE_FLASH_WORD(i, ptr[i])) { IsServiceFlashWriting = FALSE; LAST_ERROR_NUMBER(56); return FALSE; }
	// �������� ������ � ��������� ������ ����
	IsServiceFlashWriting = FALSE;
	// �������� ��������� ������ � ����
	if(!ENABLE_FLASH_WRITE(FALSE)) { LAST_ERROR_NUMBER(57); return FALSE; }
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ���������� � LBIOS
//------------------------------------------------------------------------------------
BOOL TLE440:: GetDspDescriptor(void)
{
	BYTE *bPtr;
	WORD i;
	SHORT *sPtr;

	// ������� ���������
	bPtr = (BYTE  *)&DspDescriptor;
	sPtr = (SHORT *)&DspDescriptor;
	// ���������� ���������� DSP c ���������� SHORT
	for(i = 0x0; i < sizeof(DSP_DESCRIPTOR)/2; i++) if(!GET_LBIOS_WORD((WORD)(L_DSP_INFO_STUCTURE_E440 + i), &sPtr[i])) { LAST_ERROR_NUMBER(16); return FALSE; }
	// ���� ����� - ���������� ��������� ����
	if(sizeof(DSP_DESCRIPTOR)%2)
	{
		if(!GET_LBIOS_WORD((WORD)(L_DSP_INFO_STUCTURE_E440 + sizeof(DSP_DESCRIPTOR)/2), (SHORT *)&i)) { LastErrorNumber = 13; return FALSE; }
		bPtr[sizeof(DSP_DESCRIPTOR)-0x1] = (BYTE)(i & 0xFF);
	}

	// �������� ���������� ���������: �������� � ��� ������ LBIOS ������ ���� 3.0
	if(strcmp((char *)DspDescriptor.Manufacturer, "L-Card Ltd."))
	{
		WORD lbv;

		ZeroMemory(&DspDescriptor, sizeof(DSP_DESCRIPTOR));
		strcpy((char *)DspDescriptor.DspName, "ADSP-2185M");
		// ��������� ������ LBIOS
		if(!GET_LBIOS_WORD(L_LBIOS_VERSION_E440, (SHORT *)&lbv)) { LAST_ERROR_NUMBER(16); return FALSE; }
		sprintf((char *)DspDescriptor.Version, "%2u.%2u", lbv >> 0x8, lbv & 0xFF);
		strcpy((char *)DspDescriptor.Created, "Unknown");
		strcpy((char *)DspDescriptor.Manufacturer, "L-Card Ltd.");
		strcpy((char *)DspDescriptor.Author, "Unknown");
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� � ����������� � �������� ����������������
//------------------------------------------------------------------------------------
BOOL TLE440::GetMcuDescriptor(void)
{
	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E440")) { LAST_ERROR_NUMBER(24); return FALSE; }
	// ��� ������� ������ 'E' � ���� ����� ��������� ���������� � ��������� MCU
	else if((this->ModuleDescriptor.Revision >= 'E') && (this->ModuleDescriptor.Revision <= 'Z'))
	{
		WORD InBuf[4] = { 0x1, V_GET_MCU_DESCRIPTOR, 0x0, 0x0 };

		if(!TLUSBBASE::LDeviceIoControl(	DIOC_SEND_COMMAND,
													&InBuf, sizeof(InBuf),
													&FirmwareDescriptor, sizeof(FIRMWARE_DESCRIPTOR),
													TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	}
	else
	{
		ZeroMemory(&FirmwareDescriptor, sizeof(FIRMWARE_DESCRIPTOR));
		strcpy((char *)FirmwareDescriptor.McuName, "Unknown");
		strcpy((char *)FirmwareDescriptor.Version, "Unknown");
		strcpy((char *)FirmwareDescriptor.Created, "Unknown");
		strcpy((char *)FirmwareDescriptor.Manufacturer, "L-Card");
		strcpy((char *)FirmwareDescriptor.Author, "Unknown");
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ����������� ���������� ������ ���������������� MCU ������
//------------------------------------------------------------------------------
BOOL TLE440::SetMcuAttributes(WORD McuAttributes)
{
	WORD InBuf[4] = { 0x0, V_SET_MCU_ATTRIBUTES, McuAttributes, 0x0 };

	// ��� ������� ������ 'D' � ���� ������ ��������� � ������� ������ MCU ������
	if((this->ModuleDescriptor.Revision >= 'D') && (this->ModuleDescriptor.Revision <= 'Z'))
	{
		// �������� USB-������ � MCU
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
												&InBuf, sizeof(InBuf),
												NULL, 0/*bytes*/,
												TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ����������� �������� ��� ��� ������ � ������� ������ 'C'
//------------------------------------------------------------------------------
BOOL TLE440::StopAdcForOldRevision(void)
{
	WORD InBuf[4] = { 0x0, V_START_ADC, 0x0 | DSP_DM, (WORD)(AdcPars.AdcFifoLength/2.0)};

	// ��� USB-������ � MCU �� ������ ���, ������������ �������
	// ��������������� ���������� � �������� ����������������
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ����������� �������� ��� ��� ������ � ������� ������ 'C'
//------------------------------------------------------------------------------
BOOL TLE440::StopDacForOldRevision(void)
{
	// �� ������ :)))))
	return TRUE;
}


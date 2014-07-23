//------------------------------------------------------------------------------
#include <algorithm>
#include <stdio.h>
#include "e140.h"
#include "ErrorBaseIds.h"
//------------------------------------------------------------------------------

#define 	LAST_ERROR_NUMBER(ErrorNumber)	LastErrorNumber = E140_BASE_ERROR_ID + ErrorNumber

// -----------------------------------------------------------------------------
//  ����������
// ----------------------------------------------------------------------------
TLE140::TLE140(HINSTANCE hInst) : TLUSBBASE(hInst)
{
	// ������������� ���� ��������� �������� ������
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));
	ZeroMemory(&DacPars, sizeof(DAC_PARS));
	// ������������� ����� ��������� ���������� ���
	AdcPars.AdcRate = 100.0;
	AdcPars.ChannelsQuantity = 0x1;
	ZeroMemory(AdcPars.ControlTable, sizeof(AdcPars.ControlTable));
	// ������� ������ ����� ������
	IsDataAcquisitionInProgress = FALSE;
	// ������� ����� ������ Firmware ����������������
	FirmwareVersionNumber = 0.0;
}

// -----------------------------------------------------------------------------
//  ����������
// -----------------------------------------------------------------------------
TLE140::~TLE140() { }



// =============================================================================
// ������� ������ ���������� ��� ������ � ������� �14-140
// =============================================================================
// -----------------------------------------------------------------------------
//  ������� ����������� ���� ��� ������� � USB ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::OpenLDevice (WORD VirtualSlot)
{
	char ModuleName[LONG_MODULE_NAME_STRING_LENGTH];

	//  ��������� ������� ����������� ���� ��� ������� � USB ������
	if(!TLUSBBASE::OpenLDeviceByID(VirtualSlot, E14_140_ID)) { return FALSE; }
	// ��������� ��������� �������� ������
	else if(!TLUSBBASE::GetModuleName(ModuleName)) { TLUSBBASE::CloseLDevice(); return FALSE; }
	// ��������, ��� ��� ������ E14-140
	else if(strcmp(ModuleName, "E140")) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(0); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ������� ����������� ����
//------------------------------------------------------------------------------------
BOOL WINAPI TLE140::CloseLDevice(void)
{
	// ������� ����������
	if(!TLUSBBASE::CloseLDevice()) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ��������� �� ����������
//------------------------------------------------------------------------------------
BOOL WINAPI TLE140::ReleaseLInstance(void)
{
	BOOL Status = TRUE;

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
HANDLE WINAPI TLE140::GetModuleHandle(void) { return hDevice; }

//------------------------------------------------------------------------------
//  ������ �������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::GetModuleName(PCHAR const ModuleName)
{
	// �������� �����
	if(!ModuleName) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ������ ��������� ������ �������� ������
	else if(!TLUSBBASE::GetModuleNameByLength(ModuleName, SHORT_MODULE_NAME_STRING_LENGTH)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------------
//  ��������� ������� �������� ������ ���� USB
//------------------------------------------------------------------------------------
BOOL WINAPI TLE140::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// �������� ���������
	if(!UsbSpeed) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ������ ��������� ������ �������� ������ USB ����
	else if(!TLUSBBASE::GetUsbSpeed(UsbSpeed)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ������� ������� ������������������ ������ E14-140
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::LowPowerMode(BOOL LowPowerFlag)
{
	BYTE Flag = (BYTE)(LowPowerFlag ? 0x1 : 0x0);

	// ��������� ��������� � ������ ������� ���������� ������ �������������������
	if(!PutArray(&Flag, 0x1, SUSPEND_MODE_FLAG)) { return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ������ � ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	return TLUSBBASE::GetLastErrorInfo(LastErrorInfo);
}




// =============================================================================
// ������� ��� ������ � ��� ������
// =============================================================================
// -----------------------------------------------------------------------------
// ��������� ������� ���������� ������ ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::GET_ADC_PARS(ADC_PARS_E140 * const AdcPars)
{
	BYTE b, smode;
	BYTE buf[136];
	WORD i;
	DWORD ClockRate;

	// �������� ���������
	if(!AdcPars) { LAST_ERROR_NUMBER(1); return FALSE; }
	// �������� ������� ��������� ������ ���
	if(!GetArrayFromMcu(buf, sizeof(buf), ADC_PARS_BASE)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������� ���������� �������
	for(i = 0x0; i < MAX_CONTROL_TABLE_LENGTH_E140; i++)
	{
		b = buf[i];
		this->AdcPars.ControlTable[i] = (WORD)((b & 0xF0) | ((b << 2) & 0x0C) | ((b >> 2) & 0x03));
	}
	ClockRate = ModuleDescriptor.ClockRate;
	if((ClockRate > 24000000L) || (ClockRate < 1000000L)) ClockRate = 16000000L;
	// ������� ������ ���(���)
	this->AdcPars.AdcRate = (double)ClockRate/2000.0/(*(WORD*)(buf+128));
	// ����� �������� ���������� �������
	this->AdcPars.ChannelsQuantity = buf[130];
	// ����������� �������� (��)
	this->AdcPars.InterKadrDelay = (double)((WORD)buf[131] + 1.0)/this->AdcPars.AdcRate;
	// ������� ������ (���)
	this->AdcPars.KadrRate = (double)this->AdcPars.AdcRate/((WORD)buf[130] + (WORD)buf[131]);
	// ��� �������������
	smode = (BYTE)(buf[132] & 0x0F);
	if(smode == 0x1)      	this->AdcPars.InputMode = 2;
	else if(smode == 0x2) 	this->AdcPars.InputMode = 1;
	else if(smode > 0x2) 	this->AdcPars.InputMode = 3;
	else              		this->AdcPars.InputMode = 0;
	this->AdcPars.SynchroAdType = (BYTE)((smode == 0x5 || smode == 0x6) ? 0x1 : 0x0);
	this->AdcPars.SynchroAdMode = (BYTE)((smode == 0x4 || smode == 0x6) ? 0x1 : 0x0);
	// �������� �������� ���������
	this->AdcPars.ClkSource = (BYTE)((buf[132] >> 0x6) & 0x1);
	// ���� ���������� ���������� ����������� �������� ��������� ���
	this->AdcPars.EnableClkOutput = (BYTE)((buf[132] >> 0x7) & 0x1);
	// ����� ����������� ������ ��� ���������� �������������
	b = buf[133];
	this->AdcPars.SynchroAdChannel = (WORD)((b & 0xF0) | ((b << 0x2) & 0x0C) | ((b >> 0x2) & 0x03));
	// ����� ������������ ��� ���������� �������������
	this->AdcPars.SynchroAdPorog = *(SHORT*)(buf+134);
	//
	*AdcPars = this->AdcPars;
	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ���������� ��������� ���������� ������ ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::SET_ADC_PARS(ADC_PARS_E140 * const AdcPars)
{
	BYTE b, smode;
	BYTE buf[136];
	DWORD i;
	DWORD ClockRate;

	// �������� ���������
	if(!AdcPars) { LAST_ERROR_NUMBER(1); return FALSE; }
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ��� ������
	// �������������� ���������� ������� ������� STOP_ADC()
	else if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��������� �������� ������
	else if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }

	// ������� �����
	ZeroMemory(buf, sizeof(buf));
	// ������� ���������� �������
	for(i = 0x0; i < MAX_CONTROL_TABLE_LENGTH_E140; i++)
	{
		b = AdcPars->ControlTable[i];
		buf[i] = (BYTE)((b & 0xF0) | ((b << 0x2) & 0x0C) | ((b >> 0x2) & 0x03));
	}
	// ����������� � �������� �������� ������
	ClockRate = ModuleDescriptor.ClockRate;		// � ��
	if((ClockRate > 24000000L) || (ClockRate < 1000000L)) ClockRate = 16000000L;
	ClockRate = ClockRate/2000.0 + 0.5;

	// �������� ������� ��� �� ���������� ��������� ��������
	AdcPars->AdcRate = fabs(AdcPars->AdcRate);
	if(AdcPars->AdcRate < ((double)ClockRate/(double)0xFFFFL)) AdcPars->AdcRate = (double)ClockRate/(double)0xFFFFL;
	// ��������� �������� ������� ������ ���
	i = (double)ClockRate/AdcPars->AdcRate + 0.5;
	// ������� ����������� �� ������������ �������� ������� ������ ���
	if(i > 0xFFFF) i = 0xFFFF;				// ������ �������� - WORD
	else
	{
		// � ������ ������� 'A' ������������ ������� ������ ��� ���������� 100 ���
		if(ModuleDescriptor.Revision == 'A') { if(i < 80) i = 80; }
		// ��� ��������� - 200 ���
		else { if(i < 40) i = 40; }
	}
	// ������ ������� ������������� ������� ������ ��� � ���
	AdcPars->AdcRate = (double)ClockRate/(double)i;
	//
	*(WORD*)(buf+128) = (WORD)i;
	// ����� �������� ���������� �������
	if(!AdcPars->ChannelsQuantity) { LAST_ERROR_NUMBER(26); return FALSE; }
	else if(AdcPars->ChannelsQuantity > MAX_CONTROL_TABLE_LENGTH_E140) AdcPars->ChannelsQuantity = MAX_CONTROL_TABLE_LENGTH_E140;
	buf[130] = (BYTE)AdcPars->ChannelsQuantity;
	// �������� ����������� �������� ���
	i = (DWORD)(AdcPars->AdcRate*AdcPars->InterKadrDelay + 0.5);
	// ������� ����������� �� �������� ����������� �������� ���
	if(i == 0x0) i = 0x1;
	else if(i > 0x100) i = 0x100;
	// ������ ������� ������������� ����������� �������� ��� � ��
	AdcPars->InterKadrDelay = (double)i/AdcPars->AdcRate;
	//
	buf[131] = (BYTE)(i - 0x1);
	// ������� ������ (���)
	AdcPars->KadrRate = (double)AdcPars->AdcRate/((WORD)buf[130] + (WORD)buf[131]);
	 // ��� �������������
	if(AdcPars->InputMode > ANALOG_SYNC_E140) AdcPars->InputMode = ANALOG_SYNC_E140;
	smode = AdcPars->InputMode;
	if(smode == 1) smode=2;
	else if(smode == 2) smode = (BYTE)(0x1);
	else if(smode == 3)
	{
		if(AdcPars->SynchroAdType) smode += (BYTE)(0x2);
		if(AdcPars->SynchroAdMode) smode += (BYTE)(0x1);
	}
	buf[132] = smode;
	// �������� �������� ���������
	if(AdcPars->ClkSource) buf[132] |= (0x1 << 0x6);
	// ���� ���������� ���������� ����������� �������� ��������� ���
	if(AdcPars->EnableClkOutput) buf[132] |= (0x1 << 0x7);
	// ����� ����������� ������ ��� ���������� �������������
	b = AdcPars->SynchroAdChannel;
	buf[133] = (BYTE)((b & 0xF0) | ((b << 0x2) & 0x0C) | ((b >> 0x2) & 0x03));
	// ����� ������������ ��� ���������� �������������
	*(SHORT*)(buf+134) = AdcPars->SynchroAdPorog;
	// �������� ������� ��������� ������ ���
	this->AdcPars=*AdcPars;
	// ������� ��������� ���
	if(!PutArrayToMcu(buf, sizeof(buf), ADC_PARS_BASE)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ������ ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::START_ADC(void)
{
	WORD InBuf[4] = { 0x0000, V_START_ADC, 0x0, 0x0 };

	// �������� ������ �� ������ ���
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	// ��������� ������ ����� ������
	IsDataAcquisitionInProgress = TRUE;
	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
// ������� ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::STOP_ADC(void)
{
	WORD InBuf[4] = {0x0000, V_STOP_ADC, 0x0, 0x0 };

	// �������� ������� �� ������� ���
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	// �������� ����� ����� ������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3/*reset Read Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	// ������� ������ ����� ������
	IsDataAcquisitionInProgress = FALSE;
	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ���� ����� �������� � ��� ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::ADC_KADR(SHORT * const Data)
{
	// ������ �� ����������� ���������� ����� �������� ���
	WORD InBuf[4] = { 0x0000, V_START_ADC_ONCE, 0x0, 0x0};
	// ���-�� ����������� ��������
	DWORD PointsToRead;
	// ��������� ��� ������� ������
	IO_REQUEST_LUSBAPI IoReq;

	// �������� �����
	if(!Data) { LAST_ERROR_NUMBER(1); return FALSE; }
	// �������� ���-�� ������������ �������
	else if(AdcPars.ChannelsQuantity > 32) { LAST_ERROR_NUMBER(6); return FALSE; }
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ������ �������
	// �������������� ���������� ������� ������� STOP_ADC()
	else if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��������� ��������� ������� STOP_ADC()
	else if(!STOP_ADC()) { LAST_ERROR_NUMBER(25); return FALSE; }
	// ���������� ����������� �����
//	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	// �������� ������ �� ����������� ������ ���
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ��������� ��������� ������� ���� IO_REQUEST_LUSBAPI �� ���� ������
	if(!(AdcPars.ChannelsQuantity%32)) PointsToRead = AdcPars.ChannelsQuantity;
	else { PointsToRead = AdcPars.ChannelsQuantity - AdcPars.ChannelsQuantity%32; PointsToRead += 32; }
	// ��������� ����������� ������� �� ����� ������ � ���
	IoReq.Buffer 					= new SHORT[PointsToRead];	// ����� ������
	if(!IoReq.Buffer) { LAST_ERROR_NUMBER(7); return FALSE; }
	IoReq.NumberOfWordsToPass 	= PointsToRead;	 			// ���-�� ������ � SHORT
	IoReq.NumberOfWordsPassed 	= 0x0;
	IoReq.Overlapped			  	= NULL;				 			// � ��� ����� ���������� ������
	IoReq.TimeOut					= PointsToRead/AdcPars.AdcRate + 1000;	// ������� ���������� �������
	// ������ ���� ����� ��������
	if(!ReadData(&IoReq)) { LAST_ERROR_NUMBER(8); delete [] IoReq.Buffer; return FALSE; }
	// �������� ���������� ������
	CopyMemory(Data, &IoReq.Buffer[(32 - AdcPars.ChannelsQuantity) & 0x1F], AdcPars.ChannelsQuantity*sizeof(SHORT));
	// ����������� ������
	if(IoReq.Buffer) delete [] IoReq.Buffer;
	// �� ������
	return TRUE;
}


// -----------------------------------------------------------------------------
//  ����������� ���� � ��������� ����������� ������ ��� ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::ADC_SAMPLE(SHORT * const Data, WORD Channel)
{
	WORD b = (WORD)((Channel & 0xF0) | ((Channel << 0x2) & 0x0C) | ((Channel >> 0x2) & 0x03));

	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ������ �������
	// �������������� ���������� ������� ������� STOP_ADC()
	if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ���������� ����� ������
	else if(!PutArrayToMcu((BYTE*)&b, 0x2, ADC_CHANNEL_SELECT_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ����������
	Sleep(40);
	// 2� ����������� ��������
	if(!PutArrayToMcu((BYTE*)&b, 0x2, ADC_CHANNEL_SELECT_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ������ �����
	else if(!GetArrayFromMcu((BYTE*)Data, 0x2, ADC_DATA_REGISTER)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//  ��������� ������ ������ � ��� ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::ReadData(IO_REQUEST_LUSBAPI * const ReadRequest)
{
	DWORD NumberOfBytesRead;

	// �������� ����� ������
	if(!ReadRequest->Buffer) { LAST_ERROR_NUMBER(1); return FALSE; }
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
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(9); CancelIo(hDevice); return FALSE; } }
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
		if(!Ov.hEvent) { LAST_ERROR_NUMBER(10); return FALSE; }

		// �������� ����������� ������ �� ���� ������������ ���-�� ������
		NumberOfBytesRead = 0x0;
		if(!ReadFile(hDevice, ReadRequest->Buffer, 2*ReadRequest->NumberOfWordsToPass, &NumberOfBytesRead, &Ov))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(9); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; } }
		// ��� ��������� ���������� �������
		if(WaitForSingleObject(Ov.hEvent, ReadRequest->TimeOut) == WAIT_TIMEOUT) { LAST_ERROR_NUMBER(11); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ��������� �������� ���-�� ������� ���������� ���� ������
		else if(!GetOverlappedResult(hDevice, &Ov, &NumberOfBytesRead, TRUE)) { LAST_ERROR_NUMBER(12); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ������� �������
		else if(!CloseHandle(Ov.hEvent)) { LAST_ERROR_NUMBER(13); return FALSE; }
		// ���-�� ���������� ��������
		ReadRequest->NumberOfWordsPassed = NumberOfBytesRead/2;
		// ������� ������� ���� ������� ��������� ������ (� ������) � ������������� ���-��� ��������
		if(ReadRequest->NumberOfWordsToPass != ReadRequest->NumberOfWordsPassed) { LAST_ERROR_NUMBER(14); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}




// =============================================================================
//  ������� ��� ������ � ��� ������
// =============================================================================
//------------------------------------------------------------------------------
// ��������� ������� ���������� ��������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::GET_DAC_PARS(DAC_PARS_E140 * const DacPars)
{
	// �������� ��������� ���������� ������ ���
	if(!DacPars) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ��������� �������� ������
	else if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }
	// � ������ ������� 'A' ��� ���������� ������ �� ���
	else if(ModuleDescriptor.Revision == 'A') { LAST_ERROR_NUMBER(28); return FALSE; }

	// ��������� ������� ��������� ���
	if(!GetArrayFromMcu((BYTE *)&this->DacPars, sizeof(this->DacPars), DAC_PARS_BASE)) { LAST_ERROR_NUMBER(2); return FALSE; }

	// �������� ������� ������ ��� � ��� ( f = 200/(RateDiv+1) )
	DacPars->DacRate = 200.0/(this->DacPars.DacRateDiv + 1.0);
	// ��������� ����� �� ���������� ������ ��� � ���
	DacPars->SyncWithADC = this->DacPars.SyncWithADC;
	// ��������� ����� �� ���������� ������ ��� � ���
	DacPars->SetZeroOnStop = this->DacPars.SetZeroOnStop;

	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ��������� ���������� ��� ��������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::SET_DAC_PARS(DAC_PARS_E140 * const DacPars)
{
	DWORD DacDiv;
	DWORD ClockRate;

	// �������� ��������� ���������� ������ ���
	if(!DacPars) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��������� �������� ������
	else if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }
	// � ������ ������� 'A' ��� ���������� ������ �� ���
	else if(ModuleDescriptor.Revision == 'A') { LAST_ERROR_NUMBER(28); return FALSE; }

	// ��������� � �������� ��������� ������ ���
	ClockRate = ModuleDescriptor.ClockRate;
	if((ClockRate > 24000000L) || (ClockRate < 1000000L)) ClockRate = 16000000L;
	ClockRate = ClockRate/80000.0 + 0.5;
	DacPars->DacRate = fabs(DacPars->DacRate);
	DacDiv = ClockRate/DacPars->DacRate - 0.5;
	if(DacDiv > 0x00000007L) DacDiv = 0x0007L;
//	if(DacDiv > 0x000000FFL) DacDiv = 0x00FFL;
//	if(DacDiv > 0x0000FFFFL) DacDiv = 0xFFFFL;
	this->DacPars.DacRateDiv = (WORD)DacDiv;
	// ����� ������� ������������� ������� ���
	DacPars->DacRate = ClockRate/(this->DacPars.DacRateDiv + 1.0);

	// ��������� ����� �� ���������� ������ ��� � ���
	if(DacPars->SyncWithADC) this->DacPars.SyncWithADC = 0x1;
	else this->DacPars.SyncWithADC = 0x0;

	// ��������� ����� �� ���������� ������ ��� � ���
	if(DacPars->SetZeroOnStop) this->DacPars.SetZeroOnStop = 0x1;
	else this->DacPars.SetZeroOnStop = 0x0;

	// ������� ��������� ���
	if(!PutArrayToMcu((BYTE *)&this->DacPars, sizeof(this->DacPars), DAC_PARS_BASE)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}
//------------------------------------------------------------------------------
// ������ ��������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::START_DAC(void)
{
	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }
	// � ������ ������� 'A' ��� ���������� ������ �� ���
	else if(ModuleDescriptor.Revision == 'A') { LAST_ERROR_NUMBER(28); return FALSE; }
	// � ���� �� ��� �� ������ ���������� ������?
	else if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E140)  { LAST_ERROR_NUMBER(15); return FALSE; }
	// �� � ������� - ��������� ��������� ����� �� ���
	else
	{
		WORD InBuf[4] = { 0x0, V_START_DAC, 0x0, 0x0 };

		// ������ ������� �������� Bulk-������ ����� ������ �� USB (�� ������ ������)
		if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		// ��� USB-������ � MCU �� ������ ��������� ������ ���
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	}
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ��������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::STOP_DAC(void)
{
	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }
	// � ������ ������� 'A' ��� ���������� ������ �� ���
	else if(ModuleDescriptor.Revision == 'A') { LAST_ERROR_NUMBER(28); return FALSE; }
	// � ���� �� ��� �� ������ ���������� ������?
	else if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E140)  { LAST_ERROR_NUMBER(15); return FALSE; }
	else
	{
		WORD InBuf[4] = { 0x0, V_STOP_DAC, 0x0, 0x0 };

		// ��� USB-������ � MCU �� ������� ��������� ������ ���
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		// ������ ������� �������� Bulk-������ ����� ������ �� USB (�� ������ ������)
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1/*reset Write Pipe*/, NULL, NULL, NULL, NULL, TimeOut)) { LAST_ERROR_NUMBER(3); return FALSE; }
	}
	// �� ������
	return TRUE;
}

//------------------------------------------------------------------------------
// ��������� �������� ������ ��� � ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE140::WriteData(IO_REQUEST_LUSBAPI * const WriteRequest)
{
	DWORD NumberOfBytesWritten;

	// �������� ����� ������
	if(!WriteRequest->Buffer) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ����� �������� ������ ���������� � ��������� �� 0x100(256)
	else if(WriteRequest->NumberOfWordsToPass < 0x100) WriteRequest->NumberOfWordsToPass = 0x100;
	// � ������ �� 0x100000(1024*1024)
	else if(WriteRequest->NumberOfWordsToPass > (1024*1024)) WriteRequest->NumberOfWordsToPass = 1024*1024;
	// ����� ����� �������� ������ ���� ������ 0x100(256)
	WriteRequest->NumberOfWordsToPass -= (WriteRequest->NumberOfWordsToPass)%256;

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

// -----------------------------------------------------------------------------
// ����������� ����� �� �������� ����� ���
// ������������ 12�� ������ ��� ��� - ��� ��� ������ E-14-140 Rev.'A'
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::DAC_SAMPLE(SHORT * const DacData, WORD DacChannel)
{
	SHORT DacParam;

	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }
	// � ���� �� ��� �� ������ ���������� ������
	else if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E140) { LAST_ERROR_NUMBER(15); return FALSE; }
	// �������� ����� ������ ���
	else if(DacChannel >= DAC_CHANNELS_QUANTITY_E140) { LAST_ERROR_NUMBER(16); return FALSE; }

	// ���������� �������� ��� ������ �� ���
	if((*DacData) < -2048) *DacData = -2048;
	else if((*DacData) > 2047) *DacData = 2047;
	// ��������� ��������������� ����� ��� �������� � MCU ������
	DacParam = (SHORT)((*DacData) & 0xFFF);
	DacParam |= (SHORT)(DacChannel << 12);
	// ������� � MCU ������
	if(!PutArrayToMcu((BYTE*)&DacParam, 0x2, DAC_SAMPLE_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ����������� ����� ����� �� ��� ������ ���
// ������������ 16�� ������ ��� ��� - ��� ��� ������ E-14-140 Rev.'B' � ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::DAC_SAMPLES(SHORT * const DacData1, SHORT * const DacData2)
{
	SHORT DacSamples[0x2];

	// ��������� �������� ������
	if(strcmp((char *)ModuleDescriptor.Name, "E140")) { LAST_ERROR_NUMBER(27); return FALSE; }
	// ������ ������� 'A' �� ������������ ����������� ����� �� ��� ������ ���
	else if(ModuleDescriptor.Revision == 'A') { LAST_ERROR_NUMBER(28); return FALSE; }
	// � ���� �� ��� �� ������ ���������� ������
	else if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E140) { LAST_ERROR_NUMBER(15); return FALSE; }
	// �������� �����
	DacSamples[0x0] = *DacData1; DacSamples[0x1] = *DacData2;
	// ������� � MCU ������
	if(!PutArrayToMcu((BYTE *)DacSamples, 0x4, DAC_SAMPLES_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}




// =============================================================================
//  ������� ��� ������ � ��� ������� �� ������� �������� �������
// =============================================================================
// -----------------------------------------------------------------------------
//  ������� ���������� �������� ����� �������� ��������� �������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::ENABLE_TTL_OUT(BOOL EnableTtlOut)
{
	BYTE Flag = (BYTE)(EnableTtlOut ? 0x1 : 0x0);

	// ����� ���� ����������� �������� �������
	if(!PutArrayToMcu(&Flag, 0x1, DOUT_ENABLE_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//   ������� ������ ������� ����� �������� ��������� �������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::TTL_IN(WORD * const TtlIn)
{
	// ������ ��������� �������� ������
	if(!GetArrayFromMcu((BYTE *)TtlIn, 0x2, DIN_REGISTER)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//  ������� ������ �� �������� ����� �������� ��������� �������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::TTL_OUT(WORD TtlOut)
{
	// ����� ��������� �������� �������
	if(!PutArrayToMcu((BYTE*)&TtlOut, 0x2, DOUT_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}




// =============================================================================
//  ������� ��� ������ � ���������������� ����������� ����
// =============================================================================
// -----------------------------------------------------------------------------
// ����������/���������� ������ ������ � ���������������� ������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled)
{
	BYTE Flag = (BYTE)(IsUserFlashWriteEnabled ? 0x1 : 0x0);

	// ��������� ������� ����������/���������� ������ � ����
	if(!PutArrayToMcu(&Flag, 0x1, ENABLE_FLASH_WRITE_FLAG)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� �������
	TLE140::IsUserFlashWriteEnabled = IsUserFlashWriteEnabled;
	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
// ������� ����� �� ���������������� ������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::READ_FLASH_ARRAY(USER_FLASH_E140 * const UserFlash)
{
	// �������� ���������
	if(!UserFlash) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ���� ������ ������
	else if(!GetArrayFromMcu((BYTE *)UserFlash, sizeof(USER_FLASH_E140), USER_FLASH_ADDRESS)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ����� � ���������������� ������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::WRITE_FLASH_ARRAY(USER_FLASH_E140 * const UserFlash)
{
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ������ ���������� � ���������������� ����
	// �������������� ���������� ������� ������� STOP_ADC()
	if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������ ������ ���� ���������
	else if(!IsUserFlashWriteEnabled) { LAST_ERROR_NUMBER(18); return FALSE; }
	// �������� ���������
	else if(!UserFlash) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ���� ������ ������
	else if(!PutArrayToMcu((BYTE *)UserFlash, sizeof(USER_FLASH_E140), USER_FLASH_ADDRESS)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}





// =============================================================================
//  ������� ��� ������ �� ��������� ����������� ����
// =============================================================================
// -----------------------------------------------------------------------------
//  ������� ��������� ���������� � ������ �� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E140 * const md)
{
	char *pTag;
	BYTE Buffer[128];
	BYTE Str[17];
	WORD i;

	// �������� ���������
	if(!md) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ��������� ����������� ���������� ������ �� ����
	else if(!GetArrayFromMcu(Buffer, sizeof(Buffer), DESCRIPTOR_BASE)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������������� ���������� ������
	else if(!UnpackModuleDescriptor(Buffer, &ModuleDescriptor)) { LAST_ERROR_NUMBER(19); return FALSE; }

	// Module Info
	strncpy((char *)md->Module.CompanyName,			"L-Card Ltd.",   								sizeof(md->Module.CompanyName));
	if(!strcmp((char *)ModuleDescriptor.Name,			"E140")) strcpy((char *)md->Module.DeviceName, "E14-140");
	else strncpy((char *)md->Module.DeviceName,		(char *)ModuleDescriptor.Name,	 		std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.Name)));
	strncpy((char *)md->Module.SerialNumber,			(char *)ModuleDescriptor.SerialNumber,	std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
	strcpy((char *)md->Module.Comment,					"������� ������ ���/���/��� ������ ���������� �� ���� USB 1.1");
	// ������� ������� ������
	md->Module.Revision = ModuleDescriptor.Revision;
	// ������ E14-140 ���������� � ������������ ���������� (�� ������ ���� ����)
	md->Module.Modification = NO_MODULE_MODIFICATION_LUSBAPI;

	// Interface Info
	md->Interface.Active = TRUE;
	strcpy((char *)md->Interface.Name, "USB 1.1");
	strcpy((char *)md->Interface.Comment, "Full-Speed Mode (12 Mbit/s)");

	// MCU Info
	if(!GetArrayFromMcu(Buffer, 0x10, FIRMWARE_VERSION)) { LAST_ERROR_NUMBER(2); return FALSE; }
	md->Mcu.Active = TRUE;
	sprintf((char *)Str, "%02u.%02u", Buffer[1], Buffer[0]);
	strncpy((char *)md->Mcu.Version.Version,			(char *)Str, 		std::min(sizeof(md->Mcu.Version.Version), sizeof(Str)));
	strncpy((char *)md->Mcu.Version.Date,				(char *)(Buffer+2), 14);
	md->Mcu.ClockRate = ModuleDescriptor.ClockRate/1000.0;		// � ���
	if(ModuleDescriptor.Revision == 'A')
	{
		strcpy((char *)md->Mcu.Name, 							"AVR ATmega8515");
		strcpy((char *)md->Mcu.Version.Manufacturer,		"L-Card Ltd.");
		strcpy((char *)md->Mcu.Version.Author,				"Kodorkin A.V.");
		strncpy((char *)md->Mcu.Version.Comment,			"������� �������� ��� ������ E14-140 Rev.'A'", sizeof(md->Mcu.Version.Comment));
		strncpy((char *)md->Mcu.Comment,						"8-bit Microcontroller with 8K Bytes In-System Programmable Flash", sizeof(md->Mcu.Comment));
	}
	else if(ModuleDescriptor.Revision == 'B')
	{
		strcpy((char *)md->Mcu.Name, 							"ARM AT91SAM7S256");
		strcpy((char *)md->Mcu.Version.Manufacturer,		"L-Card Ltd.");
		strcpy((char *)md->Mcu.Version.Author,				"��������� �.�.");
		strncpy((char *)md->Mcu.Version.Comment,			"������� �������� ��� ������ E14-140 Rev.'B'", sizeof(md->Mcu.Version.Comment));
		strncpy((char *)md->Mcu.Comment, 					"32-bit ARM Thumb-based Microcontroller with 256K Bytes Programmable Flash", sizeof(md->Mcu.Comment));
	}
	else { LAST_ERROR_NUMBER(23); return FALSE; }
	// ������ ����������� � ������� ������ Firmware ����������������
	FirmwareVersionNumber = strtod((char *)md->Mcu.Version.Version, &pTag);

	// ADC Info
	md->Adc.Active = TRUE;
	strcpy((char *)md->Adc.Name, "LTC1416");
	// �������� ��� �������� i
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		md->Adc.OffsetCalibration[i] = ModuleDescriptor.AdcOffsetCoefs[i];
		md->Adc.ScaleCalibration[i] = ModuleDescriptor.AdcScaleCoefs[i];
	}
	strcpy((char *)md->Adc.Comment, "14-Bit 400Ksps ADC converter");

	// DAC Info
	if(ModuleDescriptor.IsDacPresented)
	{
		md->Dac.Active = TRUE;
		strcpy((char *)md->Dac.Name, "DAC8581");
		for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E140; i++)
		{
			md->Dac.OffsetCalibration[i] = ModuleDescriptor.DacOffsetCoefs[i];
			md->Dac.ScaleCalibration[i] = ModuleDescriptor.DacScaleCoefs[i];
		}
		strcpy((char *)md->Dac.Comment, "16-bit, High-Speed, Low-Noise DAC converter");
	}
	else md->Dac.Active = FALSE;

	// IO Info
	md->DigitalIo.Active = TRUE;
	strcpy((char *)md->DigitalIo.Name, "74HCT165/74HC595");
	md->DigitalIo.InLinesQuantity = 16;
	md->DigitalIo.OutLinesQuantity = 16;
	strcpy((char *)md->DigitalIo.Comment, "TTL/CMOS compatible");

	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ������� ��������� ���������� � ������ � ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E140 * const md)
{
	BYTE Buffer[128];
	WORD i;

	strcpy((char *)ModuleDescriptor.Name, 				"E140");
//	strncpy((char *)ModuleDescriptor.Name, 			(char *)md->Module.DeviceName,	std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.Name)));
	strncpy((char *)ModuleDescriptor.SerialNumber,	(char *)md->Module.SerialNumber, std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));

	// �������� ��������� ������� ������
	if((	md->Module.Revision > REVISIONS_E140[REVISIONS_QUANTITY_E140 - 0x1] &&
			md->Module.Revision <= 'Z')) { LAST_ERROR_NUMBER(23); return FALSE; }
	ModuleDescriptor.Revision = md->Module.Revision;

	ModuleDescriptor.IsDacPresented = (BYTE)(md->Dac.Active ? 0x1 : 0x0);
	ModuleDescriptor.ClockRate = 1000.0*md->Mcu.ClockRate + 0.5;		// � ��

	// ���������������� ������������ ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		// �������� ��� ��� �������� i
		if((fabs(md->Adc.OffsetCalibration[i]) > 8191.0) ||
			(fabs(md->Adc.OffsetCalibration[i]) < FloatEps)) ModuleDescriptor.AdcOffsetCoefs[i] = 0.0;
		else ModuleDescriptor.AdcOffsetCoefs[i] = md->Adc.OffsetCalibration[i];
		// ������� ��� ��� �������� i
		if((fabs(md->Adc.ScaleCalibration[i]) > 2.0) ||
			(fabs(md->Adc.ScaleCalibration[i]) < FloatEps)) ModuleDescriptor.AdcScaleCoefs[i] = 1.0;
		else ModuleDescriptor.AdcScaleCoefs[i] = md->Adc.ScaleCalibration[i];
	}
	// ���������������� ������������ ���
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		// �������� - ���� �� ��� �� �����
		if(md->Dac.Active)
		{
			// �������� ��� ������ i
			if((fabs(md->Dac.OffsetCalibration[i]) > 2047.0) ||
				(fabs(md->Dac.OffsetCalibration[i]) < FloatEps)) ModuleDescriptor.DacOffsetCoefs[i] = 0.0;
			else ModuleDescriptor.DacOffsetCoefs[i] = md->Dac.OffsetCalibration[i];
			// ������� ��� ������ i
			if((fabs(md->Dac.ScaleCalibration[i]) > 2.0) ||
				(fabs(md->Dac.ScaleCalibration[i]) < FloatEps)) ModuleDescriptor.DacScaleCoefs[i] = 1.0;
			else ModuleDescriptor.DacScaleCoefs[i] = md->Dac.ScaleCalibration[i];
		}
		else
		{
			// �������� ��� ������ i
			ModuleDescriptor.DacOffsetCoefs[i] = 0.0;
			// ������� ��� ������ i
			ModuleDescriptor.DacScaleCoefs[i] = 1.0;
		}
	}

	// �������� ���������
	if(!md) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ����������� ���������� ������
	else if(!PackModuleDescriptor(Buffer, &ModuleDescriptor)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �������� ��������� ������ � ����
	if(!ENABLE_FLASH_WRITE(TRUE)) { LAST_ERROR_NUMBER(21); return FALSE; }
	// ������� �������� ���������� � ����
	else if(!PutArrayToMcu(Buffer, sizeof(Buffer), DESCRIPTOR_BASE)) { LAST_ERROR_NUMBER(4); ENABLE_FLASH_WRITE(FALSE); return FALSE; }
	// �������� ��������� ������ � ����
	else if(!ENABLE_FLASH_WRITE(FALSE)) { LAST_ERROR_NUMBER(21); return FALSE; }
	// ��������� ���� ��������
	else if(!UnpackModuleDescriptor(Buffer, &ModuleDescriptor)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// �� ������
	else return TRUE;
}





// =============================================================================
//  ������� ��� ������ �� ��������� ����������� ����
// =============================================================================
// -----------------------------------------------------------------------------
// ������ ������� ������ �� ������ ����������������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::GetArray(BYTE * const Buffer, WORD Size, WORD Address)
{
	// � ������ ������� 'A' ��� ������ ������� � ����������������
	if(ModuleDescriptor.Revision != 'A')
	{
		// ������ ������ ������ �� ��������������� ������
		if(!GetArrayFromMcu(Buffer, Size, Address)) { return FALSE; }
		// ��� ������ :)))))
		else return TRUE;
	}
	else { LAST_ERROR_NUMBER(28); return FALSE; }
}

// -----------------------------------------------------------------------------
// ������ ������� ������ � ������ ����������������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE140::PutArray(BYTE * const Buffer, WORD Size, WORD Address)
{
	// � ������ ������� 'A' ��� ������ ������� � ����������������
	if(ModuleDescriptor.Revision != 'A')
	{
		// ������ ������ ������ �� ��������������� ������
		if(!PutArrayToMcu(Buffer, Size, Address)) { return FALSE; }
		// ��� ������ :)))))
		else return TRUE;
	}
	else { LAST_ERROR_NUMBER(28); return FALSE; }
}






// =============================================================================
//  ���������� ������� ��� ������ � �������
// =============================================================================
// -----------------------------------------------------------------------------
// ��������� ����������� ������� ����� ������ ��� �������� ���� USB � ����� suspend
// -----------------------------------------------------------------------------
BOOL TLE140::SetSuspendModeFlag(BOOL SuspendModeFlag)
{
	BYTE Flag = (BYTE)(SuspendModeFlag ? 0x1 : 0x0);

	if(!PutArrayToMcu(&Flag, 0x1, SUSPEND_MODE_FLAG)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ������� ������ �� ������ ����������������
// -----------------------------------------------------------------------------
BOOL TLE140::GetArrayFromMcu(BYTE * const Buffer, WORD Size, WORD Address)
{
	DWORD offs;

	// �������� ����� � ������
	if(!Buffer || !Size) { LAST_ERROR_NUMBER(1); return FALSE; }

	for(offs = 0x0; offs < (DWORD)Size; offs += MAX_USB_CONTROL_PIPE_BLOCK)
	{
		WORD InBuf[4] = {0x0001, V_GET_ARRAY, (WORD)(Address+offs), 0x0};

		if(!TLUSBBASE::LDeviceIoControl(
									DIOC_SEND_COMMAND,
									&InBuf, sizeof(InBuf),
									Buffer + offs, std::min(Size - offs, (DWORD)MAX_USB_CONTROL_PIPE_BLOCK),
									TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ������� ������ � ������ ����������������
// -----------------------------------------------------------------------------
BOOL TLE140::PutArrayToMcu(BYTE * const Buffer, WORD Size, WORD Address)
{
	DWORD offs;

	// �������� ����� � ������
	if(!Buffer || !Size) { LAST_ERROR_NUMBER(1); return FALSE; }

	for(offs = 0x0; offs < (DWORD)Size; offs += MAX_USB_CONTROL_PIPE_BLOCK)
	{
		WORD InBuf[4] = {0x0000, V_PUT_ARRAY, (WORD)(Address+offs), (WORD)0x0 };

		if(!TLUSBBASE::LDeviceIoControl(
									DIOC_SEND_COMMAND,
									&InBuf, sizeof(InBuf),
									Buffer + offs, std::min(Size - offs, (DWORD)MAX_USB_CONTROL_PIPE_BLOCK),
									TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	}

	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ������� ��8
// -----------------------------------------------------------------------------
#define CRC8INIT         0x55
BYTE TLE140::CRC8ADD(BYTE a, BYTE b)
{
	return (BYTE)(a + b);
}
//
BYTE TLE140::CRC8CALC(BYTE *Buffer, WORD Size)
{
	BYTE crc = CRC8INIT;
	while(Size--) crc = CRC8ADD(crc, *Buffer++);
	return crc;
}

// -----------------------------------------------------------------------------
//  �������� ����������� ������
// -----------------------------------------------------------------------------
BOOL TLE140::PackModuleDescriptor(BYTE * const Buffer, MODULE_DESCRIPTOR * const ModuleDescriptor)
{
	WORD i;
	BYTE c;
	DWORD k, n;

	// ����������� �������� ����� ������
	if(sscanf((char *)ModuleDescriptor->SerialNumber, "%u%c%lu", &k, &c, &n) != 0x3) { LAST_ERROR_NUMBER(22); return FALSE; }
	// ������� �����
	ZeroMemory(Buffer, 128);
	// �������� ����� �������
	Buffer[0] = (BYTE)k;
	Buffer[1] = c;
	*(DWORD*)(Buffer+2) = n;
	// �������� �������
	sprintf((char *)(Buffer+6), "%.10s", ModuleDescriptor->Name);
	// ������� �������
	Buffer[16] = (BYTE)(ModuleDescriptor->Revision + (((ModuleDescriptor->Revision >= 'a') && (ModuleDescriptor->Revision <= 'z')) ? ('A'-'a') : 0));
	// ��� �������������� ����������
	sprintf((char *)(Buffer+17), "%.10s", ModuleDescriptor->CpuType);
	// ������� ������ ����������
	*(DWORD*)(Buffer+27) = ModuleDescriptor->ClockRate;
	// ���� ����������� ���
	Buffer[32] = (BYTE)(ModuleDescriptor->IsDacPresented ? 0x1 : 0x0);
	// �������� ������������� ��������� ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		*(float*)(Buffer + 33 + (i + 0)*sizeof(float)) = (float)ModuleDescriptor->AdcOffsetCoefs[i];
		*(float*)(Buffer + 33 + (i + ADC_CALIBR_COEFS_QUANTITY_E140)*sizeof(float)) = (float)ModuleDescriptor->AdcScaleCoefs[i];
	}
	// �������� ������������� ��������� ���
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		*(float*)(Buffer + 65 + (i + 0)*sizeof(float)) = (float)ModuleDescriptor->DacOffsetCoefs[i];
		*(float*)(Buffer + 65 + (i + DAC_CALIBR_COEFS_QUANTITY_E140)*sizeof(float)) = (float)ModuleDescriptor->DacScaleCoefs[i];
	}
	// ����������� ����� ������ ��������� �������
	Buffer[31]  = CRC8CALC(Buffer+0, 31);
	// ����������� ����� ������ ��������� �������
	Buffer[127] = CRC8CALC(Buffer+32, 95);
	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ���������� ����������� ������
// -----------------------------------------------------------------------------
BOOL TLE140::UnpackModuleDescriptor(BYTE * const Buffer, MODULE_DESCRIPTOR * const ModuleDescriptor)
{
	WORD i;

	// �������� ����������� �����
	if(CRC8CALC(Buffer+0, 31)  != Buffer[31]) { LAST_ERROR_NUMBER(24); return FALSE; }
	else if(CRC8CALC(Buffer+32, 95) != Buffer[127]) { LAST_ERROR_NUMBER(24); return FALSE; }
	// �������� ����� �������
	sprintf((char *)(ModuleDescriptor->SerialNumber), "%u%c%lu", (DWORD)Buffer[0], Buffer[1], (*(DWORD*)(Buffer+2)) & 0xFFFFFFL);
	// �������� �������
	sprintf((char *)(ModuleDescriptor->Name), "%.10s", Buffer+6);
	// ������� �������
	ModuleDescriptor->Revision = Buffer[16];
	// ��� �������������� ����������
	sprintf((char *)(ModuleDescriptor->CpuType), "%.10s", Buffer+17);
	// ������� ������ ����������
	ModuleDescriptor->ClockRate = *(DWORD*)(Buffer+27);
	// ���� ����������� ��� �� ����� ������
	ModuleDescriptor->IsDacPresented = Buffer[32] ? true : false;
	// ���������� ������������� ��������� ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		ModuleDescriptor->AdcOffsetCoefs[i]	= *(float*)(Buffer + 33 + (i + 0)*sizeof(float));
		ModuleDescriptor->AdcScaleCoefs[i]	= *(float*)(Buffer + 33 + (i + ADC_CALIBR_COEFS_QUANTITY_E140)*sizeof(float));
	}
	// ���������� ������������� ��������� ���
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E140; i++)
	{
		ModuleDescriptor->DacOffsetCoefs[i]	= (*(float*)(Buffer + 65 + (i + 0)*sizeof(float)));
		ModuleDescriptor->DacScaleCoefs[i]	= (*(float*)(Buffer + 65 + (i + DAC_CALIBR_COEFS_QUANTITY_E140)*sizeof(float)));
	}
	// �� ������
	return TRUE;
}


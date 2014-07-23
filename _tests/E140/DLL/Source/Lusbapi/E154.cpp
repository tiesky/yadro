//------------------------------------------------------------------------------
#include <algorithm>
#include <stdio.h>
#include "e154.h"
#include "ErrorBaseIds.h"
//------------------------------------------------------------------------------

#define 	LAST_ERROR_NUMBER(ErrorNumber)	LastErrorNumber = E154_BASE_ERROR_ID + ErrorNumber

// -----------------------------------------------------------------------------
//  ����������
// ----------------------------------------------------------------------------
TLE154::TLE154(HINSTANCE hInst) : TLUSBBASE(hInst)
{
	// ������������� ���� ��������� �������� ������
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));
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
TLE154::~TLE154() { }



// =============================================================================
// ������� ������ ���������� ��� ������ � ������� �14-154
// =============================================================================
// -----------------------------------------------------------------------------
//  ������� ����������� ���� ��� ������� � USB ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::OpenLDevice (WORD VirtualSlot)
{
	char ModuleName[LONG_MODULE_NAME_STRING_LENGTH];

	//  ��������� ������� ����������� ���� ��� ������� � USB ������
	if(!TLUSBBASE::OpenLDeviceByID(VirtualSlot, E154_ID)) { return FALSE; }
	// ��������� ��������� �������� ������
	else if(!TLUSBBASE::GetModuleName(ModuleName)) { TLUSBBASE::CloseLDevice(); return FALSE; }
	// ��������, ��� ��� ������ E-154
	else if(strcmp(ModuleName, "E154")) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(0); return FALSE; }
	// �� ������
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ������� ����������� ����
//------------------------------------------------------------------------------------
BOOL WINAPI TLE154::CloseLDevice(void)
{
	// ������� ����������
	if(!TLUSBBASE::CloseLDevice()) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------------
// ��������� ��������� �� ����������
//------------------------------------------------------------------------------------
BOOL WINAPI TLE154::ReleaseLInstance(void)
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
HANDLE WINAPI TLE154::GetModuleHandle(void) { return hDevice; }

//------------------------------------------------------------------------------
//  ������ �������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE154::GetModuleName(PCHAR const ModuleName)
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
BOOL WINAPI TLE154::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// �������� ���������
	if(!UsbSpeed) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ������ ��������� ������ �������� ������ USB ����
	else if(!TLUSBBASE::GetUsbSpeed(UsbSpeed)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ������� ������� ������������������ ������ E-154
// �� �.�. ������ ����� �� �������������� �������, �� ������ ���������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE154::LowPowerMode(BOOL LowPowerFlag)
{
	if(!TLUSBBASE::LowPowerMode(LowPowerFlag)) return FALSE;
   else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ������ � ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE154::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	return TLUSBBASE::GetLastErrorInfo(LastErrorInfo);
}




// =============================================================================
// ������� ��� ������ � ��� ������
// =============================================================================
// -----------------------------------------------------------------------------
// ��������� ������� ���������� ������ ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::GET_ADC_PARS(ADC_PARS_E154 * const AdcPars)
{
	BYTE b, smode;
	BYTE buf[26];
	WORD i;
	DWORD FREQ;
   DWORD ScaleDivs[5]={1, 4, 16, 64, 512};

	// �������� ���������
	if(!AdcPars) { LAST_ERROR_NUMBER(1); return FALSE; }
	// �������� ������� ��������� ������ ���
	if(!GetArray(buf, sizeof(buf), ADC_PARS_BASE)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������� ���������� �������
	for(i = 0x0; i < MAX_CONTROL_TABLE_LENGTH_E154; i++)
	{
		b = buf[i];
		this->AdcPars.ControlTable[i] = b;
	}
   RateScale=buf[24];
	FREQ = 24000000L/ScaleDivs[RateScale];
	// ������� ������ ���(���)
	this->AdcPars.AdcRate = (double)FREQ/1000.0/(*(WORD*)(buf+16));
	// ����� �������� ���������� �������
	this->AdcPars.ChannelsQuantity = buf[18];
	// ����������� �������� (��)
	this->AdcPars.InterKadrDelay = (double)((WORD)(buf[19]|(buf[25] << 8)) + 1.0)/this->AdcPars.AdcRate;
	// ������� ������ (���)
	this->AdcPars.KadrRate = (double)this->AdcPars.AdcRate/((WORD)buf[18] + (WORD)(buf[19]|(buf[25] << 8)));
	// ��� �������������
	smode = (BYTE)(buf[20] & 0x0F);
	if(smode == 0x1)      	this->AdcPars.InputMode = 2;
	else if(smode == 0x2) 	this->AdcPars.InputMode = 1;
	else if(smode > 0x2) 	this->AdcPars.InputMode = 3;
	else              		this->AdcPars.InputMode = 0;
	this->AdcPars.SynchroAdType = (BYTE)((smode == 0x5 || smode == 0x6) ? 0x1 : 0x0);
	this->AdcPars.SynchroAdMode = (BYTE)((smode == 0x4 || smode == 0x6) ? 0x1 : 0x0);
	// �������� �������� ���������
	this->AdcPars.ClkSource = (BYTE)((buf[20] >> 0x6) & 0x1);
	// ���� ���������� ���������� ����������� �������� ��������� ���
	this->AdcPars.EnableClkOutput = (BYTE)((buf[20] >> 0x7) & 0x1);
	// ����� ����������� ������ ��� ���������� �������������
	b = buf[21];
	this->AdcPars.SynchroAdChannel = b;
	// ����� ������������ ��� ���������� �������������
	this->AdcPars.SynchroAdPorog = *(SHORT*)(buf+22);
	//
	*AdcPars = this->AdcPars;
	// ��� ������ :)))))
   AdcConfigStruct=*AdcPars;
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ���������� ��������� ���������� ������ ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::SET_ADC_PARS(ADC_PARS_E154 * const AdcPars)
{
	BYTE b, smode;
	BYTE buf[26];
	DWORD i;
	DWORD FREQ;
   DWORD ScaleDivs[5]={1, 4, 16, 64, 512};
   double rate, kadr;
   unsigned int n, rate0, arm_rate, arm_kadr, cnt;


	// �������� ���������
	if(!AdcPars) { LAST_ERROR_NUMBER(1); return FALSE; }
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ��� ������
	// �������������� ���������� ������� ������� STOP_ADC()
	else if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }

	// ������� �����
	ZeroMemory(buf, sizeof(buf));
	// ������� ���������� �������
	for(i = 0x0; i < MAX_CONTROL_TABLE_LENGTH_E154; i++)
	{
		b = AdcPars->ControlTable[i];
		buf[i] = b;
	}

	// ����� �������� ���������� �������
	if(!AdcPars->ChannelsQuantity) { LAST_ERROR_NUMBER(26); return FALSE; }
	else if(AdcPars->ChannelsQuantity > MAX_CONTROL_TABLE_LENGTH_E154) AdcPars->ChannelsQuantity = MAX_CONTROL_TABLE_LENGTH_E154;
	buf[18] = (BYTE)AdcPars->ChannelsQuantity;

	AdcPars->AdcRate = fabs(AdcPars->AdcRate);
   if(AdcPars->AdcRate > 120) AdcPars->AdcRate=120;
   else if(AdcPars->AdcRate < 0.005) AdcPars->AdcRate=0.005;
   for(RateScale=0; RateScale < 5; RateScale++)
	{
   	// ��������� � �������� ���
		FREQ = 24000000L/ScaleDivs[RateScale];

		// ������� ������ ��� � ���
		i = (double)FREQ/1000/AdcPars->AdcRate + 0.5;
		if(i > 65530) i = 65530;
			else if(i < 10) i = 10;
		rate = (double)FREQ/1000/i;
		*(WORD*)(buf+16) = (WORD)i;
      arm_rate=i;

		// ����������� �������� (��)
		i = (DWORD)(rate*AdcPars->InterKadrDelay + 0.5);
		if(i == 0) i = 1;
		else if(i > 65530) i = 65530;
		buf[19] = (BYTE)(i - 0x1);
		buf[25] = (BYTE)((i - 0x1)>> 8);
		kadr=i/rate;
      arm_kadr=buf[19] | (buf[25] << 8);

      rate0=arm_rate*AdcPars->ChannelsQuantity;;
      n=arm_kadr*arm_rate;
      cnt=rate0+2+n;
      if(cnt <= 65534) break;
	}

   if(RateScale >= 5)
	{
      RateScale=4;
   	// ��������� � �������� ���
		FREQ = 24000000L/ScaleDivs[RateScale];

		// ������� ������ ��� � ���
		i = (double)FREQ/1000/AdcPars->AdcRate + 0.5;
		if(i > 65530) i = 65530;
		else if(i < 10) i = 10;
		rate = (double)FREQ/1000/i;
		*(WORD*)(buf+16) = (WORD)i;
      arm_rate=i;

		// ����������� �������� (��)
      arm_kadr=(65534-arm_rate*AdcPars->ChannelsQuantity-2)/arm_rate;
      if(arm_kadr > 65535) arm_kadr = 0x1;

		buf[19] = (BYTE)arm_kadr;
      buf[25]== (BYTE)(arm_kadr >> 8);
		kadr=(arm_kadr+1)/AdcPars->AdcRate;
	}

   AdcPars->AdcRate=rate;
   AdcPars->InterKadrDelay=kadr;

	// ������� ������ (���)
	AdcPars->KadrRate = (double)AdcPars->AdcRate/((WORD)buf[18] + (WORD)(buf[19] | (buf[25] << 8)));

    // ��� �������������
	if(AdcPars->InputMode > ANALOG_SYNC_E154) AdcPars->InputMode = ANALOG_SYNC_E154;
	smode = AdcPars->InputMode;
	if(smode == 1) smode=2;
	else if(smode == 2) smode = (BYTE)(0x1);
	else if(smode == 3)
	{
		if(AdcPars->SynchroAdType) smode += (BYTE)(0x2);
		if(AdcPars->SynchroAdMode) smode += (BYTE)(0x1);
	}
	buf[20] = smode;
	// �������� �������� ���������
	if(AdcPars->ClkSource) buf[20] |= (0x1 << 0x6);
	// ���� ���������� ���������� ����������� �������� ��������� ���
	if(AdcPars->EnableClkOutput) buf[20] |= (0x1 << 0x7);
	// ����� ����������� ������ ��� ���������� �������������
	b = AdcPars->SynchroAdChannel;
	buf[21] = b;
	// ����� ������������ ��� ���������� �������������
	*(SHORT*)(buf+22) = AdcPars->SynchroAdPorog;
   buf[24]=RateScale;
	// �������� ������� ��������� ������ ���
	this->AdcPars=*AdcPars;
	// ������� ��������� ���
	if(!PutArray(buf, sizeof(buf), ADC_PARS_BASE)) { LAST_ERROR_NUMBER(4); return FALSE; }
   else AdcConfigStruct = *AdcPars;
	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
// �������������� ����� ��� � ������ � ������ ����������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::ProcessArray(SHORT *src, double *dest, DWORD size, BOOL calibr, BOOL volt)
{
	double K_offset[MAX_CONTROL_TABLE_LENGTH_E154], K_scale[MAX_CONTROL_TABLE_LENGTH_E154];;
	DWORD i, j, nch, gain_index;
	MODULE_DESCRIPTION_E154 md;
	double ScaleGain[4]={5.0/2000., 1.6/2000., 0.5/2000., 0.16/2000.};

	nch=AdcConfigStruct.ChannelsQuantity;
   if(!nch) { LAST_ERROR_NUMBER(5); return FALSE; }

   if(calibr == TRUE && !DescriptorReadFlag)
   	{
      // ���� �� ���� ������� ������������� ������������, �� ������� ��
      if(TLE154::GET_MODULE_DESCRIPTION(&md) == FALSE) { LAST_ERROR_NUMBER(5); return FALSE; }
      }

   // ���������� ������������
   for(i=0; i < nch; i++)
   	{
      gain_index=(AdcConfigStruct.ControlTable[i] >> 6) & 3;
      if(calibr == FALSE)
      	{
         // ��� ����������
         if(volt == FALSE)
         	{
            // ���� ���, ��� ����������
            K_offset[i]=0;
            K_scale[i]=1.0;
            }
           else
           	{
            // ������, ��� ����������
            K_offset[i]=0;
            K_scale[i]=ScaleGain[gain_index];
            }
         }
        else
      	{
         // � �����������
         if(volt == FALSE)
         	{
            // ���� ���, � �����������
            K_offset[i]=E154DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale[i]=E154DescriptionStruct.Adc.ScaleCalibration[gain_index];
            }
           else
           	{
            // ������, � �����������
            K_offset[i]=E154DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale[i]=E154DescriptionStruct.Adc.ScaleCalibration[gain_index]*ScaleGain[gain_index];
            }
         }
      }

   for(i=j=0; i < size; i++)
   	{
      *dest++=((*src++)+K_offset[j])*K_scale[j];
      j++;
      if(j >= nch) j=0;
      }

	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
// �������������� ������ ���� ��� � ������ � ������ ����������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::ProcessOnePoint(SHORT source, double *dest, DWORD channel, BOOL calibr, BOOL volt)
{
	double K_offset, K_scale;
	int gain_index;
	MODULE_DESCRIPTION_E154 md;
	double ScaleGain[4]={5.0/2000., 1.6/2000., 0.5/2000., 0.16/2000.};

   if(calibr == TRUE && !DescriptorReadFlag)
  	{
      // ���� �� ���� ������� ������������� ������������, �� ������� ��
      if(TLE154::GET_MODULE_DESCRIPTION(&md) == FALSE) { LAST_ERROR_NUMBER(5); return FALSE; }
	}

   // ���������� ������������
	gain_index=(channel >> 6) & 3;
	if(calibr == FALSE)
	{
		// ��� ����������
		if(volt == FALSE)
         	{
            // ���� ���, ��� ����������
            K_offset=0;
            K_scale=1.0;
            }
           else
           	{
            // ������, ��� ����������
            K_offset=0;
            K_scale=ScaleGain[gain_index];
            }
         }
        else
      	{
         // � �����������
         if(volt == FALSE)
         	{
            // ���� ���, � �����������
            K_offset=E154DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale=E154DescriptionStruct.Adc.ScaleCalibration[gain_index];
            }
           else
           	{
            // ������, � �����������
            K_offset=E154DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale=E154DescriptionStruct.Adc.ScaleCalibration[gain_index]*ScaleGain[gain_index];
            }
         }
    //  }

   *dest=(source+K_offset)*K_scale;

	// ��� ������ :)))))
	return TRUE;
}


// -----------------------------------------------------------------------------
//  ����������� ���� � ��������� ����������� ������ ��� ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::FIFO_STATUS(DWORD *FifoOverflowFlag, double *FifoMaxPercentLoad, DWORD *FifoSize, DWORD *MaxFifoBytesUsed)
{
	BYTE temp[5];

	if(!GetArray((BYTE*)temp, 0x5, FIFO_STATUS_ADDRESS)) { LAST_ERROR_NUMBER(2); return FALSE; }

   *FifoOverflowFlag=temp[0];
   *FifoSize=temp[3] | (temp[4] << 8);
   *MaxFifoBytesUsed=temp[1] | (temp[2] << 8);

   *FifoMaxPercentLoad=((double)(*MaxFifoBytesUsed))*100./((double)(*FifoSize));

	// �� ������
	return TRUE;
}


// -----------------------------------------------------------------------------
// ������ ������ ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::START_ADC(void)
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
BOOL WINAPI TLE154::STOP_ADC(void)
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
BOOL WINAPI TLE154::ADC_KADR(SHORT * const Data)
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
	else if(AdcPars.ChannelsQuantity > 16) { LAST_ERROR_NUMBER(6); return FALSE; }
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ������ �������
	// �������������� ���������� ������� ������� STOP_ADC()
	else if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ��������� ��������� ������� STOP_ADC()
	else if(!STOP_ADC()) { LAST_ERROR_NUMBER(25); return FALSE; }
	// ���������� ����������� �����
	// �������� ������ �� ����������� ������ ���
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ��������� ��������� ������� ���� IO_REQUEST_LUSBAPI �� ���� ������
   PointsToRead = 32;
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
   CopyMemory(Data, &IoReq.Buffer[0], AdcPars.ChannelsQuantity*sizeof(SHORT));
	// ����������� ������
	if(IoReq.Buffer) delete [] IoReq.Buffer;
	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ����������� ���� � ��������� ����������� ������ ��� ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::ADC_SAMPLE(SHORT * const Data, WORD Channel)
{
	WORD b = (WORD)Channel ;

	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ������ �������
	// �������������� ���������� ������� ������� STOP_ADC()
	if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ���������� ����� ������
	else if(!PutArray((BYTE*)&b, 0x2, ADC_CHANNEL_SELECT_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ������ �����
	else if(!GetArray((BYTE*)Data, 0x2, ADC_DATA_REGISTER)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//  ��������� ������ ������ � ��� ������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::ReadData(IO_REQUEST_LUSBAPI * const ReadRequest)
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
// -----------------------------------------------------------------------------
//  ����������� ����� �� �������� ����� ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::DAC_SAMPLE(SHORT * const DacData, WORD DacChannel)
{
	SHORT DacParam;

	// ���������� �������� ��� ������ �� ���
	if((*DacData) < -128) *DacData = -128;
	else if((*DacData) > 127) *DacData = 127;
	// ��������� ��������������� ����� ��� �������� � AVR
	DacParam = (SHORT)((*DacData));
	// ������� � ������
	if(!PutArray((BYTE*)&DacParam, 0x2, DAC_DATA_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//  ����������� ����� �� �������� ����� ���
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::DAC_SAMPLE_VOLT(double  const DoubleDacData, BOOL calibr)
{
	double K_offset, K_scale, f;
	MODULE_DESCRIPTION_E154 md;
	double ScaleGain=128./5.4;
	SHORT  DacData;

   if(calibr == TRUE && !DescriptorReadFlag)
   	{
      // ���� �� ���� ������� ������������� ������������, �� ������� ��
      if(TLE154::GET_MODULE_DESCRIPTION(&md) == FALSE) { LAST_ERROR_NUMBER(5); return FALSE; }
      }

   // ���������� ������������
      if(calibr == FALSE)
      	{
         // ������, ��� ����������
  			f=DoubleDacData*ScaleGain;
         }
        else
      	{
         // ������, � �����������
         K_offset=E154DescriptionStruct.Dac.OffsetCalibration[0];
         K_scale=E154DescriptionStruct.Dac.ScaleCalibration[0];
		   f=DoubleDacData*128./5.0;
		   f=(f+K_offset)*K_scale;
         }

	DacData=(f > 0) ? f+0.5 : f-0.5;

	// ���������� �������� ��� ������ �� ���
	if(DacData < -128) DacData = -128;
	else if(DacData > 127) DacData = 127;
	// ������� � ������
	if(!PutArray((BYTE*)&DacData, 0x2, DAC_DATA_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}


// =============================================================================
//  ������� ��� ������ � ��� ������� �� ������� �������� �������
// =============================================================================
// -----------------------------------------------------------------------------
//  ������� ���������� �������� ����� �������� ��������� �������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::ENABLE_TTL_OUT(BOOL EnableTtlOut)
{
	BYTE Flag = (BYTE)(EnableTtlOut ? 0x1 : 0x0);

	// ����� ���� ����������� �������� �������
	if(!PutArray(&Flag, 0x1, DOUT_ENABLE_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//   ������� ������ ������� ����� �������� ��������� �������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::TTL_IN(WORD * const TtlIn)
{
	// ������ ��������� �������� ������
	if(!GetArray((BYTE *)TtlIn, 0x2, DIN_REGISTER)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
//  ������� ������ �� �������� ����� �������� ��������� �������
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::TTL_OUT(WORD TtlOut)
{
	// ����� ��������� �������� �������
	if(!PutArray((BYTE*)&TtlOut, 0x2, DOUT_REGISTER)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}




// =============================================================================
//  ������� ��� ������ � ���������������� ����������� ����
// =============================================================================
// -----------------------------------------------------------------------------
// ����������/���������� ������ ������ � ���������������� ������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled)
{
	BYTE Flag = (BYTE)(IsUserFlashWriteEnabled ? 0x1 : 0x0);

	// ��������� ������� ����������/���������� ������ � ����
	if(!PutArray(&Flag, 0x1, ENABLE_FLASH_WRITE_FLAG)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� �������
	TLE154::IsUserFlashWriteEnabled = IsUserFlashWriteEnabled;
	// �� ������
	return TRUE;
}

// -----------------------------------------------------------------------------
// ������� ����� �� ���������������� ������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::READ_FLASH_ARRAY(BYTE * const UserFlash)
{
	// �������� ��������� ����� �� ����
	if(!GetArray((BYTE*)UserFlash, 128, (WORD)(DATA_FLASH_BASE))) { LAST_ERROR_NUMBER(2); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ����� � ���������������� ������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::WRITE_FLASH_ARRAY(BYTE * const UserFlash)
{
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ������ ���������� � ���������������� ����
	// �������������� ���������� ������� ������� STOP_ADC()
	if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������ ������ ���� ���������
	else if(!IsUserFlashWriteEnabled) { LAST_ERROR_NUMBER(18); return FALSE; }
 	else if(!PutArray((BYTE*)UserFlash, 128, (WORD)(DATA_FLASH_BASE ))) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}





// =============================================================================
//  ������� ��� ������ �� ��������� ����������� ����
// =============================================================================
// -----------------------------------------------------------------------------
//  ������� ��������� ���������� � ������ �� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE154::GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E154 * const md)
{
	char *pTag;
	BYTE Buffer[128];
	BYTE Str[17];
	WORD i;
   WORD error;

   error=0;
	// �������� ���������
	if(!md) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ��������� ����������� ���������� ������ �� ����
	else if(!GetArray(Buffer, sizeof(Buffer), DESCRIPTOR_BASE)) { LAST_ERROR_NUMBER(2); return FALSE; }
	// ������������� ���������� ������
	else if(!UnpackModuleDescriptor(Buffer, &ModuleDescriptor)) { LAST_ERROR_NUMBER(19); error=1; }

	// Module Info
	strncpy((char *)md->Module.CompanyName,  			"L-Card Ltd.",    								sizeof(md->Module.CompanyName));
	if(!strcmp((char *)ModuleDescriptor.Name, 		"E-154")) strcpy((char *)md->Module.DeviceName, "E-154");
	else strncpy((char *)md->Module.DeviceName,		(char *)ModuleDescriptor.Name,	 		std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.Name)));
	strncpy((char *)md->Module.SerialNumber, 			(char *)ModuleDescriptor.SerialNumber,	std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
	strcpy((char *)md->Module.Comment,      			"������� ������ ���/���/��� ������ ���������� �� ���� USB 1.1");
	// ������� ������� ������
	md->Module.Revision = ModuleDescriptor.Revision;
	// ������ E14-140 ���������� � ������������ ���������� (�� ������ ���� ����)
	md->Module.Modification = NO_MODULE_MODIFICATION_LUSBAPI;

	// Interface Info
	md->Interface.Active = TRUE;
	strcpy((char *)md->Interface.Name, "USB 1.1");
	strcpy((char *)md->Interface.Comment, "Full-Speed Mode (12 Mbit/s)");

	// MCU Info
	if(!GetArray(Buffer, 0x10, LBIOS_VERSION)) { LAST_ERROR_NUMBER(2); return FALSE; }
	md->Mcu.Active = TRUE;
	strcpy((char *)md->Mcu.Name, "AT91SAM7S64");
	sprintf((char *)Str, "%1u.%1u", Buffer[0], Buffer[1]);
	strncpy((char *)md->Mcu.Version.Version,  		(char *)Str, 		std::min(sizeof(md->Mcu.Version.Version), sizeof(Str)));
	strncpy((char *)md->Mcu.Version.Date,  			(char *)"", 14);
	strcpy((char *)md->Mcu.Version.Manufacturer,  	"L-Card Ltd.");
	strcpy((char *)md->Mcu.Version.Author,  			"Syachinov");
	strncpy((char *)md->Mcu.Version.Comment,			"������� �������� ��� ������ E-154", sizeof(md->Mcu.Version.Comment));
	strncpy((char *)md->Mcu.Comment, 					"32-bit Microcontroller with 64K Bytes In-System Programmable Flash", sizeof(md->Mcu.Comment));
	md->Mcu.ClockRate = 24000;		// � ���
	// ����������� � ������� ������ Firmware ����������������
	FirmwareVersionNumber = strtod((char *)md->Mcu.Version.Version, &pTag);

	// ADC Info
	md->Adc.Active = TRUE;
	strcpy((char *)md->Adc.Name, "AD7895");
	// �������� ��� �������� i
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E154; i++)
	{
	   md->Adc.OffsetCalibration[i] = ModuleDescriptor.AdcOffsetCoefs[i];
	   md->Adc.ScaleCalibration[i] = ModuleDescriptor.AdcScaleCoefs[i];
	}
	strcpy((char *)md->Adc.Comment, "12-Bit 120 Ksps ADC converter");

	// DAC Info
	if(ModuleDescriptor.IsDacPresented)
	{
		md->Dac.Active = TRUE;
		strcpy((char *)md->Dac.Name, "PWM");
		for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E154; i++)
		{
			md->Dac.OffsetCalibration[i] = ModuleDescriptor.DacOffsetCoefs[i];
			md->Dac.ScaleCalibration[i] = ModuleDescriptor.DacScaleCoefs[i];
		}
		strcpy((char *)md->Dac.Comment, "8-Bit DAC converter");
	}
	else md->Dac.Active = FALSE;

	// IO Info
	md->DigitalIo.Active = TRUE;
	strcpy((char *)md->DigitalIo.Name, "74HC165/74HC574");
	md->DigitalIo.InLinesQuantity = 8;
	md->DigitalIo.OutLinesQuantity = 8;
	strcpy((char *)md->DigitalIo.Comment, "TTL/CMOS compatible");

	// �� ������
   if(!error)
   	{
      DescriptorReadFlag=TRUE;
      E154DescriptionStruct=*md;
      }

	return (!error) ? TRUE : FALSE;
}
// -----------------------------------------------------------------------------
//  ������� ��������� ���������� � ������ � ����
// -----------------------------------------------------------------------------
BOOL WINAPI WINAPI TLE154::SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E154 * const md)
{
	BYTE Buffer[128];
	WORD i;

	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ������ �������
	// �������������� ���������� ������� ������� STOP_ADC()
	if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(3); return FALSE; }

   DescriptorReadFlag=FALSE;
	strcpy((char *)ModuleDescriptor.Name, 				"E-154");
	strncpy((char *)ModuleDescriptor.SerialNumber,	(char *)md->Module.SerialNumber, std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));

	// �������� ��������� ������� ������
	if((	md->Module.Revision > REVISIONS_E154[REVISIONS_QUANTITY_E154 - 0x1] &&
			md->Module.Revision <= 'Z')) { LAST_ERROR_NUMBER(23); return FALSE; }
	ModuleDescriptor.Revision = md->Module.Revision;

	ModuleDescriptor.IsDacPresented = (BYTE)0x1;
	ModuleDescriptor.QuartzFrequency = 1000.0*md->Mcu.ClockRate + 0.5;		// � ��

	// ���������������� ������������ ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E154; i++)
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
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E154; i++)
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

	// �������� ���������
	if(!md) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ����������� ���������� ������
	else if(!PackModuleDescriptor(Buffer, &ModuleDescriptor)) { LAST_ERROR_NUMBER(20); return FALSE; }
	// �������� ��������� ������ � ����
	if(!ENABLE_FLASH_WRITE(TRUE)) { LAST_ERROR_NUMBER(21); return FALSE; }
	// ������� �������� ���������� � ����
	else if(!PutArray(Buffer, sizeof(Buffer), DESCRIPTOR_BASE)) { LAST_ERROR_NUMBER(4); ENABLE_FLASH_WRITE(FALSE); return FALSE; }
	// �������� ��������� ������ � ����
	else if(!ENABLE_FLASH_WRITE(FALSE)) { LAST_ERROR_NUMBER(21); return FALSE; }
	// ��������� ���� ��������
	else if(!UnpackModuleDescriptor(Buffer, &ModuleDescriptor)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// �� ������
	else
   	{
      return TRUE;
      }
}





// =============================================================================
//  ���������� ������� ��� ������ � �������
// =============================================================================
// -----------------------------------------------------------------------------
// ��������� ����������� ������� ����� ������ ��� �������� ���� USB � ����� suspend
// -----------------------------------------------------------------------------
BOOL TLE154::SetSuspendModeFlag(BOOL SuspendModeFlag)
{
	BYTE Flag = (BYTE)(SuspendModeFlag ? 0x1 : 0x0);

	if(!PutArray(&Flag, 0x1, SUSPEND_MODE_FLAG)) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �� ������
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ������� ������ �� ������ ����������������
// -----------------------------------------------------------------------------
BOOL WINAPI WINAPI TLE154::GetArray(BYTE * const Buffer, WORD Size, WORD Address)
{
	DWORD i;
	WORD InBuf[4] = { 0x0001, V_GET_ARRAY, Address, 0x0 };

	// �������� ����� � ������
	if(!Buffer || !Size) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ������ ������
	for(i = 0x0; i < (DWORD)Size; i += MAX_USB_CONTROL_PIPE_BLOCK)
	{
		InBuf[0x2] = (WORD)(Address + i);
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
                            &InBuf, sizeof(InBuf),
                            Buffer + i, std::min( Size - i, (DWORD)MAX_USB_CONTROL_PIPE_BLOCK),
                            TimeOut))  { LAST_ERROR_NUMBER(5); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ������� ������ � ������ ����������������
// -----------------------------------------------------------------------------
BOOL WINAPI WINAPI TLE154::PutArray(BYTE * const Buffer, WORD Size, WORD Address)
{
	DWORD i;
	WORD InBuf[4] = { 0x0000, V_PUT_ARRAY, Address, 0x0 };

	// �������� ����� � ������
	if(!Buffer || !Size) { LAST_ERROR_NUMBER(1); return FALSE; }
	// ����� ������
	for(i = 0x0; i < (DWORD)Size; i += MAX_USB_CONTROL_PIPE_BLOCK)
	{
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND,
                            &InBuf, sizeof(InBuf),
                            Buffer + i, std::min(Size - i, (DWORD)MAX_USB_CONTROL_PIPE_BLOCK),
                            TimeOut)) { LAST_ERROR_NUMBER(5); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
//  ������� ��8
// -----------------------------------------------------------------------------
#define CRC8INIT         0x55
BYTE TLE154::CRC8ADD(BYTE a, BYTE b)
{
	return (BYTE)(a + b);
}
//
BYTE TLE154::CRC8CALC(BYTE *Buffer, WORD Size)
{
	BYTE crc = CRC8INIT;
	while(Size--) crc = CRC8ADD(crc, *Buffer++);
	return crc;
}

// -----------------------------------------------------------------------------
//  �������� ����������� ������
// -----------------------------------------------------------------------------
BOOL TLE154::PackModuleDescriptor(BYTE * const Buffer, MODULE_DESCRIPTOR * const ModuleDescriptor)
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
	*(DWORD*)(Buffer+27) = ModuleDescriptor->QuartzFrequency;
	// ���� ����������� ���
	Buffer[32] = (BYTE)(ModuleDescriptor->IsDacPresented ? 0x1 : 0x0);
	// �������� ������������� ��������� ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E154; i++)
	{
		*(float*)(Buffer + 33 + (i + 0)*sizeof(float)) = (float)ModuleDescriptor->AdcOffsetCoefs[i];
		*(float*)(Buffer + 33 + (i + ADC_CALIBR_COEFS_QUANTITY_E154)*sizeof(float)) = (float)ModuleDescriptor->AdcScaleCoefs[i];
	}
	// �������� ������������� ��������� ���
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E154; i++)
	{
		*(float*)(Buffer + 65 + (i + 0)*sizeof(float)) = (float)ModuleDescriptor->DacOffsetCoefs[i];
		*(float*)(Buffer + 65 + (i + DAC_CALIBR_COEFS_QUANTITY_E154)*sizeof(float)) = (float)ModuleDescriptor->DacScaleCoefs[i];
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
BOOL TLE154::UnpackModuleDescriptor(BYTE * const Buffer, MODULE_DESCRIPTOR * const ModuleDescriptor)
{
	WORD i;
   WORD error;

   error=0;
	// �������� ����������� �����
	if(CRC8CALC(Buffer+0, 31)  != Buffer[31])
   	{
	// �������� ����� �������
	sprintf((char *)(ModuleDescriptor->SerialNumber), "%u%c%lu", (DWORD)0, 'L', (DWORD)0);
	// �������� �������
	sprintf((char *)(ModuleDescriptor->Name), "%.10s", "E-154");
	// ������� �������
	ModuleDescriptor->Revision = 0;
	// ��� �������������� ����������
	sprintf((char *)(ModuleDescriptor->CpuType), "%.10s", "AT91SAM7S64");
	// ������� ������ ����������
	ModuleDescriptor->QuartzFrequency = 24000;
	// ���� ����������� ��� �� ����� ������
	ModuleDescriptor->IsDacPresented =  true;

      LAST_ERROR_NUMBER(24);
      error=1;
      }

   if(CRC8CALC(Buffer+32, 95) != Buffer[127])
   	{
		for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E154; i++)
			{
		ModuleDescriptor->AdcOffsetCoefs[i]	= *(float*)(Buffer + 33 + (i + 0)*sizeof(float));
		ModuleDescriptor->AdcScaleCoefs[i]	= *(float*)(Buffer + 33 + (i + ADC_CALIBR_COEFS_QUANTITY_E154)*sizeof(float));
			}

		for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E154; i++)
		  	{
		ModuleDescriptor->DacOffsetCoefs[i]	= (*(float*)(Buffer + 65 + (i + 0)*sizeof(float)));
		ModuleDescriptor->DacScaleCoefs[i]	= (*(float*)(Buffer + 65 + (i + DAC_CALIBR_COEFS_QUANTITY_E154)*sizeof(float)));
	  	 	}
      LAST_ERROR_NUMBER(24);
      error=1;
      }

   if(error) return FALSE;

	// �������� ����� �������
	sprintf((char *)(ModuleDescriptor->SerialNumber), "%u%c%lu", (DWORD)Buffer[0], Buffer[1], (*(DWORD*)(Buffer+2)) & 0xFFFFFFL);
	// �������� �������
	sprintf((char *)(ModuleDescriptor->Name), "%.10s", Buffer+6);
	// ������� �������
	ModuleDescriptor->Revision = Buffer[16];
	// ��� �������������� ����������
	sprintf((char *)(ModuleDescriptor->CpuType), "%.10s", Buffer+17);
	// ������� ������ ����������
	ModuleDescriptor->QuartzFrequency = *(DWORD*)(Buffer+27);
	// ���� ����������� ��� �� ����� ������
	ModuleDescriptor->IsDacPresented = Buffer[32] ? true : false;
	// ���������� ������������� ��������� ���
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E154; i++)
	{
		ModuleDescriptor->AdcOffsetCoefs[i]	= *(float*)(Buffer + 33 + (i + 0)*sizeof(float));
		ModuleDescriptor->AdcScaleCoefs[i]	= *(float*)(Buffer + 33 + (i + ADC_CALIBR_COEFS_QUANTITY_E154)*sizeof(float));
	}
	// ���������� ������������� ��������� ���
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E154; i++)
	{
		ModuleDescriptor->DacOffsetCoefs[i]	= (*(float*)(Buffer + 65 + (i + 0)*sizeof(float)));
		ModuleDescriptor->DacScaleCoefs[i]	= (*(float*)(Buffer + 65 + (i + DAC_CALIBR_COEFS_QUANTITY_E154)*sizeof(float)));
	}
	// �� ������
	return TRUE;
}


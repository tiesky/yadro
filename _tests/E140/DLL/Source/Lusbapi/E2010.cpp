//------------------------------------------------------------------------------
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <algorithm>
#include "E2010.h"
#include "ErrorBaseIds.h"
//------------------------------------------------------------------------------

static const WORD InputRangeBitMap [8] = {  3,4, 1,2, 15,8, 13,14 };
static const WORD InputSwitchBitMap[8] = { 10,0, 9,0, 12,0, 11,0  };

#define 	LAST_ERROR_NUMBER(ErrorNumber)	LastErrorNumber = E2010_BASE_ERROR_ID + ErrorNumber

//******************************************************************************
// ���������� ���������� ��� ������ E20-10
//******************************************************************************
//------------------------------------------------------------------------------
// �����������
//------------------------------------------------------------------------------
TLE2010::TLE2010(HINSTANCE hInst) : TLUSBBASE(hInst)
{
	// ������������� ���� ��������� �������� ������
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));
	ZeroMemory(&FirmwareDescriptor, sizeof(FIRMWARE_DESCRIPTOR));
	ZeroMemory(&BootLoaderDescriptor, sizeof(BOOT_LOADER_DESCRIPTOR));
	ZeroMemory(&PldInfo, sizeof(PldInfo));
	// ������� ������ ������ ���������������� � ������ '����������' (Application)
	IsMcuApplicationActive = FALSE;
	// ������� ������ ����� ������
	IsDataAcquisitionInProgress = FALSE;
	// ������� ������ ���������� ������ � ��������������� ����
	IsUserFlashWriteEnabled = FALSE;
	// ������� ����� ������ Firmware ����������������
	FirmwareVersionNumber = 0.0;
}

//------------------------------------------------------------------------------
// ����������
//------------------------------------------------------------------------------
TLE2010::~TLE2010() { }



//==============================================================================
// ������� ������ ���������� ��� ������ � ������� �20-10
//==============================================================================
//-----------------------------------------------------------------------------
// ������� ������� ����������� �����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::OpenLDevice(WORD VirtualSlot)
{
	char ModuleName[LONG_MODULE_NAME_STRING_LENGTH];

	// ��������� ������� ����������� ���� ��� ������� � USB ������
	if(!TLUSBBASE::OpenLDeviceByID(VirtualSlot, E2010_ID)) { return FALSE; }
	// ��������� ��������� �������� ������
	else if(!TLUSBBASE::GetModuleName(ModuleName)) { TLUSBBASE::CloseLDevice(); return FALSE; }
	// ��������, ��� ��� ������ E20-10
	else if(strcmp(ModuleName, "E20-10")) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(0); return FALSE; }
	// ��������� ��������� ��������������� � ������ '����������' (BootLoader)
	else if(!RunMcuApplication(BOOT_LOADER_START_ADDRESS)) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(1); return FALSE; }
	// � ������ �������� ��������������� � ������ '����������' (Application)
	else if(!RunMcuApplication(FIRMWARE_START_ADDRESS, REINIT_SWITCH_PARAM)) { TLUSBBASE::CloseLDevice(); LAST_ERROR_NUMBER(2); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
//  ��������� ������� ����������� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::CloseLDevice(void)
{
	// ������� ����������
	if(!TLUSBBASE::CloseLDevice()) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
//  ��������� ��������� �� ��������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::ReleaseLInstance(void)
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
//  ������������ ���������� ���������� USB
//------------------------------------------------------------------------------
HANDLE WINAPI TLE2010::GetModuleHandle(void) { return TLUSBBASE::GetModuleHandle(); }

//------------------------------------------------------------------------------
//  ������ �������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GetModuleName(PCHAR const ModuleName)
{
	// �������� �����
	if(!ModuleName) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������ ��������� ������ �������� ������
	else if(!TLUSBBASE::GetModuleNameByLength(ModuleName, SHORT_MODULE_NAME_STRING_LENGTH)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
//  ��������� ������� �������� ������ ���� USB
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// �������� ���������
	if(!UsbSpeed) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������ ��������� ������ �������� ������ USB ����
	else if(!TLUSBBASE::GetUsbSpeed(UsbSpeed)) return FALSE;
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ���������� ������� ������� ������������������ ������ E20-10
// �� �.�. ������ ����� �� �������������� �������, �� ������ ���������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::LowPowerMode(BOOL LowPowerFlag)
{
	if(!TLUSBBASE::LowPowerMode(LowPowerFlag)) return FALSE;
   else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ������ � ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	return TLUSBBASE::GetLastErrorInfo(LastErrorInfo);
}






//------------------------------------------------------------------------------
// ������� ���������� ���� �� ������� ���������� ��� �����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::LOAD_MODULE(PCHAR const FileName)
{
	WORD i;
	DWORD	NBytes;
	// ��������� �� ������ � ����� ��� ����
	BYTE *PldCodeBuffer;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ���������� ����� ���������� ����
//	else if(!ResetPld()) { LAST_ERROR_NUMBER(5); return FALSE; }

	// ������� ��������� ���������� ������
	if(!GetModuleDescriptor(&ModuleDescriptor)) return FALSE;
	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E2010; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E2010[i]) break;
	if(i == REVISIONS_QUANTITY_E2010) { LAST_ERROR_NUMBER(35); return FALSE; }

	// ��� ������� 'A'
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
	{
		// ��������� ������� ����� ��� ����� ����
		PldCodeBuffer = new BYTE[EP1K10_SIZE];
		if(!PldCodeBuffer) { LAST_ERROR_NUMBER(6); return FALSE; }
		// ������� ������ PldCodeBuffer
		ZeroMemory(PldCodeBuffer, sizeof(EP1K10_SIZE));
	}

	// ����������� ������ ����� ����� ��� ��� ������� � ����
	if(!FileName)
	{
		HRSRC RsRes;

		// ������ ������ � ����� LBIOS ��� ������ ������� 'A'
		if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
			RsRes = FindResource(hInstance, "E2010PLD", RT_RCDATA);
		// ������ ������ � ����� LBIOS ��� ������ ������� 'B'
		else if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_B_E2010])
			RsRes = FindResource(hInstance, "E2010MPLD", RT_RCDATA);
		else { LAST_ERROR_NUMBER(35); return FALSE; }
		// �������� ���������� ������
		if(!RsRes) { LAST_ERROR_NUMBER(7); return FALSE; }
		// �������� ���
		HGLOBAL RcResHandle = LoadResource(hInstance, RsRes);
		if(!RcResHandle) { LAST_ERROR_NUMBER(8); return FALSE; }
		// ����������� ������
		char  *pRcData = (char *)LockResource(RcResHandle);
		if(!pRcData) { LAST_ERROR_NUMBER(9); return FALSE; }
		// ��������� ������ �������
		NBytes = SizeofResource(hInstance, RsRes);
		// ��� ������� 'A'
		if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
		{
			if(NBytes != EP1K10_SIZE) { LAST_ERROR_NUMBER(10); return FALSE; }
		}
		else
		{
			if(!NBytes) { LAST_ERROR_NUMBER(36); return FALSE; }

			// ��������� ������� ����� ��� ����� ����
			PldCodeBuffer = new BYTE[NBytes];
			if(!PldCodeBuffer) { LAST_ERROR_NUMBER(6); return FALSE; }
		}

		// ������� ������ PldCodeBuffer
		ZeroMemory(PldCodeBuffer, NBytes);
		// ��������� ������ �������
		for(DWORD i = 0x0; i < NBytes; i++) PldCodeBuffer[i] = pRcData[i];
	}
	else
	{
		int PldFile;

		// ������� ���� � ������� ����
		PldFile = open(FileName, O_BINARY | O_RDONLY);
		if(PldFile == -1) { LAST_ERROR_NUMBER(11); return FALSE; }

		// ��������� ������ ����� � ������� ����
		NBytes = filelength(PldFile);
		if((long)NBytes == -1) { LAST_ERROR_NUMBER(12); return FALSE; }

		// ��� ������� 'A'
		if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
			{ if(NBytes != EP1K10_SIZE) { LAST_ERROR_NUMBER(12); return FALSE; } }
		else
		{
			// ��������� ������� ����� ��� ����� ����
			PldCodeBuffer = new BYTE[NBytes];
			if(!PldCodeBuffer) { LAST_ERROR_NUMBER(6); return FALSE; }
		}
		// ������� ������ PldCodeBuffer
		ZeroMemory(PldCodeBuffer, NBytes);

		// �������� ���������� ����� � �����
		if(read(PldFile, PldCodeBuffer, NBytes) == -1) { LAST_ERROR_NUMBER(13); return FALSE; }

		// ������� ����
		if(close(PldFile) == -1) { LAST_ERROR_NUMBER(14); return FALSE; }
	}
	// ������� ������ ����
	if(!LoadPld(PldCodeBuffer, NBytes)) { LAST_ERROR_NUMBER(15); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// �������� ������� ���������� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::TEST_MODULE(WORD TestModeMask)
{
	// �������� ������� ���������� ������
	if(strcmp((char *)ModuleDescriptor.ModuleName, "E20-10")) { LAST_ERROR_NUMBER(37); return FALSE; }

	// �������� ����� ������ ��� ������ ������� 'B' � ����
	if((REVISIONS_E2010[REVISION_A_E2010] < ModuleDescriptor.Revision) &&
		(ModuleDescriptor.Revision <= REVISIONS_E2010[REVISIONS_QUANTITY_E2010 - 0x1]))
	{
		// ��������� ������ ������� ����
		WORD InBuf[4] = { 0x0, V_TEST_PLD, 0x0, 0x0 };

		// �������� ������ � ������
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(19); return FALSE; }

		// ���� ����� - ������� ��������� ������ � �������������� �������� ������
		if(!SetTestMode(TestModeMask)) return FALSE;
	}
	// ��� ������ :)))))
	return TRUE;
}



//==============================================================================
// ������� ��� ������ � ��� ������
//==============================================================================
//------------------------------------------------------------------------------
// ������ ���������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GET_ADC_PARS(ADC_PARS_E2010 * const AdcPars)
{
	WORD i, j, k;
	BYTE bParam;
	WORD wParam;

	// ������� ������ ParamArray
	ZeroMemory(&AdcParamsArray, sizeof(AdcParamsArray));

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ���������
	else if(!AdcPars) { LAST_ERROR_NUMBER(3); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E2010; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E2010[i]) break;
	if(i == REVISIONS_QUANTITY_E2010) { LAST_ERROR_NUMBER(35); return FALSE; }

	// ������ ����� ������� ������� ��������� ������ ���
	if(!GetArray(SEL_ADC_PARAM, AdcParamsArray, ADC_PARAM_SIZE)) { LAST_ERROR_NUMBER(16); return FALSE; }

	// *** ��������� ����� ������ ��� ***
	// ��������� ������������� ����� ������
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
		GetSynchroParsForRevA(AdcParamsArray[0], AdcPars);
	else
	{
		// �������� ��������� ������������� ����� ������
		GetSynchroParsForRevBandAbove(AdcParamsArray[0], AdcPars);
		// �������������� ��������� ������������� ����� ������
		if(!GetExtraSynchroParsForRevBandAbove(AdcPars)) return FALSE;
	}

	// ����� ����������� �������
	AdcPars->ChannelsQuantity = (WORD)(AdcParamsArray[6] + 0x1);
	// ����������� ������� � ����������� �������� �������
	for(i = 0x0; i < AdcPars->ChannelsQuantity; i++) AdcPars->ControlTable[i]=AdcParamsArray[7+i];
	// ADC Rate � ���
	AdcPars->AdcRate = 30000.0/(AdcParamsArray[1] + 1.0);
	// ADC InterFadrDelay � ��
	AdcPars->InterKadrDelay = 	(*(WORD*)(AdcParamsArray+2) + 1.0)/AdcPars->AdcRate;
	// ������� �����
	AdcPars->KadrRate = 1.0/((AdcPars->ChannelsQuantity - 1.0)/AdcPars->AdcRate + AdcPars->InterKadrDelay);
	// ��������� ������� ���������� ��� ������� ���
	wParam = *(WORD*)(AdcParamsArray+4);
	for(i = j = 0x0; i < ADC_CHANNELS_QUANTITY_E2010; i++)
	{
		j = (WORD)(((wParam >> InputRangeBitMap[2*i+0]) & 0x1) << 0x0);
		j |= (WORD)(((wParam >> InputRangeBitMap[2*i+1]) & 0x1) << 0x1);
		AdcPars->InputRange[i] = j;
	}
	// ���� ������ ��� ������� ���
	for(i = j = 0x0; i < ADC_CHANNELS_QUANTITY_E2010; i++)
	{
		j = (WORD)(((wParam >> InputSwitchBitMap[2*i+0]) & 0x1) << 0x0);
//		j |= ((wParam >> InputSwitchBitMap[2*i+1]) & 0x1) << 0x1;
		AdcPars->InputSwitch[i] = j;
	}
	// ���������� ������� ����� �������� ��� ������� 'B' � ����
	if(ModuleDescriptor.Revision != REVISIONS_E2010[REVISION_A_E2010])
	{
		if(wParam & (0x1 << 0x5)) AdcPars->InputCurrentControl = TRUE;
		else  AdcPars->InputCurrentControl = FALSE;
	}
	else AdcPars->InputCurrentControl = TRUE;

	// ************* ���������������� ������������ ��� **************************
	// �������� ����������� ���������� �������������� �������������� ������ � ���
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010]) AdcPars->IsAdcCorrectionEnabled = FALSE;
	else
	{
		// ������� ������� ������� ����������� ���������� �������������� �������������� ������ � ���
		if(!GetArray(ADC_CORRECTION_ADDRESS, &bParam, 0x1)) { LAST_ERROR_NUMBER(18); return FALSE; }
		AdcPars->IsAdcCorrectionEnabled = bParam ? TRUE : FALSE;

		// ������ �������� ������� ���������������� ����������� ��� �� ����������������
		if(!GetArray(SEL_ADC_CALIBR_KOEFS, (BYTE *)AdcCalibrCoefsArray, 2*ADC_CALIBR_COEFS_QUANTITY)) { LAST_ERROR_NUMBER(16); return FALSE; }
		// ��������� � ����������������� �������������� ���
		for(i = k = 0x0; i < ADC_INPUT_RANGES_QUANTITY_E2010; i++)
			for(j = 0x0; j < ADC_CHANNELS_QUANTITY_E2010; j++)
			{
				// ������������� ��������
				AdcPars->AdcOffsetCoefs[i][j] = (SHORT)AdcCalibrCoefsArray[k++];
				// ������������� ��������
				AdcPars->AdcScaleCoefs[i][j] = (double)AdcCalibrCoefsArray[k++]/(double)0x8000;
			}
	}
	// **************************************************************************

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ��������� ���������� ������ ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::SET_ADC_PARS(ADC_PARS_E2010 * const AdcPars)
{
	WORD i, j, k;
	BYTE bParam;
	WORD wParam;
	DWORD dwParam;

	// ������� ������ AdcParamsArray
	ZeroMemory(&AdcParamsArray, sizeof(AdcParamsArray));

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ���������
	else if(!AdcPars) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ������ ����� ������ - �� ����� ����� ������ ������ ��������� ��� ������
	else if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(17); return FALSE; }
	// �������� ���������� ������
	else if(strcmp((char *)ModuleDescriptor.ModuleName, "E20-10")) { LAST_ERROR_NUMBER(37); return FALSE; }

	// �������� ���������� ������� ������
	for(i = 0x0; i< REVISIONS_QUANTITY_E2010; i++)
		if(ModuleDescriptor.Revision == REVISIONS_E2010[i]) break;
	if(i == REVISIONS_QUANTITY_E2010) { LAST_ERROR_NUMBER(35); return FALSE; }

	// ����� ����������� ������� �� ����� ���� ����� ����
	if(!AdcPars->ChannelsQuantity) { LAST_ERROR_NUMBER(34); return FALSE; }
	// �������� �� ������������ ����� ����������� �������
	else if(AdcPars->ChannelsQuantity > MAX_CONTROL_TABLE_LENGTH_E2010)
					AdcPars->ChannelsQuantity = MAX_CONTROL_TABLE_LENGTH_E2010;
	AdcParamsArray[6] = (BYTE)(AdcPars->ChannelsQuantity - 0x1);
	// ����������� ������� � ����������� �������� �������
	for(i = 0x0; i < AdcPars->ChannelsQuantity; i++)
			AdcParamsArray[7+i] = (BYTE)AdcPars->ControlTable[i];

	// ����� ���� ������������ ����� ������ ��� ������ ������� ������
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
	{
		// ������� ��������� ����� ������
		MakeSynchroParsForRevA(AdcParamsArray[0], AdcPars);
		// ������� �������������� ��������� ����� ������
		AdcPars->SynchroPars.StartDelay						= 0x0;
		AdcPars->SynchroPars.StopAfterNKadrs				= 0x0;
		AdcPars->SynchroPars.SynchroAdMode					= NO_ANALOG_SYNCHRO_E2010;
		AdcPars->SynchroPars.SynchroAdChannel				= 0x0;
		AdcPars->SynchroPars.SynchroAdPorog					= 0x0;
		AdcPars->SynchroPars.IsBlockDataMarkerEnabled	= 0x0;
		// ��� ������� ������������� ������ ���������� �������� Firmware MCU �� ���� 1.8
		if(AdcPars->SynchroPars.StartSource > INT_ADC_START_WITH_TRANS_E2010)
			{ if(FirmwareVersionNumber < 1.75) { LAST_ERROR_NUMBER(31); return FALSE; } }
	}
	else
	{
		// ������� ��������� ����� ������
		MakeSynchroParsForRevBandAbove(AdcParamsArray[0], AdcPars);
		// �������������� ��������� ����� ������
		if(!SetExtraSynchroParsForRevBandAbove(AdcPars)) return FALSE;
	}

	// ������� ������ ��� � ���
	AdcPars->AdcRate = fabs(AdcPars->AdcRate);
	if(AdcPars->AdcRate > 10000.0) AdcPars->AdcRate = 10000.0;
	else if(AdcPars->AdcRate < 1000.0) AdcPars->AdcRate = 1000.0;
	AdcParamsArray[1] = (BYTE)(30000.0/AdcPars->AdcRate - 0.5);
	AdcPars->AdcRate = 30000.0/(AdcParamsArray[1] + 1.0);
	// �������� ����������� ��������
	AdcPars->InterKadrDelay = fabs(AdcPars->InterKadrDelay);
	if((1.0/(AdcPars->AdcRate)) > (AdcPars->InterKadrDelay)) AdcPars->InterKadrDelay = 1.0/AdcPars->AdcRate;
	dwParam = (DWORD)(AdcPars->InterKadrDelay*AdcPars->AdcRate - 0.5);
	// �������� ����. ����������� ��������
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
		{ if(dwParam > 255) dwParam = 255; }
	else
		{ if(dwParam > 65535) dwParam = 65535; }
	*(WORD*)(AdcParamsArray+2) = (WORD)(dwParam & 0xFFFF);
	// ������������� �������� ����������� ��������
	AdcPars->InterKadrDelay = (dwParam + 1.0)/AdcPars->AdcRate;
	// ������� ����� �������
	AdcPars->KadrRate = 1.0/((AdcPars->ChannelsQuantity - 1.0)/AdcPars->AdcRate + AdcPars->InterKadrDelay);
	// ��������� ������� ���������� ��� ������� ���
	for(i = wParam = 0x0; i < ADC_CHANNELS_QUANTITY_E2010; i++)
	{
		wParam |= (WORD)(((AdcPars->InputRange[i] >> 0x0) & 0x1) << InputRangeBitMap[2*i + 0x0]);
		wParam |= (WORD)(((AdcPars->InputRange[i] >> 0x1) & 0x1) << InputRangeBitMap[2*i + 0x1]);
	}
	// ���� ������ ��� ������� ���
	for(i = 0x0; i < ADC_CHANNELS_QUANTITY_E2010; i++)
	{
		wParam |= (WORD)(((AdcPars->InputSwitch[i] >> 0x0) & 0x1) << InputSwitchBitMap[2*i + 0x0]);
//		wParam|=((AdcPars->InputSwitch[i]>>1)&1)<<InputSwitchBitMap[2*i+1];
	}
	// ���������� ������� ����� �������� ��� ������� 'B' � ����
	if(ModuleDescriptor.Revision != REVISIONS_E2010[REVISION_A_E2010])
	{
		if(AdcPars->InputCurrentControl) wParam |= 0x1 << 0x5;
	}
	//
	*(WORD*)(AdcParamsArray+4) = wParam;

	// ������ ��������� ��������������� ������ � ���������������
	if(!PutArray(SEL_ADC_PARAM, AdcParamsArray, ADC_PARAM_SIZE)) { LAST_ERROR_NUMBER(18); return FALSE; }

	// ************* ���������������� ������������ ��� **************************
	// �������� ����������� ���������� �������������� �������������� ������ � ���
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010]) AdcPars->IsAdcCorrectionEnabled = FALSE;
	else
	{
		// ��������� ������� ����������� ���������� �������������� �������������� ������ � ���
		bParam = (BYTE)(AdcPars->IsAdcCorrectionEnabled ? 0x1 : 0x0);
		if(!PutArray(ADC_CORRECTION_ADDRESS, &bParam, 0x1)) { LAST_ERROR_NUMBER(18); return FALSE; }

		// ��������� � ����������������� �������������� ���
		for(i = k = 0x0; i < ADC_INPUT_RANGES_QUANTITY_E2010; i++)
			for(j = 0x0; j < ADC_CHANNELS_QUANTITY_E2010; j++)
			{
				if(AdcPars->IsAdcCorrectionEnabled)
				{
					// ������������� ��������
					if(AdcPars->AdcOffsetCoefs[i][j] < 0.0) AdcCalibrCoefsArray[k++] = AdcPars->AdcOffsetCoefs[i][j] - 0.5;
					else AdcCalibrCoefsArray[k++] = AdcPars->AdcOffsetCoefs[i][j] + 0.5;
					// ������������� ��������
					if((AdcPars->AdcScaleCoefs[i][j] < 0.1) || (AdcPars->AdcScaleCoefs[i][j] > 1.9))  { LAST_ERROR_NUMBER(38); return FALSE; }
					AdcCalibrCoefsArray[k++] = (WORD)(AdcPars->AdcScaleCoefs[i][j]*(double)0x8000 + 0.5);
				}
				else
				{
					// �������������� ������������� ��������
					AdcCalibrCoefsArray[k++] = 0x0;
					// �������������� ������������� ��������
					AdcCalibrCoefsArray[k++] = 0x8000;
				}
			}
		// ������ ��������� ���������������� ����������� ��� � ��������������� ������
		if(!PutArray(SEL_ADC_CALIBR_KOEFS, (BYTE *)AdcCalibrCoefsArray, 2*ADC_CALIBR_COEFS_QUANTITY)) { LAST_ERROR_NUMBER(18); return FALSE; }
	}
	// **************************************************************************

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����� ������ � ���
//-----------------------------------------------------------------------------
BOOL WINAPI TLE2010::START_ADC(void)
{
	WORD InBuf[4] = { 0x0, V_START_ADC, 0x0, 0x1};

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ������ � ������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// ��������� ������ ����� ������
	IsDataAcquisitionInProgress = TRUE;
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ����� ������ � ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::STOP_ADC(void)
{
	WORD InBuf[4] = { 0x0, V_STOP_ADC, 0x0, 0x1};

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ������ � ������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// ������� ��� ���������� ������� �� ���� ������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_ABORT_PIPE3/*abort Read Pipe*/, NULL, 0, NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(39); return FALSE; }
	// ������ �������� ����� ����� ������
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3/*reset Read Pipe */, NULL, 0, NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(40); return FALSE; }
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3/*reset Read Pipe */, NULL, 0, NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(40); return FALSE; }
	// ������� ������ ����� ������
	IsDataAcquisitionInProgress = FALSE;

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// �������� ������� ���������� ������, � ��� ����� ��������� ����������� ������ ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GET_DATA_STATE(DATA_STATE_E2010 * const DataState)
{
	WORD InBuf[4] = { 0x01, V_GET_ARRAY, (WORD)(DATA_STATE_ADDRESS & 0xFFFF), (WORD)(DATA_STATE_ADDRESS >> 0x10)};

	// �������� ��������� �� ���������
	if(!DataState) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������� ���������
	ZeroMemory(DataState, sizeof(DATA_STATE_E2010));

	// ��� ������� 'A'
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
	{
		// ����������� � ������� ������ Firmware ����������������
		if(FirmwareVersionNumber < 1.65) { LAST_ERROR_NUMBER(31); return FALSE; }

		// �������� ������ ����������� ������
		EnterCriticalSection(&cs);
		// �������� ������ ������ ���������������� � ������ '����������' (Application)
		if(!IsMcuApplicationActive) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(4); return FALSE; }
		// �������� ������ � ������ �� ���������� �������� �������� ����������� ���������� ������
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), &LocDataStatus.Status, sizeof(LocDataStatus.Status), TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }
		// ��������� ������ ����������� ������
		LeaveCriticalSection(&cs);
		// ������ ����� ����������� ��� ����� :)
		DataState->BufferOverrun = (BYTE)(LocDataStatus.Status & 0x1);
	}
	else
	{
		// �������� ������ ����������� ������
		EnterCriticalSection(&cs);
		// �������� ������ ������ ���������������� � ������ '����������' (Application)
		if(!IsMcuApplicationActive) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(4); return FALSE; }
		// �������� ������ � ������ �� ���������� �������� �������� ����������� ���������� ������
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), &LocDataStatus, sizeof(LocDataStatus), TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }
		// ��������� ������ ����������� ������
		LeaveCriticalSection(&cs);

		// ������ ����� ����������� ��� ����� :)
		DataState->ChannelsOverFlow = (BYTE)(((LocDataStatus.Status & 0x08) << 0x4) |
														 ((LocDataStatus.Status & 0xF0) >> 0x4));
		DataState->BufferOverrun = (BYTE)(LocDataStatus.Status & 0x1);
		DataState->CurBufferFilling = LocDataStatus.CurBufferFilling >> 0x1;
		DataState->MaxOfBufferFilling = LocDataStatus.MaxOfBufferFilling >> 0x1;
		DataState->BufferSize = LocDataStatus.BufferSize >> 0x1;
		DataState->CurBufferFillingPercent = 100.0*LocDataStatus.CurBufferFilling/LocDataStatus.BufferSize;
		DataState->MaxOfBufferFillingPercent = 100.0*LocDataStatus.MaxOfBufferFilling/LocDataStatus.BufferSize;
	}

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ��������� ������ ������ �� ������
//   ���� ReadRequest->Overlapped != NULL - ����������� ������ �� ���� ������������ ���-�� ������
//   ���� ReadRequest->Overlapped == NULL - ���������� ���� ������������ ���-�� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::ReadData(IO_REQUEST_LUSBAPI * const ReadRequest)
{
	DWORD NumberOfBytesRead;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ����� ������
	else if(!ReadRequest->Buffer) { LAST_ERROR_NUMBER(3); return FALSE; }

	// �������� ������ ����������� ������
	EnterCriticalSection(&cs);
	//
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
	{
		// ����� �������� ������ ���������� � ��������� �� 0x100(256)
		if(ReadRequest->NumberOfWordsToPass < 0x100) ReadRequest->NumberOfWordsToPass = 0x100;
		// � ������ �� 0x100000(1024*1024)
		else if(ReadRequest->NumberOfWordsToPass > (1024*1024)) ReadRequest->NumberOfWordsToPass = 1024*1024;
		// ����� ����� �������� ������ ���� ������ 0x100(256)
		ReadRequest->NumberOfWordsToPass -= ReadRequest->NumberOfWordsToPass%256;
	}
	else
	{
		DWORD BytesToPass;
		WORD InBuf[4] = { 0x00, V_PUT_ARRAY, 0x0, (WORD)(SEL_BULK_REQ_SIZE >> 0x10) };

		// ����� �������� ������ ���������� � ��������� �� 0x1
		if(ReadRequest->NumberOfWordsToPass < 0x1) ReadRequest->NumberOfWordsToPass = 0x1;
		// � ������ �� 0x100000(1024*1024)
		else if(ReadRequest->NumberOfWordsToPass > (1024*1024)) ReadRequest->NumberOfWordsToPass = 1024*1024;
		// �������� ���-�� ������������ ���� (������ �������)
		BytesToPass = 2*ReadRequest->NumberOfWordsToPass;
		// ������� � ������ ������ ���������� ������� �� ����� ������
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), &BytesToPass, sizeof(DWORD), TimeOut)) { LAST_ERROR_NUMBER(19); LeaveCriticalSection(&cs); return FALSE; }
	}
	//
	NumberOfBytesRead = 0x0;

	// ��������� ����������� ������
	if(ReadRequest->Overlapped)
	{
		// �������� ����������� ������ �� ���� ������������ ���-�� ������
		if(!ReadFile(hDevice, ReadRequest->Buffer, 2*ReadRequest->NumberOfWordsToPass, &NumberOfBytesRead, ReadRequest->Overlapped))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(21); LeaveCriticalSection(&cs); CancelIo(hDevice); return FALSE; } }
		// ��������� ������ ����������� ������
		LeaveCriticalSection(&cs);
		// ���-�� ���������� �������� (� ���� ������������� ������� ��� �������� ������ ����� ���� ����� 0)
		ReadRequest->NumberOfWordsPassed = NumberOfBytesRead/2;
	}
	// ��������� ���������� ������
	else
	{
		OVERLAPPED Ov;

		// �������������� OVERLAPPED ���������
		ZeroMemory(&Ov, sizeof(OVERLAPPED));
		Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
		if(!Ov.hEvent) { LAST_ERROR_NUMBER(20); LeaveCriticalSection(&cs); return FALSE; }
		// �������� ����������� ������ �� ���� ������������ ���-�� ������
		if(!ReadFile(hDevice, ReadRequest->Buffer, 2*ReadRequest->NumberOfWordsToPass, &NumberOfBytesRead, &Ov))
			{ if(GetLastError() != ERROR_IO_PENDING) { LAST_ERROR_NUMBER(21); LeaveCriticalSection(&cs); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; } }
		// ��������� ������ ����������� ������
		LeaveCriticalSection(&cs);
		// ��� ��������� ���������� �������
		if(WaitForSingleObject(Ov.hEvent, ReadRequest->TimeOut) == WAIT_TIMEOUT) { LAST_ERROR_NUMBER(22); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ��������� �������� ���-�� ������� ���������� ���� ������
		else if(!GetOverlappedResult(hDevice, &Ov, &NumberOfBytesRead, TRUE)) { LAST_ERROR_NUMBER(23); CancelIo(hDevice); CloseHandle(Ov.hEvent); return FALSE; }
		// ������� �������
		else if(!CloseHandle(Ov.hEvent)) { LAST_ERROR_NUMBER(24); return FALSE; }
		// ���-�� ���������� ��������
		ReadRequest->NumberOfWordsPassed = NumberOfBytesRead/2;
		// ������� ������� ���� ������� ��������� ������ (� ������) � ������������� ���-��� ��������
		if(ReadRequest->NumberOfWordsToPass != ReadRequest->NumberOfWordsPassed) { LAST_ERROR_NUMBER(25); return FALSE; }
	}
	// ��� ������ :)))))
	return TRUE;
}




//==============================================================================
// ������� ��� ������ � ��� ������
//==============================================================================
//------------------------------------------------------------------------------
// ����������� ����� ������� �� ���
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::DAC_SAMPLE(SHORT * const DacData, WORD DacChannel)
{
	SHORT Param;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// � ���� �� ��� �� ������ ���������� ������
	else if(ModuleDescriptor.IsDacPresented == DAC_INACCESSIBLED_E440)  { LAST_ERROR_NUMBER(26); return FALSE; }
	// �������� ���������
	else if(!DacData) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ����� ������ ���
	else if(DacChannel >= DAC_CHANNELS_QUANTITY_E2010) { LAST_ERROR_NUMBER(27); return FALSE; }
	// ���������� �������� ��� ������ �� ���
	else if((*DacData) < -2048) *DacData = -2048;
	else if((*DacData) > 2047) *DacData = 2047;
	// ��������� ��������������� �������� ��� ����������������
	Param = (SHORT)(((*DacData) & 0xFFF) | (WORD)(DacChannel << 12));
	Param = (SHORT)(((Param << 0x8) & 0xFF00) | ((Param >> 0x8) & 0x00FF));
	// ��������� ��������������� ������ � ���������������
	if(!PutArray(SEL_DAC_DATA, (BYTE*)&Param, 2)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}




//==============================================================================
// ������� ��� ������ � ��������� ������� �� ������� �������� �������
//==============================================================================
//------------------------------------------------------------------------------
// ������� ���������� �������� ����� �������� ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::ENABLE_TTL_OUT(BOOL EnableTtlOut)
{
	BYTE b = (BYTE)(EnableTtlOut ? 0x1 : 0x0);

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ���������  ��������������� ������ � ���������������
	else if(!PutArray(SEL_DIO_PARAM, &b, 0x1)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ ������� ����� �������� ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::TTL_IN(WORD * const TtlIn)
{
	SHORT TtlParam;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ��������� ����������
	else if(!TtlIn) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������ ��������������� ������ �� ����������������
	else if(!GetArray(SEL_DIO_DATA, (BYTE*)&TtlParam, 0x2)) { LAST_ERROR_NUMBER(16); return FALSE; }
	// ��������� ����� ��������� �������� ������� �����
	*TtlIn = (WORD)(((TtlParam << 0x8) & 0xFF00) | ((TtlParam >> 0x8) & 0x00FF));
	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������� ������ �� �������� ����� �������� ��������� �������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::TTL_OUT(WORD TtlOut)
{
	SHORT TtlParam;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// ��������� ����� ��������� �������� �������� �����
	TtlParam = (WORD)(((TtlOut << 0x8) & 0xFF00) | ((TtlOut >> 0x8) & 0x00FF));
	// ��������� ��������������� ������ � ���������������
	if(!PutArray(SEL_DIO_DATA, (BYTE*)&TtlParam, 0x2)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}




// =============================================================================
//  ������� ��� ������ � ���������������� ����������� ����
// =============================================================================
// -----------------------------------------------------------------------------
// ����������/���������� ������ ������ � ���������������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE2010::ENABLE_FLASH_WRITE(BOOL IsUserFlashWriteEnabled)
{
	// �������� �������
	TLE2010::IsUserFlashWriteEnabled = IsUserFlashWriteEnabled;
	// ��� ������ :)))))
	return TRUE;
}

// -----------------------------------------------------------------------------
// ������� ������� �� ����������������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE2010::READ_FLASH_ARRAY(USER_FLASH_E2010 * const UserFlash)
{
	// �������� ���������
	if(!UserFlash) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ���� ������ ������
	else if(!GetArray(USER_FLASH_ADDRESS, (BYTE *)UserFlash, sizeof(USER_FLASH_E2010))) { LAST_ERROR_NUMBER(16); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

// -----------------------------------------------------------------------------
// ������ ������� � ���������������� ����
// -----------------------------------------------------------------------------
BOOL WINAPI TLE2010::WRITE_FLASH_ARRAY(USER_FLASH_E2010 * const UserFlash)
{
	BYTE *ptr;
	DWORD Address, Size;
	DWORD i = 0x0;

	// �������� ������ ����� ������ - �� ����� ����� ������ ������
	// ������ ���������� � ���������������� ����
	if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(17); return FALSE; }
	// ��� ������ �������� ���������� �� ������
	else if(!IsUserFlashWriteEnabled) { LAST_ERROR_NUMBER(28); return FALSE; }
	// ������ �������� ���������
	else if(!UserFlash) { LAST_ERROR_NUMBER(3); return FALSE; }

	// �������� ������ ����������� ������
	EnterCriticalSection(&cs);
	// ���� ����� ��������� ��������� ��������������� � ����� '����������' (BootLoader)
	if(IsMcuApplicationActive)
		if(!RunMcuApplication(BOOT_LOADER_START_ADDRESS)) { LAST_ERROR_NUMBER(1); LeaveCriticalSection(&cs); return FALSE; }

	Address = USER_FLASH_ADDRESS;
	Size = sizeof(USER_FLASH_E2010);
	ptr = (BYTE *)UserFlash;
	while(i < Size)
	{
		DWORD sz = std::min((DWORD)MAX_USB_CONTROL_PIPE_BLOCK, Size - i);
		WORD InBuf[4] = {0x0000, 13/*Special PutArray*/, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10) };

		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), ptr + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }
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




//==============================================================================
// ������� ��� ������ � ����������� � ������ E20-10
//==============================================================================
//------------------------------------------------------------------------------
// ������ ���������� � ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GET_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E2010 * const md)
{
	char *pTag, *p1, *p2;
//	BYTE *ptr;
	WORD i;//, crc16;

	// �������� ������ ������ ���������������� � ������ '����������' (Application)
	if(!IsMcuApplicationActive) { LAST_ERROR_NUMBER(4); return FALSE; }
	// �������� ��������� �� ���������
	else if(!md) { LAST_ERROR_NUMBER(3); return FALSE; }

	// ������� ��������� ���������� ������
	if(!GetModuleDescriptor(&ModuleDescriptor)) return FALSE;

	// ������ ��������� �����������
	if(!GetArray(FIRMWARE_DESCRIPTOR_ADDRESS, (BYTE*)&FirmwareDescriptor, sizeof(FIRMWARE_DESCRIPTOR))) { LAST_ERROR_NUMBER(16); return FALSE; }
	else if(!GetArray(BOOT_LOADER_DESCRIPTOR_ADDRESS, (BYTE*)&BootLoaderDescriptor, sizeof(BOOT_LOADER_DESCRIPTOR))) { LAST_ERROR_NUMBER(16); return FALSE; }

	// ������� ��� ��������� MODULE_DESCRIPTION_E2010
	ZeroMemory(md, sizeof(MODULE_DESCRIPTION_E2010));

	// Module Info
	strncpy((char *)md->Module.CompanyName,  "L-Card Ltd.",    sizeof(md->Module.CompanyName));
	strncpy((char *)md->Module.DeviceName,   (char *)ModuleDescriptor.ModuleName,    std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.ModuleName)));
	strncpy((char *)md->Module.SerialNumber, (char *)ModuleDescriptor.SerialNumber,  std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
	strcpy((char *)md->Module.Comment,      			"������� ����������������� USB ������ ���/���/��� ������ ���������� �� ���� USB 2.0");
	md->Module.Revision = ModuleDescriptor.Revision;
	md->Module.Modification = ModuleDescriptor.Modification;

	// Interface Info
	md->Interface.Active = TRUE;
	strcpy((char *)md->Interface.Name, "USB 2.0");
	strcpy((char *)md->Interface.Comment, "Full-Speed (12 Mbit/s) and High-Speed(480 Mbit/s) Modes");

	// MCU Info
	md->Mcu.Active = TRUE;
	strncpy((char *)md->Mcu.Name, 									(char *)FirmwareDescriptor.McuName,  			std::min(sizeof(md->Mcu.Name), sizeof(FirmwareDescriptor.McuName)));
	strncpy((char *)md->Mcu.Version.FwVersion.Version,  		(char *)FirmwareDescriptor.Version, 			std::min(sizeof(md->Mcu.Version.FwVersion.Version), sizeof(FirmwareDescriptor.Version)));
	strncpy((char *)md->Mcu.Version.FwVersion.Date,  			(char *)FirmwareDescriptor.Created, 			std::min(sizeof(md->Mcu.Version.FwVersion.Date), sizeof(FirmwareDescriptor.Created)));
	strncpy((char *)md->Mcu.Version.FwVersion.Manufacturer,  (char *)FirmwareDescriptor.Manufacturer, 		std::min(sizeof(md->Mcu.Version.FwVersion.Manufacturer), sizeof(FirmwareDescriptor.Manufacturer)));
	strncpy((char *)md->Mcu.Version.FwVersion.Author,  		(char *)FirmwareDescriptor.Author, 				std::min(sizeof(md->Mcu.Version.FwVersion.Author), sizeof(FirmwareDescriptor.Author)));
	strncpy((char *)md->Mcu.Version.FwVersion.Comment,			(char *)FirmwareDescriptor.Comment, 			std::min(sizeof(md->Mcu.Version.FwVersion.Comment), sizeof(FirmwareDescriptor.Comment)));
	strncpy((char *)md->Mcu.Version.BlVersion.Version,  		(char *)BootLoaderDescriptor.Version, 			std::min(sizeof(md->Mcu.Version.BlVersion.Version), sizeof(BootLoaderDescriptor.Version)));
	strncpy((char *)md->Mcu.Version.BlVersion.Date,  			(char *)BootLoaderDescriptor.Created, 			std::min(sizeof(md->Mcu.Version.BlVersion.Date), sizeof(BootLoaderDescriptor.Created)));
	strncpy((char *)md->Mcu.Version.BlVersion.Manufacturer,  (char *)BootLoaderDescriptor.Manufacturer, 	std::min(sizeof(md->Mcu.Version.BlVersion.Manufacturer), sizeof(BootLoaderDescriptor.Manufacturer)));
	strncpy((char *)md->Mcu.Version.BlVersion.Author,  		(char *)BootLoaderDescriptor.Author, 			std::min(sizeof(md->Mcu.Version.BlVersion.Author), sizeof(BootLoaderDescriptor.Author)));
	strncpy((char *)md->Mcu.Version.BlVersion.Comment,			"������� ��������� ��� �������� USB ������ E20-10", sizeof(md->Mcu.Version.BlVersion.Comment));
	strncpy((char *)md->Mcu.Comment, 								"8-bit Microcontroller with 16K Bytes In-System Programmable Flash", sizeof(md->Mcu.Comment));
	md->Mcu.ClockRate = ModuleDescriptor.ClockRate/1000.0;	// � ���
	// ����������� � ������� ������ Firmware ����������������
	FirmwareVersionNumber = atof((char *)md->Mcu.Version.FwVersion.Version);
	if((FirmwareVersionNumber < 0.75) || (FirmwareVersionNumber > 20.0)) { LAST_ERROR_NUMBER(30); return FALSE; }

	// ���� Info
	if(PldInfo[0])
	{
		md->Pld.Active = TRUE;
		// ��� ������� 'A'
		if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
			strncpy((char *)md->Pld.Name, "ALTERA ACEX EP1K10TC144", sizeof(md->Pld.Name));
		else if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_B_E2010])
			strncpy((char *)md->Pld.Name, "ALTERA Cyclone EP1C3T144", sizeof(md->Pld.Name));
		else
			strncpy((char *)md->Pld.Name, "Unknown PLD type", sizeof(md->Pld.Name));
		md->Pld.ClockRate = MASTER_QUARTZ;		// � ���
		// ���� Name
/*		pTag = strstr((char*)PldInfo, "<Name>");
		if(pTag && ((p1 = strchr(pTag, '{')) != NULL) && ((p2 = strchr(pTag, '}')) != NULL))
			strncpy((char *)md->Pld.Name, (char *)(p1 + 0x1), std::min(sizeof(md->Pld.Name), (UINT)(p2-p1-0x1)));
		else
			strcpy((char *)md->Pld.Name, "Invalid Pld Name");
*/		// ���� Version.Version
		pTag = strstr((char*)PldInfo, "<Version>");
		if(pTag && ((p1 = strchr(pTag, '{')) != NULL) && ((p2 = strchr(pTag, ' ')) != NULL))
		{
			strncpy((char *)md->Pld.Version.Version, (char *)(p1 + 0x1), std::min(sizeof(md->Pld.Version.Version), (UINT)(p2-p1-0x1)));
//			else
//				md->Pld.FirmwareVersion = INVALID_PLD_FIRMWARE_VERSION;
		}
		else strncpy((char *)md->Pld.Version.Version, "????????", sizeof(md->Pld.Version.Version));
		// ���� Version.Date
		pTag = strstr((char*)PldInfo, "(build");
		if(pTag && ((p1 = strchr(pTag, ' ')) != NULL) && ((p2 = strchr(pTag, ')')) != NULL))
		{
			strncpy((char *)md->Pld.Version.Date, (char *)(p1 + 0x1), std::min(sizeof(md->Pld.Version.Date), (UINT)(p2-p1-0x1)));
//			else
//				md->Pld.FirmwareVersion = INVALID_PLD_FIRMWARE_VERSION;
		}
		else strncpy((char *)md->Pld.Version.Date, "????????", sizeof(md->Pld.Version.Version));
		// ���� Version.Manufacturer
		strncpy((char *)md->Pld.Version.Manufacturer, "L-Card Ltd.", sizeof(md->Pld.Version.Manufacturer));
		// ���� Version.Author
		md->Pld.Version.Author[0x0] = '\0';
		// ���� Version.Comment
		md->Pld.Version.Comment[0x0] = '\0';
		// ���� Comment
		pTag = strstr((char*)PldInfo, "<Comments>");
		if(pTag && ((p1 = strchr(pTag, '{')) != NULL) && ((p2 = strchr(pTag, '}')) != NULL))
			strncpy((char *)md->Pld.Version.Comment, (char *)(p1 + 0x1), std::min(sizeof(md->Pld.Version.Comment), (UINT)(p2-p1-0x1)));
		else
			strncpy((char *)md->Pld.Version.Comment, "Invalid Pld Comments", sizeof(md->Pld.Version.Comment));
	}
	else md->Pld.Active = FALSE;

	// ADC Info
	md->Adc.Active = TRUE;
	strcpy((char *)md->Adc.Name, "LTC2245");
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E2010; i++)
	{
		md->Adc.OffsetCalibration[i] 	= ModuleDescriptor.AdcOffsetCoefs[i];
		md->Adc.ScaleCalibration[i] 	= ModuleDescriptor.AdcScaleCoefs[i];
	}
	strcpy((char *)md->Adc.Comment, "14-Bit 10Msps six pipelined stages ADC converter");

	// DAC Info
	if(ModuleDescriptor.IsDacPresented)
	{
		md->Dac.Active = TRUE;
		strcpy((char *)md->Dac.Name, "AD7249");
		for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E2010; i++)
		{
			md->Dac.OffsetCalibration[i] = ModuleDescriptor.DacOffsetCoefs[i];
			md->Dac.ScaleCalibration[i] = ModuleDescriptor.DacScaleCoefs[i];
		}
		strcpy((char *)md->Dac.Comment, "12-Bit 125Ksps DAC converter");
	}
	else md->Dac.Active = FALSE;

	// IO Info
	md->DigitalIo.Active = TRUE;
	strcpy((char *)md->DigitalIo.Name, "74HCT595");
	md->DigitalIo.InLinesQuantity = 16;
	md->DigitalIo.OutLinesQuantity = 16;
	strcpy((char *)md->DigitalIo.Comment, "TTL/CMOS compatible");

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ���������� � ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::SAVE_MODULE_DESCRIPTION(MODULE_DESCRIPTION_E2010 * const md)
{
	WORD i;

	// �������� ��������� �� ���������
	if(!md) { LAST_ERROR_NUMBER(3); return FALSE; }
	// �������� ������ ����� ������ - �� ����� ����� ������ ������
	// ���������� ���������� ������
	else if(IsDataAcquisitionInProgress) { LAST_ERROR_NUMBER(17); return FALSE; }

	// ������� ��� ��������� MODULE_DESCRIPTOR
	ZeroMemory(&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR));

	// Module Info
	strncpy((char *)ModuleDescriptor.ModuleName,		(char *)md->Module.DeviceName,	 	std::min(sizeof(md->Module.DeviceName), sizeof(ModuleDescriptor.ModuleName)));
	strncpy((char *)ModuleDescriptor.SerialNumber, 	(char *)md->Module.SerialNumber, 	std::min(sizeof(md->Module.SerialNumber), sizeof(ModuleDescriptor.SerialNumber)));
//	strncpy((char *)ModuleDescriptor.Reserved,     	(char *)md->Module.Comment,      	std::min(sizeof(md->Module.Comment), sizeof(ModuleDescriptor.Reserved)));

	// �������� ��������� ������� ������
	if((	md->Module.Revision > REVISIONS_E2010[REVISIONS_QUANTITY_E2010 - 0x1] ||
			md->Module.Revision < REVISIONS_E2010[REVISION_A_E2010])) { LAST_ERROR_NUMBER(35); return FALSE; }
	else ModuleDescriptor.Revision = md->Module.Revision;

	// �������� ��������� ���������� ������
	if(md->Module.Modification > F5_MODIFICATION_E2010) { LAST_ERROR_NUMBER(42); return FALSE; }
	ModuleDescriptor.Modification = md->Module.Modification;

	// MCU Info
	strncpy((char *)ModuleDescriptor.McuType, (char *)md->Mcu.Name, std::min(sizeof(md->Mcu.Name), sizeof(ModuleDescriptor.McuType)));
	ModuleDescriptor.ClockRate = 1000.0*MASTER_QUARTZ/8.0 + 0.5;	// � ��

	// ADC Info
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E2010; i++)
	{
		// �������� ��� ������ i
		if((fabs(md->Adc.OffsetCalibration[i]) > 2048.0) ||
			(fabs(md->Adc.OffsetCalibration[i]) < FloatEps)) md->Adc.OffsetCalibration[i] = 0.0;
		ModuleDescriptor.AdcOffsetCoefs[i] = md->Adc.OffsetCalibration[i];
		// ������� ��� ������ i
		if((fabs(md->Adc.ScaleCalibration[i]) > 1.9) ||
			(fabs(md->Adc.ScaleCalibration[i]) < FloatEps)) md->Adc.ScaleCalibration[i] = 1.0;
		ModuleDescriptor.AdcScaleCoefs[i]	= md->Adc.ScaleCalibration[i];
	}

	// DAC Info
	ModuleDescriptor.IsDacPresented = (BYTE)(md->Dac.Active ? 0x1 : 0x0);
	for(i = 0x0; i < DAC_CALIBR_COEFS_QUANTITY_E2010; i++)
	{
		// �������� - ���� �� ��� �� �����
		if(md->Dac.Active)
		{
			// �������� ��� ������ i
			if((fabs(md->Dac.OffsetCalibration[i]) > 512.0) ||
				(fabs(md->Dac.OffsetCalibration[i]) < FloatEps)) md->Dac.OffsetCalibration[i] = 0.0;
			ModuleDescriptor.DacOffsetCoefs[i] = md->Dac.OffsetCalibration[i];
			// ������� ��� ������ i
			if((fabs(md->Dac.ScaleCalibration[i]) > 1.9) ||
				(fabs(md->Dac.ScaleCalibration[i]) < FloatEps)) md->Dac.ScaleCalibration[i] = 1.0;
			ModuleDescriptor.DacScaleCoefs[i] = md->Dac.ScaleCalibration[i];
		}
		else
		{
			// �������� ��� ������ i
			ModuleDescriptor.DacOffsetCoefs[i] = 0.0;
			// ������� ��� ������ i
			ModuleDescriptor.DacScaleCoefs[i] = 1.0;
		}
	}

	// ������ �������� CRC16 ������������ ���������
	ModuleDescriptor.CRC16 = 0x0;
	ModuleDescriptor.CRC16 = CalculateCrc16((BYTE *)&ModuleDescriptor, sizeof(MODULE_DESCRIPTOR) - sizeof(WORD));

	// ��������� �������� �������������� ���������� ������
	if(!SaveModuleDescriptor()) { LAST_ERROR_NUMBER(29); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}




//==============================================================================
// ���������� ������� ��� ������ � ������� E20-10
//==============================================================================
//------------------------------------------------------------------------------
// ����� ���������� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::ResetPld(void)
{
	// ��������� ������ ������ ���������� ����
	WORD InBuf[4] = { 0x0, V_RESET_PLD, 0x0, 0x0 };

	// �������� ������ � ������
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����������� ���������� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::StartPld(void)
{
	// ��������� ������ ������� ����
	WORD InBuf[4] = { 0x0, V_START_PLD, 0x0, 0x0 };

	// �������� ������ � ������
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// �������� ���������� ����
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::LoadPld(BYTE * const PldData, DWORD NBytes)
{
	IO_REQUEST_LUSBAPI IoReq;

	// ��������� �����
	if(!PldData) { LAST_ERROR_NUMBER(3); return FALSE; }
	// ������������ ����� ���������� ����
	else if(!ResetPld()) { return FALSE; }
	// �������� ����� �������� ������  - Reset Write Pipe
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE1, NULL, 0, NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(19); return FALSE; }
	// �������� ����� ����� ������ - Reset Read Pipe
	else if(!TLUSBBASE::LDeviceIoControl(DIOC_RESET_PIPE3, NULL, 0, NULL, 0, TimeOut)) { LAST_ERROR_NUMBER(40); return FALSE; }
	// �������� ����
	else if(!PutArray(SEL_PLD_DATA, PldData, NBytes)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��������� ����
	else if(!StartPld()) { return FALSE; }

	// ��������� ��������� ������� ���� IO_REQUEST_LUSBAPI �� ���� ������
	IoReq.Buffer 					= (SHORT *)PldInfo;			// ����� ������
	if(ModuleDescriptor.Revision == REVISIONS_E2010[REVISION_A_E2010])
		IoReq.NumberOfWordsToPass 	= sizeof(PldInfo)/2;		// ���-�� ������ � SHORT
	else
		IoReq.NumberOfWordsToPass 	= sizeof(PldInfo)/2 - 0x1;	// ���-�� ������ � SHORT
	IoReq.NumberOfWordsPassed 	= 0x0;
	IoReq.Overlapped			  	= NULL;							// � ��� ����� ���������� ������
	IoReq.TimeOut					= TimeOut;						// ������� ���������� �������
	// ������ ����� ���������� ���� � ��������� ��������� PldInfo
	if(!ReadData(&IoReq))
	{
		// ������ ����������� ����� 255 ���� (510 ����)
		if(IoReq.NumberOfWordsPassed == 255) return TRUE;
		else { return FALSE; }
	}
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ �������� ������ ������ ������ (������ Rev.A)
//------------------------------------------------------------------------------
void WINAPI TLE2010::GetSynchroParsForRevA(BYTE ModeParam, ADC_PARS_E2010 * const AdcPars)
{
	// ����� �������� ����� ���������� ������� ������� ������ (������ ��� Rev.A)
	AdcPars->OverloadMode = (BYTE)((ModeParam & (0x1 << 0x3)) ? MARKER_OVERLOAD_E2010 : CLIPPING_OVERLOAD_E2010);

	// �������� �������� ������ ����� ����� � ���
	if(ModeParam & (0x1 << 0x0))
	{
		// ���������� ������� ������ ����� ������ ��� ���������� �� ������ ������
		if(ModeParam &(0x1 << 0x7)) AdcPars->SynchroPars.StartSource = INT_ADC_START_E2010;
		// ���������� ������� ������ ����� ������ � ����������� �� ������ ������
		else AdcPars->SynchroPars.StartSource = INT_ADC_START_WITH_TRANS_E2010;
	}
	else
	{
		// ������� ������� ������ ����� ������ �� ������
		if(!(ModeParam & (0x1 << 0x4))) AdcPars->SynchroPars.StartSource = EXT_ADC_START_ON_RISING_EDGE_E2010;
		// ������� ������� ������ ����� ������ �� �����
		else AdcPars->SynchroPars.StartSource = EXT_ADC_START_ON_FALLING_EDGE_E2010;
	}
	// �������� �������� ��������� ������� ���
	if(!(ModeParam & (0x1 << 0x1)))
	{
		// ���������� �������� �������� ��� ��� ���������� �� ������ ������
		if(ModeParam & (0x1 << 0x6)) AdcPars->SynchroPars.SynhroSource = INT_ADC_CLOCK_E2010;
		// ���������� �������� �������� ��� � ����������� �� ������ ������
		else AdcPars->SynchroPars.SynhroSource = INT_ADC_CLOCK_WITH_TRANS_E2010;
	}
	else
	{
		// ������� �������� �������� ���, �� ������
		if(ModeParam & (0x1 << 0x5)) AdcPars->SynchroPars.SynhroSource = EXT_ADC_CLOCK_ON_FALLING_EDGE_E2010;
		// ������� �������� �������� ���, �� �����
		else AdcPars->SynchroPars.SynhroSource = EXT_ADC_CLOCK_ON_RISING_EDGE_E2010;
	}
}

//------------------------------------------------------------------------------
// ������ �������� ������ ������ ������ (Rev.B � ����)
//------------------------------------------------------------------------------
void WINAPI TLE2010::GetSynchroParsForRevBandAbove(BYTE ModeParam, ADC_PARS_E2010 * const AdcPars)
{
	// ������ ���� ����� �������� ����� ���������� ������� ������� ������: �����������
	AdcPars->OverloadMode = CLIPPING_OVERLOAD_E2010;

	// ���������� �������� ������� ������ ����� ������ � ���
	if(ModeParam & (0x1 << 0x0))
	{
		// ���������� ������� ������ ����� ������ ��� ���������� �� ������ ������
		if(ModeParam &(0x1 << 0x7)) AdcPars->SynchroPars.StartSource = INT_ADC_START_E2010;
		// ���������� ������� ������ ����� ������ � ����������� �� ������ ������
		else AdcPars->SynchroPars.StartSource = INT_ADC_START_WITH_TRANS_E2010;
	}
	// ������� �������� ������� ������ ����� ������ � ���
	else AdcPars->SynchroPars.StartSource = (WORD)((ModeParam >> 0x3) & 0x3);

	// �������� �������� ��������� ������� ���
	if(!(ModeParam & (0x1 << 0x1)))
	{
		// ���������� �������� �������� ��� ��� ���������� �� ������ ������
		if(ModeParam & (0x1 << 0x6)) AdcPars->SynchroPars.SynhroSource = INT_ADC_CLOCK_E2010;
		// ���������� �������� �������� ��� � ����������� �� ������ ������
		else AdcPars->SynchroPars.SynhroSource = INT_ADC_CLOCK_WITH_TRANS_E2010;
	}
	else
	{
		// ������� �������� �������� ���, �� ������
		if(ModeParam & (0x1 << 0x5)) AdcPars->SynchroPars.SynhroSource = EXT_ADC_CLOCK_ON_FALLING_EDGE_E2010;
		// ������� �������� �������� ���, �� �����
		else AdcPars->SynchroPars.SynhroSource = EXT_ADC_CLOCK_ON_RISING_EDGE_E2010;
	}
}

//------------------------------------------------------------------------------
// ������ �������������� ���������� ������������� ����� ������ (��� Rev.B � ����)
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GetExtraSynchroParsForRevBandAbove(ADC_PARS_E2010 * const AdcPars)
{
	// ������� �������������� ��������� ������������� ����� ������
	if(!GetArray(EXTRA_SYNCHRO_PARS_ADDRESS, (BYTE *)&ExtraSynchroPars, sizeof(EXTRA_SYNCHRO_PARS))) { LAST_ERROR_NUMBER(16); return FALSE; }

	// �������� ���� ���������� �������������
	if(ExtraSynchroPars.SynchroAdMode & (0x1 << AD_ENA))
	{
		// ���������� ����� ��� ��� ���������� �������������
		AdcPars->SynchroPars.SynchroAdChannel = (WORD)(ExtraSynchroPars.SynchroAdMode & 0x3);
		// ����� ���������� ������������: ������� ��� �������
		AdcPars->SynchroPars.SynchroAdMode = (WORD)(((ExtraSynchroPars.SynchroAdMode >> AD_M0) & 0x3) + 0x1);
	}
	else AdcPars->SynchroPars.SynchroAdMode = NO_ANALOG_SYNCHRO_E2010;
	// ���������� ����� ������ ��� ���������� �������������
	AdcPars->SynchroPars.SynchroAdChannel = (WORD)(ExtraSynchroPars.SynchroAdMode & 0x3);
	// ����� ������������ ��� ���������� �������������
	AdcPars->SynchroPars.SynchroAdPorog = ExtraSynchroPars.SynchroAdPorog;
	// �������� ������ ����� ������ � ������ �������� ���
	AdcPars->SynchroPars.StartDelay = ExtraSynchroPars.StartDelay;
	// ������� ����� ������ ����� ����������� ����� ���-�� ��������� ������ �������� ���
	AdcPars->SynchroPars.StopAfterNKadrs = ExtraSynchroPars.StopAfterNKadrs;
	// ������������ ������ ������ �������� ������ (������, ��������, ��� ���������� ������������� ����� �� ������)
	AdcPars->SynchroPars.IsBlockDataMarkerEnabled = ExtraSynchroPars.IsBlockDataMarkerEnabled;

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������������ ������ ������ ������ (������ Rev.A)
//------------------------------------------------------------------------------
void WINAPI TLE2010::MakeSynchroParsForRevA(BYTE &ModeParam, ADC_PARS_E2010 * const AdcPars)
{
	// ����� ����� �������� ����� ���������� ������� ������� (������ ��� Rev.A)
	if(AdcPars->OverloadMode >= INVALID_OVERLOAD_E2010) AdcPars->OverloadMode = CLIPPING_OVERLOAD_E2010;
	AdcParamsArray[0] |= (BYTE)((AdcPars->OverloadMode ? 0x1 : 0x0) << 0x3);

	// �������� ���� ADC mode
	ModeParam = 0x0;
	// �������� �������� ������ ����� ������ � ���
	if(AdcPars->SynchroPars.StartSource <= INT_ADC_START_WITH_TRANS_E2010)
	{
		// ���������� ������� ������ ����� ������ � ����������� �� ������ ������
		ModeParam |= (0x1 << 0x0);
		// ���������� ������� ������ ����� ������ ��� ���������� �� ������ ������
		if(AdcPars->SynchroPars.StartSource == INT_ADC_START_E2010) ModeParam |= (0x1 << 0x7);
	}
	else
	{
		// �� ��� ������ ������������ �������������� ������ �������� ������
		if(AdcPars->SynchroPars.StartSource > EXT_ADC_START_ON_FALLING_EDGE_E2010)
							AdcPars->SynchroPars.StartSource = EXT_ADC_START_ON_FALLING_EDGE_E2010;
		// ������� ������� ������ ����� ������, �� ������
		ModeParam |= (0x1 << 0x2) | (0x1 << 0x7);
		// ������� �������� ������ ����� ������ �� �����
		if(AdcPars->SynchroPars.StartSource == EXT_ADC_START_ON_FALLING_EDGE_E2010) ModeParam |= (0x1 << 0x4);
	}
	// �������� �������� ��������� ������� ���
	if(AdcPars->SynchroPars.SynhroSource <= INT_ADC_CLOCK_WITH_TRANS_E2010)
	{
		// ���������� �������� �������� ��� � ����������� �� ������ ������
		// .....
		// ���������� �������� �������� ��� ��� ���������� �� ������ ������
		if(AdcPars->SynchroPars.SynhroSource == INT_ADC_CLOCK_E2010) ModeParam |= (0x1 << 0x6);
	}
	else
	{
		// ������� �������� �������� ���, �� ������
		ModeParam |= (0x1 << 0x1) | (0x1 << 0x6);
		// ������� �������� �������� ���, �� �����
		if(AdcPars->SynchroPars.SynhroSource == EXT_ADC_CLOCK_ON_FALLING_EDGE_E2010) ModeParam |= (0x1 << 0x5);
	}
}

//------------------------------------------------------------------------------
// ������������ ������ ������ ������ (Rev.B � ����)
//------------------------------------------------------------------------------
void WINAPI TLE2010::MakeSynchroParsForRevBandAbove(BYTE &ModeParam, ADC_PARS_E2010 * const AdcPars)
{
	// �������� ���� ADC mode
	ModeParam = 0x0;
	// �������� �������� ������ ����� ������ � ���
	if(AdcPars->SynchroPars.StartSource <= INT_ADC_START_WITH_TRANS_E2010)
	{
		// ���������� ������� ������ ����� ������ � ����������� �� ������ ������
		ModeParam |= (0x1 << 0x0);
		// ���������� ������� ������ ����� ������ ��� ���������� �� ������ ������
		if(AdcPars->SynchroPars.StartSource == INT_ADC_START_E2010) ModeParam |= (0x1 << 0x7);
	}
	else
	{
		// �������� �������� ����� �������� ������������ ����� ������
		if(AdcPars->SynchroPars.StartSource >= INVALID_ADC_START_E2010)
							AdcPars->SynchroPars.StartSource = EXT_ADC_START_ON_FALLING_EDGE_E2010;//EXT_ADC_START_ON_LOW_LEVEL_E2010;
		// ������� ������� ������ ����� ������
		ModeParam |= (0x1 << 0x2) | (0x1 << 0x7);
		// ��� �������� �������� ������ ����� ������: �� ������ ��� �����
		ModeParam |= (BYTE)(((AdcPars->SynchroPars.StartSource - 0x2) & 0x3) << 0x3);
	}
	// �������� �������� ��������� ������� ���
	if(AdcPars->SynchroPars.SynhroSource <= INT_ADC_CLOCK_WITH_TRANS_E2010)
	{
		// ���������� �������� �������� ��� � ����������� �� ������ ������
		// .....
		// ���������� �������� �������� ��� ��� ���������� �� ������ ������
		if(AdcPars->SynchroPars.SynhroSource == INT_ADC_CLOCK_E2010) ModeParam |= (0x1 << 0x6);
	}
	else
	{
		// ������� �������� �������� ���, �� ������
		ModeParam |= (0x1 << 0x1) | (0x1 << 0x6);
		// ������� �������� �������� ���, �� �����
		if(AdcPars->SynchroPars.SynhroSource == EXT_ADC_CLOCK_ON_FALLING_EDGE_E2010) ModeParam |= (0x1 << 0x5);
	}
}

//------------------------------------------------------------------------------
// �������������� ��������� ����� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::SetExtraSynchroParsForRevBandAbove(ADC_PARS_E2010 * const AdcPars)
{
	BYTE i;

	// �������� ����� ����������� ������ ���
	if(AdcPars->SynchroPars.SynchroAdChannel >= ADC_CHANNELS_QUANTITY_E2010)
			AdcPars->SynchroPars.SynchroAdChannel = ADC_CHANNELS_QUANTITY_E2010 - 0x1;
	// �������� ���� ���������� �������������
	if(AdcPars->SynchroPars.SynchroAdMode != NO_ANALOG_SYNCHRO_E2010)
	{
		// �������� ������� � ����������� ������� ����������� ������ ���������� �������������
		for(i = 0x0; i < AdcPars->ChannelsQuantity; i++)
			if(AdcPars->SynchroPars.SynchroAdChannel == AdcPars->ControlTable[i]) break;
		if(i == AdcPars->ChannelsQuantity)  { LAST_ERROR_NUMBER(41); return FALSE; }

		// ���������� ���������� �������������
		ExtraSynchroPars.SynchroAdMode = 0x1 << AD_ENA;
		// ����� ���������� ������������: ������� ��� �������
		ExtraSynchroPars.SynchroAdMode |= (WORD)(((AdcPars->SynchroPars.SynchroAdMode - 0x1) & 0x3) << AD_M0);
	}
	else ExtraSynchroPars.SynchroAdMode = 0x0;
	// ���������� ����� ��� ��� ���������� �������������
	ExtraSynchroPars.SynchroAdMode |= (WORD)(AdcPars->SynchroPars.SynchroAdChannel & 0x3);
	// ����� ������������ ��� ���������� �������������
	ExtraSynchroPars.SynchroAdPorog = AdcPars->SynchroPars.SynchroAdPorog;
	// �������� ���� �� ���������� ������������� �� ������
	if((AdcPars->SynchroPars.SynchroAdMode == ANALOG_SYNCHRO_ON_HIGH_LEVEL_E2010) ||
		(AdcPars->SynchroPars.SynchroAdMode == ANALOG_SYNCHRO_ON_LOW_LEVEL_E2010))
	{
		// �������� ������ ����� ������ � ������ �������� ���
		ExtraSynchroPars.StartDelay = AdcPars->SynchroPars.StartDelay = 0x0;
		// ������� ����� ������ ����� ����������� ����� ���-�� ��������� ������ �������� ���
		ExtraSynchroPars.StopAfterNKadrs = AdcPars->SynchroPars.StopAfterNKadrs = 0x0;
	}
	else
	{
		// �������� ������ ����� ������ � ������ �������� ���
		if(AdcPars->SynchroPars.StartDelay <= MAX_START_DELAY) ExtraSynchroPars.StartDelay = AdcPars->SynchroPars.StartDelay;
		else ExtraSynchroPars.StartDelay = AdcPars->SynchroPars.StartDelay = MAX_START_DELAY;
		// ������� ����� ������ ����� ����������� ����� ���-�� ��������� ������ �������� ���
		if(AdcPars->SynchroPars.StopAfterNKadrs <= MAX_STOP_AFTER_NKADRS) ExtraSynchroPars.StopAfterNKadrs = AdcPars->SynchroPars.StopAfterNKadrs;
		else ExtraSynchroPars.StopAfterNKadrs = AdcPars->SynchroPars.StopAfterNKadrs = MAX_STOP_AFTER_NKADRS;
	}
	// ������������ ������ ������ �������� ������ (������, ��������, ��� ���������� ������������� ����� �� ������)
	ExtraSynchroPars.IsBlockDataMarkerEnabled = (AdcPars->SynchroPars.IsBlockDataMarkerEnabled == 0x0) ? 0x0 : 0x1;

	// ������ ��������� ��������������� ������ � ���������������
	if(!PutArray(EXTRA_SYNCHRO_PARS_ADDRESS, (BYTE *)&ExtraSynchroPars, sizeof(EXTRA_SYNCHRO_PARS))) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GetModuleDescriptor(MODULE_DESCRIPTOR const * ModuleDescriptor)
{
	BOOL Error;
	BYTE *ptr;
	WORD crc16;

	// ��������� �������� ����� ��� ������ �������� sizeof(MODULE_DESCRIPTOR) ������
	ptr = new BYTE[sizeof(MODULE_DESCRIPTOR)];
	if(!ptr) { LAST_ERROR_NUMBER(32); return FALSE; }

	// ������� ���� ������
	Error = FALSE;
	// ������� ������ � �������
	do
	{
		// ������ �� ���� �������� ����� ��������� ���� MODULE_DESCRIPTOR
		if(!GetArray(MODULE_DESCRIPTOR_ADDRESS, ptr, sizeof(MODULE_DESCRIPTOR))) { LAST_ERROR_NUMBER(16); Error = TRUE; break; }
		// ��������� CRC ���������� �� ���� ��������� ������
		crc16 = (WORD)(((WORD)ptr[sizeof(MODULE_DESCRIPTOR) - 0x1] << 0x8) | ptr[sizeof(MODULE_DESCRIPTOR) - 0x2]);
		// ���� CRC16 = 0x0, �� ��������, ��� � ������ ���������� ��������� ��
		// ���������� ������ ���������� � ������ �� ����� ��������� � ����������� �����
		if(crc16)
		{
			// ����� �������� CRC ���������� �� ���� ��������� ������
			if(crc16 != CalculateCrc16(ptr, sizeof(MODULE_DESCRIPTOR) - sizeof(WORD))) { LAST_ERROR_NUMBER(33); Error = TRUE; break; }
		}
		// �� � ������� - �������� �� � ��������� ���� MODULE_DESCRIPTOR
		CopyMemory((BYTE *)ModuleDescriptor, ptr, sizeof(MODULE_DESCRIPTOR));
	}
	while(false);
	// ��������� ������
	delete[] ptr;

	if(Error) return FALSE;
	else return TRUE;
}

//------------------------------------------------------------------------------
// ������ ������� ������ �� ������ ����������������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GetArray(DWORD Address, BYTE * const Data, DWORD Size)
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
		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }

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
// ������ ������� ������ � ������ ����������������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::PutArray(DWORD Address, BYTE * const Data, DWORD Size)
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
		else if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), Data + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }

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
BOOL WINAPI TLE2010::RunMcuApplication(DWORD Address, WORD BitParam)
{
	// ��������� ������ ������� ���������� � ����������������
	WORD InBuf[4] = { 0x00, V_CALL_APPLICATION, (WORD)(Address & 0xFFFF), 0x0 };//(WORD)(Address >> 0x10) };
//	WORD InBuf[4] = { 0x00, V_CALL_APPLICATION, (WORD)(Address & 0xFFFF), (WORD)(Address >> 0x10) };
//	WORD InBuf[4] = { 0x00, 0x0, 0x0, 0x0 };

	// �������� �����
	if(Address == (DWORD)FIRMWARE_START_ADDRESS)
	{
		InBuf[0x3] = BitParam;
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
	if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, InBuf, sizeof(InBuf), NULL, 0x0, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }
	// �� ��������� ���������� � ����������������?
	IsMcuApplicationActive = (Address >= (DWORD)BOOT_LOADER_START_ADDRESS) ? FALSE : TRUE;
	// ��������� ������ ����������� ������
	LeaveCriticalSection(&cs);

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
// ������ ����������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::SaveModuleDescriptor(void)
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

		if(!TLUSBBASE::LDeviceIoControl(DIOC_SEND_COMMAND, &InBuf, sizeof(InBuf), ptr + i, sz, TimeOut)) { LeaveCriticalSection(&cs); LAST_ERROR_NUMBER(19); return FALSE; }
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

//------------------------------------------------------------------------------
// ������� ������ � �������������� �������� ������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::SetTestMode(WORD TestModeMask)
{
	WORD LocTestModeMask;

	// �������� �������� � ��������� ����������
	LocTestModeMask = TestModeMask;
	// ��������� ��������������� ������ � ���������������
	if(!PutArray(SEL_TEST_MODE, (BYTE *)&LocTestModeMask, 2)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}


//------------------------------------------------------------------------------
// ��������� ���������� ����������
//------------------------------------------------------------------------------
BOOL WINAPI TLE2010::GET_DEBUG_INFO(WORD * const DebugInfo)
{
	// ��������� ������ �� ��������� ���������� ����������
	if(!GetArray(SEL_DEBUG_INFO, (BYTE *)DebugInfo, 0x2)) { LAST_ERROR_NUMBER(18); return FALSE; }
	// ��� ������ :)))))
	else return TRUE;
}


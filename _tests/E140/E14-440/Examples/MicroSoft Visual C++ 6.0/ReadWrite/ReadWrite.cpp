//******************************************************************************
// ������ E14-440.
// ���������� ��������� � ������������ �������������� ���������� ����� � ������ ������.
// ���� �������������� � ������ ������ ������� ���. ������� ����� 100(Rev.A-C) ��� 250(Rev.D � ����) ���.
// ��������� �������������� ������ �� ������ ����� ���. ������� ������ 100(Rev.A-C) ��� 125(Rev.D � ����) ���.
//******************************************************************************
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <FCNTL.h>
#include <io.h>
#include "Lusbapi.h"

// ���-�� �������� ������ ������
#define		ADC_DATA_BLOCK_QUANTITY			(20)

// ��������� ����� �� ���������
void AbortProgram(char *ErrorString, bool AbortionFlag = true);
// ������� ������ ��������� � ��������
void ShowErrorMessage(void);
// ����������
WORD Round(double Data);

// ���������� ��������� pi
const double M_PI = 3.14159265358979323846;

// ***** ����� ����� ������ *****
// ������� ������ ����� ������ � ���
DWORD 	WINAPI ServiceReadThread(PVOID /*Context*/);
// �������� ���������� ������������ ������� �� ���� ������
BOOL WaitingForReadRequestCompleted(OVERLAPPED *ReadOv);
// ������������� ������ �����
HANDLE 	hReadThread;
DWORD 	ReadTid;
// ******************************

// ***** ����� ������ ������ *****
// ������� ������ ������ ������ �� ���
DWORD 	WINAPI ServiceWriteThread(PVOID /*Context*/);
// �������� ���������� ������������ ������� �� ����� ������
BOOL WaitingForWriteRequestCompleted(OVERLAPPED *ReadOv);
// ������������� ������ ������
HANDLE 	hWriteThread;
DWORD 	WriteTid;
// ******************************

// ������������� ����� ��� ���������� ������
HANDLE hFile;
DWORD FileBytesWritten;

// ������ ����������
DWORD DllVersion;
// ��������� �� ��������� ������
ILE440 *pModule;
// ���������� ����������
HANDLE ModuleHandle;
// �������� ������
char ModuleName[7];
// �������� ������ ���� USB
BYTE UsbSpeed;
// ��������� � ������ ����������� � ������
MODULE_DESCRIPTION_E440 ModuleDescription;
// ��������� ���������� ������ ��� ������
ADC_PARS_E440 ap;
// ��������� ���������� ������ ���
DAC_PARS_E440 dp;
// ������� ������ ��� � ���
double AdcRate, DacRate;

//max ��������� ���-�� ������������ �������� (������� 32) ��� �. ReadData � WriteData()
DWORD DataStep = 256*1024;
// ����� ���-�� ������ ���  ����� � ���
DWORD PointsToRead = ADC_DATA_BLOCK_QUANTITY * DataStep;
// ��������� �� ����� ��� ������ � ���
SHORT	*ReadBuffer;
// ��������� �� ����� ������ ��� ���
WORD 	*WriteBuffer;

// ��������� �������, ���������� �� ���
const WORD DacNumber = 0x0;						// ����� ������ ���, �� ������� ����� ���������� ������
const double SignalFrequency = 0.73;//0.073; 	  		// � ���
const double SignalAmplitude = 2000;			// � ����� ���

// ������ ���������� ������� ����� � ������ ������
bool IsReadThreadComplete, IsWriteThreadComplete;
// ����� ������ ��� ���������� ����� ������
WORD ReadThreadErrorNumber;
// ����� ������ ��� ���������� ������ ������ ������ �� ���
WORD WriteThreadErrorNumber;

// �������� �������-���������
DWORD Counter = 0x0, OldCounter = 0xFFFFFFFF;

//------------------------------------------------------------------------
// �������� ���������
//------------------------------------------------------------------------
void main(void)
{
	WORD i;
	SHORT DacSample;

	// ������� ������ ���������� ������ ����� ������
	IsReadThreadComplete = false;
	// ���� ������ �� �������� ��� ����� ������
	ReadBuffer = NULL;
	// ���� �� ������ ����� ����� ������
	hReadThread = NULL;
	// ���� �������� ����� ��� :(
	hFile = INVALID_HANDLE_VALUE;
	// ������� ���� ������ ������ ����� ������
	ReadThreadErrorNumber = 0x0;

	// ������� ������ ���������� ������ ������ ������
	IsWriteThreadComplete = false;
	// ���� ������ �� �������� ��� ����� ������
	WriteBuffer = NULL;
	// ���� �� ������ ����� ����� ������
	hWriteThread = NULL;
	// ������� ���� ������ ������ ����� ������
	WriteThreadErrorNumber = 0x0;

	// �������� ����� ��������
	system("cls");

	printf(" ************************************\n");
	printf(" Module E14-440                      \n");
	printf(" Console example for ADC&DAC Streams \n");
	printf(" ************************************\n\n");

	// �������� ������ ������������ ���������� Lusbapi.dll
	if((DllVersion = GetDllVersion()) != CURRENT_VERSION_LUSBAPI)
	{
		char String[128];
		sprintf(String, " Lusbapi.dll Version Error!!!\n   Current: %1u.%1u. Required: %1u.%1u",
											DllVersion >> 0x10, DllVersion & 0xFFFF,
											CURRENT_VERSION_LUSBAPI >> 0x10, CURRENT_VERSION_LUSBAPI & 0xFFFF);

		AbortProgram(String);
	}
	else printf(" Lusbapi.dll Version --> OK\n");

	// ��������� �������� ��������� �� ���������
	pModule = static_cast<ILE440 *>(CreateLInstance("e440"));
	if(!pModule) AbortProgram(" Module Interface --> Bad\n");
	else printf(" Module Interface --> OK\n");

	// ��������� ���������� ������ E14-440 � ������ MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI ����������� ������
	for(i = 0x0; i < MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI; i++) if(pModule->OpenLDevice(i)) break;
	// ���-������ ����������?
	if(i == MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI) AbortProgram(" Can't find any module E14-440 in first 127 virtual slots!\n");
	else printf(" OpenLDevice(%u) --> OK\n", i);

	// ��������� ��������� ���������� ����������
	ModuleHandle = pModule->GetModuleHandle();
	if(ModuleHandle == INVALID_HANDLE_VALUE) AbortProgram(" GetModuleHandle() --> Bad\n");
	else printf(" GetModuleHandle() --> OK\n");

	// ��������� �������� ������ � ������������ ����������� �����
	if(!pModule->GetModuleName(ModuleName)) AbortProgram(" GetModuleName() --> Bad\n");
	else printf(" GetModuleName() --> OK\n");
	// ��������, ��� ��� 'E14-440'
	if(strcmp(ModuleName, "E440")) AbortProgram(" The module is not 'E14-440'\n");
	else printf(" The module is 'E14-440'\n");

	// ��������� �������� �������� ������ ���� USB
	if(!pModule->GetUsbSpeed(&UsbSpeed)) AbortProgram(" GetUsbSpeed() --> Bad\n");
	else printf(" GetUsbSpeed() --> OK\n");
	// ������ ��������� �������� ������ ���� USB
	printf("   USB is in %s\n", UsbSpeed ? "High-Speed Mode (480 Mbit/s)" : "Full-Speed Mode (12 Mbit/s)");

	// ��� LBIOS'� ������ �� ���������������� ������� ������� DLL ����������
	if(!pModule->LOAD_MODULE()) AbortProgram(" LOAD_MODULE() --> Bad\n");
	else printf(" LOAD_MODULE() --> OK\n");

	// �������� �������� ������
 	if(!pModule->TEST_MODULE()) AbortProgram(" TEST_MODULE() --> Bad\n");
	else printf(" TEST_MODULE() --> OK\n");

	// ������� ���������� �� ���� ������
	if(!pModule->GET_MODULE_DESCRIPTION(&ModuleDescription)) AbortProgram(" GET_MODULE_DESCRIPTION() --> Bad\n");
	else printf(" GET_MODULE_DESCRIPTION() --> OK\n");

	// �������� ���� �� �� ������ ���
	if(ModuleDescription.Dac.Active == DAC_INACCESSIBLED_E440) AbortProgram(" DAC is absent on this module E14-440!\n");
	//  
	if((ModuleDescription.Module.Revision >= 'A') && (ModuleDescription.Module.Revision <= 'C'))
		{ AdcRate = DacRate = 100.0; }
	else { AdcRate = 250.0; DacRate = 125.0; }

	// ***** ��������� ��� *****
	// ������� ������� ��������� ������ ���
	if(!pModule->GET_ADC_PARS(&ap)) AbortProgram(" GET_ADC_PARS() --> Bad\n");
	else printf(" GET_ADC_PARS() --> OK\n");

	// ��������� �������� ��������� ������ ���
	ap.IsCorrectionEnabled = TRUE;			// �������� ������������� ������ �� ������ �������� DSP
	ap.InputMode = NO_SYNC_E440;				// ������� ���� ������ ���� ������ ������������� �����
	ap.ChannelsQuantity = 0x4;					// ������ �������� ������
	// ��������� ����������� �������
	for(i = 0x0; i < ap.ChannelsQuantity; i++)
//		ap.ControlTable[i] = (WORD)(i | (ADC_INPUT_RANGE_2500mV_E440 << 0x6));
		ap.ControlTable[i] = (WORD)(i | (ADC_INPUT_RANGE_10000mV_E440 << 0x6));
	ap.AdcRate = AdcRate;						// ������� ������ ��� � ���
	ap.InterKadrDelay = 0.0;					// ����������� �������� � ��
	ap.AdcFifoBaseAddress = 0x0;			  			// ������� ����� FIFO ������ ��� � DSP ������
	ap.AdcFifoLength = MAX_ADC_FIFO_SIZE_E440;	// ����� FIFO ������ ��� � DSP ������
	// ����� ������������ ��������� ������������� ������������, ������� ��������� � ���� ������
	for(i = 0x0; i < ADC_CALIBR_COEFS_QUANTITY_E440; i++)
	{
		ap.AdcOffsetCoefs[i] =  ModuleDescription.Adc.OffsetCalibration[i];
		ap.AdcScaleCoefs[i] =  ModuleDescription.Adc.ScaleCalibration[i];
	}

	// ��������� ��������� ��������� ������ ��� � ������
	if(!pModule->SET_ADC_PARS(&ap)) AbortProgram(" SET_ADC_PARS() --> Bad\n");
	else printf(" SET_ADC_PARS() --> OK\n");
	// *************************


	// ***** ��������� ��� *****
	// ������� ������� ��������� ������ ���
	if(!pModule->GET_DAC_PARS(&dp)) AbortProgram(" GET_CUR_DAC_PARS() --> Bad\n");
	else printf(" GET_CUR_DAC_PARS() --> OK\n");

	// ��������� �������� ��������� ���
	dp.DacRate = DacRate;								// ������� ������ ��� � ���
	dp.DacFifoLength = MAX_DAC_FIFO_SIZE_E440;  	// ����� FIFO ������ ��� � DSP ������

	if(!pModule->SET_DAC_PARS(&dp)) AbortProgram(" SET_DAC_PARS() --> Bad\n");
	else printf(" SET_DAC_PARS() --> OK\n");
	// *************************

	// ��������� �������� ������ ��� ����� ���
	ReadBuffer = new SHORT[PointsToRead];
	if(!ReadBuffer) AbortProgram(" Can not allocate memory\n");
	// ��������� �������� ������ ��� ����� ���
	WriteBuffer = new WORD[2*DataStep];
	if(!WriteBuffer) AbortProgram(" Cannot allocate WriteBuffer memory\n");

	// ������� ���� ��� ������ ���������� � ������ ������
	hFile = CreateFile("Test.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_WRITE_THROUGH, NULL);
	if(hFile == INVALID_HANDLE_VALUE) AbortProgram("\n Can't create file 'Test.dat'!\n");

	// �������� ������� �������� �� ������� ����� ������� ���
	DacSample = (SHORT)(0x0);
	if(!pModule->DAC_SAMPLE(&DacSample, WORD(0x0))) AbortProgram(" DAC_SAMPLE(0) --> Bad\n");
	else if(!pModule->DAC_SAMPLE(&DacSample, WORD(0x1))) AbortProgram(" DAC_SAMPLE(1) --> Bad\n");

	// ������� � ��������� ����� ����� ������ � ���
	printf("\n");
	hReadThread = CreateThread(0, 0x2000, ServiceReadThread, 0, 0, &ReadTid);
	if(!hReadThread) AbortProgram(" ServiceReadThread() --> Bad\n");
	else printf(" ServiceReadThread() --> OK\n");
	// ������� � ��������� ����� ������ ������ �� ���
	hWriteThread = CreateThread(0, 0x2000, ServiceWriteThread, 0, 0, &WriteTid);
	if(!hWriteThread) AbortProgram(" ServiceWriteThread() --> Bad\n");
	else printf(" ServiceWriteThread() --> OK\n");

	// ��������� ��������� ������ ������ �� ������ ��������
	printf(" \n");
	printf(" Module E14-440 (S/N %s) is ready ... \n", ModuleDescription.Module.SerialNumber);
	printf("   Module Info:\n");
	printf("     Module  Revision   is '%c'\n", ModuleDescription.Module.Revision);
	printf("     AVR Driver Version is %s (%s)\n", ModuleDescription.Mcu.Version.Version, ModuleDescription.Mcu.Version.Date);
	printf("     LBIOS   Version    is %s (%s)\n", ModuleDescription.Dsp.Version.Version, ModuleDescription.Dsp.Version.Date);
	printf("  Adc parameters:\n");
	printf("     Data Correction is %s\n", ap.IsCorrectionEnabled ? "enabled" : "disabled");
	printf("     ChannelsQuantity = %2d\n", ap.ChannelsQuantity);
	printf("     AdcRate = %8.3f kHz\n", ap.AdcRate);
	printf("     InterKadrDelay = %2.4f ms\n", ap.InterKadrDelay);
	printf("     KadrRate = %8.3f kHz\n", ap.KadrRate);
	printf("     FIFO Length = 0x%4X(%5u)\n", ap.AdcFifoLength, ap.AdcFifoLength);
	printf("  Dac parameters:\n");
	printf("     DacRate = %8.3f kHz\n", dp.DacRate);
	printf("     FIFO Length = 0x%04X(%5u)\n", dp.DacFifoLength, dp.DacFifoLength);
	printf("  Dac Signal parameters:\n");
	printf("    Frequency = %5.3f kHz\n", SignalFrequency);
	printf("    Amplitude = %d DAC code\n\n", (WORD)SignalAmplitude);

	// ���� ���������� ������ ����� �������
	printf("\n Counter %3u from %3u\r", Counter, ADC_DATA_BLOCK_QUANTITY);
	while(!IsReadThreadComplete || !IsWriteThreadComplete)
	{
		if(OldCounter != Counter) { printf(" Counter %3u from %3u\r", Counter, ADC_DATA_BLOCK_QUANTITY); OldCounter = Counter; }
		Sleep(20);
	}

	// ��� ��������� ������ ������ ����� ������
	WaitForSingleObject(hReadThread, INFINITE);
	// ��� ��������� ������ ������ ������ ������
	WaitForSingleObject(hWriteThread, INFINITE);

	// �������� ���� �� ������ ���������� ������� �����/������ ������
	printf("\n\n");
	if(ReadThreadErrorNumber | WriteThreadErrorNumber) { AbortProgram(NULL, false); ShowErrorMessage(); }
	else AbortProgram(" The program was completed successfully!!!\n", false);
}

//------------------------------------------------------------------------
// ����� � ������� �������������� ���� ������ � ���
//------------------------------------------------------------------------
DWORD WINAPI ServiceReadThread(PVOID /*Context*/)
{
	WORD i;
	WORD RequestNumber;
	// ������ OVERLAPPED �������� �� ���� ���������
	OVERLAPPED ReadOv[2];
	// ������ �������� � ����������� ������� �� ����/����� ������
	IO_REQUEST_LUSBAPI IoReq[2];

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(!pModule->STOP_ADC()) { ReadThreadErrorNumber = 0x1; IsReadThreadComplete = true; return 0x0; }

	// ��������� ����������� ��� ����� ������ ���������
	for(i = 0x0; i < 0x2; i++)
	{
		// ������������� ��������� ���� OVERLAPPED
		ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED));
		// ������ ������� ��� ������������ �������
		ReadOv[i].hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
		// ��������� ��������� IoReq
		IoReq[i].Buffer = ReadBuffer + i*DataStep;
		IoReq[i].NumberOfWordsToPass = DataStep;
		IoReq[i].NumberOfWordsPassed = 0x0;
		IoReq[i].Overlapped = &ReadOv[i];
		IoReq[i].TimeOut = (DWORD)(DataStep/ap.AdcRate + 1000);
	}

	// ������ ��������������� ������ �� ���� ������
	RequestNumber = 0x0;
	if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadOv[0].hEvent); CloseHandle(ReadOv[1].hEvent); ReadThreadErrorNumber = 0x2; IsReadThreadComplete = true; return 0x0; }

	// �������� ���
	if(pModule->START_ADC())
	{
		// ���� ����� ������
		for(i = 0x1; i < ADC_DATA_BLOCK_QUANTITY; i++)
		{
			// ������� ������ �� ��������� ������ ������
			RequestNumber ^= 0x1;
			if(!pModule->ReadData(&IoReq[RequestNumber])) { ReadThreadErrorNumber = 0x2; break; }
			if(ReadThreadErrorNumber) break;

			// ��� ���������� �������� ����� ���������� ������ ������
			if(!WaitingForReadRequestCompleted(IoReq[RequestNumber^0x1].Overlapped)) break;
//			if(WaitForSingleObject(IoReq[RequestNumber^0x1].Overlapped->hEvent, IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) { ReadThreadErrorNumber = 0x3; break; }
			if(ReadThreadErrorNumber) break;

			// ������� ���������� ������ ������ � ����
			if(!WriteFile(	hFile,													// handle to file to write to
		    					IoReq[RequestNumber^0x1].Buffer,					// pointer to data to write to file
								2*DataStep,	 											// number of bytes to write
	    						&FileBytesWritten,									// pointer to number of bytes written
						   	NULL			  											// pointer to structure needed for overlapped I/O
							   )) { ReadThreadErrorNumber = 0x4; break; }

			if(ReadThreadErrorNumber) break;
			else if(kbhit()) { ReadThreadErrorNumber = 0x5; break; }
			else Sleep(20);
			Counter++;
		}

		// ��������� ������ ������
		if(!ReadThreadErrorNumber)
		{
			RequestNumber ^= 0x1;
			// ��� ��������� �������� ����� ��������� ������ ������
			if(WaitForSingleObject(ReadOv[RequestNumber^0x1].hEvent, IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) ReadThreadErrorNumber = 0x3;
			// ������� ��������� ������ ������ � ����
			if(!WriteFile(	hFile,													// handle to file to write to
		    					IoReq[RequestNumber^0x1].Buffer,					// pointer to data to write to file
								2*DataStep,	 											// number of bytes to write
	    						&FileBytesWritten,									// pointer to number of bytes written
						   	NULL			  											// pointer to structure needed for overlapped I/O
							   )) ReadThreadErrorNumber = 0x4;
			Counter++;
		}
	}
	else { ReadThreadErrorNumber = 0x6; }

	// ��������� ������ ���
	if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x1;
	// ������ �������� ������������� ����������� ������ �� ���� ������
	if(!CancelIo(ModuleHandle)) { ReadThreadErrorNumber = 0x7; }
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(ReadOv[i].hEvent);
	// ��������� ��������
	Sleep(100);
	// ��������� ������ ���������� ������ ������ ����� ������
	IsReadThreadComplete = true;
	// ������ ����� �������� �������� �� ������
	return 0x0;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
BOOL WaitingForReadRequestCompleted(OVERLAPPED *ReadOv)
{
	DWORD ReadBytesTransferred;

	while(TRUE)
	{
		if(GetOverlappedResult(ModuleHandle, ReadOv, &ReadBytesTransferred, FALSE)) break;
		else if(GetLastError() !=  ERROR_IO_INCOMPLETE) { ReadThreadErrorNumber = 0x3; return FALSE; }
		else if(kbhit()) { ReadThreadErrorNumber = 0x5; return FALSE; }
		else Sleep(20);
	}
	return TRUE;
}

//------------------------------------------------------------------------
// ����� � ������� �������������� ������ ������ �� ���
//------------------------------------------------------------------------
DWORD WINAPI ServiceWriteThread(PVOID /*Context*/)
{
	WORD RequestNumber;
	DWORD i, BaseIndex;
	// OVERLAPPED ���������
	OVERLAPPED WriteOv[2];
	// ������ �������� � ����������� ������� �� ����/����� ������
	IO_REQUEST_LUSBAPI IoReq[2];
	// ��������� ���������� �������
	double CurrentTime;						// ������� ����� � ���

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(!pModule->STOP_DAC()) { WriteThreadErrorNumber = 0x6; IsWriteThreadComplete = true; return 0; }

	// ��������� ������ ��� ������ FIFO ������ ��� � ������
	CurrentTime = 0.0;
	for(i = 0; i < (DWORD)dp.DacFifoLength; i++)
	{
		WriteBuffer[i] = Round(SignalAmplitude*sin(2.0*M_PI*SignalFrequency*CurrentTime));
		WriteBuffer[i] &= (WORD)(0x0FFF);
		WriteBuffer[i] |= (WORD)(DacNumber << 12);
		CurrentTime += 1.0/dp.DacRate;
	}
/*
// !!! ������� !!!
for(i = 0; i < (DWORD)15; i++)
{
	WriteBuffer[i] = SignalAmplitude;
	WriteBuffer[i] &= (WORD)(0xFFF);
}
*/
	// ��������� ������� FIFO ����� ��� � ������
	if(!pModule->PUT_DM_ARRAY(dp.DacFifoBaseAddress, dp.DacFifoLength, (SHORT *)WriteBuffer))
   										{ WriteThreadErrorNumber = 0x1; IsWriteThreadComplete = true; return 1; }

	// ������ ��������� ������ ������� ��� ����� ������ WriteBuffer
	for(i = 0x0; i < 2*DataStep; i++)
	{
		WriteBuffer[i] = Round(SignalAmplitude*sin(2.0*M_PI*SignalFrequency*CurrentTime));
		WriteBuffer[i] &= (WORD)(0x0FFF);
		WriteBuffer[i] |= (WORD)(DacNumber << 12);
		CurrentTime += 1.0/dp.DacRate;
	}
/*
// !!! ������� !!!
for(i = 0; i < (DWORD)15; i++)
{
	WriteBuffer[i] = -SignalAmplitude;
	WriteBuffer[i] &= (WORD)(0xFFF);
}
*/
	// ��������� ���������, ����������� ��� ���������� ������ ������
	for(i = 0x0; i < 0x2; i++)
	{
		// ������������� ��������� ���� OVERLAPPED
		ZeroMemory(&WriteOv[i], sizeof(OVERLAPPED));
		// ������ ������� ��� ������������ �������
		WriteOv[i].hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
		// ��������� ��������� IoReq
		IoReq[i].Buffer = (SHORT *)(WriteBuffer + i*DataStep);
		IoReq[i].NumberOfWordsToPass = DataStep;
		IoReq[i].NumberOfWordsPassed = 0x0;
		IoReq[i].Overlapped = &WriteOv[i];
		IoReq[i].TimeOut = (DWORD)(DataStep/dp.DacRate + 1000);
	}

	// ��� ������� � �������� 'D' � ���� ����� ������ ���������������,
	// �.�. �� ������� START_DAC(), ������ �� ����� ������
	if(('D' <= ModuleDescription.Module.Revision) && (ModuleDescription.Module.Revision <= 'Z'))
	{
		RequestNumber = 0x0;
		if(!pModule->WriteData(&IoReq[RequestNumber])) { CloseHandle(WriteOv[0].hEvent); CloseHandle(WriteOv[1].hEvent); WriteThreadErrorNumber = 0x2; IsWriteThreadComplete = true; return 0; }
	}

	// �������� ���
	if(pModule->START_DAC())
	{
		// ��� ������ ������� ������ (�� 'A' �� 'C') ������ ������ ��
		// ����� ������ ������ ����������� ����� ������� START_DAC()
		if(('A' <= ModuleDescription.Module.Revision) && (ModuleDescription.Module.Revision <= 'C'))
		{
			RequestNumber = 0x0;
			if(!pModule->WriteData(&IoReq[RequestNumber])) { CloseHandle(WriteOv[0].hEvent); CloseHandle(WriteOv[1].hEvent); WriteThreadErrorNumber = 0x2; IsWriteThreadComplete = true; return 0; }
		}
		// ������������ ���� ������ ������ �� ���
		while(!IsReadThreadComplete)
		{
			// ������� ������ �� ��������� ������ ������������ ������
			RequestNumber ^= 0x1;
			if(!pModule->WriteData(&IoReq[RequestNumber])) { WriteThreadErrorNumber = 0x2; break; }
			if(WriteThreadErrorNumber) break;

			// ���� ���������� ����������� �������� ������ ������ � ������
//			if(!WaitingForWriteRequestCompleted(IoReq[RequestNumber^0x1].Overlapped)) break;
			if(WaitForSingleObject(WriteOv[RequestNumber^0x1].hEvent, IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) { WriteThreadErrorNumber = 0x4; break; }
			if(WriteThreadErrorNumber) break;

			// ���������� ��������� ������ ������ ��� ������ ��������� ������ WriteBuffer
			BaseIndex = (RequestNumber^0x1)*DataStep;
			for(i = 0x0; i < DataStep; i++)
			{
				WriteBuffer[i + BaseIndex] = Round(SignalAmplitude*sin(2.0*M_PI*SignalFrequency*CurrentTime));
				WriteBuffer[i + BaseIndex] &= (WORD)(0x0FFF);
				WriteBuffer[i + BaseIndex] |= (WORD)(DacNumber << 12);
				CurrentTime += 1.0/dp.DacRate;
	  		}

			// ���� ����� ����� ��������, �� ������ ������
			if(WriteThreadErrorNumber) break;
			else if(IsReadThreadComplete) break;
			else if(kbhit()) { WriteThreadErrorNumber = 0x3; break; }
			else Sleep(20);
		}
	}
	else { WriteThreadErrorNumber = 0x5; }

	// ��������� ������ ���
	if(!pModule->STOP_DAC()) WriteThreadErrorNumber = 0x6;

	// ������ �������� ������������� ����������� ������ �� ���� ������
	if(!CancelIo(ModuleHandle)) WriteThreadErrorNumber = 0x7;
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(IoReq[i].Overlapped->hEvent);
	// ��������� ��������
	Sleep(100);
	// ��������� ������ ��������� ������ ������ ������
	IsWriteThreadComplete = true;
	// ������ ����� �������� �������� �� ������
	return 0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
WORD Round(double Data)
{
	if(Data > 0.0) Data += 0.5;
	else if(Data < 0.0) Data += -0.5;
	return (WORD)Data;
}

//------------------------------------------------------------------------------
// �������� ���������� ���������� ���������� ������� �� ����� ������
//------------------------------------------------------------------------------
BOOL WaitingForWriteRequestCompleted(OVERLAPPED * const WriteOv)
{
	DWORD BytesTransferred;

	while(true)
	{
			if(GetOverlappedResult(ModuleHandle, WriteOv, &BytesTransferred, FALSE)) break;
			else if(GetLastError() !=  ERROR_IO_INCOMPLETE) { WriteThreadErrorNumber = 0x2; return FALSE; }
			else if(IsReadThreadComplete) return FALSE;
			else Sleep(20);
	}
	return TRUE;
}

//------------------------------------------------------------------------------
// ��������� ��������� � �������
//------------------------------------------------------------------------------
void ShowErrorMessage(void)
{
	switch(ReadThreadErrorNumber)
	{
		case 0x1:
			printf(" ADC Thread: STOP_ADC() --> Bad\n");
			break;

		case 0x2:
			printf(" ADC Thread: ReadData() --> Bad\n");
			break;

		case 0x3:
			printf(" ADC Thread: Timeout is occured!\n");
			break;

		case 0x4:
			printf(" ADC Thread: Writing file error!\n");
			break;

		case 0x5:
			// ���� ��������� ���� ������ ��������, ��������� ���� ��������
			printf(" ADC Thread: The program was terminated!\n");
			break;

		case 0x6:
			printf(" ADC Thread: START_ADC() --> Bad\n");
			break;

		case 0x7:
			printf(" ADC Thread: Can't cancel ending input and output (I/O) operations!\n");
			break;

		default:
			printf(" Unknown error!\n");
			break;
	}

	switch(WriteThreadErrorNumber)
	{
		case 0x0:
			break;

		case 0x1:
			printf(" DAC Thread: PUT_DM_ARRAY() --> Bad! :(((\n");
			break;

		case 0x2:
			printf(" DAC Thread: WriteData() --> Bad :(((\n");
			break;

		case 0x3:
			// ���� ��������� ���� ������ ��������, ��������� ���� ��������
			printf(" DAC Thread: The program was terminated! :(((\n");
			break;

		case 0x4:
			printf(" DAC Thread: Timeout is occured :(((\n");
			break;

		case 0x5:
			printf(" DAC Thread: START_DAC() --> Bad :(((\n");
			break;

		case 0x6:
			printf(" DAC Thread: STOP_DAC() --> Bad! :(((\n");
			break;

		case 0x7:
			printf("\n DAC Thread: Can't complete input and output (I/O) operations! :(((");
			break;

		default:
			printf(" DAC Thread: Unknown error! :(((\n");
			break;
	}

	printf("\n");

	return;
}

//------------------------------------------------------------------------
// ��������� ���������� ���������
//------------------------------------------------------------------------
void AbortProgram(char *ErrorString, bool AbortionFlag)
{
	SHORT DacSample;

	// ��������� ��������� ������
	if(pModule)
	{
		// �������� ������� �������� �� ������� ����� ������� ���
		DacSample = (SHORT)(0x0);
		if(!pModule->DAC_SAMPLE(&DacSample, WORD(0x0))) printf(" DAC_SAMPLE(0) --> Bad\n");
		else if(!pModule->DAC_SAMPLE(&DacSample, WORD(0x1))) printf(" DAC_SAMPLE(1) --> Bad\n");

		// ��������� ��������� ������
		if(!pModule->ReleaseLInstance()) printf(" ReleaseLInstance() --> Bad\n");
		else printf(" ReleaseLInstance() --> OK\n");
		// ������� ��������� �� ��������� ������
		pModule = NULL;
	}
	// ��������� ������ ������ ����� ������
	if(ReadBuffer) { delete[] ReadBuffer; ReadBuffer = NULL; }
	// ��������� ������������� ������ ����� ������
	if(hReadThread) { CloseHandle(hReadThread); hReadThread = NULL; }
	// ��������� ������ ������ ������ ������
	if(WriteBuffer) { delete[] WriteBuffer; WriteBuffer = NULL; }
	// ��������� ������������� ������ ������ ������
	if(hWriteThread) { CloseHandle(hWriteThread); hWriteThread = NULL; }
	// ��������� ������������� ����� ������
	if(hFile != INVALID_HANDLE_VALUE) { CloseHandle(hFile); hFile = INVALID_HANDLE_VALUE; }

	// ������� ����� ���������
	if(ErrorString) printf(ErrorString);

	// ��������� ������� ����������
	if(kbhit()) { while(kbhit()) getch(); }

	// ���� ����� - �������� ��������� ���������
	if(AbortionFlag) exit(0x1);
	else return;
}


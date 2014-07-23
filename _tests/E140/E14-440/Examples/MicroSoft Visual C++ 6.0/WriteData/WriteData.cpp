//******************************************************************************
// ������ E14-440.
// ���������� ��������� � ������������ ���������� ������ ������ �� ���.
// ��������� �������������� ������ �� ������ ����� ���. ������ ������ 125 ���.
//******************************************************************************
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "Lusbapi.h"

// ��������� ����� �� ���������
void AbortProgram(char *ErrorString, bool AbortionFlag = true);
// ������� ������ ������ ������ �� ���
DWORD WINAPI ServiceWriteThread(PVOID /*Context*/);
// �������� ���������� ������������ ������� �� ����� ������
BOOL WaitingForRequestCompleted(OVERLAPPED *ReadOv);
// ������� ������ ��������� � ��������
void ShowErrorMessage(void);
// ����������
WORD Round(double Data);

// ���������� ��������� pi
const double M_PI = 3.14159265358979323846;

// ������������� ������
HANDLE 	hWriteThread;
DWORD 	WriteTid;

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
// ��������� ���������� ������ ���
DAC_PARS_E440 dp;

// ���-�� ������������ �������� (������� 32) ��� �. WriteData()
DWORD DataStep = 128*1024;
// ����� ������
WORD *WriteBuffer;
// ����� ������ ���, �� ������� ����� ���������� ������
WORD DacNumber = 0x0;
// ��������� ���������� �������
double SignalFrequency = 1.2;	  		// ������� ������� � ���
WORD SignalAmplitude = 2000;      	// ��������� ������� � ����� ���

// ������ ���������� ������ ������ ������ ������ �� ���
bool IsWriteThreadComplete;
// ����� ������ ��� ���������� ������ ������ ������ �� ���
WORD WriteThreadErrorNumber;

// �������� �������-���������
DWORD Counter;

//------------------------------------------------------------------------------
// �������� ���������
//------------------------------------------------------------------------------
void main(void)
{
	WORD i;

	// ���� ������ �� �������� ��� ����� ������
	WriteBuffer = NULL;
	// ���� �� ������ ����� ����� ������
	hWriteThread = NULL;
	// ������� ���� ������ ������ ����� ������
	WriteThreadErrorNumber = 0x0;

	// �������� ����� ��������
	system("cls");

	printf(" **************************************\n");
	printf(" Module E14-440                        \n");
	printf(" Console example for E14-440 DAC stream\n");
	printf(" **************************************\n\n");


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

	// ������� ������� ��������� ������ ���
	if(!pModule->GET_DAC_PARS(&dp)) AbortProgram(" GET_CUR_DAC_PARS() --> Bad\n");
	else printf(" GET_CUR_DAC_PARS() --> OK\n");

	// ��������� �������� ��������� ���
	dp.DacRate = 125.0;									// ������� ������ ��� � ���
	dp.DacFifoLength = MAX_DAC_FIFO_SIZE_E440;  	// ����� FIFO ������ ��� � DSP ������
	if(!pModule->SET_DAC_PARS(&dp)) AbortProgram(" SET_DAC_PARS() --> Bad\n");
	else printf(" SET_DAC_PARS() --> OK\n");

	// ��������� �������� ������ ��� ����� ���
	WriteBuffer = new WORD[2*DataStep];
	if(!WriteBuffer) AbortProgram(" Cannot allocate WriteBuffer memory\n");

	// ������� � ��������� ����� ������ ������ �� ���
	printf(" \n");
	hWriteThread = CreateThread(0, 0x2000, ServiceWriteThread, 0, 0, &WriteTid);
	if(!hWriteThread) AbortProgram(" ServiceWriteThread() --> Bad\n");
	else printf(" ServiceWriteThread() --> OK\n");

	// ��������� ��������� ��� �� �������
	printf(" \n");
	printf(" Module E14-440 (S/N %s) is ready ... \n", ModuleDescription.Module.SerialNumber);
	printf("   Module Info:\n");
	printf("     Module  Revision   is '%c'\n", ModuleDescription.Module.Revision);
	printf("     AVR Driver Version is %s (%s)\n", ModuleDescription.Mcu.Version.Version, ModuleDescription.Mcu.Version.Date);
	printf("     LBIOS   Version    is %s (%s)\n", ModuleDescription.Dsp.Version.Version, ModuleDescription.Dsp.Version.Date);
	printf("  Dac parameters:\n");
	printf("    DacRate = %7.2f kHz\n", dp.DacRate);
	printf("    DacFifoLength = %d\n", dp.DacFifoLength);
	printf("  Signal parameters:\n");
	printf("    Frequency = %5.3f kHz\n", SignalFrequency);
	printf("    Amplitude = %d DAC code\n", SignalAmplitude);
	printf("\n Now SINUS signal is on the first DAC channel... \n");
	printf("  (Press any key to terminate the program)\n");

	// ���� �������� ��������� ������ ���������� �� ������� ����� �������
	printf("\n Time: %8lu s\r", Counter);
	while(!IsWriteThreadComplete)
	{
		Sleep(1000);
		printf(" Time: %8lu s\r", ++Counter);
	}

	// ��� ��������� ������ ������ ����� ������
	WaitForSingleObject(hWriteThread, INFINITE);

	// �������� ���� �� ������ ���������� ������ ����� ������
	printf("\n\n");
	if(WriteThreadErrorNumber) { AbortProgram(NULL, false); ShowErrorMessage(); }
	else AbortProgram(" The program was completed successfully!!!\n", false);
}

//------------------------------------------------------------------------------
// ����� � ������� �������������� ������ ������ �� ���
//------------------------------------------------------------------------------
DWORD WINAPI ServiceWriteThread(PVOID /*Context*/)
{
	WORD RequestNumber;
	DWORD i, BaseIndex;
	// ��������� ���������� �������
	double CurrentTime;						// ������� ����� � ���
	// OVERLAPPED ���������
	OVERLAPPED WriteOv[2];
	// ������ �������� � ����������� ������� �� ����/����� ������
	IO_REQUEST_LUSBAPI IoReq[2];

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(!pModule->STOP_DAC()) { WriteThreadErrorNumber = 0x6; IsWriteThreadComplete = true; return 0; }

	// ��������� ������ ��� ������ FIFO ������ ��� � ������
	CurrentTime = 0.0;
	for(i = 0; i < (DWORD)dp.DacFifoLength; i++)
	{
		WriteBuffer[i] = Round(SignalAmplitude*sin(2.0*M_PI*SignalFrequency*CurrentTime));
		WriteBuffer[i] &= (WORD)(0xFFF);
		WriteBuffer[i] |= (WORD)(DacNumber << 12);
		CurrentTime += 1.0/dp.DacRate;
	}

	// ��������� ������� FIFO ����� ��� � ������
	if(!pModule->PUT_DM_ARRAY(dp.DacFifoBaseAddress, dp.DacFifoLength, (SHORT *)WriteBuffer))
   										{ WriteThreadErrorNumber = 0x1; IsWriteThreadComplete = true; return 1; }

	// ������ ��������� ������ ������� ��� ����� ������ WriteBuffer
	for(i = 0x0; i < 2*DataStep; i++)
	{
		WriteBuffer[i] = Round(SignalAmplitude*sin(2.0*M_PI*SignalFrequency*CurrentTime));
		WriteBuffer[i] &= (WORD)(0xFFF);
		WriteBuffer[i] |= (WORD)(DacNumber << 12);
		CurrentTime += 1.0/dp.DacRate;
	}

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
		while(true)
		{
			// ������� ������ �� ��������� ������ ������������ ������
			RequestNumber ^= 0x1;
			if(!pModule->WriteData(&IoReq[RequestNumber])) { WriteThreadErrorNumber = 0x2; break; }
			if(WriteThreadErrorNumber) break;

			// ���� ���������� ����������� �������� ������ ������ � ������
			if(!WaitingForRequestCompleted(IoReq[RequestNumber^0x1].Overlapped)) break;
//			if(WaitForSingleObject(WriteOv[RequestNumber^0x1].hEvent, IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) { WriteThreadErrorNumber = 0x4; break; }
			if(WriteThreadErrorNumber) break;

			// ���������� ��������� ������ ������ ��� ������ ��������� ������ WriteBuffer
			BaseIndex = (RequestNumber^0x1)*DataStep;
			for(i = 0x0; i < DataStep; i++)
			{
				WriteBuffer[i + BaseIndex] = Round(SignalAmplitude*sin(2.*M_PI*SignalFrequency*CurrentTime));
				WriteBuffer[i + BaseIndex] &= (WORD)(0xFFF);
				WriteBuffer[i + BaseIndex] |= (WORD)(DacNumber << 12);
				CurrentTime += 1.0/dp.DacRate;
	  		}

			// ���� ����� ����� ��������, �� ������ ������
			if(WriteThreadErrorNumber) break;
			if(kbhit()) { WriteThreadErrorNumber = 0x3; break; }
			else Sleep(20);
		}
	}
	else { WriteThreadErrorNumber = 0x5; }

	// ��������� ������ ���
	if(!pModule->STOP_DAC()) WriteThreadErrorNumber = 0x6;
	// ������ �������� ������������� ����������� ������ �� ���� ������
	if(!CancelIo(ModuleHandle)) WriteThreadErrorNumber = 0x7;
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(WriteOv[i].hEvent);
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

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
BOOL WaitingForRequestCompleted(OVERLAPPED *ReadOv)
{
	DWORD ReadBytesTransferred;

	while(TRUE)
	{
		if(GetOverlappedResult(ModuleHandle, ReadOv, &ReadBytesTransferred, FALSE)) break;
		else if(GetLastError() !=  ERROR_IO_INCOMPLETE) { WriteThreadErrorNumber = 0x4; return FALSE; }
		else if(kbhit()) { WriteThreadErrorNumber = 0x3; return FALSE; }
		else Sleep(20);
	}
	return TRUE;
}

//------------------------------------------------------------------------------
// ��������� ��������� � �������
//------------------------------------------------------------------------------
void ShowErrorMessage(void)
{
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

//------------------------------------------------------------------------------
// ��������� ���������� ���������
//------------------------------------------------------------------------------
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

	// ��������� ������ ������
	if(WriteBuffer) { delete[] WriteBuffer; WriteBuffer = NULL; }
	// ��������� ������������� ������ ����� ������
	if(hWriteThread) { CloseHandle(hWriteThread); hWriteThread = NULL; }

	// ������� ����� ���������
	if(ErrorString) printf(ErrorString);

	// ��������� ������� ����������
	if(kbhit()) { while(kbhit()) getch(); }

	// ���� ����� - �������� ��������� ���������
	if(AbortionFlag) exit(0x1);
	// ��� �������� ������� �� �������
	else return;
}


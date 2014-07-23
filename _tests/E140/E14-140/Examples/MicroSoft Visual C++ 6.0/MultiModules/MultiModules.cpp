//******************************************************************************
// ������ E14-140.
// ���������� ��������� ���������� ��������� ���� ������ ������������
// � ���������� �������.
//******************************************************************************
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "Lusbapi.h"

// ��������� ����� �� ���������
void AbortProgram(char *ErrorString, bool AbortionFlag = true);
// ������� ������ ����� ������
DWORD WINAPI ServiceReadThread(PVOID ThreadIndex);
// �������� ���������� ������������ ������� �� ���� ������
BOOL WaitingForIoRequestCompleted(OVERLAPPED *ReadOv, DWORD ThreadTid);
// 
void SetXY(COORD * const Coord);
void GetXY(COORD * const Coord);

// ����������� ��������� ���-�� ������������ ����������� ������
const WORD MaxVirtualSoltsQuantity	= 0x7;
// ���-�� ������������ ������� ��� ������� ������
const WORD ADC_CHANNELS_QUANTITY 	= 0x4;
// ������� ������ ��� ��� ����� ������
const double AdcRate = 2500.0;

// �������������� ������
HANDLE 	ReadThreadHandle[MaxVirtualSoltsQuantity];
DWORD		ReadThreadIndex[MaxVirtualSoltsQuantity];
DWORD		ReadThreadTid[MaxVirtualSoltsQuantity];

// ������ ���������� �� ��������� ������� ���� E14-140
ILE140 *pModules[MaxVirtualSoltsQuantity];
// ������ ����������
DWORD DllVersion;
// ���������� ����������
HANDLE ModuleHandle;
// �������� ������
char ModuleName[7];
// �������� ������ ���� USB
BYTE UsbSpeed;
// ��������� � ������ ����������� � ������
MODULE_DESCRIPTION_E140 ModuleDescription;
// ��������� ���������� ������ ��� ������
ADC_PARS_E140 ap;
// ���-�� ������� ������������ ������� ���� E14-140
WORD ModulesQuantity;

// ������ ������� ����� ������
DWORD DataStep = 64*1024;
// ������ ������� ��� ���������� ������
SHORT *Buffer[MaxVirtualSoltsQuantity];
// ������-������� ���������� ���������� ���������������� ������
bool IsReadThreadTerminated[MaxVirtualSoltsQuantity];
// ����� ������ ������� �� �������
WORD ReadThreadErrorNumber[MaxVirtualSoltsQuantity];
// ����� ������ ���������� �������� ���������
WORD MainErrorNumber;

// �������� ���������� ������ �������� ���������
COORD Coord;  
// ������� �������
WORD Counter[MaxVirtualSoltsQuantity];

// ����������� ������
CRITICAL_SECTION cs;
// ���������� ������������ ���������� ������ ���������� - �������
HANDLE OutputConsoleHandle;

//------------------------------------------------------------------------
// �������� ���������
//------------------------------------------------------------------------
void main(void)
{
	WORD i, j;

	// ������������� ����������� ������
	InitializeCriticalSection(&cs);
	// �������� ���������� ������������ ���������� ������ ���������� - �����
	OutputConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// ����������������� ��������� � ����������
	for(i = 0x0; i < MaxVirtualSoltsQuantity; i++)
	{
		// ������� ��������� �� ���������
		pModules[i] = NULL;
		// ������� ������ ���������� ������ ����� ������
		IsReadThreadTerminated[i] = false;
		// ���� ������ �� �������� ��� ����� ������
		Buffer[i] = NULL;
		// ���� �� ������ ����� ����� ������
		ReadThreadHandle[i] = NULL;
		// ������� ���� ������ ������ ����� ������
		ReadThreadErrorNumber[i] = 0x0;
	}

	// �������� ����� ��������
	system("cls");

	printf(" ***********************************\n");
	printf(" Module E14-140                     \n");
	printf(" Console example for Multi Modules  \n");
	printf(" ***********************************\n\n");

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

	printf("\n E14-140 modules List:\n");
	// �������� ������� ������������ �������
	ModulesQuantity = 0x0;
	// ��������� ������ MaxVirtualSoltsQuantity ����������� ������ � ������� ������� ���� E14-140
	for(i = 0x0; i < MaxVirtualSoltsQuantity; i++)
	{
		// ������� ��������� �� ���������
		pModules[i] = static_cast<ILE140 *>(CreateLInstance("e140"));
		if(!pModules[i]) AbortProgram(" Module Interface --> Bad\n");
		// ��������� ���������� ������ ���� E14-140 � ��������������� ����������� �����
		else if(!pModules[i]->OpenLDevice(i))  { pModules[i]->ReleaseLInstance(); pModules[i] = NULL; continue; }
		// ��������� �������� ������ � ������� ����������� �����
		else if(!pModules[i]->GetModuleName(ModuleName)) AbortProgram(" GetModuleName() --> Bad\n");
		// ��������, ��� ��� E14-140
		else if(strcmp(ModuleName, "E140")) AbortProgram(" The detected module is not E14-140. The appication is terminated.\n");
		// ��������� ������ �������� USB ��� �������� ������
		else if(!pModules[i]->GetUsbSpeed(&UsbSpeed)) AbortProgram(" GetUsbSpeed() --> Bad\n");
		// ������� ���������� �� ���� ������
		else if(!pModules[i]->GET_MODULE_DESCRIPTION(&ModuleDescription)) AbortProgram(" GET_MODULE_DESCRIPTION() --> Bad\n");
		// �������� �������� MCU ������
	   else if((ModuleDescription.Module.Revision == REVISIONS_E140[REVISION_B_E140]) &&
				  (strtod((char *)ModuleDescription.Mcu.Version.Version, NULL) < 3.05)) AbortProgram(" For module E14-140(Rev.'B') firmware version must be 3.05 or above --> !!! ERROR !!!\n");

		// ������� ������� ��������� ������ ���
		if(!pModules[i]->GET_ADC_PARS(&ap)) AbortProgram(" GET_ADC_PARS() --> Bad\n");
		// ��������� �������� ��������� ���
		ap.ClkSource = INT_ADC_CLOCK_E140;							// ���������� ������ ���
		ap.EnableClkOutput = ADC_CLOCK_TRANS_DISABLED_E140; 	// ��� ���������� �������� �������� ���
		ap.InputMode = NO_SYNC_E140;									// ��� ������������� ����� ������
		ap.ChannelsQuantity = ADC_CHANNELS_QUANTITY; 			// ���-�� �������� �������
		for(j = 0x0; j < ADC_CHANNELS_QUANTITY; j++) ap.ControlTable[j] = (WORD)(j | (ADC_INPUT_RANGE_2500mV_E140 << 0x6));
		ap.AdcRate = AdcRate;								// ������� ������ ��� � ���
		ap.InterKadrDelay = 0.0;							// ����������� �������� � ��
		// ��������� ��������� ��������� ������ ��� � ������
		if(!pModules[i]->SET_ADC_PARS(&ap)) AbortProgram(" SET_ADC_PARS() --> Bad\n");

		// ������� ������ ��� ������� �����
		Buffer[i] = new SHORT[2*DataStep];
		if(!Buffer[i]) AbortProgram(" Cannot allocate buffer memory.\n");

		// �������������� ������� ������������ �������
		ModulesQuantity++;
		printf("   %2u. Virtual Slot %2u. Module (S/N %s, %s) is ready ... \n", ModulesQuantity, i, ModuleDescription.Module.SerialNumber, UsbSpeed ? "HIGH-SPEED (480 Mbit/s)" : "FULL-SPEED (12 Mbit/s)");
	}

	// ��������: �� ����� ���� �����-������ ������?
	if(!ModulesQuantity) { printf("   Empty...\n"); AbortProgram("\n Can't detect any E14-140 module  :(((\n"); }

	// �������� ������� ���������� �������
	GetXY(&Coord); Coord.Y += (WORD)0x3;

	// ��������� ��������� ����� ����� ������ ��� ������� �� ������������ ������
	MainErrorNumber = 0x0;
	for(i = 0x0; i < MaxVirtualSoltsQuantity; i++)
	{
		// ������� � ��������� ������ ����� ������
		if(!pModules[i]) continue;
		ReadThreadIndex[i] = i;
		ReadThreadHandle[i] = CreateThread(0, 0x2000, ServiceReadThread, &ReadThreadIndex[i], 0, &ReadThreadTid[i]);
		if(!ReadThreadHandle[i]) AbortProgram("\n CreateThread() --> Bad\n");
	}

	// ���� ���������� ������ ������� �� ����� ������� ����� ������� ����������
	printf("\n  Press any key to terminate the program ...\n\n");
	while(true)
	{
		if(kbhit()) { MainErrorNumber = 0x1; break; }
		Sleep(20);
	}

	// ���� ��������� ���� ������ ��������, ��������� ���� ��������
	if(MainErrorNumber == 0x1) AbortProgram(" The program was terminated successfully!\n", false);
	else AbortProgram(" The program was completed successfully!!!\n", false);
}

//------------------------------------------------------------------------
// ����� ����� ������ � ������ E14-140
//------------------------------------------------------------------------
DWORD WINAPI ServiceReadThread(PVOID ThreadIndex)
{
	WORD i;
	// ����� �������
	WORD RequestNumber;
	// ����� �������� ������
	DWORD ti = *(DWORD *)ThreadIndex;
	// ��������� ��������� ��������� �������
	COORD LocCoord;
	// ������ OVERLAPPED ��������
	OVERLAPPED ReadOv[2];
	// ������ �������� � ����������� ������� �� ����/����� ������
	IO_REQUEST_LUSBAPI IoReq[2];

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(pModules[ti]->STOP_ADC())
	{
		// ��������� ����������� ��� ����� ������ ���������
		for(i = 0x0; i < 0x2; i++)
		{
			// ������������� ��������� ���� OVERLAPPED
			ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED));
			// ������ ������� ��� ������������ �������
			ReadOv[i].hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
			// ��������� ��������� IoReq
			IoReq[i].Buffer = Buffer[ti] + i*DataStep;
			IoReq[i].NumberOfWordsToPass = DataStep;
			IoReq[i].NumberOfWordsPassed = 0x0;
			IoReq[i].Overlapped = &ReadOv[i];
			IoReq[i].TimeOut = (DWORD)(DataStep/ap.AdcRate + 1000);
		}

		// ���������� ������� �������� ������
		EnterCriticalSection(&cs);
//			gotoxy(XCoordCounter, YCoordCounter + 1*ti);
			LocCoord.X = Coord.X; LocCoord.Y = Coord.Y + (WORD)(1*ti); SetXY(&LocCoord);
			printf("  Counter[%2u]: %8u", ti/*+0x1*/, Counter[ti] = 0x0);
		LeaveCriticalSection(&cs);

		// ������ ��������������� ������ �� ���� ������
		RequestNumber = 0x0;
		if(!pModules[ti]->ReadData(&IoReq[RequestNumber])) { ReadThreadErrorNumber[ti] = 0x2; goto ReadThreadFinish; }

		// ������ ��������� ���� ������
		if(pModules[ti]->START_ADC())
		{
			// ���� ����� ������
			while(!IsReadThreadTerminated[ti])
			{
				// ������� ������ �� ��������� ������ ������
				RequestNumber ^= 0x1;
				if(!pModules[ti]->ReadData(&IoReq[RequestNumber])) { ReadThreadErrorNumber[ti] = 0x2; break; }
				if(IsReadThreadTerminated[ti]) break;

				// ��� ���������� �������� ����� ���������� ������ ������
				if(!WaitingForIoRequestCompleted(IoReq[RequestNumber^0x1].Overlapped, ti)) break;
//				if(WaitForSingleObject(ReadOv[RequestNumber^0x1].hEvent, IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) { /*ReadThreadErrorNumber = 0x3;*/ break; }
				if(IsReadThreadTerminated[ti]) break;

				// ��������� �������� �������� ������
				Counter[ti]++;
				// ��������� ��������� �������� ������� ������ ��� ���������� ������
				if(!IsReadThreadTerminated[ti] && !ReadThreadErrorNumber[ti])
				{
					EnterCriticalSection(&cs);
//						gotoxy(XCoordCounter, YCoordCounter + 1*ti);
						LocCoord.X = Coord.X; LocCoord.Y = Coord.Y + (WORD)(1*ti); SetXY(&LocCoord);
						printf("  Counter[%2u]: %8u", ti/*+0x1*/, Counter[ti]);
					LeaveCriticalSection(&cs);
				}

				// ���� �� �����-������ ������ � ������ ������� ������
				if(IsReadThreadTerminated[ti]) break;
				else if(ReadThreadErrorNumber[ti]) break;
				else Sleep(20);
			}
		}
		else ReadThreadErrorNumber[ti] = 0x4;
	}
	else ReadThreadErrorNumber[ti] = 0x6;

ReadThreadFinish:
	// ��������� ���� ������
	if(!pModules[ti]->STOP_ADC()) ReadThreadErrorNumber[ti] = 0x6;
	// ������, ���� �����, ����������� ������
	if(!CancelIo(pModules[ti]->GetModuleHandle())) ReadThreadErrorNumber[ti] = 0x7;
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(IoReq[i].Overlapped->hEvent);
	// ��������� ��������� �������� ������� ������ ��� ������� ������
	if(ReadThreadErrorNumber[ti])
	{
		EnterCriticalSection(&cs);
//			gotoxy(XCoordCounter, YCoordCounter + 1*ti);
			LocCoord.X = Coord.X; LocCoord.Y = Coord.Y + (WORD)(1*ti); SetXY(&LocCoord);
			printf("  Counter[%2u]: Thread Error!", ti/*+0x1*/, Counter[ti]);
		LeaveCriticalSection(&cs);
	}
	// ������ �������� ����� ����� �� ������
	return 0x0;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
BOOL WaitingForIoRequestCompleted(OVERLAPPED *ReadOv, DWORD ThreadTid)
{
	// ��� ���������� ���������� �������
	while(!IsReadThreadTerminated[ThreadTid])
	{
		if(HasOverlappedIoCompleted(ReadOv)) break;
		else if(IsReadThreadTerminated[ThreadTid]) break;
		else Sleep(20);
	}
	return TRUE;
}

//------------------------------------------------------------------------
// ����� ��������� �� ������
//------------------------------------------------------------------------
void AbortProgram(char *ErrorString, bool AbortionFlag)
{
	WORD i;

	// ��������� �� �����
	for(i = 0x0; i < MaxVirtualSoltsQuantity; i++)
	{
		if(ReadThreadHandle[i] != NULL)
		{
			// ������� ������ �����
			IsReadThreadTerminated[i] = true;
			// ��� ��������� ������ ������� ������
			WaitForSingleObject(ReadThreadHandle[i], INFINITE);
			// ��������� ������������� ������� ������
			CloseHandle(ReadThreadHandle[i]);
		}
		// ��������� ������ ������
		if(Buffer[i]) { delete[] Buffer[i]; Buffer[i] = NULL; }
		// ��������� ������
		if(pModules[i]) { pModules[i]->ReleaseLInstance(); pModules[i] = NULL; }
	}

	// ��������� ����������� ������
	DeleteCriticalSection(&cs);

	// �������� ��������� �������
//	if(ModulesQuantity) gotoxy(XCoordCounter, YCoordCounter + ModulesQuantity + 0x2);
	if(ModulesQuantity) { Coord.Y += ModulesQuantity + 0x2; SetXY(&Coord); }

	else printf("\n\n");

	// ������� ����� ���������
	if(ErrorString) printf(ErrorString);

	// ���� ����� - �������� ��������� ���������
	if(AbortionFlag) exit(0x1);
	else return;
}

//------------------------------------------------------------------------------
// ��������� ������� � ���������� X, Y
//------------------------------------------------------------------------------
void SetXY(COORD * const Coord)
{
	SetConsoleCursorPosition(OutputConsoleHandle, *Coord);
}

//---------------------------------------------------------------------------
// ������ ������� ���������� X, Y �������
//---------------------------------------------------------------------------
void GetXY(COORD * const Coord)
{
	CONSOLE_SCREEN_BUFFER_INFO Csi;

	GetConsoleScreenBufferInfo(OutputConsoleHandle, &Csi);
	*Coord = Csi.dwCursorPosition;
}		
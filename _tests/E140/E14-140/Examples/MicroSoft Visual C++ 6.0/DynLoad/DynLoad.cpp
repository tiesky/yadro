// ������ E14-140.
// ���������� ��������� � �������� ������������ �������� ���������� Lusbapi.dll
//******************************************************************************
#include <stdio.h>
#include <conio.h>
#include "LoadDll.h"				// ������������ �������� ����������
#include "Lusbapi.h"				// �������� ������

#define CHANNELS_QUANTITY			(0x4)

// ��������� ����� �� ���������
void AbortProgram(char *ErrorString, bool AbortionFlag = true);

// ����������� ��������� ���-�� ������������ ����������� ������
const WORD MaxVirtualSoltsQuantity = 127;

// ��������� �� ����� ������������ �������� DLL
TLoadDll *pLoadDll;

// ������ ����������
DWORD DllVersion;
// ��������� �� ��������� ������
ILE140 *pModule;
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
// ������� ������ ��� � ���
const double AdcRate = 100.0;
// ������ �������� ��������� ����������
const WORD InputRangeIndex = ADC_INPUT_RANGE_2500mV_E140;

//------------------------------------------------------------------------
// �������� ���������
//------------------------------------------------------------------------
void main(void)
{
	WORD i;

	// �������� ����� ��������
	system("cls");

	printf(" ******************************************\n");
	printf(" Module E14-140                            \n");
	printf(" Console example for DLL dynamic loading 	\n");
	printf(" ******************************************\n");

	// ������������ �������� ���������� "Lusbapi.dll"
	pLoadDll = new TLoadDll("Lusbapi.dll");
	if(!pLoadDll) AbortProgram(" Can't alloc 'TLoadDll' object!!!\n");
	// �������� ������ �� �� ��������� ����������?
	if(!pLoadDll->GetDllHinstance()) AbortProgram(" 'Lusbapi.dll' Dynamic Loading --> Bad\n");
	else printf(" 'Lusbapi.dll' Dynamic Loading --> OK\n");

	// ����� ������� ��������� ������ ����������
	pGetDllVersion GetDllVersion = (pGetDllVersion)pLoadDll->CallGetDllVersion();
	if(!GetDllVersion) AbortProgram(" Address of GetDllVersion() --> Bad\n");
	else printf(" Address of GetDllVersion() --> OK\n");

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

	// ����� ������� ��������� ���������� ������
	pCreateInstance CreateLInstance = (pCreateInstance)pLoadDll->CallCreateLInstance();
	if(!CreateLInstance) AbortProgram(" Address of CreateLInstance() --> Bad!!!\n");
	else printf(" Address of CreateLInstance() --> OK\n");

	// ��������� �������� ��������� �� ���������
	pModule = static_cast<ILE140 *>(CreateLInstance("e140"));
	if(!pModule) AbortProgram(" Module Interface --> Bad\n");
	else printf(" Module Interface --> OK\n");

	// ��������� ���������� ������ E14-140 � ������ 256 ����������� ������
	for(i = 0x0; i < MaxVirtualSoltsQuantity; i++) if(pModule->OpenLDevice(i)) break;
	// ���-������ ����������?
	if(i == MaxVirtualSoltsQuantity) AbortProgram(" Can't find any module E14-140 in first 127 virtual slots!\n");
	else printf(" OpenLDevice(%u) --> OK\n", i);

	// ��������� ��������� ���������� ����������
	ModuleHandle = pModule->GetModuleHandle();
	if(ModuleHandle == INVALID_HANDLE_VALUE) AbortProgram(" GetModuleHandle() --> Bad\n");
	else printf(" GetModuleHandle() --> OK\n");

	// ��������� �������� ������ � ������������ ����������� �����
	if(!pModule->GetModuleName(ModuleName)) AbortProgram(" GetModuleName() --> Bad\n");
	else printf(" GetModuleName() --> OK\n");
	// ��������, ��� ��� 'E14-140'
	if(strcmp(ModuleName, "E140")) AbortProgram(" The module is not 'E14-140'\n");
	else printf(" The module is 'E14-140'\n");

	// ��������� �������� �������� ������ ���� USB
	if(!pModule->GetUsbSpeed(&UsbSpeed)) AbortProgram(" GetUsbSpeed() --> Bad\n");
	else printf(" GetUsbSpeed() --> OK\n");
	// ������ ��������� �������� ������ ���� USB
	printf("   USB is in %s\n", UsbSpeed ? "High-Speed Mode (480 Mbit/s)" : "Full-Speed Mode (12 Mbit/s)");

	// ������� ���������� �� ���� ������
	if(!pModule->GET_MODULE_DESCRIPTION(&ModuleDescription)) AbortProgram(" GET_MODULE_DESCRIPTION() --> Bad\n");
	else printf(" GET_MODULE_DESCRIPTION() --> OK\n");

	// �������� �������� MCU ������
   if((ModuleDescription.Module.Revision == REVISIONS_E140[REVISION_B_E140]) &&
   	(strtod((char *)ModuleDescription.Mcu.Version.Version, NULL) < 3.05)) AbortProgram(" For module E14-140(Rev.'B') firmware version must be 3.05 or above --> !!! ERROR !!!\n");

	// ������� ������� ��������� ������ ���
	if(!pModule->GET_ADC_PARS(&ap)) AbortProgram(" GET_ADC_PARS() --> Bad\n");
	else printf(" GET_ADC_PARS() --> OK\n");
	// ��������� �������� ��������� ������ ���
	ap.ClkSource = INT_ADC_CLOCK_E140;							// ���������� ������ ���
	ap.EnableClkOutput = ADC_CLOCK_TRANS_DISABLED_E140; 	// ��� ���������� �������� �������� ���
	ap.InputMode = NO_SYNC_E140;									// ��� ������������� ����� ������
	ap.ChannelsQuantity = CHANNELS_QUANTITY; 					// ���-�� �������� �������
	// ��������� ����������� �������
	for(i = 0x0; i < ap.ChannelsQuantity; i++) ap.ControlTable[i] = (WORD)(i | (ADC_INPUT_RANGE_2500mV_E140 << 0x6));
	ap.AdcRate = AdcRate;								// ������� ������ ��� � ���
	ap.InterKadrDelay = 0.0;							// ����������� �������� � ��
	// ��������� ��������� ��������� ������ ��� � ������
	if(!pModule->SET_ADC_PARS(&ap)) AbortProgram(" SET_ADC_PARS() --> Bad\n");
	else printf(" SET_ADC_PARS() --> OK\n");

	// ��������� ���������� �� ������
	printf(" \n");
	printf(" Module E14-140 (S/N %s) is ready ... \n", ModuleDescription.Module.SerialNumber);
	printf("   Module Info:\n");
	printf("     Module  Revision   is '%c'\n", ModuleDescription.Module.Revision);
	printf("     AVR Driver Version is %s (%s)\n", ModuleDescription.Mcu.Version.Version, ModuleDescription.Mcu.Version.Date);
	printf("   Adc parameters:\n");
	printf("     ChannelsQuantity = %2d\n", ap.ChannelsQuantity);
	printf("     AdcRate = %8.2f kHz\n", ap.AdcRate);
	printf("     InterKadrDelay = %2.5f ms\n", ap.InterKadrDelay);
	printf("     KadrRate = %8.2f kHz\n", ap.KadrRate);

	// .....
	// ����� ����� �������������� ���� �������� ������ � �������,
	// ��������, ���� ���� ����� ������
	// .....

	// ��� ���������� ��������� - ��������� ��������� ������
	printf("\n");
	AbortProgram(" The program was completed successfully!!!\n", false);
}

//------------------------------------------------------------------------
// ��������� ���������� ���������
//------------------------------------------------------------------------
void AbortProgram(char *ErrorString, bool AbortionFlag)
{
	// ��������� ��������� ������
	if(pModule)
	{
		// ��������� ��������� ������
		if(!pModule->ReleaseLInstance()) printf(" ReleaseLInstance() --> Bad\n");
		else printf(" ReleaseLInstance() --> OK\n");
		// ������� ��������� �� ��������� ������
		pModule = NULL;
	}
	// ��������� ����������
	if(pLoadDll) { delete pLoadDll; pLoadDll = NULL; }

	// ������� ����� ���������
	if(ErrorString) printf(ErrorString);

	// ��������� ������� ����������
	if(kbhit()) { while(kbhit()) getch(); }

	// ���� ����� - �������� ��������� ���������
	if(AbortionFlag) exit(0x1);
	// ��� �������� ������� �� �������
	else return;
}


// This is the main DLL file.

//http://www.codeproject.com/Articles/505791/A-Beginner-Tutorial-for-Writing-Simple-COM-ATL-DLL?q=creating+an+atl+dll+c%2b%2b
//http://msdn.microsoft.com/en-us/library/ms235636(v=vs.100).aspx

#include "stdafx.h"
//Must be right after "stdafx.h" to get rid ambigious symbols due to http://qualapps.blogspot.de/2008/07/using-atlstrh-in-managed-project.html
#include "Lusbapi.h"

#include "LusbApiCppWrapper.h"



namespace LusbApiCppWrapper
{	
	// указатель на интерфейс модуля
	ILE140 *pModule;
	// структура параметров работы АЦП модуля
	ADC_PARS_E140 ap;

	ADC_PARS_E140_MARSHALL ap_m;
	//byte nice[10];
	
// максимально возможное кол-во опрашиваемых 
// виртуальных слотов (при поиске модуля)
const WORD MaxVirtualSoltsQuantity = 127;
// дескриптор устройства
HANDLE ModuleHandle;
// название модуля
char ModuleName[7];
// скорость работы шины USB
BYTE UsbSpeed;
// структура с полной информацией о модуле
MODULE_DESCRIPTION_E140 ModuleDescription;

// частота работы АЦП в кГц
const double AdcRate = 5000.0;

	UNIVERSAL_RETURN X_TEST()
	{
		UNIVERSAL_RETURN x;
		x.ptr = new BYTE[15];
		x.ptr[0] = 15;
		x.ptr[1] = 25;
		x.ptr[2] = 35;

		x.ptrShort = new SHORT[15];
		x.ptrShort[0]=15;
		x.ptrShort[1]=25;
		x.ptrShort[2]=-35;

		return x;
	}


	bool L_InitModule(PCHAR moduleName)
	{
		WORD i;

		pModule = static_cast<ILE140 *>(CreateLInstance(moduleName));

		if(!pModule)
			return false;

		// попробуем обнаружить модуль E14-140 в первых 256 виртуальных слотах
		for(i = 0x0; i < MaxVirtualSoltsQuantity; i++) if(pModule->OpenLDevice(i)) break;
		// что-нибудь обнаружили?
		if(i == MaxVirtualSoltsQuantity) return false;
		//else printf(" OpenLDevice(%u) --> OK\n", i);

		//// попробуем прочитать дескриптор устройства
		ModuleHandle = pModule->GetModuleHandle();
		if(ModuleHandle == INVALID_HANDLE_VALUE) return false;
		//else printf(" GetModuleHandle() --> OK\n");

		//// прочитаем название модуля в обнаруженном виртуальном слоте
		if(!pModule->GetModuleName(ModuleName)) return false;
		//else printf(" GetModuleName() --> OK\n");

		//// проверим, что это 'E14-140'
		if(strcmp(ModuleName, "E140")) return false;
		//else printf(" The module is 'E14-140'\n");

		//// попробуем получить скорость работы шины USB
		if(!pModule->GetUsbSpeed(&UsbSpeed)) return false;
		//else printf(" GetUsbSpeed() --> OK\n");
		//// теперь отобразим скорость работы шины USB
		//printf("   USB is in %s\n", UsbSpeed ? "High-Speed Mode (480 Mbit/s)" : "Full-Speed Mode (12 Mbit/s)");

		//// получим информацию из ППЗУ модуля
		if(!pModule->GET_MODULE_DESCRIPTION(&ModuleDescription)) return false;
		//else printf(" GET_MODULE_DESCRIPTION() --> OK\n");

		//// проверим прошивку MCU модуля
	   if((ModuleDescription.Module.Revision == REVISIONS_E140[REVISION_B_E140]) &&
   		(strtod((char *)ModuleDescription.Mcu.Version.Version, NULL) < 3.05))  return false;

	  // if(!pModule->GET_ADC_PARS(&ap)) return false;

	  // // установим желаемые параметры работы АЦП
			//ap.ClkSource = INT_ADC_CLOCK_E140;							// внутренний запуск АЦП
			//ap.EnableClkOutput = ADC_CLOCK_TRANS_DISABLED_E140; 	// без трансляции тактовых импульсо АЦП
			//ap.InputMode = NO_SYNC_E140;									// без синхронизации ввода данных
			//ap.ChannelsQuantity = 4;					// кол-во активных каналов
			//// формируем управляющую таблицу 
			//for(i = 0x0; i < 8; i++) ap.ControlTable[i] = (WORD)(i | (ADC_INPUT_RANGE_2500mV_E140 << 0x6));

			//ap.AdcRate = AdcRate;								// частота работы АЦП в кГц
			//ap.InterKadrDelay = 0.0;							// межкадровая задержка в мс

	  // if(!pModule->SET_ADC_PARS(&ap)) return false;

		return true;
	}
	
	unsigned int L_GetDllVersion()
	{
		return GetDllVersion();
	}



	ADC_PARS_E140_MARSHALL L_GetParams_ADC_PARS_E140()
	{
		if(!pModule) ap_m.OK = 1;
		 if(!pModule->GET_ADC_PARS(&ap)) ap_m.OK = 2;

		ap_m.ClkSource = ap.ClkSource;			
		ap_m.EnableClkOutput = ap.EnableClkOutput;	
		ap_m.InputMode = ap.InputMode;			
		ap_m.SynchroAdType = ap.SynchroAdType;		
		ap_m.SynchroAdMode = ap.SynchroAdMode; 	
		ap_m.SynchroAdChannel = ap.SynchroAdChannel;  
		ap_m.SynchroAdPorog = ap.SynchroAdPorog; 	
		ap_m.ChannelsQuantity = ap.ChannelsQuantity;	
			//WORD ControlTable[128];
		ap_m.AdcRate = ap.AdcRate;			
		ap_m.InterKadrDelay = ap.InterKadrDelay;	
		ap_m.KadrRate = ap.KadrRate;

		
		ap_m.Channel1 = ap.ControlTable[0];
		ap_m.Channel2 = ap.ControlTable[1];
		ap_m.Channel3 = ap.ControlTable[2];
		ap_m.Channel4 = ap.ControlTable[3];
		ap_m.Channel5 = ap.ControlTable[4];
		ap_m.Channel6 = ap.ControlTable[5];
		ap_m.Channel7 = ap.ControlTable[6];
		ap_m.Channel8 = ap.ControlTable[7];

		return ap_m;
	}


	bool L_SetParams_ADC_PARS_E140(ADC_PARS_E140_MARSHALL ap_m)
	{
			WORD channels;
			WORD i;
			channels = 0;

			if(!pModule->GET_ADC_PARS(&ap)) return false;

		/*	if(!pModule->GET_ADC_PARS(&ap)) return false;			
			if(!pModule->SET_ADC_PARS(&ap)) return false;
			return true;*/
			
			// установим желаемые параметры работы АЦП
			ap.ClkSource = INT_ADC_CLOCK_E140;							// внутренний запуск АЦП
			ap.EnableClkOutput = ADC_CLOCK_TRANS_DISABLED_E140; 	// без трансляции тактовых импульсо АЦП
			ap.InputMode = NO_SYNC_E140;									// без синхронизации ввода данных
			ap.ChannelsQuantity = ap_m.ChannelsQuantity;					// кол-во активных каналов
			
						

			//// формируем управляющую таблицу 
			//for(i = 0x0; i < ap.ChannelsQuantity; i++) ap.ControlTable[i] = (WORD)(i | (ADC_INPUT_RANGE_2500mV_E140 << 0x6));
	
					

			ap.ControlTable[0]=ap_m.Channel1;
			if(ap_m.Channel1 > 0) channels++;
			ap.ControlTable[1]=ap_m.Channel2;
			if(ap_m.Channel2 > 0) channels++;
			ap.ControlTable[2]=ap_m.Channel3;
			if(ap_m.Channel3 > 0) channels++;
			ap.ControlTable[3]=ap_m.Channel4;

			if(ap_m.Channel4 > 0) channels++;
			ap.ControlTable[4]=ap_m.Channel5;
			if(ap_m.Channel5 > 0) channels++;
			ap.ControlTable[5]=ap_m.Channel6;
			if(ap_m.Channel6 > 0) channels++;
			ap.ControlTable[6]=ap_m.Channel7;
			if(ap_m.Channel7 > 0) channels++;
			ap.ControlTable[7]=ap_m.Channel8;
			if(ap_m.Channel8 > 0) channels++;
		
			

			//ap.AdcRate = AdcRate;								// частота работы АЦП в кГц
			ap.AdcRate = ap_m.AdcRate;
			ap.InterKadrDelay = 0.0;							// межкадровая задержка в мс



		//	WORD ClkSource;							// источник тактовых импульсов для запуска АПП
		//WORD EnableClkOutput;					// разрешение трансляции тактовых импульсов запуска АЦП
		//WORD InputMode;							// режим ввода даных с АЦП
		//WORD SynchroAdType;						// тип аналоговой синхронизации
		//WORD SynchroAdMode; 						// режим аналоговой сихронизации
		//WORD SynchroAdChannel;  				// канал АЦП при аналоговой синхронизации
		//SHORT SynchroAdPorog; 					// порог срабатывания АЦП при аналоговой синхронизации
		//WORD ChannelsQuantity;					// число активных каналов
		//double AdcRate;							// частота работы АЦП в кГц
		//double InterKadrDelay;					// межкадровая задержка в мс
		//double KadrRate;							// частота кадра в кГц

			
			
			// передадим требуемые параметры работы АЦП в модуль
			if(!pModule->SET_ADC_PARS(&ap)) return false;

		return true;
	}

	// буфер данных
	SHORT *ReadBuffer;	
	// кол-во получаемых отсчетов (кратное 32) для Ф. ReadData()
	DWORD DataStep = 64*1024;
	// номер ошибки при выполнении сбора данных
	WORD ReadThreadErrorNumber;

	// массив OVERLAPPED структур из двух элементов
	OVERLAPPED ReadOv[2];
	// массив структур с параметрами запроса на ввод/вывод данных
	IO_REQUEST_LUSBAPI IoReq[2];

	WORD RequestNumber;

	WORD L_READ_START()
	{
			// пока ничего не выделено под буфер данных
			//ReadBuffer = NULL;

			// сбросим флаг ошибок потока ввода данных
			ReadThreadErrorNumber = 0x0;

			WORD i;
			
			//DWORD FileBytesWritten;
			//// массив OVERLAPPED структур из двух элементов
			//OVERLAPPED ReadOv[2];
			//// массив структур с параметрами запроса на ввод/вывод данных
			//IO_REQUEST_LUSBAPI IoReq[2];

			// остановим работу АЦП и одновременно сбросим USB-канал чтения данных
			if(!pModule->STOP_ADC()) {  return 0x1; }

			ReadBuffer = new SHORT[2*DataStep];

			if(!ReadBuffer) return 0x7;

			// формируем необходимые для сбора данных структуры
			for(i = 0x0; i < 0x2; i++)
			{
				// инициализация структуры типа OVERLAPPED
				ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED));
				// создаём событие для асинхронного запроса
				ReadOv[i].hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
				// формируем структуру IoReq
				IoReq[i].Buffer = ReadBuffer + i*DataStep;
				IoReq[i].NumberOfWordsToPass = DataStep;
				IoReq[i].NumberOfWordsPassed = 0x0;
				IoReq[i].Overlapped = &ReadOv[i];
				//IoReq[i].TimeOut = (DWORD)(DataStep/ap.AdcRate + 1000);
				IoReq[i].TimeOut = (DWORD)(DataStep/200 + 1000);
			}

			// делаем предварительный запрос на ввод данных
			RequestNumber = 0x0;
			//if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadOv[0].hEvent); CloseHandle(ReadOv[1].hEvent); return 0x2; }

			// запустим АЦП
			if(!pModule->START_ADC()) return 0x3;

			return 0;
	}

	SHORT L_READ_DO()
	{
		//WORD RequestNumber;
		SHORT res=0;
		//RequestNumber ^= 0x1;
		//		if(!pModule->ReadData(&IoReq[RequestNumber^0x1])) { return 0; }

		//		// ждём завершения операции сбора предыдущей порции данных
		//		if(!WaitingForRequestCompleted(IoReq[RequestNumber^0x1].Overlapped)) return 0;

		//		res = IoReq[RequestNumber^0x1].Buffer[12];		

		//RequestNumber = 0x0;
		//if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadOv[0].hEvent); CloseHandle(ReadOv[1].hEvent); return 0x2; }

			   
				RequestNumber = 0x1;
				if(!pModule->ReadData(&IoReq[RequestNumber])) { return 0; }

				// ждём завершения операции сбора предыдущей порции данных
				if(!WaitingForRequestCompleted(IoReq[RequestNumber].Overlapped)) { return 0;}

				res = IoReq[RequestNumber].Buffer[0];					

				//return res;
				return res;
	}


	WORD L_READ_STOP()
	{
		// сбросим флаг ошибок потока ввода данных
			ReadThreadErrorNumber = 0x0;

			WORD i;

		// остановим работу АЦП
			if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x5;
			// прервём возможно незавершённый асинхронный запрос на приём данных
			if(!CancelIo(ModuleHandle)) { ReadThreadErrorNumber = 0x6; }
			// освободим все идентификаторы событий
		   for(i = 0x0; i < 0x2; i++) CloseHandle(ReadOv[i].hEvent);

			/*if(ReadThreadErrorNumber == 0)
				return IoReq[RequestNumber^0x1].Buffer;*/

			return ReadThreadErrorNumber;
	}




	DWORD L_ReadDataOnce(SHORT * data)
	{
			// пока ничего не выделено под буфер данных
			//ReadBuffer = NULL;

			// сбросим флаг ошибок потока ввода данных
			ReadThreadErrorNumber = 0x0;

			WORD i;
			WORD RequestNumber;
			DWORD FileBytesWritten;
			// массив OVERLAPPED структур из двух элементов
			OVERLAPPED ReadOv[2];
			// массив структур с параметрами запроса на ввод/вывод данных
			IO_REQUEST_LUSBAPI IoReq[2];

			// остановим работу АЦП и одновременно сбросим USB-канал чтения данных
			if(!pModule->STOP_ADC()) {  return 0x1; }

			ReadBuffer = new SHORT[2*DataStep];

			if(!ReadBuffer) return 0x7;

			// формируем необходимые для сбора данных структуры
			for(i = 0x0; i < 0x2; i++)
			{
				// инициализация структуры типа OVERLAPPED
				ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED));
				// создаём событие для асинхронного запроса
				ReadOv[i].hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
				// формируем структуру IoReq
				IoReq[i].Buffer = ReadBuffer + i*DataStep;
				IoReq[i].NumberOfWordsToPass = DataStep;
				IoReq[i].NumberOfWordsPassed = 0x0;
				IoReq[i].Overlapped = &ReadOv[i];
				IoReq[i].TimeOut = (DWORD)(DataStep/ap.AdcRate + 1000);
			}

			// делаем предварительный запрос на ввод данных
			RequestNumber = 0x0;
			if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadOv[0].hEvent); CloseHandle(ReadOv[1].hEvent); return 0x2; }

			// запустим АЦП
			if(pModule->START_ADC())
			{
				RequestNumber ^= 0x1;
				if(!pModule->ReadData(&IoReq[RequestNumber])) { return 0x3; }

				// ждём завершения операции сбора предыдущей порции данных
				if(!WaitingForRequestCompleted(IoReq[RequestNumber^0x1].Overlapped)) return 0x4;

				data = IoReq[RequestNumber^0x1].Buffer;
				ReadThreadErrorNumber = data[23];
			}

			


			// остановим работу АЦП
			if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x5;
			// прервём возможно незавершённый асинхронный запрос на приём данных
			if(!CancelIo(ModuleHandle)) { ReadThreadErrorNumber = 0x6; }
			// освободим все идентификаторы событий
			for(i = 0x0; i < 0x2; i++) CloseHandle(ReadOv[i].hEvent);

			/*if(ReadThreadErrorNumber == 0)
				return IoReq[RequestNumber^0x1].Buffer;*/

			return ReadThreadErrorNumber;
	}//eof data






	BOOL WaitingForRequestCompleted(OVERLAPPED *ReadOv)
	{
		DWORD ReadBytesTransferred;

		while(TRUE)
		{
			if(GetOverlappedResult(ModuleHandle, ReadOv, &ReadBytesTransferred, FALSE)) break;
			else if(GetLastError() !=  ERROR_IO_INCOMPLETE) { ReadThreadErrorNumber = 0x3; return FALSE; }
		//	else if(kbhit()) { ReadThreadErrorNumber = 0x5; return FALSE; }
			else Sleep(20);
		}
		return TRUE;
	}

	/*ADC L_GetParams_ADC()
	{		
		adc.ClkSource = 1;
		nice[0]=15;
		nice[1]=12;

		adc.ptr=nice;

		return adc;
	}*/





	/*double Subtract1(double a, double b)
    {
        return a - b;
    }
	
    double Add(double a, double b)
    {
		
        return a + b;
    }*/



	 //double MyMathFuncs::Add(double a, double b)
  //  {
		//
  //      return a + b + xyz;
  //  }



    //double MyMathFuncs::Subtract(double a, double b)
    //{
    //    return a - b;
    //}

    //double MyMathFuncs::Multiply(double a, double b)
    //{
    //    return a * b;
    //}

}

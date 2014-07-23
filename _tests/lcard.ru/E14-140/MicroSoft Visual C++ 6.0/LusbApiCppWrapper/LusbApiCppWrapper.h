// LusbApiCppWrapper.h

//http://social.msdn.microsoft.com/Forums/en-US/32f1a857-91d6-472d-b333-469ed7f6f6dd/passing-a-byte-array-to-a-c-dll-from-c?forum=csharplanguage
//http://www.codeproject.com/Articles/66244/Marshaling-with-Csharp-Chapter-2-Marshaling-Simple.aspx
//http://dependencywalker.com/
//http://stackoverflow.com/questions/5902103/how-do-i-marshal-a-struct-that-contains-a-variable-sized-array-to-c
//http://stackoverflow.com/questions/8268625/get-pointer-on-byte-array-from-unmanaged-c-dll-in-c-sharp

//project property--->General--->Output Directory--->.\Debug

#ifdef LUSBAPICPPWRAPPER_EXPORTS_EXPORTS
#define LUSBAPICPPWRAPPER_EXPORTS_API __declspec(dllexport) 
#else
#define LUSBAPICPPWRAPPER_EXPORTS_API __declspec(dllimport) 
#endif

#pragma once

using namespace System;


namespace LusbApiCppWrapper
{
	struct ADC_PARS_E140_MARSHALL
	{
		int OK;				//If no errors 0
		WORD ClkSource;							// источник тактовых импульсов дл€ запуска јѕѕ
		WORD EnableClkOutput;					// разрешение трансл€ции тактовых импульсов запуска ј÷ѕ
		WORD InputMode;							// режим ввода даных с ј÷ѕ
		WORD SynchroAdType;						// тип аналоговой синхронизации
		WORD SynchroAdMode; 						// режим аналоговой сихронизации
		WORD SynchroAdChannel;  				// канал ј÷ѕ при аналоговой синхронизации
		SHORT SynchroAdPorog; 					// порог срабатывани€ ј÷ѕ при аналоговой синхронизации
		WORD ChannelsQuantity;					// число активных каналов

		//BYTE* ptr;
		
		//int arss[10];
		//WORD ControlTable[128];					// управл€юща€ таблица с активными каналами
		double AdcRate;							// частота работы ј÷ѕ в к√ц
		double InterKadrDelay;					// межкадрова€ задержка в мс
		double KadrRate;							// частота кадра в к√ц

		WORD Channel1;
		WORD Channel2;
		WORD Channel3;
		WORD Channel4;
		WORD Channel5;
		WORD Channel6;
		WORD Channel7;
		WORD Channel8;
	};

	struct UNIVERSAL_RETURN
	{
		int result;
		BYTE* ptr;
		SHORT* ptrShort;
	};

	extern "C"
	{		
		 //Can be also DWORD
		__declspec(dllexport) unsigned int L_GetDllVersion(); 
		__declspec(dllexport) bool L_InitModule(PCHAR moduleName); 
		__declspec(dllexport) ADC_PARS_E140_MARSHALL L_GetParams_ADC_PARS_E140();
		__declspec(dllexport) bool L_SetParams_ADC_PARS_E140(ADC_PARS_E140_MARSHALL ap_m);		
		__declspec(dllexport) DWORD L_ReadDataOnce(SHORT * data);	

		__declspec(dllexport) WORD L_READ_START();	
		__declspec(dllexport) SHORT L_READ_DO();	
		__declspec(dllexport) WORD L_READ_STOP();	
				 
		__declspec(dllexport) UNIVERSAL_RETURN X_TEST();	
		//__declspec(dllexport) ADC L_GetParams_ADC();
	}

	BOOL WaitingForRequestCompleted(OVERLAPPED *ReadOv);
	//extern "C" __declspec(dllexport) double Subtract1(double a, double b);
	// //Can be also DWORD
 //   extern "C" __declspec(dllexport) unsigned int l_GetDllVersion(); 

	//extern "C" __declspec(dllexport) double Add(double a, double b); 

	//	extern "C" LUSBAPICPPWRAPPER_EXPORTS_API double Subtract1(double a, double b);
	// //Can be also DWORD
 //   extern "C" LUSBAPICPPWRAPPER_EXPORTS_API unsigned int l_GetDllVersion(); 

	//extern "C" LUSBAPICPPWRAPPER_EXPORTS_API double Add(double a, double b); 



    // This class is exported from the MathFuncsDll.dll
    class MyMathFuncs
    {
		

    public: 
        // Returns a + b
        //static LUSBAPICPPWRAPPER_EXPORTS_API double Add(double a, double b); 

		 ////Can be also DWORD
   //     static LUSBAPICPPWRAPPER_EXPORTS_API unsigned int l_GetDllVersion(); 

        // Returns a - b
       // extern "C" LUSBAPICPPWRAPPER_EXPORTS_API double Subtract(double a, double b); 
		static LUSBAPICPPWRAPPER_EXPORTS_API double Subtract(double a, double b); 

        // Returns a * b
        static LUSBAPICPPWRAPPER_EXPORTS_API double Multiply(double a, double b); 
	
		/*extern "C" static int GetDllVersion(void);*/
    };
}

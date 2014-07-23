using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace LusbApiSharpWrapper
{
    //<DllImport("LusbApiCppWrapper.dll", CallingConvention := CallingConvention.Cdecl, EntryPoint := "?Add@MyMathFuncs@LusbApiCppWrapper@@SANNN@Z")> _
    //Public Shared Function Add(a As Double, b As Double) As Double
    //End Function

    class Importer
    {
        //With dependency walker
        //[DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "?Add@MyMathFuncs@LusbApiCppWrapper@@SANNN@Z")]
        //public static extern double Add(double a, double b);

        /// <summary>
        /// Initialize module 
        /// </summary>
        /// <param name="moduleName"></param>
        /// <returns></returns>
        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_InitModule")]
        public static extern bool L_InitModule(string moduleName);

        /// <summary>
        /// returns module version
        /// </summary>
        /// <returns></returns>
        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_GetDllVersion")]
        public static extern uint L_GetDllVersion();

        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_GetParams_ADC_PARS_E140")]
        public static extern ADC_PARS_E140_MARSHALL L_GetParams_ADC_PARS_E140();

        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_SetParams_ADC_PARS_E140")]
        public static extern bool L_SetParams_ADC_PARS_E140(ADC_PARS_E140_MARSHALL ap_m);

        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_ReadDataOnce")]
        public static extern ushort L_ReadDataOnce(ref short data);
        //public static extern ushort L_ReadDataOnce(ref short data);


        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_READ_START")]
        public static extern ushort L_READ_START();
        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_READ_DO")]
        public static extern short L_READ_DO();
        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "L_READ_STOP")]
        public static extern ushort L_READ_STOP();



        [DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "X_TEST")]
        public static extern UNIVERSAL_RETURN X_TEST();

        //[DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "Add")]
        //public static extern double Add(double a, double b);

        //[DllImport("LusbApiCppWrapper.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "Subtract1")]
        //public static extern double Subtract1(double a, double b);
    }


    [StructLayout(LayoutKind.Sequential)]
    public struct ADC_PARS_E140_MARSHALL
	{
        [MarshalAs(UnmanagedType.I4)]
        public int OK;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 ClkSource;							// источник тактовых импульсов для запуска АПП
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 EnableClkOutput;					// разрешение трансляции тактовых импульсов запуска АЦП
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 InputMode;							// режим ввода даных с АЦП
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 SynchroAdType;						// тип аналоговой синхронизации
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 SynchroAdMode; 						// режим аналоговой сихронизации
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 SynchroAdChannel;  				// канал АЦП при аналоговой синхронизации
        [MarshalAs(UnmanagedType.I2)]
        public Int16 SynchroAdPorog; 					// порог срабатывания АЦП при аналоговой синхронизации
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 ChannelsQuantity;					// число активных каналов


        //[MarshalAs(UnmanagedType.U4)]
        //public uint ptr;

        //[MarshalAs(UnmanagedType.LPArray, SizeConst=128)]
        //UInt16[] ControlTable;					// управляющая таблица с активными каналами

        public double AdcRate;							// частота работы АЦП в кГц
        public double InterKadrDelay;					// межкадровая задержка в мс
        public double KadrRate;							// частота кадра в кГц

        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel1;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel2;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel3;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel4;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel5;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel6;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel7;
        [MarshalAs(UnmanagedType.U2)]
        public UInt16 Channel8;
	};

    [StructLayout(LayoutKind.Sequential)]
    public struct UNIVERSAL_RETURN
    {
         [MarshalAs(UnmanagedType.I4)]
        public int result;
        [MarshalAs(UnmanagedType.U4)]
        public uint ptr;
        [MarshalAs(UnmanagedType.U4)]
        public uint ptrShort;
        //BYTE* ptr;
    };


    //[StructLayout(LayoutKind.Sequential)]
    //public struct ADC
    //{
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 ClkSource;							// источник тактовых импульсов для запуска АПП
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 EnableClkOutput;					// разрешение трансляции тактовых импульсов запуска АЦП
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 InputMode;							// режим ввода даных с АЦП
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 SynchroAdType;						// тип аналоговой синхронизации
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 SynchroAdMode; 						// режим аналоговой сихронизации
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 SynchroAdChannel;  				// канал АЦП при аналоговой синхронизации
    //    [MarshalAs(UnmanagedType.I2)]
    //    public Int16 SynchroAdPorog; 					// порог срабатывания АЦП при аналоговой синхронизации
    //    [MarshalAs(UnmanagedType.U2)]
    //    public UInt16 ChannelsQuantity;					// число активных каналов

    //    [MarshalAs(UnmanagedType.U4)]
    //    public uint ptr;					
    //    //[MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I4, SizeConst = 10)]
    //    //int[] arss;

    //    //[MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U2, SizeConst = 128)]
    //    //[MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U2, SizeConst = 128)]
    //    //UInt16[] ControlTable;					// управляющая таблица с активными каналами

    //    //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
    //    //UInt16[] ControlTable;					// управляющая таблица с активными каналами
    //    //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
    //    //public byte[] ControlTable;

    //    public double AdcRate;							// частота работы АЦП в кГц
    //    public double InterKadrDelay;					// межкадровая задержка в мс
    //    public double KadrRate;							// частота кадра в кГц
    //};

}

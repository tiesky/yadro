using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace LusbApiSharpWrapper
{
    class Program
    {
        static void Main(string[] args)
        {
            var rt = Importer.X_TEST();
            var arr = new byte[15];
            Marshal.Copy((IntPtr)rt.ptr, arr, 0, arr.Length);

            var arr1 = new short[15];
            Marshal.Copy((IntPtr)rt.ptrShort, arr1, 0, arr1.Length);

            return;


            int RequestNumber = 0x0;
            RequestNumber ^= 0x01;

            //var t = Importer.Add(1, 1);

            uint ver = Importer.L_GetDllVersion();

            bool mini = Importer.L_InitModule("e140");

         
            ADC_PARS_E140_MARSHALL pars = Importer.L_GetParams_ADC_PARS_E140();

            //pars.AdcRate = 5000;
            pars.AdcRate = 250;
            pars.ChannelsQuantity = 8;

            pars.Channel1 = 62;
            pars.Channel2 = 63;
            pars.Channel3 = 64;
            pars.Channel4 = 65;

            pars.Channel5 = 66;
            pars.Channel6 = 67;
            pars.Channel7 = 68;
            pars.Channel8 = 69;

            bool setRes = Importer.L_SetParams_ADC_PARS_E140(pars);

            pars = Importer.L_GetParams_ADC_PARS_E140();

            //Console.WriteLine(pars.Channel1);

            short data = new short();
           // var readOnce = Importer.L_ReadDataOnce(ref data);

            var start = Importer.L_READ_START();
            Console.WriteLine(start);

            //var doit = Importer.L_READ_DO();
            //Console.WriteLine(doit);


            for (int i = 0; i < 1000; i++)
            {
                var doit = Importer.L_READ_DO();
                System.Threading.Thread.Sleep(50);
                Console.WriteLine(doit);
            }

            var stop = Importer.L_READ_STOP();
            Console.WriteLine(stop);




            //var arr = new byte[10];
            //Marshal.Copy((IntPtr)pars.ptr, arr, 0, arr.Length);
            
           // //var t1 = Importer.Subtract1(15, 10);

           // int a = 1 << 0x6;

           // for (int i = 0x0; i < 4; i++)
           //     Console.WriteLine((ushort)(i | (1 << 0x6)));

           // //Dim u As UShort() = New UShort(127) {}
           // //u(0) = 64
           // //u(1) = &H41
           // //u(2) = 66
           // ushort[] u = new ushort[128];
           // u[0] = 64;
           // u[1] = 0x41;

           //// Console.ReadLine();
        }
    }
}

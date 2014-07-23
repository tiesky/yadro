{ ------------------------------------------------------------------------	}
{ *************************** ������ E14-440 *****************************	}
{ ------------------------------------------------------------------------	}
{ !!!!!!!!!!!!!!!!!!!!!!!! ���������� ���������� !!!!!!!!!!!!!!!!!!!!!!!!! }
{ ------------------------------------------------------------------------	}
{ ��।���� ����室��� ������															}
#define M(x) x*256 						{ ᤢ�� �� 8 ��� �����					  	}
#define PutPm(Reg,Addr) I4 = Addr; PM(I4, M4) = Reg
#define PutValPm(Reg,Val,Addr) Reg = Val; I4 = Addr; PM(I4, M4) = Reg
#define GetPm(Reg,Addr) I4 = Addr;  Reg = PM(I4, M4)

{ ************************************************************************ }
{ LBIOS Variables																				}
{ ************************************************************************ }
{ ������ ���� ᥣ���� �ணࠬ�														}
.VAR/SEG=INT_PM_USER/ABS=(VariableBaseAddress+0x0)/PM/RAM ProgaramBaseAddressVal;
.INIT ProgaramBaseAddressVal: M(ProgaramBaseAddress);

{ 䫠��� ��⮢���� � ���쭥�襩 ࠡ�� ��᫥ ����㧪� DSP						}
.VAR/SEG=INT_PM_USER/ABS=ReadyAddr/PM/RAM Ready;
.INIT Ready: M(0x0);

{ Test Var 1                                                     				}
.VAR/SEG=INT_PM_USER/ABS=TMode1Addr/PM/RAM TMode1;
.INIT TMode1: M(0x1111);

{ Test Var 2                                                     				}
.VAR/SEG=INT_PM_USER/ABS=TMode2Addr/PM/RAM TMode2;
.INIT TMode2: M(0x3333);

{ Test Variable                                                 				}
.VAR/SEG=INT_PM_USER/ABS=TestLoadVarAddr/PM/RAM TestLoadVar;
.INIT TestLoadVar: M(0x7777);

{ command for IRQE DSP                                           				}
.VAR/SEG=INT_PM_USER/ABS=CommandAddr/PM/RAM Command;
.INIT Command: M(0x77);

{ ࠧ�襭�� ���뢠��� IRQE (�������)													}
.VAR/SEG=INT_PM_USER/ABS=ImaskValueAddr/PM/RAM ImaskValue;
.INIT ImaskValue: M(0x010);

{ ����⥫� SCLK0_DIV ��� ࠡ��� ���'��	 	              							}
.VAR/SEG=INT_PM_USER/ABS=Sclk0DivAddr/PM/RAM Sclk0Div;
.INIT Sclk0Div: M(0x5);

{ DAC Rate Variable                                               			}
.VAR/SEG=INT_PM_USER/ABS=DacRateAddr/PM/RAM DacRate;
.INIT DacRate: M(49);

{ ADC Rate Variable                                                 			}
.VAR/SEG=INT_PM_USER/ABS=AdcRateAddr/PM/RAM AdcRate;
.INIT AdcRate: M(59);

{ ������ ࠧ�襭��/����饭�� ࠡ��� ���                 						}
.VAR/SEG=INT_PM_USER/ABS=AdcEnableAddr/PM/RAM AdcEnable;
.INIT AdcEnable: M(0x0);

{ ����騩 ������ ���� FIFO ���� ���                         				}
.VAR/SEG=INT_PM_USER/ABS=AdcFifoBaseAddressAddr/PM/RAM AdcFifoBaseAddress;
.INIT AdcFifoBaseAddress: M(0x0);

{ ������ ����� FIFO ���� ���								           				}
.VAR/SEG=INT_PM_USER/ABS=CurAdcFifoLengthAddr/PM/RAM CurAdcFifoLength;
.INIT CurAdcFifoLength: M(0x3000);

{ �������� ����� FIFO ���� ���								           				}
.VAR/SEG=INT_PM_USER/ABS=AdcFifoLengthOver2Addr/PM/RAM AdcFifoLengthOver2;
.INIT AdcFifoLengthOver2: M(0x1800);

{ �ॡ㥬�� ���� FIFO ���� ���, ������ �⥫��� �� ��⠭�����  			}
.VAR/SEG=INT_PM_USER/ABS=AdcFifoLengthAddr/PM/RAM AdcFifoLength;
.INIT AdcFifoLength: M(0x3000);

{ ������ ࠧ�襭��/����饭�� ���४�஢�� �室��� ������						}
.VAR/SEG=INT_PM_USER/ABS=IsCorrectionEnabledAddr/PM/RAM IsCorrectionEnabled;
.INIT IsCorrectionEnabled: M(0x0);

{ ���筨� ⠪⮢�� �����ᮢ ����᪠ ���												}
.VAR/SEG=INT_PM_USER/ABS=AdcClockSourceAddr/PM/RAM AdcClockSource;
.INIT AdcClockSource: M(0x0);

{ ���祭�� ������⭮�� ����� � ���													}
.VAR/SEG=INT_PM_USER/ABS=AdcSampleAddr/PM/RAM AdcSample;
.INIT AdcSample: M(0x0);

{ �����᪨� ����� ������ ��� ������⭮�� ����� � ���							}
.VAR/SEG=INT_PM_USER/ABS=AdcSampleChannelAddr/PM/RAM AdcSampleChannel;
.INIT AdcSampleChannel: M(0x0);

{ ------------------------------------------------------------------------ }
{ ��ࠬ���� ᨭ�஭���樨 ����� ������													}
.VAR/SEG=INT_PM_USER/ABS=InputModeAddr/PM/RAM InputMode;
.INIT InputMode: M(0x0);

/*.VAR/SEG=INT_PM_USER/ABS=IsTtlKadrSynchroAddr/PM/RAM IsTtlKadrSynchro;
.INIT IsTtlKadrSynchro: M(0x0);*/

.VAR/SEG=INT_PM_USER/ABS=SynchroAdFlagAddr/PM/RAM SynchroAdFlag;
.INIT SynchroAdFlag: M(0x0);

.VAR/SEG=INT_PM_USER/ABS=SynchroAdChannelAddr/PM/RAM SynchroAdChannel;
.INIT SynchroAdChannel: M(0x0);

.VAR/SEG=INT_PM_USER/ABS=SynchroAdPorogAddr/PM/RAM SynchroAdPorog;
.INIT SynchroAdPorog: M(0x0);

.VAR/SEG=INT_PM_USER/ABS=SynchroAdModeAddr/PM/RAM SynchroAdMode;
.INIT SynchroAdMode: M(0x0);

.VAR/SEG=INT_PM_USER/ABS=SynchroAdTypeAddr/PM/RAM SynchroAdType;
.INIT SynchroAdType: M(0x0);

.VAR/SEG=INT_PM_USER/ABS=SynchroAdStepAddr/PM/RAM SynchroAdStep;
.INIT SynchroAdStep: M(0x0);
{ ------------------------------------------------------------------------ }

{ ����� Control Table 																		}
.VAR/SEG=INT_PM_USER/ABS=ControlTableLenghtAddr/PM/RAM ControlTableLenght;
.INIT ControlTableLenght: M(0x8);

{ ����প� ��� ��ࢮ�� ����� � FIFO ���� ���                 				}
.VAR/SEG=INT_PM_USER/ABS=FirstSampleDelayAddr/PM/RAM FirstSampleDelay;
.INIT FirstSampleDelay: M(0x64); 	{ 100 - ����প� �� 2.5 ���				}

{ ����稭� ������஢�� ����প� � ������� ������ SCLK SPORT1					}
.VAR/SEG=INT_PM_USER/ABS=InterKadrDelayAddr/PM/RAM InterKadrDelay;
.INIT InterKadrDelay: M(0x0);

{ �����᪨� ����� ������ ��� ��� ������⭮�� ����� ������						}
.VAR/SEG=INT_PM_USER/ABS=AdcChannelAddr/PM/RAM AdcChannel;
.INIT AdcChannel: M(0x0);

{ ⥪�騩 ���४��㥬� �����᪨� ����� ������ ���								}
.VAR/SEG=INT_PM_USER/ABS=CorrectableAdcChannelAddr/PM/RAM CorrectableAdcChannel;
.INIT CorrectableAdcChannel: M(0x0);

{ ������⭮� ��室��� ���祭�� ��� ���                                		}
.VAR/SEG=INT_PM_USER/ABS=DacSampleAddr/PM/RAM DacSample;
.INIT DacSample: M(0x0);

{ ������ ࠧ�襭��/����饭�� ࠡ��� ��� � ०��� �뢮�� �� ����			}
.VAR/SEG=INT_PM_USER/ABS=DacEnableAddr/PM/RAM DacEnable;
.INIT DacEnable: M(0x0);

{ ����騩 ������ ���� FIFO ���� ���                         				}
.VAR/SEG=INT_PM_USER/ABS=DacFifoBaseAddressAddr/PM/RAM DacFifoBaseAddress;
.INIT DacFifoBaseAddress: M(0x3000);

{ ������ ����� FIFO ���� ���								             			}
.VAR/SEG=INT_PM_USER/ABS=CurDacFifoLengthAddr/PM/RAM CurDacFifoLength;
.INIT CurDacFifoLength: M(0xFC0);

{ �������� ����� FIFO ���� ���								           				}
.VAR/SEG=INT_PM_USER/ABS=DacFifoLengthOver2Addr/PM/RAM DacFifoLengthOver2;
.INIT DacFifoLengthOver2: M(0x7E0);

{ �ॡ㥬�� ����� FIFO ���� ���, ������ �⥫��� �� ��⠭�����   			}
.VAR/SEG=INT_PM_USER/ABS=DacFifoLengthAddr/PM/RAM DacFifoLength;
.INIT DacFifoLength: M(0xFC0);

{ --------------------- ��६���� ��� ࠡ��� � ���� --------------------- }
.VAR/SEG=INT_PM_USER/ABS=EnableFlashAddr/PM/RAM EnableFlash;
.INIT EnableFlash: M(0x0);
.VAR/SEG=INT_PM_USER/ABS=FlashAddressAddr/PM/RAM FlashAddress;
.INIT FlashAddress: M(0x0);
.VAR/SEG=INT_PM_USER/ABS=FlashDataAddr/PM/RAM FlashData;
.INIT FlashData: M(0x0);

{ ------------------ ��६���� ��� ࠡ��� � ��� ����� ------------------- }
{ ������ ࠧ�襭��/����饭�� �뢮�� �� ���											}
.VAR/SEG=INT_PM_USER/ABS=EnableTtlOutAddr/PM/RAM EnableTtlOut;
.INIT EnableTtlOut: M(0x0);
{ Ttl Out var                                       								}
.VAR/SEG=INT_PM_USER/ABS=TtlOutAddr/PM/RAM TtlOut;
.INIT TtlOut: M(0x0);
{ Ttl in var                                                    				}
.VAR/SEG=INT_PM_USER/ABS=TtlInAddr/PM/RAM TtlIn;
.INIT TtlIn: M(0x0);
{ ------------------------------------------------------------------------ }

{ ------------- ��६���� ��� ࠡ��� � ������筮� ०��� ---------------- }
.VAR/SEG=INT_PM_USER/ABS=NaladkaTestNumberAddr/PM/RAM NaladkaTestNumber;
.INIT NaladkaTestNumber: M(0x0);

.VAR/SEG=INT_PM_USER/ABS=NaladkaIrq0CounterAddr/PM/RAM NaladkaIrq0Counter;
.INIT NaladkaIrq0Counter: M(0x0);
{ ------------------------------------------------------------------------ }

{ Scale array for all gains                                      				}
.VAR/SEG=INT_PM_USER/ABS=ScaleFactorAddr/PM/RAM ScaleFactor[4];
.INIT ScaleFactor:      M(0x8000), M(0x8000), M(0x8000), M(0x8000);

{ ZeroOffset array for all gains                   								}
.VAR/SEG=INT_PM_USER/ABS=ZeroOffsetAddr/PM/RAM ZeroOffset[4];
.INIT ZeroOffset:      	M(0x0), M(0x0), M(0x0), M(0x0);

{ Control Table 																				}
.VAR/SEG=INT_PM_USER/ABS=ControlTableAddr/PM/RAM ControlTable[128];
.INIT ControlTable:   	M(0x0), M(0x1), M(0x2), M(0x3), M(0x4), M(0x5), M(0x6), M(0x7),
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
								0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0;

.VAR/SEG=INT_PM_USER/ABS=CalibrationArrayAddr/PM/RAM CalibrationArray[256];

.VAR/SEG=INT_PM_USER/ABS=DspInfoStructAddr/PM/RAM DspInfoStruct[256];
.INIT DspInfoStruct: <e440.dat>;

{ �������� ���������� � ������ ��������												}
/*.VAR/SEG=INT_PM_USER/ABS=0x4F/PM/RAM Test1;
.INIT Test1: M(0x2222);  */

{ � ������ ������ ������� ������� �� 0x3FC0 �� 0x3FDF ��������� ��������	}
{ ����७��� ��६����� ��� �࠭���� ॣ���� ��᪨								}
.VAR/SEG=INT_DM_USER/ABS=0x3FC0/DM/RAM TempImask;
{ ����७��� ��६����� ��� �࠭���� ������ � ���									}
.VAR/SEG=INT_DM_USER/ABS=0x3FC1/DM/RAM TempAdcSample;

{ �६���� ����७��� ��६����														}
.VAR/SEG=INT_DM_USER/ABS=0x3FC4/DM/RAM TempArray[4];

{ 0x3FDF -> Last Data Memory Word  														}

{ ---------------------------------------------------------------------	}
{	�㭪樨 ��� ࠡ��� � ���															}
{ ---------------------------------------------------------------------	}
{ *********************************************************************	}
{ ����祭�� ������ � ���																}
{ *********************************************************************	}
GetAdData:
{ ��३��� �� ��ன ���� ॣ���஢													}
	ENA SEC_REG;

{ ��⠭���� ᫥���饣� ������ ���													}
	AR = PM(I5, M5); IO(SET_ADC_CHANNEL) = AR;

{ �⥭�� ������ � ���																	}
	AR = IO(READ_ADC);
	AR = AR + AY1, AY1 = PM(I6, M6);
	MR = AR * MY1(SU), MY1 = PM(I6, M6);
	MR = MR(RND); DM(I3, M3) = MR1;
	M3 = 0x1;

{ �᫨ �㦭�, � ���६����㥬 ���稪 ����⮢ � ����, ���� RTI		}
Irq2Lab0:
	AF = AF - 0x1;
	IF NE RTI;

{ ��� ������஢�� ����প� � AF ���-�� ������ ������஢�� ����প�,		}
{ 				� ��� �����஢�� ᨭ�஭���樨 ��� JUMP EnableSynchroPulse	}
Irq2Lab1:
	AF = PASS AX1;

{ 㪠��⥫� I7 �� ��ࠡ��稪 ���뢠��� ��� ������஢�� ����প�			}
	I7 = ^InterKadrDelayIrq2;
{ 㪠��⥫� I5 �� ��砫� �ࠢ���饩 ⠡���� 									}
	I5 = ^ControlTable;
{ ��१���襬 ���� �����																}
	AR = PM(I5, M5); IO(SET_ADC_CHANNEL) = AR;
{ }
	M6 = 0x0;
{ ��室�� �� ��ࠡ��稪� ���뢠���													}
	RTI;

{ *********************************************************************	}
{ ��ࠡ��稪 ���뢠��� IRQ2 ��� ������஢�� ����প�							}
{ *********************************************************************	}
InterKadrDelayIrq2:
{ ��३��� �� ��ன ���� ॣ���஢													}
	ENA SEC_REG;

{ �⥭�� ������ � ���																	}
	AR = IO(READ_ADC);
	AR = AR + AY1, AY1 = PM(I6,M6);
	MR = AR*MY1(SU), MY1 = PM(I6,M6);
	MR = MR(RND);
	DM(I3, M3) = MR1;
	M3 = 0x0;

{ ����⠥� ���-�� ������ �㦭�� ��� ������஢�� ����প�					}
	AF = AF - 0x1;
	IF NE RTI; 			{ �᫨ �� �� ��, � ���� �멤�� �� ���뢠���	}

{ ������ ��ண� ������																	}
	AR = PM(I5, M5); IO(SET_ADC_CHANNEL) = AR;
{ AF - ���-�� ������� � ����															}
	AF = PASS AX0;
{ }
	M6 = 0x1;
{ ����⠭���� �⠭����� ��ࠡ��稪 ���뢠��� IRQ2(���)					}
	I7 = ^GetAdData;
{ ��室�� �� ��ࠡ��稪� ���뢠���													}
	RTI;

{ *********************************************************************	}
{ ����訬 IRQ1 ��� ������ ��-���஢�� ᨭ�஭���樨						}
{ *********************************************************************	}
EnableSynchroPulse:
{ ��⠭���� ���, �몫�稢 ����� SCLK , �.�. ᤥ���� �� ���譨�				}
	AR = 0x3F1F; 						{ 0011 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}

/*
!!!!!!!!!
ᤥ���� ࠧ�� �㭪樨 ��� ����७��� � ���譨� ������ ���
!!!!!!!!!
*/

{ ��࠭�� ॣ���� I4																	}
	AY0 = I4;

{ ࠧ�訬 ���뢠��� IRQE(�������) � IRQ1(������ ᨭ�஭�����)			}
	GetPm(AR,ImaskValueAddr);
	AR= SETBIT 2 OF AR;				{ ࠧ�訬 ���뢠��� IRQ1(����.ᨭ��.)}
	AR= CLRBIT 9 OF AR;				{ ����⨬ ���뢠��� IRQ2(���)			}
	PutPm(AR,ImaskValueAddr);

{ � ॣ���� AF ������ ��室����� ⥪�騩 ���稪 ���뢠���				}
	AF = PASS AX0;

{ ����⠭���� ॣ���� I4																}
	I4 = AY0;

{ �६���� ����⨬ �� ���뢠���													}
	DIS INTS;

{ ������� � ��� ॣ���� IMASK � ���� ���祭���								}
	POP STS;
	ENA SEC_REG;						{ ��३��� �� ��ன ���� ॣ���஢		}
	IMASK = AR;
	DIS SEC_REG; 						{ ��३��� �� ���� ���� ॣ���஢		}
	PUSH STS;
	
{ ⥯��� ����᪨�㥬 �� ���뢠���													}
	IMASK = 0x0; NOP;
{ ࠧ�訬 �� ࠧ��᪨஢���� ���뢠���										}
	ENA INTS;

{ �������� ����থ窠																	}
	CNTR = 43; DO ConvertLoop UNTIL CE; ConvertLoop: NOP;
/*
cntr=20; do ConvertLoop UNTIL CE; ConvertLoop: NOP;
ena sec_reg;
ar=dm(Test0); ar=clrbit 0 of ar; dm(Test0)=ar; io(ttl_out) = ar;
  */
{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF; NOP;
	
{ ��室�� �� ��ࠡ��稪� ���뢠���													}
	RTI;

{ *********************************************************************	}
{ ������� ����᪠ ࠡ��� ���															}
{ *********************************************************************	}
Start_Adc_cmd:
{ ��⠭���� ᡮ� ������ � ���															}
	CALL StopAdc;

{ �� ����� ��⪨ MainLoop �ய�襬 ��� CALL PutAcdData						}
	I4 = ^MainLoop; AR = ^PutAcdData; CALL ModifyCall;

{ ��ᨬ 䫠� ࠡ��� ���																}
	PutValPm(AR, 0x0, AdcEnableAddr);

{ ��⠭���� ����� ����᪠ ���														}
	GetPm(AR, AdcRateAddr);
	DM(Sport1_Sclkdiv) = AR;		{ 0x3FF1 - Serial Clock Divide Modulus	}

{ �ந��樠�����㥬 ����室��� 㪠��⥫� � ��६���� ���					}
	CALL InitAdcPointers;

{ �஢�ਬ ⨯ ����� ������ � ���													}
	GetPm(AR, InputModeAddr); NONE = PASS AR;
	IF NE JUMP Synchronization;	{ �᫨ ���� ᨭ��., � ��룠�� �� ���	}

{ ��⠭���� ���� � �����⮢�� ��ன ����� ��� ࠡ��� ���					}
	CALL SetAdcFirstChannel;
{ ����প� �� ��⠭������� ����������� �ࠪ�									}
	CALL SettlingDelay;
	
{ ��⠭���� 䫠� ࠡ��� ���															}
	PutValPm(AR, 0x1, AdcEnableAddr);

{ ࠧ��᪨�㥬 ���뢠��� IRQE(�������)				)							}
	GetPm(AR,ImaskValueAddr);
	AR = SETBIT 9 OF AR;				{ ࠧ��᪨�㥬 ���뢠��� IRQ2(���)		}
	PutPm(AR,ImaskValueAddr);
	IMASK = AR; NOP;

{ ��⠭���� ���筨� �����ᮢ ����᪠ ���										}	
	CALL SetAdcClockSource;
{ �����⨬ ���, ����稢 ����� SCLK , �.�. ᤥ���� �� ����७���			}
	AR = 0x7F1F; 						{ 0111 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR; 		{ 0x3FF2 - SPORT1 Control Register		}

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ������� ��⠭��� ࠡ��� ���															}
{ *********************************************************************	}
Stop_Adc_cmd:
{ ��⠭���� ᡮ� ������ � ���															}
	CALL StopAdc;

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ �㭪�� ��⠭��� ࠡ��� ���															}
{ *********************************************************************	}
StopAdc:
{ �६���� ����⨬ �� ���뢠���													}
	DIS INTS;							
	
{ ��⠭���� ���, �몫�稢 ����� SCLK , �.�. ᤥ���� �� ���譨�				}
	AR = 0x3F1F; 						{ 0011 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}
	
{ ��ᨬ�� ���筨� �����ᮢ ����᪠ ���										}	
	CALL ClearAdcClockSource;

{ ࠧ��᪨�㥬 ���뢠��� IRQE(�������)											}
	GetPm(AR,ImaskValueAddr);
	AR = CLRBIT 2 OF AR;			{ ����᪨�㥬 ���뢠��� IRQ1(����.ᨭ��.)}
	AR = CLRBIT 9 OF AR;			{ ����᪨�㥬 ���뢠��� IRQ2(���)			}
	PutPm(AR,ImaskValueAddr);
	IMASK = AR; NOP;

{ ࠧ�訬 �� ࠧ��᪨஢���� ���뢠���										}
	ENA INTS;							

{ ��ᨬ 䫠� ࠡ��� ���																}
	PutValPm(AR, 0x0, AdcEnableAddr);
	
{ ����প� �� 2 ��� ��� ����砭�� ���஢��, ⠪ ��� � ��� ������ 		}
{								����� ����� � ��室��� ॣ���� ����� ������	}
	CALL Delay2mks;

{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ������� ����祭�� ���� ����⮢ � ��� (�᫨ ��������)						}
{ *********************************************************************	}
Adc_Kadr_cmd:
{ � ࠡ�� ��� ࠧ�襭�?																}
	GetPm(AR, AdcEnableAddr); NONE= PASS AR;
	IF NE JUMP EndOfCommand; 	{ �������� �ਥ�� ��� ��� - ���� �멤��	}

{ ��⠭���� ᡮ� ������ � ���															}
	CALL StopAdc;

{ � �� ����� ��⪨ MainLoop ���� �ய���� ��� CALL GetAcdKadr				}
	I4 = ^MainLoop; AR = ^GetAcdKadr; CALL ModifyCall;

{ ��⠭���� ����� ����᪠ ���														}
	GetPm(AR, AdcRateAddr);
	DM(Sport1_Sclkdiv) = AR;		{ 0x3FF1 - Serial Clock Divide Modulus	}

{ �ந��樠�����㥬 ����室��� 㪠��⥫� � ��६���� ���					}
	CALL InitAdcPointers;
{ ��⠭���� ��ࢮ� ����� ��� ࠡ��� ���											}
	CALL SetAdcFirstChannel;
{ ����প� �� ��⠭������� ����������� �ࠪ�									}
	CALL SettlingDelay;

{ ࠧ��᪨�㥬 ���뢠��� IRQE(�������) � IRQ2(���)							}
	GetPm(AR,ImaskValueAddr);
	AR = SETBIT 9 OF AR;				{ ࠧ��᪨�㥬 ���뢠��� IRQ2(���)		}
	PutPm(AR,ImaskValueAddr);
	IMASK = AR; NOP;

{ �����⨬ ���, ����稢 ����� SCLK , �.�. ᤥ���� �� ����७���			}
	AR = 0x7F1F; 						{ 0111 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR; 		{ 0x3FF2 - SPORT1 Control Register		}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ �㭪�� ᡮ� ������ ���� ����⮢ � ���										}
{ *********************************************************************	}
GetAcdKadr:
{ � �� ��⮢� �� 㦥 ����� ���� ����⮢ � ���?								}
	AR = I3;									{ ����稬 '������' FIFO ���			}
	AY0 = I2;  								{ ����稬 '墮��' FIFO ���				}
	AR = AR - AY0;
	GetPm(AY0, CurAdcFifoLengthAddr);{ ⥪��� ����� FIFO ���� ���		}
	IF LT AR = AR + AY0;		{ ⥯��� � AR ��⨭��� ࠧ��� ����� I3 � I2	}
	GetPm(AY0, ControlTableLenghtAddr);	{ ����� �ࠢ���饩 ⠡����		}
	NONE = AR - AY0; 						{ �ࠢ��� �� ����稭�					}
	IF LT RTS;								{ �᫨ �����, � �멤��				}

{ �� ����� ��⪨ MainLoop �ய�襬 ��� CALL PutAcdData						}
	I4 = ^MainLoop; AR = ^PutAcdData; CALL ModifyCall;

	JUMP Stop_Adc_cmd;

{ *********************************************************************	}
{ ������� ����祭�� ������⭮�� ����� � ��� (�᫨ ��������)				}
{ *********************************************************************	}
Adc_Sample_cmd:
{ � ࠡ�� ��� ࠧ�襭�?																}
	GetPm(AR, AdcEnableAddr); NONE= PASS AR;
	IF NE JUMP EndOfCommand; 	{ �������� �ਥ�� ��� ��� - ���� �멤��	}

{ ��⠭���� ᡮ� ������ � ���															}
	CALL StopAdc;

{ ��⠭���� �ॡ㥬� ��������� ����� ��� ������⭮�� ����� ������		}
	GetPm(AR, AdcSampleChannelAddr); PutPm(AR, AdcChannelAddr);	CALL SetAdcChannel;
{ ����প� �� ��⠭������� ᨣ���� � ���������� �ࠪ� (� ����ᮬ)		}
	CNTR = 1000;
	DO SettlingDelayLoop0 UNTIL CE;
SettlingDelayLoop0: NOP;

{ ��⠭���� ���� ��ࠡ��稪 ���뢠��� IRQ2(���)								}
{									ᯥ樠�쭮 ������⭮�� ����� ������ � ���	}
	I7 = ^Irq2AdcSample;

{ � �� ����� ��⪨ MainLoop ���� �ய���� ��� CALL AdcSampleIdle0		}
	I4 = ^MainLoop; AR = ^AdcSampleIdle0; CALL ModifyCall;

{ ��⠭���� ���ᨬ����� ����� ����᪠ ��� ࠢ��� 400���					}
	AR = 59;
	DM(Sport1_Sclkdiv) = AR;		{ 0x3FF1 - Serial Clock Divide Modulus	}

{ ��ᨬ 䫠���-�ਧ��� ������ ����ࠡ�⠭���� ���뢠��� �� ���		}
	SI = 0x0;

{ ��।���� ����� ����� �㤥� ���४��㥬�										}	
	GetPm(AR, AdcChannelAddr); PutPm(AR, CorrectableAdcChannelAddr);
{ ������� � ॣ����� � AY1 � MY1 ���४�஢��� ����. ���४. ������	}
	CALL GetCalibrCoef;

{ ࠧ�訬 ���뢠��� IRQE(�������) � IRQ2(���)									}
	GetPm(AR, ImaskValueAddr);
	AR = SETBIT 9 OF AR;				{ ࠧ�訬 ���뢠��� IRQ2(���)			}
	PutPm(AR, ImaskValueAddr);
	IMASK = AR; NOP;

{ ࠧ�訬 ��� ����稢 �����, �.�. ᤥ���� SCLK1 ����७���					}
	AR = 0x7F1F;						{ 0111 1101 1100 1111						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ �ய�᪠�� ���� ����� ��� - �� ����											}
{ *********************************************************************	}
AdcSampleIdle0:
{ �஢�ਬ ����稥 ��⮢��� ������ � ���											}
	AR = SI; NONE = PASS AR; IF EQ RTS;

{ ⥯��� �� ����� ��⪨ MainLoop ���� �ய���� ��� CALL AdcSampleIdle1	}
	I4 = ^MainLoop; AR = ^AdcSampleIdle1; CALL ModifyCall;
{ ��ᨬ �ਧ��� ������ ������ � ���											}
	SI = 0x0;
{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ � ᫥���騩 �, �� ����																}
{ *********************************************************************	}
AdcSampleIdle1:
{ �஢�ਬ ����稥 ��⮢��� ������ � ���											}
	AR = SI; NONE = PASS AR; IF EQ RTS;

{ � TempAdcSample ��室���� ᪮�४�஢���� ��� � �ॡ㥬��� ������ ���}
	AR = DM(TempAdcSample); PutPm(AR, AdcSampleAddr);

{ ��⠭���� ���, �몫�稢 ����� SCLK , �.�. ᤥ���� �� ���譨�				}
	AR = 0x3F1F; 						{ 0011 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}

{ ࠧ��᪨�㥬 ���뢠��� IRQE(�������)											}
	GetPm(AR,ImaskValueAddr);
	AR = CLRBIT 2 OF AR;			{ ����᪨�㥬 ���뢠��� IRQ1(����.ᨭ��.)}
	AR = CLRBIT 9 OF AR;			{ ����᪨�㥬 ���뢠��� IRQ2(���)			}
	PutPm(AR,ImaskValueAddr);
	IMASK = AR; NOP;

{ ����⠭���� �⠭����� ��ࠡ��稪 ���뢠��� IRQ2(���)					}
	I7 = ^GetAdData;
{ �� ����� ��⪨ MainLoop ����室��� �ய���� ��� CALL PutAcdData		}
	I4 = ^MainLoop; AR = ^PutAcdData; CALL ModifyCall;
{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ������ �����᪮�� ������ � �஬������ � ��室��� ॣ��� �������	}
{ *********************************************************************	}
SetAdcChannel:
{ ������� �����᪨� ������ ��� ᫥����� ���� ����⮢						}
{ PF5 � ���� - ������ � �஬������ ॣ���� �������							}
	AR=DM(Prog_Flag_Data); AR=CLRBIT 5 OF AR; DM(Prog_Flag_Data)=AR;

{ ������ �����᪮�� ������ � �஬������ ॣ���� �������					}
	GetPm(AR, AdcChannelAddr); IO(SET_ADC_CHANNEL) = AR;

{ PF5 � ������� - ��१����� � ��室��� ॣ���� �������						}
	AR=DM(Prog_Flag_Data); AR=SETBIT 5 OF AR; DM(Prog_Flag_Data)=AR;
{ PF5 � ���� - ������ � �஬������ ॣ���� �������							}
	AR=DM(Prog_Flag_Data); AR=CLRBIT 5 OF AR; DM(Prog_Flag_Data)=AR;

{ ������ �����᪮�� ������ � �஬������ ॣ���� �������					}
	GetPm(AR, AdcChannelAddr); IO(SET_ADC_CHANNEL) = AR;
{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ��।������ � ���४�஢��묨 �����樥�⠬�								}
{ *********************************************************************	}
GetCalibrCoef:
{ 㬮��⥫�� ���४�஢��� ����. ᬥ饭�� � �ᨫ���� ��� ���		}
	AR = 0x0000; DM(TempArray + 0) = AR;
	AR = 0x8000; DM(TempArray + 1) = AR;

{ ࠧ�襭� �� ���४�஢�� �室��� ������										}
	GetPm(AR, IsCorrectionEnabledAddr); NONE = PASS AR; IF EQ JUMP SetCorrectionCoefs;

{ ��ࠬ���� ࠡ��� 㪠��⥫� I4														}
	M4 = 0x0; L4 = 0x0;

{ 㧭��� ������ �ᨫ���� ��� �ᯮ��㥬��� ������ ���							}
	GetPm(AR, CorrectableAdcChannelAddr); SR = LSHIFT AR BY -6 (LO);
	M4 = SR0;

{ 	Set ZeroOffset																			}
	I4 = ^ZeroOffset; MODIFY(I4, M4);
	AR = PM(I4, M4); DM(TempArray + 0) = AR;

{ 	Set ScaleFactor																		}
	I4 = ^ScaleFactor; MODIFY(I4, M4);
	AR = PM(I4, M4); DM(TempArray + 1) = AR;

{ ����⠭���� ��ࠬ���� ࠡ��� 㪠��⥫� I4										}
	M4 = 0x0; L4 = 0x0;

{ �����⥫쭮 ࠧ��ࠥ��� � ���४�஢��묨 �����樥�⠬�				}
SetCorrectionCoefs:
	ENA SEC_REG;						{ ��३��� �� ��ன ���� ॣ���஢		}
		AY1 = DM(TempArray + 0);	{ AY1 - ����. �����樥�� ᬥ饭�� ���	}
		MY1 = DM(TempArray + 1);	{ MY1 - ����. �����樥�� ����⠡ ���	}
	DIS SEC_REG;					  	{ ��३��� �� ���� ���� ॣ���஢		}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ��⠭���� ���筨� �����ᮢ ����᪠ ���										}	
{ *********************************************************************	}
SetAdcClockSource:
{ ����� �� ������ ���筨� ⠪⮢�� �����ᮢ ����᪠ ��� �ॡ����?		}
	GetPm(AR, AdcClockSourceAddr); AR = AR - 0x1; AY0 = AR;

{ 㧭��� ⥪�饥 ���ﭨ� 䫠��� PFx												}	
	AR = DM(Prog_Flag_Data); 
{ ��ᨬ 䫠�� PF2 � PF6, �⢥��騥 �� ���筨� ⠪⮢�� �����ᮢ		}
{																				����᪠ ���	}	
	AR = SETBIT 2 OF AR; AR = CLRBIT 6 OF AR; 	{ ����७��� ����� ���	}
	
{ ᥩ�� ����� ���஡����� ��।������� � ������� ���筨���				}
{ ⠪⮢�� �����ᮢ ����᪠ ���														}
	NONE = PASS AY0;	IF EQ AR = CLRBIT 2 OF AR;	{ ����. � �࠭��樥�	}
/*	NONE = AY0 - 0x1;	IF EQ AR = CLRBIT 2 OF AR;*/
	NONE = AY0 - 0x1;	IF EQ AR = SETBIT 6 OF AR;	{ ���譨� ����� ���		}

{ ⥯��� ����� ��⠭����� 䫠�� PF2 � PF6 � ᮮ⢥��ᢨ� �					}
{									�ॡ㥬� ���筨��� �����ᮢ ����᪠ ���	}
	DM(Prog_Flag_Data) = AR;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ��ᨬ ���筨� �����ᮢ ����᪠ ���											}	
{ *********************************************************************	}
ClearAdcClockSource:
{ 㧭��� ⥪�饥 ���ﭨ� 䫠��� PFx												}	
	AR = DM(Prog_Flag_Data); 
{ ��ᨬ 䫠�� PF2 � PF6, �⢥��騥 �� ���筨� ⠪⮢�� �����ᮢ		}
{																				����᪠ ���	}	
	AR = SETBIT 2 OF AR; AR = CLRBIT 6 OF AR;
{ ⥯��� ����� ��⠭����� 䫠�� PF2 � PF6 										}
	DM(Prog_Flag_Data) = AR;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ ********************************************************************* }
{  �᭮���� �ணࠬ�� ��� ࠡ��� c ���㫥� E14-440               			}
{ ********************************************************************* }
#include "const.h"

.MODULE/ABS=ProgaramBaseAddress/SEG=int_pm_user             main_program;
.ENTRY StartBios, Irq1Handler, CommandProcessing;

#include "var.h"

{ After DSP RESET we appear to be here                                  }
StartBios:
	DIS SEC_REG;  						{ ��३��� �� ���� ���� ॣ���஢		}

{ I4, M4, L4 �ᯮ������� ��� ����㯠 � ��६���� LBIOS						}
	M4 = 0x0; L4 = 0x0;
	M7 = 0x0; L7 = 0x0;

{ ����襬 � ᮮ⢥�����騥 ��६���� ����室��� ���祭��					}
	PutValPm(AR, 0x5555, TMode1Addr);
	PutValPm(AR, 0xAAAA, TMode2Addr);

{ ��ନ�㥬 ⥪���� ����� LBIOS													}
/*	AR = LBIOS_VERMAJOR; SR = LSHIFT AR BY 8 (LO);
	AR = LBIOS_VERMINOR; SR = SR OR LSHIFT AR BY 0 (LO);
	PutPm(SR0,LbiosVersionAddr);*/

{ Set Wait State for ALL External Devices                   				}
	AR = 0x0001;						{ 0000 0000 0000 0001 						}
	DM(Dm_Wait_Reg) = AR;			{ 0x3FFE - Waitstate Control Register 	}

{ SPORT0 - disable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1}
	AR = 0x0000;
	DM(Sys_Ctrl_Reg)=AR;				{ 0x3FFF - System Control Register 		}

{ *********************************************************************	}
{ 	Set SPORT0 for transmit digital codes in DAC                    	 	}
{ 	SCLK and Transmit Frame - internal, word = 16 bits                	}
{ 	Serial Clock Divide Modulus 														}
	GetPm(AR, Sclk0DivAddr);		{ SCLK0 period = 250 ns						}
	DM(Sport0_Sclkdiv) = AR;		{ 0x3FF5 - Serial Clock Divide Modulus	}

{ Receive Frame Sync Divide Modulus 												}
	GetPm(AR, DacRateAddr); { ��।���� ����� �뢮�� ����⮢ � ���	}
	DM(Sport0_Rfsdiv) = AR; { 0x3FF4 - Receive Frame Sync Divide Modulus }

{ ����頥� ��⮡���ਧ��� SPORT0 � �⪫�砥� CLOCKOUT						}
	AR = 0x4000; DM(Sport0_Autobuf_Ctrl) = AR;

{ Control word for SPORT0 : SCLK - internal 										}
{ low level, alternate internal receive frame on each word(16 bit) 		}
{ low level, alternate external transmit frame on each word(16 bit)		}
	AR = 0x7DCF;						{ 0111 1101 1100 1111						}
	DM(Sport0_Ctrl_Reg) = AR;		{ 0x3FF6 - SPORT0 Control Register		}
{ *********************************************************************	}

{ *********************************************************************	}
{ * Set SPORT1 for start of ADC chip	                           	 *	}
{ Serial Clock Divide Modulus 														}
	GetPm(AR, AdcRateAddr);		{ ���� ����᪠ ���							}
	DM(Sport1_Sclkdiv) = AR;	{ 0x3FF1 - Serial Clock Divide Modulus		}

{ Receive Frame Sync Divide Modulus 												}
	AR = 0xF;					{ RFS period = 16 SCLK	 							}
	DM(Sport1_Rfsdiv) = AR;	{ 0x3FF0 - Receive Frame Sync Divide Modulus	}

{ Control word for SPORT1 : SCLK - external 										}
{ high level, alternate internal receive frame on each word(16 bit)		}
{ high level, alternate internal transmit frame on each word(16 bit)		}
	AR = 0x3F1F; 						{ 0011 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}
{ *********************************************************************	}

{ IRQ0, IRQ1, IRQ2 - negative front, disable Interrupt Nesting 			}
	ICNTL = 0x07;

{ ���⨬ ������ �� �� ���뢠���													}
	IFC = 0xFF; NOP; NOP;

{ ��⠭���� 䫠� FO � ������ ���ﭨ�	- ࠧ�襭�� ����㧪� ���			}
	RESET FLAG_OUT;

{ ��⠭���� 䫠�� FLx � ��室��� ���ﭨ�										}
	RESET FL0, RESET FL1, RESET FL2;

{ PF0 � '1' ��� ��ॢ��� TTL DSP OUT � ���� ���ﭨ�						}
{ PF2 � '1' � PF6 � '0' ��� ����७���� ⠪�஢���� ��� ��� �࠭��樨	}
{ PF5 � '0' ��� ����� ����� ������ � �஬������ ����					}
	AR = 0x05; DM(Prog_Flag_Data) = AR;

{ ��⠭���� 䫠�� PFx: PF0-PF3,PF5 � PF6 - ��室��, PF4 � PF7 - �室��}
	AR = 0x6F; DM(Prog_Flag_Comp_Sel_Ctrl) = AR;

{ � ��� �� ����� �ᥣ�� ⮫쪮 ADSP-2185, �.�. ���� ����७�� ������ 	}
	DMOVLAY = 0x0; PMOVLAY = 0x0;
{ ����稬 �஡�� ०�� 㬭���⥫�													}
	DIS M_MODE;

{ �ந��樠�����㥬 ����室��� 㪠��⥫� � ��६����						}
	CALL InitAdcPointers;
	CALL InitDacPointers;

{ ���㫨� ॣ���� TX0 (����� ��� ���)												}
	TX0 = 0x0;

{ SPORT0 - enable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1	}
	AR = 0x1000;					 	{ 0001 0000 0000 0000 						}
	DM(Sys_Ctrl_Reg) = AR;      	{ 0x3FFF - System Control Register 		}

{ �᫨ �㦭� - �஢�ਬ ����稥 ���筮�� ��⠭�� �����					}
	CALL WaitForSecondaryPowerOn;

{ ����� ����� �᭮���� �ணࠬ��, �� ���� ��������� ��⮢� � ࠡ��	}
	PutValPm(AR, 0x1, ReadyAddr);

{ �᫨ ����饭� ࠡ�� ���, � ����� SCLK ������ �� �㤥�					}
	GetPm(AR, AdcEnableAddr); NONE = PASS AR;
	IF EQ JUMP MainLoopEntry;

{ ����稬 �����, �.�.ᤥ���� SCLK ����७���										}
	AR = 0x7F1F; 						{ 0111 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}

{ =====================================================================	}
{ �᭮���� 横� ࠡ��� ���	�/��� ���												}
{ =====================================================================	}
MainLoopEntry:
{ ࠧ�訬 ����室��� ���뢠��� 													}
	GetPm(AR,ImaskValueAddr);
	IMASK = AR; NOP;

MainLoop:
	CALL PutAcdData;
	CALL GetDacData;
	CALL CheckDacSample;
	CALL CommandProcessing;

	JUMP MainLoop;

{ *********************************************************************	}
{ ��楤�� ��।�� ��⮢�� ������ �� FIFO ���� ��� � ��					}
{ *********************************************************************	}
PutAcdData:
{ � ࠡ�� ��� ࠧ�襭�?																}
	GetPm(AR, AdcEnableAddr); NONE = PASS AR;
	IF EQ RTS;								{ �᫨ ���, � ���� �멤��			}

{ � �� ��� �� ���뫠�� ����� � ��?												}
	AR = I3;									{ ����稬 '������' FIFO ���			}
	AY0 = I2;  								{ ����稬 '墮��' FIFO ���				}
	AR = AR-AY0;
	GetPm(AY0, CurAdcFifoLengthAddr);{ ⥪��� ����� FIFO ���� ���		}
	IF LT AR = AR+AY0;		{ ⥯��� � AR ��⨭��� ࠧ��� ����� I3 � I2	}
	GetPm(AY0, AdcFifoLengthOver2Addr);	{ �������� ����� FIFO ���� ���	}
	NONE = AR-AY0;   						{ �ࠢ��� �� � ��������� �����		}
	IF LT RTS;								{ �᫨ �����, � �멤��				}

{ �᫨ ����� ��⮢�, � ᣥ��ਬ ���뢠��� � AVR �� ����� ���			}
	AR = DM(Prog_Flag_Data); AR = SETBIT 1 OF AR; DM(Prog_Flag_Data) = AR;
	NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP;
	AR = CLRBIT 1 OF AR; DM(Prog_Flag_Data) = AR;
{ �������㥬 㪠��⥫� �� 墮�� FIFO ���� ��								}
	MODIFY(I2, M2);

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ��楤�� �ਥ�� ����� ������ ��� ���� ��� �� ��							}
{ *********************************************************************	}
GetDacData:
{ � ࠡ�� ��� ࠧ�襭�?																}
	GetPm(AR, DacEnableAddr); NONE = PASS AR;
	IF EQ RTS;								{ �᫨ ���, � ���� �멤��			}

{ � �� ��� �� �ਭ����� ����� �� ��?												}
	AR = I1;									{ ����稬 '������' FIFO ���			}
	AY0 = I0;  								{ ����稬 '墮��' FIFO ���				}
	AR = AR - AY0;
	GetPm(AY0, CurDacFifoLengthAddr);{ ⥪��� ����� FIFO ���� ���		}
	IF LT AR = AR + AY0;		{ ⥯��� � AR ��⨭��� ࠧ��� ����� I3 � I2	}
	GetPm(AY0, DacFifoLengthOver2Addr);	{ �������� ����� FIFO ���� ���	}
	NONE = AR - AY0; 						{ �ࠢ��� �� � ��������� �����		}
	IF LT RTS;								{ �᫨ �����, � �멤��				}

{ �᫨ ����� ��⮢�, � ᣥ��ਬ ���뢠��� � AVR �� ����� ���			}
	AR = DM(Prog_Flag_Data); AR = SETBIT 3 OF AR; DM(Prog_Flag_Data) = AR;
	NOP; NOP; NOP; NOP; NOP; NOP;
	NOP; NOP; NOP; NOP; NOP; NOP;
	AR= CLRBIT 3 OF AR; DM(Prog_Flag_Data) = AR;
{ �������㥬 㪠��⥫� �� 墮�� FIFO ���� ���								}
	MODIFY(I0, M0);

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ �஢�ઠ ����砭�� ������⭮�� �뢮�� �� ���									}
{ *********************************************************************	}
CheckDacSample:
{ 䫠��� �����襭�� �뤠� ����� �� ���											}
	AR = SB; NONE = PASS AR; IF EQ RTS;

{ ����⨬ �� ���뢠���																}
	DIS INTS;
{ ࠧ�訬 ���뢠��� IRQE(�������)													}
	GetPm(AR, ImaskValueAddr);
	AR = CLRBIT 6 OF AR;			{ ����⨬ ���뢠��� SPORT0 Transmit(���)}
	PutPm(AR, ImaskValueAddr);
	IMASK = AR; NOP;
{ ࠧ�訬 �� ���뢠���																}
	ENA INTS;

{ ��ᨬ 䫠��� �����襭�� ������⭮� �뤠� ����� �� ���				}
	SB = 0x0;
{ ����থ窠 �� 4 ���																	}
	CALL Delay2mks; CALL Delay2mks;

{ ⥯��� �몫�稬 SPORT0 (���)														}
{ SPORT0 - disable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1}
	AR = 0x0;
	DM(Sys_Ctrl_Reg)=AR;		  		{ 0x3FFF - System Control Register 		}
{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ��ࠡ��稪 ���뢠��� IRQ1 (����. ᨭ�஭�����)								}
{ *********************************************************************	}
Irq1Handler:

/*
!!!!!!!!!
ᤥ���� ࠧ�� ��ࠡ��稪� ��� ����७��� � ���譨� ������ ���
!!!!!!!!!
*/

	ENA SEC_REG;			{ ��३��� �� ��ன ���� ॣ���஢					}

{ ࠧ�訬 ���뢠��� IRQE(�������) � IRQ2(���)									}
	I7 = ImaskValueAddr; AR = PM(I7, M7);
	AR = CLRBIT 2 OF AR;				{ ����⨬ ���뢠��� IRQ1(����.ᨭ��.)}
	AR = SETBIT 9 OF AR;				{ ࠧ�訬 ���뢠��� IRQ2(���)			}
	PM(I7, M7) = AR; DM(TempImask) = AR;

{ ������� � ��� ॣ���� IMASK � ���� ���祭���								}
	DIS INTS;				{ ����⨬ �� ���뢠���								}
	POP STS;
	IMASK = DM(TempImask);
	PUSH STS;
	IMASK = 0x0;
	ENA SEC_REG;			{ ��३��� �� ��ன ���� ॣ���஢					}
	ENA INTS;				{ ࠧ�訬 �� ���뢠���								}

{ ��⠭���� 䫠���, �� ࠧ�襭� ࠡ�� ���										}
	AR = 0x1; I7 = AdcEnableAddr; PM(I7, M7) = AR;
{ ����⠭���� �⠭����� ��ࠡ��稪 ���뢠��� IRQ2(���)					}
	I7 = ^GetAdData;
{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF; /*NOP;*/

{ �⫠���																					}
/*ar=dm(Test0); ar=setbit 0 of ar; dm(Test0)=ar; io(ttl_out) = ar;*/

{ �����⨬ ���, ����稢 ����� SCLK1, �.�. ᤥ���� �� ����७���			}
	AR = 0x7F1F; 						{ 0111 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR; 		{ 0x3FF2 - SPORT1 Control Register		}

{ ��室�� �� ��ࠡ��稪� ���뢠���													}
	RTI;

{ ********************************************************************  }
{ ��� ����� �������																	}
{ ********************************************************************  }
CommandProcessing:
{ �஢�ਬ ���� �� ����ࠡ�⠭��� �������?										}
	AR = SE; NONE = PASS AR;
	IF EQ RTS; 							{ �᫨ ���, � ���� �멤��				}

{ ��ᨬ 䫠��� ����ࠡ�⠭��� �������											}
	SE = 0x0;

	GetPm(AR, CommandAddr);
	AR = PASS AR;
	IF EQ JUMP Test_cmd;       				{ ����� ������� 0					}
	AR = AR-0x1;
	IF EQ JUMP Enable_Flash_Write_cmd;		{ ������� ����� 1					}
	AR = AR-0x1;
	IF EQ JUMP Read_Flash_Word_cmd; 			{ ������� ����� 2					}
	AR = AR-0x1;
	IF EQ JUMP Write_Flash_Word_cmd;			{ ������� ����� 3					}
	AR = AR-0x1;
	IF EQ JUMP Start_Adc_cmd;			 		{ ����� ������� 4					}
	AR = AR-0x1;
	IF EQ JUMP Stop_Adc_cmd;			 		{ ����� ������� 5					}
	AR = AR-0x1;
	IF EQ JUMP Adc_Kadr_cmd;  					{ ����� ������� 6					}
	AR = AR-0x1;
	IF EQ JUMP Adc_Sample_cmd;  				{ ����� ������� 7					}
	AR = AR-0x1;
	IF EQ JUMP Start_Dac_cmd;			  		{ ����� ������� 8					}
	AR = AR-0x1;
	IF EQ JUMP Stop_Dac_cmd;			  		{ ����� ������� 9					}
	AR = AR-0x1;
	IF EQ JUMP Dac_Sample_cmd;			  		{ ����� ������� 10				}
	AR = AR-0x1;
	IF EQ JUMP Enable_TTL_Out_cmd; 			{ ����� ������� 11  				}
	AR = AR-0x1;
	IF EQ JUMP Get_TTL_cmd;   					{ ����� ������� 12				}
	AR = AR-0x1;
	IF EQ JUMP Put_TTL_cmd;   					{ ����� ������� 13				}
	AR = AR-0x1;
	IF EQ JUMP Nakadka_Mode_cmd;				{ ����� ������� 14				}

{ �訡��� ����� �������																}
ErrorEndOfCommand:
	PutValPm(AR, 0xFFFF, CommandAddr);

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;			   								{ �த����� ࠡ���				}

{ *********************************************************************	}
{ ���⮢�� �������.                                                   	}
{ ��᫥ ��室� � �祩��	����� ������ TestLoadVar								}
{  													������ ���� �᫮ 0xAA55		}
{ *********************************************************************	}
Test_cmd:
	PutValPm(AR,0xAA55,TestLoadVarAddr);

{ *********************************************************************	}
{ ��室 �� ��ࠡ��稪� ������� IRQE													}
{ ��᫥ ��室� � �祩��	����� ������ Command ������ ���� �᫮ 0x0		}
{ *********************************************************************	}
EndOfCommand:
	PutValPm(AR, 0x0, CommandAddr);

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;			   								{ �த����� ࠡ���				}

{ *********************************************************************	}
{	��⠭���� 㪠��⥫��	��� ���														}
{ *********************************************************************	}
InitAdcPointers:
{ ��⠭���� ��ࠬ���� ��� ���४�஢�� �室��� ������						}
	I5 = ^ControlTable; M5 = 0x1; L5 = 0x0;

	I6 = ^CalibrationArray; M6 = 0x1;
	GetPm(AR, ControlTableLenghtAddr); 	{ ����� �ࠢ���饩 ⠡����		}
	SR = LSHIFT AR BY 1 (LO);				{ 㬭���� �� ���						}
	L6 = SR0;

	GetPm(AR, IsCorrectionEnabledAddr);
	NONE = PASS AR;			{ ࠧ�襭� �� ���४�஢�� �室��� ������	}
	IF EQ JUMP NoAdcDataCorrection;{ �᫨ ��, ��⮢�� ᮮ⢥�����騩 ���ᨢ}

	GetPm(AR, ControlTableLenghtAddr); 	{ ����� �ࠢ���饩 ⠡����		}
	CNTR = AR;
	DO CtrlLoop UNTIL CE;
		AR = PM(I5, M5);
		SR = LSHIFT AR BY -6 (LO);
		M4 = SR0;

{	 	Set ZeroOffset																		}
		I4 = ^ZeroOffset;
		MODIFY(I4, M4);
		AR = PM(I4, M4);
		PM(I6, M6) = AR;

{	 	Set ScaleFactor																	}
		I4 = ^ScaleFactor;
		MODIFY(I4, M4);
		AR = PM(I4, M4);
		PM(I6, M6) = AR;
CtrlLoop: NOP;

ContinueAdcPar:
{ ��⠭���� �ॡ㥬� ࠧ���� FIFO ���� ���									}
	CALL CheckAdcFifoLimits;

{ �������� ����� FIFO ���� ���														}
	GetPm(AR, CurAdcFifoLengthAddr);
	SR = LSHIFT AR BY -1 (LO);
	PutPm(SR0, AdcFifoLengthOver2Addr);

{ Set ADC Fifo Parameters (㪠��⥫� �� '������' FIFO ���)					}
	GetPm(AR, AdcFifoBaseAddressAddr);
	I3 = AR;
	M3 = 0x0;									{ ���� ����� ����				}
	GetPm(AR, CurAdcFifoLengthAddr);
	L3 = AR;

{ 㪠��⥫� �� '墮��' FIFO ���� ���												}
	I2 = I3;
{ � M2 ������ ���� �������� ����� FIFO ���� ���								}
	GetPm(AR, AdcFifoLengthOver2Addr);
	M2 = AR;
	L2 = L3;

{ Set pointer to ControlTable															}
	I5 = ^ControlTable;
	M5 = 0x1;
	GetPm(AR, ControlTableLenghtAddr); 	{ ����� �ࠢ���饩 ⠡����		}
	L5 = AR;

{ ���ᨢ �����樥�⮢ ��� ���४�஢�� ᬥ饭�� ��� � ����⠡� ���	}
	I6 = ^CalibrationArray;
	M6 = 0x1;
	GetPm(AR, ControlTableLenghtAddr); 	{ ����� �ࠢ���饩 ⠡����		}
	SR = LSHIFT AR BY 1 (LO);				{ 㬭���� �� ���						}
	L6 = SR0;

{ ��⠭���� �⠭����� ��ࠡ��稪 ���뢠��� IRQ2(���)						}
	I7 = ^GetAdData;
{ �� ����� ��⪨ Irq2Lab0 �ய�襬 ��� RTI										}
	I4 = ^Irq2Lab0; AR = 0x0A00; PX = 0x1F; PM(I4, M4) = AR;
{ �஢�ਬ ���� �� ������஢�� ����প�											}
	GetPm(AR, InterKadrDelayAddr); NONE = PASS AR;
	IF EQ RTS;									{ �᫨ ���, � �宭�窮 �멤��	}

{ ���! � ��� ���� ������஢�� ����প�												}
{ ⥯��� �� ����� ��⪨ Irq2Lab0 ���� �ய���� ��� AF=AF-0x1				}
	I4 = ^Irq2Lab0; AR = 0x2710; PX = 0x0F; PM(I4, M4) = AR;
{ � �� ����� ��⪨ Irq2Lab1 ���� �ய���� ��� AF= PASS AX1					}
	I4 = ^Irq2Lab1; AR = 0x2679; PX = 0x0F; PM(I4, M4) = AR;

	ENA SEC_REG;						{ ��३��� �� ��ன ���� ॣ���஢		}
	GetPm(AX0, ControlTableLenghtAddr);	{ AX0 - ���-�� ��⨢��� ������� 	}
	AF = PASS AX0;								{ AF - ⥪�騩 ���稪 ���뢠���}
	GetPm(AX1, InterKadrDelayAddr);		{ AX1 - ������஢�� ����প�		}
	DIS SEC_REG;					  	{ ��३��� �� ���� ���� ॣ���஢		}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ �᫨ ���४�஢�� �� �㦭�, ��⮢�� ᮮ⢥�����騩 ���ᨢ				}
{ *********************************************************************	}
NoAdcDataCorrection:
	GetPm(AR, ControlTableLenghtAddr); 	{ ����� �ࠢ���饩 ⠡����		}
	CNTR = AR;
	DO CtrlLoop1 UNTIL CE;
{	 	Set ZeroOffset																		}
		AR = 0x0;
		PM(I6, M6)=AR;

{	 	Set ScaleFactor																	}
		AR = 0x8000;
		PM(I6, M6)=AR;

CtrlLoop1: 	NOP;

	JUMP ContinueAdcPar;

{ *********************************************************************	}
{ ��⠭���� ��ࢮ�� ������ ��� ࠡ��� ���											}
{ *********************************************************************	}
SetAdcFirstChannel:
{ ������� �����᪨� ������ ��� ᫥����� ���� ����⮢						}
{ PF5 � ���� - ������ � �஬������ ॣ���� �������							}
	AR = DM(Prog_Flag_Data); AR = CLRBIT 5 OF AR; DM(Prog_Flag_Data) = AR;

{ ������ ��ࢮ�� ������																	}
	AR = PM(I5, M5); IO(SET_ADC_CHANNEL) = AR;

{ PF5 � ������� - ��१����� � ��室��� ॣ���� �������						}
	AR = DM(Prog_Flag_Data); AR = SETBIT 5 OF AR; DM(Prog_Flag_Data) = AR;
{ PF5 � ���� - ������ � �஬������ ॣ���� �������							}
	AR = DM(Prog_Flag_Data); AR = CLRBIT 5 OF AR; DM(Prog_Flag_Data) = AR;

{ ������ ��ண� ������																	}
	AR = PM(I5, M5); IO(SET_ADC_CHANNEL) = AR;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ��⠭���� 㪠��⥫�� ��� ���														}
{ *********************************************************************	}
InitDacPointers:
{ ��⠭���� �ॡ㥬� ࠧ���� FIFO ���� ���									}
	CALL CheckDacFifoLimits;

{ �������� ����� FIFO ���� ���														}
	GetPm(AR, CurDacFifoLengthAddr);
	SR = LSHIFT AR BY -1 (LO);
	PutPm(SR0, DacFifoLengthOver2Addr);

{ Set DAC Fifo Parameters (㪠��⥫� �� '������' FIFO ���)					}
	GetPm(AR, DacFifoBaseAddressAddr);
	I1 = AR;
	M1 = 0x1;
	GetPm(AR, CurDacFifoLengthAddr);
	L1 = AR;

{ 㪠��⥫� �� '墮��' FIFO ���� ���												}
	I0 = I1;
{ � M0 ������ ���� �������� ����� FIFO ���� ���								}
	GetPm(AR, DacFifoLengthOver2Addr);
	M0 = AR;
	L0 = L1;

{ ��ᨬ 䫠��� �����襭�� ������⭮� �뤠� ����� �� ���				}
	SB = 0x0;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ ********************************************************************  }
{ �஢�ਬ ����稥 ���筮�� ��⠭�� �����										}
{ ********************************************************************  }
WaitForSecondaryPowerOn:
{ ���⠥� ॢ���� �����																}
	PutValPm(AR, 0xC400, FlashAddressAddr); CALL Read_Flash_Word;
	AY0 = 0X00FF; AR = SR0 AND AY0;
{ ॢ���� ����� ������ ���� 'F' ��� ���											}
	AY0 = 0x0046;/*Rev.F*/ NONE = AR - AY0; IF LT RTS;
{ ��� ����砭�� ��⠭���� ���筮�� ��⠭�� �����							}
SecondaryPowerOnLoop:	
	AR = DM(Prog_Flag_Data); AR = TSTBIT 7 OF AR; 
	IF EQ JUMP SecondaryPowerOnLoop;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;


{ ********************************************************************  }
{ ����প� ��⠭������� ����������� �ࠪ�										}
{ ********************************************************************  }
SettlingDelay:
	GetPm(AR, FirstSampleDelayAddr);
	CNTR = AR;
	DO SettlingDelayLoop UNTIL CE;
SettlingDelayLoop: NOP;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ ********************************************************************  }
{ ����প� �� 2.0 ��� 																	}
{ ********************************************************************  }
Delay2mks:
	CNTR = 100;
	DO Delay2mksLoop UNTIL CE;
Delay2mksLoop: NOP;

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

#include "adc.h"
#include "dac.h"
#include "synchro.h"
#include "fifo.h"
#include "flash.h"
#include "ttl.h"
#include "naladka.h"

.ENDMOD;

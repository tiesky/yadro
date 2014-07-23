{ ---------------------------------------------------------------------	}
{ � ������ 䠩�� ��室���� �㭪樨 ����᪠ ᨭ�஭���樨, � ������:		}
{ - ��஢�� ᨭ�஭���樨 ���� ��� �����஢��                    		}
{ - ���������� ᨭ�஭���樨 �� �஢�� ��� ���室�							}
{ ---------------------------------------------------------------------	}

{ *********************************************************************	}
{ ��⠭���� ⨯� ᨭ�஭���樨														}
{ *********************************************************************	}
Synchronization:
	GetPm(AR, InputModeAddr);		{ �஢�ਬ ⨯ ᨭ�஭���樨				}

	AR = AR - 0x1;
	IF EQ JUMP TtlSynchroStart;	{ �᫨ 1, � ��஢�� 						}
											{						ᨭ�஭����� ����	}
	AR = AR - 0x1;
	IF EQ JUMP TtlSynchroKadr;		{ �᫨ 2, � �����஢��        			}
											{ 					��஢�� ᨭ�஭�����	}
	AR = AR - 0x1;
	IF EQ JUMP AdcSynchroStart;	{ �᫨ 3, � ᨭ�஭����� 				}
											{								�� ������ ���	}
	JUMP ErrorEndOfCommand;			{ �멤�� � �訡��� � �������				}

{ *********************************************************************	}
{ ��஢�� ᨭ�஭����� ����														}
{ *********************************************************************	}
TtlSynchroStart:
{ �ந��樠�����㥬 ����室��� 㪠��⥫� � ��६����						}
/*	CALL InitAdcPointers;*/
{ ��⠭���� ��ࢮ� ����� ��� ࠡ��� ���											}
	CALL SetAdcFirstChannel;
{ ����প� �� ��⠭������� ����������� �ࠪ�									}
	CALL SettlingDelay;

{ ࠧ�訬 ���뢠��� IRQE(�������) � IRQ1(������ ᨭ�஭�����)			}
	GetPm(AR, ImaskValueAddr);
	AR = SETBIT 2 OF AR;				{ ࠧ�訬 ���뢠��� IRQ1					}
	PutPm(AR, ImaskValueAddr);
	
{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF; NOP;

{ ⥯��� ����� ��⠭����� �ॡ㥬�� ���� ���뢠���							}
	IMASK = AR;

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ �����஢�� ��஢�� ᨭ�஭�����													}
{ *********************************************************************	}
TtlSynchroKadr:
{ �ந��樠�����㥬 ����室��� 㪠��⥫� � ��६����						}
/*	CALL InitAdcPointers;*/
{ ��⠭���� ���� ����� ��� ࠡ��� ���											}
	CALL SetAdcFirstChannel;
{ ����প� �� ��⠭������� ����������� �ࠪ�									}
	CALL SettlingDelay;

{ ⥯��� �� ����� ��⪨ Irq2Lab0 ���� �ய���� ��� AF = AF - 0x1			}
	I4 = ^Irq2Lab0; AR = 0x2710; PX = 0x0F; PM(I4, M4) = AR;

{ � �� ����� ��⪨ Irq2Lab1 ���� �ய���� ��� JUMP EnableSynchroPulse	}
	I4 = ^Irq2Lab1; AR = ^EnableSynchroPulse; CALL ModifyJump;

	ENA SEC_REG;						{ ��३��� �� ��ன ���� ॣ���஢		}
		GetPm(AX0, ControlTableLenghtAddr);	{ AX0 - ���-�� ��⨢��� ������� 	}
		AF= PASS AX0;								{ AF - ⥪�騩 ���稪 ���뢠���}
	DIS SEC_REG;					  	{ ��३��� �� ���� ���� ॣ���஢		}

{ ࠧ�訬 ���뢠��� IRQ2(�������) � IRQ1(������ ᨭ�஭�����)			}
	GetPm(AR, ImaskValueAddr);
	AR = SETBIT 2 OF AR;				{ ࠧ�訬 ���뢠��� IRQ1					}
	PutPm(AR, ImaskValueAddr);
	
{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF; NOP;

{ ⥯��� ����� ��⠭����� �ॡ㥬�� ���� ���뢠���							}
	IMASK = AR;

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ᨭ�஭����� �� ����������� ������ ���											}
{ *********************************************************************	}
AdcSynchroStart:
{ �ந��樠�����㥬 ����室��� 㪠��⥫� � ��६����						}
/*	CALL InitAdcPointers;*/
{ ��⠭���� ᨭ�ப����	���������� ᨭ�஭���樨								}
	GetPm(AR, SynchroAdChannelAddr); PutPm(AR, AdcChannelAddr); CALL SetAdcChannel;
{ ����প� �� ��⠭������� ����������� �ࠪ�									}
	CALL SettlingDelay;

{ ��⠭���� ���ᨬ����� ����� ����᪠ ��� ࠢ��� 400.0 ���	 			}
{												��� ���� ����������� ᨭ�ப�����	}
	AR = 59;
	DM(Sport1_Sclkdiv) = AR;		{ 0x3FF1 - Serial Clock Divide Modulus	}
	
{ ��⠭���� ���� ��ࠡ��稪 ���뢠��� IRQ2	 									}
{									ᯥ樠�쭮 ������⭮�� ����� ������ � ���	}
	I7 = ^Irq2AdcSample;

{ � �� ����� ��⪨ MainLoop ���� �ய���� ��� CALL AdSynchroIdle0		}
	I4 = ^MainLoop; AR = ^AdSynchroIdle0; CALL ModifyCall;

{ ���� 蠣 ���������� ᨭ�஭���樨												}
	PutValPm(AR, 0x0, SynchroAdStepAddr);

{ ��ᨬ 䫠���-�ਧ��� ������ ����ࠡ�⠭���� ���뢠��� �� ���		}
	SI = 0x0;

{ ��।���� ����� ����� �㤥� ���४��㥬�										}	
	GetPm(AR, AdcChannelAddr); PutPm(AR, CorrectableAdcChannelAddr);
{ ������� � ॣ����� � AY0 � MY1 ���४�஢��� ����. ���४. ������	}
	CALL GetCalibrCoef;

{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF; NOP;

{ ࠧ�訬 ���뢠��� IRQE(�������)													}
	GetPm(AR, ImaskValueAddr);
	AR = SETBIT 9 OF AR;				{ ࠧ�訬 ���뢠���  IRQ2(���)			}
	PutPm(AR, ImaskValueAddr);
	IMASK = AR; NOP;

{ ࠧ�訬 ��� ����稢 �����, �.�. ᤥ���� SCLK1 ����७���					}
	AR = 0x7F1F;						{ 0111 1101 1100 1111						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ��ࠡ��稪 ���뢠��� IRQ2 ��� ������⭮�� ����� ������ ���				}
{ *********************************************************************	}
Irq2AdcSample:
{ ��३��� �� ��ன ���� ॣ���஢													}
	ENA SEC_REG;

{ ��⠥� ���祭�� � ⥪�饣� ������ ���											}
	AR = IO(READ_ADC);
{ ���४�஢�� ᬥ饭��																}
	AR = AR + AY1;
{ ���४�஢�� ����⠡�																}
	MR = AR * MY1(SU);
{ ���㣫���� १����																	}
	MR = MR(RND);
{ ������� ᪮�४�஢���� ������ � ��� �� �६����� ��६�����			}
	Dm(TempAdcSample) = MR1;
	
{ ��३��� �� ���� ���� ॣ���஢													}
	DIS SEC_REG; 						
	
{ ��⠭���� 䫠���-�ਧ���, �� ���� ����ࠡ�⠭��� ���뢠��� �� ���	}
	SI = 0x1;

{ ��室�� �� ��ࠡ��稪� ���뢠���													}
	RTI;

{ *********************************************************************	}
{ �ய�᪠�� ���� ����� ��� - �� ����											}
{ *********************************************************************	}
AdSynchroIdle0:
{ �஢�ਬ ����稥 ��⮢��� ������ � ���											}
	AR = SI; NONE = PASS AR; IF EQ RTS;

{ ⥯��� �� ����� ��⪨ MainLoop ���� �ய���� ��� CALL AdSynchroIdle1	}
	I4 = ^MainLoop; AR = ^AdSynchroIdle1; CALL ModifyCall;
{ ��ᨬ �ਧ��� ������ ������ � ���											}
	SI = 0x0;
{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ � ��⠫�� �, �� ����																}
{ *********************************************************************	}
AdSynchroIdle1:
{ �஢�ਬ ����稥 ��⮢��� ������ � ���											}
	AR = SI; NONE = PASS AR; IF EQ RTS;

{ ��५���� ᪮�४�஢����� ���祭�� ������ � ��� � ॣ���� AR			}
	AR = Dm(TempAdcSample);
{ ⥯��� ������� �஢�મ� �᫮��� ᨭ�஭���樨 								}
	GetPm(AY0, SynchroAdTypeAddr);		{ �� �஢�� ��� ���室�			}
	NONE = PASS AY0;
	IF NE JUMP StepSynchro;

LastAdSynchro:
	GetPm(AY0, SynchroAdPorogAddr); 		{ ��ண ���������� ᨭ�஭���樨	}
	AF = AR - AY0;
	GetPm(AR, SynchroAdModeAddr);
	NONE = PASS AR;
	IF EQ AF = -AF;
	AF = PASS AF;
	IF LT JUMP SynchroAdDetected;
	RTS;

StepSynchro:
	GetPm(AY0, SynchroAdStepAddr);
	NONE = PASS AY0;
	IF NE JUMP LastAdSynchro;
	GetPm(AY0, SynchroAdPorogAddr);
	AF = AR - AY0;
	GetPm(AR, SynchroAdModeAddr);
	NONE = PASS AR;
	IF NE AF = -AF;
	AF = PASS AF;
	IF GE RTS;
	PutValPm(AR, 0x1, SynchroAdStepAddr);
	RTS;

SynchroAdDetected:
{ ��⠭���� ��� �몫�稢 �����, �.�. ᤥ���� SCLK1 ���譨�					}
	AR = 0x3F1F; 						{ 0011 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR;		{ 0x3FF2 - SPORT1 Control Register		}

{ ����᪨�㥬 �� ���뢠���															}
	IMASK = 0x0; NOP;

{ ����প� �� 2 ��� ��� ����砭�� ���஢��, ⠪ ��� � ��� ������ 		}
{								����� ����� � ��室��� ॣ���� ����� ������	}
	CALL Delay2mks;
	
{ ��⠭���� ��ࢮ� ����� ��� ࠡ��� ���											}
	CALL SetAdcFirstChannel;
{ ����প� �� ��⠭������� ����������� �ࠪ�									}
	CALL SettlingDelay;

{ ��⠭���� �ॡ㥬�� ����� ����᪠ ���											}
	GetPm(AR, AdcRateAddr);
	DM(Sport1_Sclkdiv) = AR;		{ 0x3FF1 - Serial Clock Divide Modulus	}
	
{ ����⠭���� ���� ��ࠡ��稪 ���뢠��� IRQ2 								}
	I7 = ^GetAdData;
{ ������� �� ����� ��⪨ MainLoop ���� �ய���� ��� CALL PutAcdData		}
	I4 = ^MainLoop; AR = ^PutAcdData; CALL ModifyCall;

{ ��⠭���� 䫠���, �� ࠧ�襭� ࠡ�� ���										}
	PutValPm(AR, 0x1, AdcEnableAddr);

{ ���⨬ ������, �஬� IRQE(�������) � SPORT0 Transmit (���)				}
	IFC = 0xAF; NOP;

{ ࠧ�訬 ���뢠��� IRQE(�������)													}
	GetPm(AR, ImaskValueAddr);
	AR = SETBIT 9 OF AR;				{ ࠧ�訬 ���뢠��� IRQ2(���)			}
	PutPm(AR, ImaskValueAddr);
	IMASK = AR; NOP;

{ �����⨬ ���, ����稢 ����� SCLK , �.�. ᤥ���� �� ����७���			}
	AR = 0x7F1F; 						{ 0111 1111 0001 1111 						}
	DM(Sport1_Ctrl_Reg) = AR; 		{ 0x3FF2 - SPORT1 Control Register		}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ������� � PM �� ��������� ����� ������� JUMP xxx			    			}
{ �室:                                                     	  			}
{      I4 - ����, �㤠 �����뢠���� �������             					}
{      AR - ���� ���室� xxx														}
{ *********************************************************************	}
ModifyJump:
{ ����頥� �� ���뢠���																}
/*	DIS INTS;*/

{ ��ନ�㥬 ��� ������� 																}
	SR1=0x0018;								{ 00000000 00011000 										}
	SR0=0x000F;								{ 00000000 00001111 						}
	SR=SR OR LSHIFT AR BY 4(LO);
	PX=SR0;									{ ����稫� 8 ������ ��� 				}
	AR=SR1;
	SR=LSHIFT SR0 BY -8(LO);
	SR=SR OR LSHIFT AR BY -8(HI);		{ ����稫� ���訥 16 ��� 				}
	M4=0x0;
	PM(I4, M4)=SR0;						{ ���࠭��� �� 24-��⮢�� ᫮�� 	}

{ ࠧ�蠥� �� ���뢠���																}
/*	ENA INTS;*/

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ������� � PM �� ��������� ����� ������� CALL xxx			    			}
{ �室:                                                     	  			}
{      I4 - ����, �㤠 �����뢠���� �������             					}
{      AR - ���� ���室� xxx														}
{ *********************************************************************	}
ModifyCall:
{ ����頥� �� ���뢠���																}
/*	DIS INTS;*/

{ ��ନ�㥬 ��� ������� 																}
	SR1=0x001C;								{ 00000000 00011100 						}
	SR0=0x000F;								{ 00000000 00001111 						}
	SR=SR OR LSHIFT AR BY 4(LO);
	PX=SR0;									{ ����稫� 8 ������ ��� 				}
	AR=SR1;
	SR=LSHIFT SR0 BY -8(LO);
	SR=SR OR LSHIFT AR BY -8(HI);		{ ����稫� ���訥 16 ��� 				}
	M4=0x0;
	PM(I4, M4)=SR0;						{ ���࠭��� �� 24-��⮢�� ᫮�� 	}

{ ࠧ�蠥� �� ���뢠���																}
/*	ENA INTS;*/

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

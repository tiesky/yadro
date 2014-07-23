{ ---------------------------------------------------------------------	}
{	�㭪樨 ��� ࠡ��� � ���															}
{ ---------------------------------------------------------------------	}

{ *********************************************************************	}
{ ������� ����᪠ ���																	}
{ *********************************************************************	}
Start_Dac_cmd:
{ SPORT0 - disable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1}
	AR = 0x0;
	DM(Sys_Ctrl_Reg) = AR;			{ 0x3FFF - System Control Register 		}

{ �ந��樠�����㥬 ����室��� 㪠��⥫� ���									}
	CALL InitDacPointers;

{ ��⠭���� �ॡ㥬�� ����� �뢮�� ����⮢ �� ���							}
	GetPm(AR, DacRateAddr);
	DM(Sport0_Rfsdiv) = AR; { 0x3FF4 - Receive Frame Sync Divide Modulus }

{ ࠧ�蠥� ��⮡���ਧ��� ��� ��।�� ������ �� SPORT0					}
{  - ���� ������ ��� ���: I1, M1, L1								 				}
{ �⪫�砥� CLOCKOUT																		}
	AR = 0x4282;					{ 0100 0010 1000 0010							}
	DM(Sport0_Autobuf_Ctrl) = AR;

{ ��⠭���� 䫠� ࠡ��� ���															}
	PutValPm(AR, 0x1, DacEnableAddr);

{ SPORT0 - enable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1	}
	AR = 0x1000;					 	{ 0001 0000 0000 0000 						}
	DM(Sys_Ctrl_Reg) = AR;      	{ 0x3FFF - System Control Register 		}

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ������� ��⠭��� ���																	}
{ *********************************************************************	}
Stop_Dac_cmd:
{ SPORT0 - disable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1}
	AR = 0x0;
	DM(Sys_Ctrl_Reg)=AR;				{ 0x3FFF - System Control Register 		}

{ ����頥� ��⮡���ਧ��� SPORT0 � �⪫�砥� CLOCKOUT						}
	AR = 0x4000; DM(Sport0_Autobuf_Ctrl) = AR;

{ ��⠭���� �ॡ㥬�� ����� �뢮�� ����⮢ �� ���							}
	GetPm(AR, DacRateAddr);
	DM(Sport0_Rfsdiv) = AR; { 0x3FF4 - Receive Frame Sync Divide Modulus }

{ SPORT0 - enable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1	}
	AR = 0x1000;					 	{ 0001 0000 0000 0000 						}
	DM(Sys_Ctrl_Reg) = AR;      	{ 0x3FFF - System Control Register 		}

{ ��ᨬ 䫠� ࠡ��� ���																}
	PutValPm(AR, 0x0, DacEnableAddr);

{ ��⭮� �����襭�� �믮������ �������											}
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ������� �뤠� ������⭮�� ����� �� ��� (�᫨ ��������)				}
{ *********************************************************************	}
Dac_Sample_cmd:
{ � ࠡ�� ��� ࠧ�襭�?																}
	GetPm(AR, DacEnableAddr); NONE = PASS AR;
	IF NE JUMP EndOfCommand; 	{ ������� �뤠� �� ��� - ���� �멤��	}

{ SPORT0 - disable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1}
	AR = 0x0;
	DM(Sys_Ctrl_Reg) = AR;			{ 0x3FFF - System Control Register 		}

{ ����頥� ��⮡���ਧ��� SPORT0 � �⪫�砥� CLOCKOUT						}
	AR = 0x4000; DM(Sport0_Autobuf_Ctrl) = AR;

{ ��⠭���� ���ᨬ����� ����� �뢮�� ����⮢ �� ��� (125 ���)			}
	AR = 31;
	DM(Sport0_Rfsdiv) = AR; { 0x3FF4 - Receive Frame Sync Divide Modulus }

{ SPORT0 - enable, SPORT1 - disable, SPORT1 - FI, FO, IRQ0, IRQ1, SCLK1	}
	AR = 0x1000;					 	{ 0001 0000 0000 0000 						}
	DM(Sys_Ctrl_Reg) = AR;      	{ 0x3FFF - System Control Register 		}

{ ��ᨬ 䫠��� �����襭�� ������⭮� �뤠� ����� �� ���				}
	SB = 0x0;
{ ���⨬ ������, �஬� IRQE(�������) � IRQ2 (���)							}
	IFC = 0x6F; NOP;

{ ����稬 �������� ����� ��� �뢮�� �� ���									}
	GetPm(AR, DacSampleAddr); TX0 = AR;

{ �६���� ����⨬ �� ���뢠���													}
	DIS INTS;							
{ �������⥫쭮 ࠧ��᪨�㥬 ���뢠��� SPORT0 Transmit(���)				}
	GetPm(AR, ImaskValueAddr);
	AR = SETBIT 6 OF AR;				{ ��� ���뢠��� SPORT0 Transmit			}
	PutPm(AR, ImaskValueAddr);
	IMASK = AR; NOP;
{ ࠧ�訬 �� ࠧ��᪨஢���� ���뢠���										}
	ENA INTS;							

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

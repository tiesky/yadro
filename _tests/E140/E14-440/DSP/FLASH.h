{ ---------------------------------------------------------------------	}
{	�㭪樨 ��� ࠡ��� � ���짮��⥫�᪨� ����									}
{ ---------------------------------------------------------------------	}

{ *********************************************************************	}
{ ����襭��/����饭�� ��楤��� ����� � ���짮��⥫�᪮� ����			}
{ *********************************************************************	}
Enable_Flash_Write_cmd:
	GetPm(SR0, EnableFlashAddr);		{ ��⠥� 䫠�	ࠧ�襭�� ����		}

	SET FL0, RESET FL1;					{ ��⠭���� chip select ����			}
												{ ����� ���� � ����						}
	CNTR = 0x9;
	DO EnableFlashLoop UNTIL CE;
		AR = TSTBIT 15 OF SR0;
		IF NE SET FL2;    				{ ����� � ����							}
		IF EQ RESET FL2;

		CALL Delay3mks;
		TOGGLE FL1;
		CALL Delay3mks;
		TOGGLE FL1;
EnableFlashLoop:	SR = LSHIFT SR0 BY 1 (LO);

	RESET FL0, RESET FL1;		{ ��ᨬ chip select  ����� ���� � ����	}

{ �����訬 �������																		}	
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ������� �⥭�� ᫮�� �� ���짮��⥫�᪮�� ����								}
{ *********************************************************************	}
Read_Flash_Word_cmd:
{ �⠥� ᫮�� �� ���짮��⥫�᪮�� ����											}
	CALL Read_Flash_Word;
{ �����訬 �������																		}	
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ��楤�� ����� ᫮�� � ���짮��⥫�᪮� ����								}
{ *********************************************************************	}
Write_Flash_Word_cmd:
	GetPm(SR0, FlashAddressAddr);		{ ��⠥� ���� �祩�� ����			}

	SET FL0, RESET FL1;					{ ��⠭���� chip select ����			}
												{ ����� ���� � ����						}
{ ��।���� ���� �祩�� ����															}
	CNTR = 0x9;
	DO WriteFlashLoop1 UNTIL CE;
		AR = TSTBIT 15 OF SR0;
		IF NE SET FL2;    				{ ����� � ����							}
		IF EQ RESET FL2;

		CALL Delay3mks;
		TOGGLE FL1;
		CALL Delay3mks;
		TOGGLE FL1;
WriteFlashLoop1: 	SR = LSHIFT SR0 BY 1 (LO);

	GetPm(SR0, FlashDataAddr);			{ ��⠥� ����� ��� ����� � ����	}

{ ����襬 ���祭�� � �������� �祩�� ����											}
	CNTR = 0x10;
	DO WriteFlashLoop2 UNTIL CE;
		AR = TSTBIT 15 OF SR0;
		IF NE SET FL2;    				{ ����� � ����							}
		IF EQ RESET FL2;

		CALL Delay3mks;
		TOGGLE FL1;
		CALL Delay3mks;
		TOGGLE FL1;
WriteFlashLoop2:	SR = LSHIFT SR0 BY 1 (LO);

	RESET FL0, RESET FL1;		{ ��ᨬ chip select  ����� ���� � ����	}

{ �����訬 �������																		}	
	JUMP EndOfCommand;

{ *********************************************************************	}
{ ��楤�� �⥭�� ᫮�� �� ���짮��⥫�᪮�� ����								}
{ *********************************************************************	}
Read_Flash_Word:
	GetPm(SR0, FlashAddressAddr);		{ ��⠥� ���� �祩�� ����			}

	SET FL0, RESET FL1;					{ ��⠭���� chip select ����			}
												{ ����� ���� � ����						}
{ ��।���� ���� �祩�� ����															}
	CNTR = 0x9;
	DO ReadFlashLoop1 UNTIL CE;
		AR = TSTBIT 15 OF SR0;
		IF NE SET FL2;    				{ ����� � ����							}
		IF EQ RESET FL2;

		CALL Delay3mks;
		TOGGLE FL1;
		CALL Delay3mks;
		TOGGLE FL1;
ReadFlashLoop1:	SR = LSHIFT SR0 BY 1 (LO);

	CALL Delay3mks;

{ ��⠥� ���祭�� �� �������� �祩�� ����										}
	SR0 = 0x0;
	CNTR = 0x10;
	DO ReadFlashLoop2 UNTIL CE;
		SR = LSHIFT SR0 BY 1 (LO);

		CALL Delay3mks;
		TOGGLE FL1;
		CALL Delay3mks;
		TOGGLE FL1;

		AR = 0x0;
		IF NOT FLAG_IN JUMP ReadFlashLoop2;
		AR = 0x1;
ReadFlashLoop2:	SR = SR OR LSHIFT AR BY 0 (LO);

	PutPm(SR0, FlashDataAddr);	{ ��࠭�� ����祭��� ᫮��					}

	RESET FL0, RESET FL1;		{ ��ᨬ chip select  ����� ���� � ����	}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ *********************************************************************	}
{ ����প� �� 3.0 ���																	}
{ *********************************************************************	}
Delay3mks:
	CNTR = 145;
	DO Delay3mksLoop UNTIL CE;
Delay3mksLoop: NOP;
	RTS;

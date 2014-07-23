{ ********************************************************************* }
{ �㭪樨 ��� ࠡ��� � FIFO ���஬ ��� � ��� �� ���� E-440,				}
{ 							�� ���ன ����� ���� ��⠭����� ⮫쪮 ADSP-2185	}
{ ********************************************************************* }

{ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	}
{ *********************************************************************	}
{  ������� ���䨣��஢���� FIFO ���� ��� 										}
{ 	 ���祭�� ���� FIFO ����� ���� ࠢ�� ⮫쪮 0x0 � ����� ������		}
{	 ���祭�� ����� FIFO � ��������� �� 64 (0x40) �� 12288 (0x3000) ����	}
{ *********************************************************************	}
CheckAdcFifoLimits:
	PutValPm(AR, 0x0, AdcFifoBaseAddressAddr);	{ ������ ���� FIFO ���	}

	GetPm(AR, AdcFifoLengthAddr);
	AY0 = 0x40;							{ �������쭮 ��������� ����� FIFO ���	}
	NONE = AR-AY0;
	IF GE JUMP CheckMaxAdcFifoLength;
	PutPm(AY0, CurAdcFifoLengthAddr);
	RTS;

CheckMaxAdcFifoLength:
	AY0 = 0x3000;	 	  				{ ���ᨬ��쭮 ��������� ����� FIFO ���	}
	NONE = AR-AY0;
	IF GT AR = PASS AY0;
	PutPm(AR, CurAdcFifoLengthAddr);

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

{ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	}
{ ********************************************************************* }
{  �㭪�� ���䨣��஢���� FIFO ���� ��� 										}
{   ���祭�� ���� FIFO ����� ���� ࠢ�� ⮫쪮 0x3000 � ����� ������	}
{	 ���祭�� ����� FIFO � ��������� �� 64 (0x40) �� 4032 (0xFC0) ����	}
{ ********************************************************************* }
CheckDacFifoLimits:
	PutValPm(AR, 0x3000, DacFifoBaseAddressAddr);	{ ������ ���� FIFO ���}

	GetPm(AR, DacFifoLengthAddr);	{ �㦭�� ����� FIFO ���						}
	AY0 = 0x40;							{ �������쭮 ��������� ����� FIFO ���	}
	NONE = AR-AY0;
	IF GE JUMP CheckMaxDacFifoLength;
	AR = PASS AY0;
	PutPm(AR, CurDacFifoLengthAddr);
	RTS;

CheckMaxDacFifoLength:
	AY0 = 0xFC0;						{ ���ᨬ��쭮 ��������� ����� FIFO ���	}
	NONE = AR-AY0;
	IF GT AR= PASS AY0;
	PutPm(AR, CurDacFifoLengthAddr);	{ ��⠭�������� ����� FIFO ���	}

{ ᯮ����� ��室�� �� �㭪樨															}
	RTS;

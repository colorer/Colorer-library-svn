PROGRAM $NDIR B16 ."
������� �����p NORTON�.          ��p�� 1.3. 25 ����    1997."
[�����஢�� ��� ����� 3.3 � ���� ���ᨨ 5.0.]
:: 1 VALUE KOLWIN    [�������⢮ ����-���㯠�����]


       FIX BYTE VAR  Panel   [H���p ���㯠���� 0/1]
::     FIX BYTE VAR RAZDCELL [������ ࠧ����⥫� �祥�] #  ! RAZDCELL
KOLWIN     WORD VCTR I       [������ ��p����뢠���� ��p���]
KOLWIN ::  WORD VCTR L       [������ ��p����뢠���� ��p���]
       FIX BYTE VAR  LR      [���饭�� ��� ��p��� ���㮪��] 0 ! LR
KOLWIN ::  WORD VCTR KOLVO   [�������⢮ ����祭��� 䠩���]
KOLWIN     WORD VCTR UPNUM   [����p ⥪�饣� 䠩��
                             � ��p孥� ��p��� ������]
KOLWIN ::  WORD VCTR NOMER   [����p ⥪�饣� 䠩��]
KOLWIN     LONG VCTR VOLIUME [C㬬�p��� ��ꥬ ��p����� 䠩���]
           BYTE VAR  WW      [���� ���� � ���窠�, �᫮ 䠩��� � �������]
           BYTE VAR  FWW     [�� 1 ����� 祬 �ᥣ� ���祪 � 䠩����� ������]

[���ᨬ��쭮� ������⢮ �p�p㥬�� 䠩���]
NOMVERS 3 = C %IF   400 VALUE MAXFILE %FI
          NOT %IF  1000 VALUE MAXFILE %FI

[ 13 VALUE LENNFIL [������ ����� 䠩��]

  255 STRING PATHNAME [���p ��� ᯥ�䨪�樨 ����� ���]
  255 STRING TEKSPEC  [��ப� ᯥ�䨪�樨 ⥪�饣� 䠩��]
  255 STRING STRTMP   [�६����� ��ப�]
:: 0D STRING OBRFILE  [���ᨢ ����� ⥪�饣� 䠩�� ��� �஡����]
[:: 0D BYTE           VCTR OBRFILE [���ᨢ ����� ⥪�饣� 䠩�� ��� �஡����]
:: MAXFILE      WORD VCTR SORT    [���ᨢ �����ᮢ ��� �p�p���� 䠩���]
:: MAXFILE 0D * BYTE VCTR NAME    [���ᨢ ��� ���� 䠩���]
:: MAXFILE      LONG VCTR SIZEF   [���ᨢ ��� p����p�� 䠩���]
:: MAXFILE      WORD VCTR DATEF   [���ᨢ ��� ���� ᮧ����� 䠩���]
:: MAXFILE      WORD VCTR TIMEF   [���ᨢ ��� �p����� ᮧ����� 䠩���]
:: MAXFILE      BYTE VCTR ATRIBUT [���ᨢ ��� ��p���⮢ 䠩���]
[�.�. ����� 䠩� � ᯨ᪥ �������� 21 ���� �����]
:: 0D           STRING    DISKLABEL   [���ᨢ ��� ����� ��⪨]
::              BYTE VAR  PRESLAB     [�ਧ��� �����㦥��� 䠩����� ��⪨]
:: TRAP NOTFILE NOP [����㦤����� �᫨ 㪠�뢠���� ��४���]

[���뢠��� ������ � ���ᨢ�]
: NDIR [] S( FATRS ) 37 ! FATRS "NO-NAME    " ! DISKLABEL
  NGCDIR 0 Panel ! UPNUM 0 Panel ! KOLVO 0 Panel ! VOLIUME
  #  !!! NAME !0 PRESLAB
  0 MAXFILE 1+ DO SORTINI D [#  'PSP 80 + 40 !!!MB]
  PATHNAME E2D 3 = C IF0 INIUP NOT Panel ! NOMER FFF IF0 DIR1 [] ;
  : SORTINI C C ! SORT 1+ ;
  : INIUP [] ".." 0 ' NAME !SB 10 0 ! ATRIBUT 0 0 ! SIZEF 1 Panel ! NOMER [] ;

[�⮡p������ ���� 䠩��� �� ���ᨢ��]
:: : SHOWFILE [] Panel L Panel UPNUM Panel ! L FWW 1+ DO INCSHOW
     Panel ! L [] ;
     : INCSHOW [] RESHOWTEK Panel L 1+ Panel ! L [] ;

[NGCDIR - ��p������� ��p���p�� � ��p����� �� � ���ᨢ�]
: NGCDIR [] GCDIR [A,L] ! PATHNAME [] ;

: DIR1 [] .DIR MAXFILE DO DIR2 [] ;
  : PEREPOLN "�����㦥�� ����� 祬" ! STRTMP ON ?SPlus 0
    MAXFILE FIN SADD STRTMP " 䠩���!!!" SADD STRTMP STRTMP InfErr [] ;
  : DIR2 [] FNF [���] NOT EX0 .DIR [] ;
    : .DIR [��� �p����� �����p���p��
            ��� p���p���� ��p���� ����� ���� ����� ��� �� ������]
      [���砥� ��p�� ᨬ��� ����� 䠩��]
      @S_ALFNAME D @B BR 0 NOP 5 .DIR0PFS 2E NOP 0E5 NOP ELSE .DIR0 [] ;

      : .DIR0PFS [] 0E5 @S_ALFNAME D !TB [] .DIR0 [] ;
        : .DIR0 [] @B_ATR C Panel NOMER ! ATRIBUT
          08 = BR+ ?LABEL .DIR1 [] ;
      : ?LABEL !1 PRESLAB @S_ALFNAME ! DISKLABEL ;

    : .DIR1 [] @S_ALFNAME C2 C2 Panel NOMER ATRIBUT 10 & BR0 TOLITL DD
      #. C3 C3 SRCHB [A,L,L.] C2 C2 = PUSH [A,L,L.]
      C3 C2 Panel NOMER DISTR !SB [A,L,L.] POP BR+ DDD OBREXTR
      [.DIR1r] DIR1SIZE DIR1DATE DIR1TIME [] Panel NOMER 1+ Panel ! NOMER ;
      : OBREXTR 1+ [A,L,L.+1] E3 [L.+1,L,A] C3 + [L.+1,L,A{L.+1}]
        E3 [A{L.+1},L,L.+1] - [AE,LE] Panel NOMER DISTR 9 + !SB ;
    : DIR1SIZE [] @_SIZE Panel NOMER ! SIZEF [] ;
    : DIR1TIME [] @_TIME Panel NOMER ! TIMEF [] ;
    : DIR1DATE [] @_DATE Panel NOMER ! DATEF [] ;

[�⮡p������ ��p���⮢ 䠩���]
: SHATRIB [] [S( BASE@ ) Panel L SORT] ATRIBUT 3F &
  BR 0 EMSTR   1 "r   "  2 "h   "  3 "hr  "  4 "s   "  5 "sr  "  6 "shr "
    10 "SUBD" 20 "a   " 21 "ar  " 22 "ah  " 23 "ahr " 24 "as  " 25 "asr "
    26 "ashr" ELSE "xxxx" ;

[��p�����p���� � �����쪨� �㪢�]
: TOLITL [ADR,DL] DO PERECOD D [] ;
  : PERECOD [ADR] C @B C #A #Z SEG IF+ MASK C2 !TB 1+ [ADR+1] ;
    : MASK 20 &0 ;

[��p�p���� 䠩��� �� �� ������]
100 STRING STRSCMP [C�ப� ��� �ࠢ����� ��ப]
: SORTNAME [] 0 Panel NOMER 1- C [��᫮ ��p����� �p�p����]
  DO SORTIR DD [] ;
  : SORTIR [������ �p���� ��� ���p쪮�] 0 Panel ! I C DO SORTIR1 1-
    [������p������� �p����] [SHOWFILE TRB D] ;
    : SORTIR1 Panel I SORT DISTR 0C ! STRSCMP ".." SSSB STRSCMP 
      E2D IF0 SORTIR2 Panel I 1+ Panel ! I ;
      : SORTIR2 Panel I 1+ SORT DISTR 0C ! STRSCMP
        ".." SSSB STRSCMP E2D BR+ OBMEN SORTIR3 [] ;
        : SORTIR3 Panel I SORT ATRIBUT 10 & Panel I 1+ SORT ATRIBUT 10 & -
          BRS OBMEN SPOSSORT NOP [] ;
          : NAMESORT Panel I SORT DISTR 0C SCMP STRSCMP IF+ OBMEN ;
        : DATESORT Panel I SORT DATEF Panel I 1+ SORT DATEF < IF+ OBMEN ;
        : SIZESORT Panel I SORT SIZEF Panel I 1+ SORT SIZEF < IF+ OBMEN ;
          : OBMEN [] Panel I ' SORT C @L SWW E2 !TL [] ;

FIX ACT VAR SPOSSORT [���ᮡ ���஢�� 䠩���] '' NAMESORT ! SPOSSORT
: MKSNAME [] '' NAMESORT ! SPOSSORT SORTNAME SHOWFILE [] ;
: MKSDATE [] '' DATESORT ! SPOSSORT SORTNAME SHOWFILE [] ;
: MKSSIZE [] '' SIZESORT ! SPOSSORT SORTNAME SHOWFILE [] ;

B10
FIX BYTE VAR CLD   3 ! CLD  [���p���� 梥�]
FIX BYTE VAR CLN  14 ! CLN  [梥� 蠯�� ���� 䠩�� ���p�� ⠡����]
FIX BYTE VAR CLM  14 ! CLM  [梥� ����� 䠩�� ����祭���� �� INS]
FIX BYTE VAR CLFN  1 ! CLFN [⥪�騩 䮭 NORTONA]
FIX BYTE VAR CLTN  7 ! CLTN [梥� ������ ����p��� ����� 䠩��]
FIX BYTE VAR CLFT  0 ! CLFT [梥� 䠩�� � ����� ����p��� ����� 䠩��]
FIX BYTE VAR CLNN 11 ! CLNN [����� 梥� ����� 䠩��]

:: FIX ACT VAR NORTKEYS [�᭮���� ��楤�� �ᯮ������]  '' NOP ! NORTKEYS
:: FIX ACT VAR NORTF9KEY [�᭮���� ��楤�� �ᯮ������] '' NOP ! NORTKEYS

[�������� �p�楤�p�]
:: : NM S( BASE@ ) B10 NCUR [����ᨫ� ��p�p]
     INIVMEM NOBLINCK MENUINIT [���樫����� ��⥬� ����]
     ON MESC EXSV CNM [] ;

::   :  CNM ON ?SPlus #  ON ?SZapit 1 Panel 1 MIN ! Panel
        HS 5 - C ! WW 3 * 1- ! FWW 0 Panel ! L HS ! TimI 64 ! TimJ
        2  ! TimFon 4  ! TimTxt
        [28 IniDisp] EON MF10 ENDNM MouOFF ON ?Shadow 1
        CLFN ! CLFON  [���� 䮭�]
        CLNN ! CLKANT [���� ����]
        SAVESCR [���p����� �p��]
[        IniTim START TIMER         ]
        CHDGR [�뢥�� 蠡���]
        SCANSUBS [�p�᪠��p����� ��p���p��]                40 ! LR

        CHDGR SCANSUBS [�p�᪠��p����� ��p���p��] SPECDOWN  0 ! LR
        CONTROL ENDNM ;
        : ENDNM [STOP TIMER EndDisp MouOFF [DELWIND] LOADWIN [] ;
        DEFINE? SAVESCR NOT %IF
        : SAVESCR 0 0 HS 1+ WS 1+ SAVEWIN [���p����� �p��] ;
                             %FI

       [�����p������ ��p���p��]
  :: : SCANSUBS NDIR 0 1 LR + #� RFKF38L
       0 13 LR + #� RFKFS  0 26 LR + #� RFKFS
       3 0 0 20 PATHNAME C 38 MIN RJUST
       1- E3 C3 2 / D - LR + E3 CLTOS
       SORTNAME SHOWFILE Panel NOMER MAXFILE = IF+ PEREPOLN [] ;

    [��᫠�� ��砫�� ��p�� ��p��� ����� � ����p�� ������� � �⥪�]
    :: : DISTR [i] 12 * ' NAME [ADR] ;

[��ᮢ���� 蠡���� ⠡����]
: CHDGR [] #  CLFN CLNN FORMSYMB
  0 LR HS 1- 40 RBOX [] 0 LR HS 1- 40 RFRAME2 []
  HS 4-    LR   #� RFKFS HS 4-  1 LR + #� RFKF38L HS 4- 39 LR + #� RFKFS
   1 HS 4- DO CHDGR' D
      0 13 LR + #� RFKFS     0 26 LR + #� RFKFS
  HS 4- 13 LR + #� RFKFS HS 4- 26 LR + #� RFKFS ;

: CHDGR' [line] C 13 LR + #� RFKFS  C 26 LR + #� RFKFS  1+ [line] [SHNAME] ;
  : RFKF38L FKFORMS 38 RLINE ;
  : RFKFS FKFORMS RSYMB ;
    : FKFORMS CLFN CLNN FORMSYMB ;

[�⮡p������ ���� �� 蠡����]
: SHNAME 1 5 LR + 3 DO OUTNAME DD ;
  : OUTNAME CLFN CLN C4 C4 "Name" CLTOS 13 + ;

[�⮡p������ ᯥ�䨪��p� �����]
: SPECDOWN Panel KOLVO BR0 NOSELLINE SHOWSEL SHDOWN [] ;
  : SHDOWN [] CLFN CLNN HS 3- 1 LR + SPECFILE D 38 CLTOS [] ;

:: : SPECFILE S( BASE@ ) B10 FNAMNORT 12 LJUST ! TEKSPEC RAZDCELL SADDB TEKSPEC
     Panel L SORT C SHATRIB                 SADD TEKSPEC RAZDCELL SADDB TEKSPEC
     C SIZEF             FIN       11 RJUST SADD TEKSPEC RAZDCELL SADDB TEKSPEC
     C DATEF C 31 &      2 VAL>STR  2 RJUST SADD TEKSPEC #- SADDB TEKSPEC
     C -5 SHT 15 & 100 + 2 VAL>STR          SADD TEKSPEC #- SADDB TEKSPEC
           -9 SHT 1980 + 2 VAL>STR          SADD TEKSPEC RAZDCELL SADDB TEKSPEC
         TIMEF C -11 SHT 2 VAL>STR  2 RJUST SADD TEKSPEC #: SADDB TEKSPEC
             -5 SHT 63 & 2 VAL>STR          SADD TEKSPEC TEKSPEC [Adr,Dl] ;

DEFINE? STRING C %IF
37 STRING SELSPEC
: SHOWSEL NOSELLINE
  CLFN CLM WW 1+ 20 LR + Panel KOLVO FIN ! SELSPEC " files " SADD SELSPEC
  Panel VOLIUME FIN SADD SELSPEC SELSPEC
  [FON,TXT,I,J,A,L] E3 C3 SHR - E3 CLTOS ;
  : NOSELLINE HS 4-  1 LR + #� RFKF38L
    HS 4- 13 LR + #� RFKFS  HS 4- 26 LR + #� RFKFS ;
%FI NOT %IF
  : SHOWSEL CLFN CLM WW 1+ 1 LR +
    "��࠭� -      䠩��� ��ꥬ��         "
    Panel KOLVO   4 VAL>STR C4 10 + !SB
    Panel VOLIUME 8 VAL>STR C4 30 + !SB CLTOS ;
    %FI

:: FIX ACT VAR FUNCTION
'' NOP ! FUNCTION

17408 VALUE KEYF10
[����p��� �������� ������]
:: : CONTROL [] RP OPROS RESHOWTEK [] ;
     : OPROS SHOWTEK SPECDOWN TRBMou
       BR [09] [3849  EX       [��室 �� Tab]
          [12] 20736 UPAGEDN   [Page Down]
          [14] 18688 UPAGEUP   [Page Up]
          [13] 7181  IZMDIR    [����� ��p���p��]
          [19] 20992 SELFILE   [INS]
          [20] 21248 DLFILES   [�������� �� del]
          [25] 19712 URIGHT    [��p���]
          [26] 19200 ULEFT     [�����]
          [27] 283   MESC      [��室]
          [28] 18432 UUP       [���p�]
          [29] 20480 UDOWN     [����]
          [32] 14624 SELFILE   [ins - �� �஡���]
          [43] 20011 SELALLF   [+]
          [45] 18989 UNSELALLF [-]
               24576 MKSNAME   [CtrlF3- �� ������]
              [24832 by type]
               25088 MKSDATE   [CtrlF5- by time]
               25344 MKSSIZE   [CtrlF6- by size]
               15872 EDITFILE  [F4-।���஢����]
               15616 TYPEFILE  [F3-��ᬮ��]
               15872 EDITFILE  [F4-।���஢����]
               15616 TYPEFILE  [F3-��ᬮ��]
          [#A] [ 7745  DUMP  ]
          [#D] [8260  DLFILES   [��������]
               15104 NMHELP    [F1-������]
               26368 EX
               3592  UPSUB?    [BackSpace]
               18176 TOHOME    [Home]
               20224 TOFPEND   [End]
               17152 NORTF9KEY [�맮� ���� ����p���� �� F9]
              KEYF10 NORTKEYS  [�맮� ����]
    ELSE NOP [] ;
    : TOHOME 0 Panel ! UPNUM 0 Panel ! L SHOWFILE ;
    : TOFPEND Panel NOMER C 1- Panel ! L FWW 1+ - 0 MAX Panel ! UPNUM
      SHOWFILE [] ;

: DLFILES [] Panel L SORT ATRIBUT 16 & IF0 YESNDEL [] ;
  : YESNDEL [] PATHNAME 1- CDIR FNAMNORT DELETE [] SCANSUBS [] ;

: TYPEFILE  [] Panel L SORT ATRIBUT 16 & IF0 YESTYPE [] ;
  : YESTYPE [] SAVESCR VIEWSPEC TEKNAME EXEFLCL DDDD LOADWIN [] ;
: EDITFILE  [] Panel L SORT ATRIBUT 16 & IF0 YESEDIT [] ;
  : YESEDIT [] SAVESCR '' FNAMNORT ! NAMENRT NFE NCUR LOADWIN ;
  : FNAMNORT TEKNAME 1- E2 1+ E2 ;

[�뤠� ����� ⥪�饣� 䠩�� � ���� ⥪�⮢�� ��ப�]
:: : TEKNAME [] Panel L SORT ATRIBUT 16 & IF+ NOTFILE
     " " ! OBRFILE Panel L SORT DISTR C 8
     [Adr,Dl] C DO UBRSP [Adr,Dl] SADD OBRFILE
     9 + 3 [Adr,Dl] C DO UBRSP [Adr,Dl]
     C IF+ DOBAV. [Adr,Dl] SADD OBRFILE OBRFILE [Adr,Dl] ;
     : UBRSP [Adr,Dl] C2 C2 1- + @B #  = BR+ 1- EX [Adr,Dl] ;
     : DOBAV. #. SADDB OBRFILE ;

[��������� ��p���p��]
: IZMDIR [] Panel L BR0 UPSUB? UDWNSUB [] ;
  : UPSUB? PATHNAME E2D 3 = BR0 UPSUB UDWNSUB ;
  :: : UPSUB ".." CDIR 0 Panel ! L SCANSUBS [] ;

  [���� �� ��p���]
  : UDWNSUB [] Panel L SORT ATRIBUT 16 & IF+ DOWNSUB [] ;
    : DOWNSUB [] TEKNAME 1- E2 1+ E2 SADD PATHNAME
      PATHNAME CDIR 0 Panel ! L SCANSUBS [] ;

[���⠭������� ⥪�饣� �����]
:: : RESHOWTEK CLFN Panel L SORT ATRIBUT 128 & BR+ CLM CLNN SHOWLSTR [] ;

[�뤥����� ⥪�饣� �����]
: SHOWTEK CLTN Panel L SORT ATRIBUT 128 & BR+ CLM CLFT SHOWLSTR [] ;
  : SHOWLSTR Panel L Panel UPNUM - WW / 1+ E2 [Nstr,Nstlb]
    13 * 1+ LR + Panel L SORT DISTR 12 CLTOS [] ;

[��p���⪠ ������ ��p���]
: URIGHT [] Panel L Panel NOMER 1- < IF+ RIGHT [] ;
  : RIGHT [] Panel UPNUM FWW + C Panel L WW + <
    E2 Panel NOMER < & BR+ ROLRSTEP NRSTEP [] ;
    [��諨 �� �।��� ᯨ᪠ �� �࠭�, �㦥� �஫���]
    : ROLRSTEP Panel UPNUM WW + Panel NOMER FWW 1+ - MIN Panel ! UPNUM
      NRSTEP SHOWFILE [] ;
      : NRSTEP RESHOWTEK Panel L WW + Panel NOMER 1- MIN Panel ! L [] ;

[��p���⪠ ������ �����]
: ULEFT [] Panel L WW - 0 < BR0 LEFT UPAGEUP [] ;
  : LEFT [] Panel L Panel UPNUM - WW - BR- ROLLSTEP NLSTEP [] ;
    [��諨 �� �।��� ᯨ᪠ �� �࠭�, �㦥� �஫���]
    : ROLLSTEP [] Panel UPNUM WW - 0 MAX Panel ! UPNUM NLSTEP SHOWFILE [] ;
      : NLSTEP RESHOWTEK WW Panel L - Panel ! L [] ;

[��p���⪠ ������ ���p�]
: UUP Panel L IF+ UPLAB [] ;
  : UPLAB Panel L Panel UPNUM - BR0 ROLUSTEP NUSTEP [] ;
    : ROLUSTEP Panel UPNUM 1- Panel ! UPNUM NUSTEP SHOWFILE [] ;
      : NUSTEP [] RESHOWTEK Panel L 1- Panel ! L [] ;

[��p���⪠ ������ ����]
: UDOWN Panel L 1+ Panel NOMER = IF0 DOWNLAB [] ;
  : DOWNLAB Panel L Panel UPNUM - FWW - BR- NDSTEP ROLDSTEP [] ;
    : ROLDSTEP [1000 100 BEEP] Panel UPNUM 1+ Panel ! UPNUM
      NDSTEP SHOWFILE [] ;
      : NDSTEP [] RESHOWTEK Panel L 1+ Panel ! L [] ;

[��p���⪠ ������ ��p���� ����]
: UPAGEDN RESHOWTEK Panel UPNUM FWW + Panel L = BR+ PAGEDN? PAGEDN [] ;
  : PAGEDN? [] Panel UPNUM WW 3 * 1- + C Panel NOMER > BR0 CORPAGEDN D ;
    : CORPAGEDN Panel ! UPNUM PAGEDN SHOWFILE [] ;
      : PAGEDN [] Panel UPNUM FWW + Panel NOMER 1- MIN Panel ! L [] ;

[��p���⪠ ������ ��p���� ���p�]
: UPAGEUP Panel UPNUM Panel L = BR+ POSUP? PAGEUP [] ;
  : POSUP? Panel L IF+ DECUPNUM [] ;
    : DECUPNUM Panel UPNUM FWW - 0 MAX Panel ! UPNUM PAGEUP [] ;
      : PAGEUP [] RESHOWTEK Panel UPNUM Panel ! L SHOWFILE [] ;

[�롮p 䠩�� �� ������ SELECT]
: SELALLF [] UNSELALLF 0 Panel NOMER 1+ DO CSELF D SHOWFILE [] ;
: SELFILE [] Panel L CSELF D UDOWN [] ;
  : CSELF [L] C SORT C ATRIBUT C 16 & IF0 MARKSEL [L,ATR]
    E2 ! ATRIBUT 1+ [L] ;
      : MARKSEL [ATR] C 128 & BR0 ADDFILE ADLFILE 128 '+' [ATR] ;
        : ADDFILE Panel KOLVO 1+ Panel !  KOLVO C2 SIZEF 
          Panel VOLIUME + Panel ! VOLIUME ;
        : ADLFILE [L,SORT,ATRIB] Panel KOLVO 1- Panel ! KOLVO
          Panel VOLIUME C3 SIZEF - Panel ! VOLIUME [L,SORT,ATRIB] ;

[�⬥�� �롮p� ��� 䠩���]
: UNSELALLF [] 0 Panel NOMER 1+ DO UNSELF D 0 Panel ! KOLVO
  0 Panel ! VOLIUME SHOWFILE [] ;
:: : UNSELFILE Panel L UNSELF 1- DLFILE D UDOWN [] ;
:: : UNSELF [L] C SORT C ATRIBUT 127 & E2 ! ATRIBUT [L] 1+ [L] ;
     : DLFILE [L] Panel KOLVO 1- Panel ! KOLVO
       Panel VOLIUME C2 SORT SIZEF - Panel ! VOLIUME ;

B16
:: : FREEVOL 36 SWB ! RAX !0 RDX 21 INTERR RAX C MAXC =
     BR+ T0 RASKR [0/VOL] ;
     : RASKR [RAX] RBX * RCX * [Nb] ;
       : MAXC 0FFFF ;

DEFINE? ZVOOK NOT %IF
    :: : ZVOOK 2000 10 3456 10 7450 10 500 10 4 DO BEEP ;
                  %FI
B10

: SHRUM SouSwi IF+ YESSHRUM [] ;
: YESSHRUM 30000 10 BEEP 25000 10 BEEP 20000 10 BEEP
    18000 10 BEEP 16000 10 BEEP 14000 10 BEEP ;

[CLEAR $NDIR]
:: COPYW EndDisp END

: NMHELP ON ?SPALL 12 '' TNMHELP 16 WS 2+ SHR 18 C2 3- GENINF TRB D DELWIND ;
  : TNMHELP BR 1 "������� ������ LNM-DSSP"
               2 ",Home,End,PgUP/Down"
               3 "��६�饭�� �� 䠩���"
               4 "�஡��,Insert-�⬥���"
               5 "Delete-㤠���� 䠩�"
               6 "F3-��ᬮ���� 䠩�"
               7 "F4-।���஢��� 䠩�"
               8 "+ - �⬥��� �� 䠩��"
               9 "- - ࠧ�⬥��� �� 䠩��"
              10 "BackSp - � ���孨� ��⠫��"
              11 "CntrlF3-���஢��� �� �����"
[             12 "CntrlF4-���஢��� �� ࠧ����" ]
              13 "CntrlF5-���஢��� �� ���"
              14 "CntrlF6-���஢��� �� ࠧ����"
              15 "F10 - �맢��� ���� ������"
              ELSE EMSTR [Adr,Dl] ;
UNDEF
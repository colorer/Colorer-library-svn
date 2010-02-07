PROGRAM $IBM-DVK USE SYSTEM B16 ."
����� � ��᪠�� DEC-��設 �� ����� �裡 RS-232 - ���" [."
�ᯮ������� ��������� �ணࠬ��: RS232.DSP SCRWORK1.DSP MENU4.DSP
 �맮�: PDP
�� ����� ����� IBM-䠩�� ����᪠���� �������� ᨬ���� * � ?.
�� ����� ����� PDP-䠩�� ���뫪� ���⮩ ��ப� ����砥� �뤠�� �� �࠭
��४��� ��᪠ � ����������� �롮� �� ���� �㦭��� �����"]

     LONG VAR  AIBUF  [���� ���� �����]
     LONG VAR  AOBUF  [���� ���� �뢮��]
     BYTE VAR  COMIN  [����祭��� �������]
     BYTE VAR  COMOUT [������� ��� ��।��]
          VAR  DLIBL  [����� ���������� �����]
          VAR  DLOBL  [����� ����� ��� �뢮��]
     BYTE VAR  YBL    [�ਧ���: 1 - ���� ����祭; 0 - ���]
          VAR  KSUM   [����஫쭠� �㬬�]
100      VALUE LBUF   [����� ���஢ ����� � �뢮��]
LBUF BYTE VCTR IBUF   [���� �����]
LBUF BYTE VCTR OBUF   [���� �뢮��]
100  BYTE VCTR DNAM   [����� ��������� ��᪮�����]
     BYTE  VAR NDISC  [������⢮ ��������� ��᪮�����]

FIX 40 STRING IBFNAME "*.DSP" ! IBFNAME
EMPTY 0FF00 BYTE VCTR [SegDATA] PFILE [䠩� � PDP]
LONG        VAR LFIL  [����� 䠩��]

[���������� � ��ࠢ����� � �⥢� �ணࠬ���]
COPYW LOB LOB0
: LOB [b] C LOB0 KSUM + ! KSUM [] ;
: LIW [] LIB LIB SWB + [w] ;
: LOW [w] C LOB SWB LOB [] ;
: LIS [a,l] DO LIS1 D [] ;
  : LIS1 [a] LIB C2 !TB 1+ [a+1] ;
: LOS [a,l] DO LOS1 D [] ;
  : LOS1 [a] C @B LOB 1+ [a+1] ;

B10
FIX WORD VAR MM.I    7 ! MM.I
FIX WORD VAR MM.J   30 ! MM.J
FIX WORD VAR MM.LI   9 ! MM.LI
FIX WORD VAR MM.LJT 42 ! MM.LJT
FIX WORD VAR MM.LJV  0 ! MM.LJV

B16
:: : PDP [] [LINIT 0 LOB] PDP1 DELWIND [] ;
   : PDP1 [] "���� PDP (� ��) <--> ���� IBM" ON ?SPALL 3 EON MESC NOP
     '' PV1 '' TV1 '' NOP MM.I MM.J MM.LI MM.LJT MM.LJV GLBMENU [] ;
   : PV1 [i] BR 1 PDPDIR   2 CN.D-I    3 CT.D-I    4 CN.I-D    5 CT.I-D
     6 NEWDISK  7 SHDVKFIL  8 SHIBMFIL  9 SETPORT  ELSE NOP [] ;
   : TV1 [i] BR 1 "��⠫�� PDP"
                2 "����஢���� ����筮��  䠩�� � PDP �� IBM"
                3 "����஢���� ⥪�⮢��� 䠩�� � PDP �� IBM"
                4 "����஢���� ����筮��  䠩�� � IBM �� PDP"
                5 "����஢���� ⥪�⮢��� 䠩�� � IBM �� PDP"
                6 "�롮� ��᪮���� �� PDP-��設�"
                7 "����� ⥪�⮢��� 䠩�� �� ���"
                8 "����� ⥪�⮢��� 䠩�� �� IBM"
                9 "�롮� ���� COMi"
           ELSE EMSTR [A,L] ;
: SETPORT [] ;

[�ଠ� �����, ��।�������� �� ����� �裡:
 <������� (1�)> <����� (2�)> <���ଠ�� (N�)> <�����.�㬬� (1�)> ]

[���� ����� � ����� � ������ � ���� AIBUF]
: INBLOCK [] 0 ! YBL 4 LOB LIB BR 1 INBL0 7 LERR ELSE RESTART [] ;
  : INBL0 [] LIB ! COMIN  LIW C ! DLIBL AIBUF E2 LIS LIB [KS]
    DLIBL 0FF & DLIBL SWB 0FF & + COMIN + AIBUF DLIBL KS 0FF &
    [KS,KS'] = BR+ INBL1 NOK! [] ;
 : INBL1 [] OK! 1 ! YBL [] ;
 : OK! [] 1 LOB [] ;
 : NOK! [] 2 LOB [] ;
 : KS [ks0,a,l] DO KS1 D [KS] ;
   : KS1 [ks,a] C @B C3 + E3 D 1+ [ks',a+1] ;

[��।�� ����� (AOBUF,DLOBL) � �����]
: OUTBLOCK [] RP OBL1 [] ;
  : OBL1 [] LIB  BR 1 EX  2 OBL2  4 OBL2  7 LERR ELSE RESTART [] ;
  : OBL2 [] OK! 0 ! KSUM COMOUT LOB DLOBL LOW AOBUF DLOBL LOS KSUM LOB [] ;

  : ERRR 4 ! CLFON 0F ! CLKANT 0F ! CLTXT '' ERRRT
    [''P] MM.I MM.LI + 4+ [=I]
    MM.J 3-               [=J]
    5                     [=LI]
    2+ MM.LJT MM.LJV + 3+ [=LJ] GENINF [] 5 DO BELL TRB D DELWIND ;
    : ERRRT BR 4 "�訡�� ��᪠! ������ ���� �������." ELSE EMSTR ;

 : WAITI '' WAITIT WINFORM [] ;
 : WAITIT BR 4 "���� ����� ������ ���ଠ樥�" ELSE EMSTR ;

 : WAITS '' WAITST WINFORM [] ;
 : WAITST BR 4 "����� ���뢠���/��࠭���� 䠩��" ELSE EMSTR ;

 : WINFORM [''P] ON ?SPALL 3
   MM.I 2- MM.J 3- MM.LI 2+ MM.LJT MM.LJV + 3+ GENINF [] ;

[�ਥ� ��ப� � ����� � ������ �� �� ���]
: STOD [] EON DERR ABEND @WIBUF [L] 0 ' IBUF ! AIBUF RP STOD1 D [] ;
  : STOD1 [L] INBLOCK YBL IF+ STOD2 [L'] ;
  : STOD2 [L] AIBUF DLIBL OS CH  DLIBL - [L-DLIBL] C EX0 [L'] ;
  : ABEND [] 7 LOB [] ;
  : @WIBUF 0 IBUF 1 IBUF SWB + [w] ;

[�⥭�� ��ப� � ��᪠ � ��।�� �� � �����;
 � ���� ����� ����� ��ப� (2�)]
: STOL [] EON DERR ABEND1  0 ' OBUF ! AOBUF
  @WIBUF [L] C LBUF / IF+ 1+ [L,NBL] DO STOL1 D [] ;
  : STOL1 [L] C LBUF MIN ! DLOBL AOBUF DLOBL IS CH OUTBLOCK
    [L] DLOBL - [L'] ;
  : ABEND1 [] LIB BR  4 ABEND ELSE NOP [] ;

[�롮� ��᪮����]
: NEWDISK [] 4 ! COMOUT [��� ᯨ᮪ ��᪮�] 0 ! DLOBL OUTBLOCK
  0 ' IBUF ! AIBUF  INBLOCK  AIBUF DLIBL  0 ' DNAM !SB
  DLIBL -3 SHT ! NDISC NE1 [] ;
  [���� �롮� ��᪮����]
  : NE1 [] "��᪮����"
    '' PV2 '' TV2 '' NOP 5 0A NDISC 10 MIN 9 0 NDISC GLBMEN2 [] ;
  : TV2 [i] 1- 3 SHT ' DNAM 8 [a,8] ;
  : PV2 [i] 1- 3 SHT ' DNAM ! AOBUF 8 ! DLOBL 1 ! COMOUT OUTBLOCK
    LIB 1 = IF0 LERR DELWIND [] ;

  [��⠭���� �� 䠩� A,L]
  : SFIL [A,L] !SOBUF 41 ! COMOUT 0 ' OBUF ! AOBUF
    OUTBLOCK LIB 1 = IF0 LERR [] ;

    : !SOBUF [A,L] E2 C2 0 ' OBUF !SB  ! DLOBL [] ;
    : !WOBUF [w] C 0 ! OBUF SWB 1 ! OBUF [] 2 ! DLOBL [] ;

[����� ����� 䠩��]
: GDLFIL [] 44 WCALLFS [L] ;
[������ 䠩�]
: OPFIL  []  42 CALLFS [] ;
[������ 䠩�]
: WOPFIL []  4A CALLFS [] ;
[������� 䠩�]
: CLFIL  []  43 CALLFS [] ;
[��१��� 䠩�]
: CUTFIL []  4B CALLFS [] ;

[�������� � ������� 䠩����� ��⥬�]
: CALLFS [kop] ! COMOUT [] 0 ! DLOBL OUTBLOCK LIB 1 = IF0 LERR [] ;

[�������� � ������� 䠩����� ��⥬� � ������� ���� ᫮��]
: WCALLFS [kop] CALLFS 0 ' IBUF ! AIBUF INBLOCK @WIBUF [W] ;

[������� ��ப� � ��᪠ ������ l � ������� �� �� ����� a]
: GSTR [a,l] C !WOBUF 47 ! COMOUT 0 ' OBUF ! AOBUF
  OUTBLOCK [a,l] LBUF / IF+ 1+ DO GST1 D [] ;
  : GST1 [a] C ! AIBUF INBLOCK DLIBL + [a+DLIBL] ;

[����� 䠩�� �� �࠭�]
USE $SCRWORK

[����� DVK - 䠩��]
: SHDVKFIL [] EON LERR SFERRR EON ESCR WSCOFF
  VVNOMF [a,l] C BR+ SHDFL1 DD [] ;
: SHDFL1 [a,l] CT10 [] 0 0 19 50 SAVEWIN CLS SCON EON ESCR WSCOFF
  0 ' PFILE LFIL C2 C2 KOI-ALT TOS SCOFF [] CONT TRB D LOADWIN [] ;

: SFERRR WSCOFF ERRR ;
: WSCOFF SCOFF LOADWIN ;

[����� IBM-䠩��]
: SHIBMFIL [] EON DERR SFERRR VVNAMF [a,l] C BR+ SHIFL1 DD [] ;
: SHIFL1 [a,l] SAVESCR CLS 0 ' COMBUF [A,L,AT] "TYPE " C3 !SB [A,L,AT]
  C3 C3 C3 5 + !SB E3 D 5 + TEXEC [] CONT TRB D LOADWIN [] ;

MLPATHNAME 5 + BYTE VCTR COMBUF [���� ��� �ନ஢���� �������]
: CONT CR ."               *$ ������ ���� ������� ��� �த������� $*" ;

[��४���஢�� ���-8 -> ��������������]
: KOI-ALT [a,l] DO KO1 D [] ;
  : KO1 [a] C @B  0C0 0FF SEG IF+ KO2  1+ [a+1] ;
    : KO2 [a] C @B 0C0 - TALT [a,c] C2 !TB [a] ;

[��४���஢�� �������������� -> ���-8]
: ALT-KOI [a,l] DO ALT1 D [] ;
  : ALT1 [a] C @B [a,k] C 80 0AF SEG BR+ ALT2 ALT3 1+ [a+1] ;
    : ALT2 [a,k] 80 - TKOI1 C2 !TB [a] ;
    : ALT3 [a,k] C 0E0 0EF SEG BR+ ALT4 D [a] ;
      : ALT4 [a,k] 0E0 - TKOI2 C2 !TB [a] ;

B10
BYTE CNST TALT
238 160 161 230 164 165 228 163 229 168 169 170 171 172 173 174
175 239 224 225 226 227 166 162 236 235 167 232 237 233 231 234
158 128 129 150 132 133 148 131 149 136 137 138 139 140 141 142
143 159 144 145 146 147 134 130 156 155 135 152 157 153 151 219 ;
BYTE CNST TKOI1
225 226 247 231 228 229 246 250 233 234 235 236 237 238 239 240
242 243 244 245 230 232 227 254 251 253 255 249 248 252 224 241
193 194 215 199 196 197 214 218 201 202 203 204 205 206 207 208 ;
BYTE CNST TKOI2
210 211 212 213 198 200 195 222 219 221 233 217 216 220 192 209 ;

B16
[����஢���� ����筮�� 䠩�� � PDP �� IBM]
: CN.D-I [] EON LERR ERRR  EON MESC NOP  VVNOMF [a,l] C BR+ CN1 DD [] ;
  : CN1 [a,l] WAITI SFIL OPFIL GDLFIL [L] ! LFIL 0 ' PFILE LFIL GSTR CLFIL
    DELWIND VVNAMF [a,l] C BR+ CN2 DD [] ;
    : CN2 [a,l] WAITS CONNECT CH  WOPEN CH
      0 ' PFILE LFIL OS CH CLOSE CH DELWIND [] ;

[���� ����� 䠩�� IBM]
: VVNAMF [] ON ?SPALL 24 '' TV4  8 10 1 20 GENVVOD IBFNAME
  IBFNAME C IF0 DD*.* [A,L] SEL [A,L] ;
  : TV4 [i] D "������ ��� 䠩�� IBM" [A,L] ;
  : DD*.* DD "*.DSP" ;

 [���� ����� 䠩�� PDP]
 : VVNOMF [] ON ?SPALL 25 '' TV3  8 10 1 20 GENVVOD PBFNAME
   PBFNAME [a,l] C IF0 DDPDIR0 ;
   : TV3 [i] D "������ ��� 䠩�� DVK" [A,L] ;
   : DDPDIR0 DD PDPDIR0 ;

FIX 40 STRING PBFNAME

[����஢���� ⥪�⮢��� 䠩�� � PDP �� IBM]
: CT.D-I [] EON LERR ERRR EON MESC NOP
  VVNOMF [a,l] C BR+ CT1 DD [] ;
  : CT1 [a,l] CT10 VVNAMF [a,l] C BR+ CT2 DD [] ;
[CT10 - ���� ⥪�⮢��� 䠩�� � ����]
: CT10 [a,l] WAITI
  SFIL  OPFIL GDLFIL [L] ! LFIL 0 ' PFILE LFIL GSTR CLFIL DELWIND [] ;
  : CT2 [a,l] WAITS CONNECT CH  WOPEN CH
    0 ' PFILE LFIL KOI-ALT 0 ' PFILE LFIL DO CT3 D CLOSE CH DELWIND [] ;
  : CT3 [a] C @B C 0A = IF+ CT4 OB CH 1+ [a+1] ;
  : CT4 [] 0D OB CH [] ;

[����஢���� ⥪�⮢��� 䠩�� � IBM �� DEC]
: CT.I-D [] EON LERR ERRR EON MESC NOP
  VVNAMF [a,l] C BR+ CNN2 DD [] ;
  : CNN2 [a,l] WAITS CONNECT CH  OPEN CH 0 ' PFILE  LENB CH ! LFIL
    LFIL DO CNN3 D CLOSE CH 0 ' PFILE LFIL ALT-KOI DELWIND
    VVNOMF [a,l] C BR+ CNN1 DD [] ;
    : CNN1 [A,L] WAITI SFIL WOPFIL  0 ' PFILE LFIL SSTR
      CLFIL CUTFIL DELWIND [] ;
 : CNN3 [a] IB CH C BR 0D CNN4 1A CNN4 9 CNN6 ELSE CNN5 [a'] ;
 : CNN4 [a,0D] D LFIL 1- ! LFIL [a] ;
 : CNN5 [a,k] C2 !TB 1+ [a+1] ;
 : CNN6 [a,9] D #   CNN5 [a+1] ;

[��।�� ��ப� � ����� �����筮 ��� ����� �� ���]
: SSTR [A,L] C LBUF / IF+ 1+ [A,L,NBL] DO SSTR1 DD [] ;
  : SSTR1 [A,L] C LBUF MIN ! DLOBL C2 ! AOBUF [A,L]
    48 ! COMOUT OUTBLOCK  LIB 1 = IF0 LERR
    [A,L] DLOBL - [A,L'] E2 DLOBL + E2 [A',L'] ;

[����஢���� ����筮�� 䠩�� � IBM �� DEC]
: CN.I-D [] S( XORD YORD ) EON MESC NOP EON LERR ERRR
  ON ?SPALL 2 2 ! YORD 2 ! XORD "*.*" SEL [a,l] C BR+ CTN2 DD [] ;
  : CTN2 [a,l] WAITS CONNECT CH  OPEN CH
    0 ' PFILE LENB CH ! LFIL LFIL IS CH CLOSE CH DELWIND
    VVNOMF [f] C BR+ CTN1 D [] ;
    : CTN1 [A,L] WAITI SFIL WOPFIL 0 ' PFILE LFIL SSTR
      CLFIL CUTFIL DELWIND [] ;

:: TRAP LERR NOP

10 %IF
[*********   PDP DIR ***********]
B10
: PDPDIR PDPDIR0 DD [] ;

[PDPDIR0 - ����祭�� ᯨ᪠ ���� 䠩��� �� ���]
: PDPDIR0 [] EON NOF NOP 0 CALLFS !0 QFILES MFS0 MFS1 MFS2 MFS3 [A,L] ;

[**********************************************]

[MFS0 - ���� ��ࢮ�� �宦�����, �᫨ ��� - NOF]
: MFS0 [] 0 ' IBUF ! AIBUF INBLOCK DLIBL IF0 NOF [] ;

[MFS1 - ��ᬮ�� ��४���, ���������� ������]
: MFS1 [] MQFILES DO MFS11 [] ;

[MFS2 - ���஢�� ���� 䠩��� ��⮤�� ����쪠]
: MFS2 [] QFILES C 1- DO MFS20 D [] ;

[MFS3 - �뤠� ���� ]
: MFS3 [] " �롥�� 䠩�" '' MFS3P '' MFS3T 0 14 27 10 L1PFILE 10 + 0 QFILES
  GLBMEN2 [] NOMFILE AFNAME [Aname] SP C2 L1PFILE SRCHB [Aname,Lname] ;

[**********************************************]
B10
[MFS11 - ���������� ���ᨢ� � 䠩���묨 �������]
: MFS11 [] [���������� DTA]  QFILES L1PFILE * [index]
  !1+ QFILES [] QFILES ! FLSPTR [] MFS2NAME [] SRCHNXT [] ;

[MFS2NAME - ����뫪� �����]
: MFS2NAME [] SP QFILES AFNAME L1PFILE !!!MB []
  AIBUF [���� �����] DLIBL [A,L] QFILES AFNAME !SB [] ;

B16
[SRCHNXT - ���� ᫥���饣� �室� � ���������� DTA, ��室 �᫨ ���]
: SRCHNXT [] 0 ' IBUF ! AIBUF INBLOCK DLIBL EX0 [] ;

[**********************************************]
B10
[MFS20 - ��릭�� ⮭�� (��᫥���� �� ��䠢��� ���) �� ��㡨�� fdeep]
: MFS20 [fdeep] 1 C2 1- DO MFS21 D 1- [fdeep-1] ;

[MFS21 - 㯮�冷祭�� ���� ���� � ��㡨�� cdeep � cdeep-1]
: MFS21 [cdeep] 1+ [cdeep+1] CMP-1 IF0 XCHG-1 [cdeep+1] ;

[CMP-1 - �ࠢ����� i-�� � i-1-�� �����, 1-㯮�冷祭�, 0-����]
: CMP-1 [i] 1 C2 AFNAME C3 1- AFNAME [i,1,Ai,Ai-1]
  L1PFILE DO CMP12 DD [i,1/0] ;
: CMP12 [1,Ai,Ai-1] C @B #  = EX+
  C2 @B C2 @B - BRS CMP13 NOP EX E2 1+ E2 1+ [,,] ;
: CMP13 T0 E3 [0,*,*] EX ;

[XCHG-1 - ����� i-�� � i-1-�� �����]
: XCHG-1 [i] C FLSPTR C2 1- FLSPTR C3 ! FLSPTR C2 1- ! FLSPTR [i] ;

[**********************************************]
B10
[MFS3P - ��࠭� i-� ��� 䠩��]
: MFS3P [i] ! NOMFILE DELWIND [] ;

[ WORD VAR NOMFILE ]
: MFS3T [i] ! NOMFILE [] "                    " #  C3 C3 !!!MB [A,L]
  NOMFILE AFNAME L1PFILE [A,L,Aname,Lname] C4 4+ !SB [A,L] ;

[**********************************************]
[AFNAME - ����祭�� ���� ����� 䠩�� � FLSNAME ��� 䠩�� i]
: AFNAME [i] FLSPTR ' FLSNAME [A] ;
[**********************************************]

B10 [������]
[MQFILES - ���ᨬ��쭮 �����⨬�� �᫮ 䠩���]
200 VALUE MQFILES

[QFILES - ⥪�饥 �᫮ ��ࠡ��뢠���� ��� ��������� ���� 䠩���]
WORD VAR QFILES

[L1PFILE - �᫮ ���⮢, �⢮����� ��� ���� 䠩�]
[7 - ����� �����]
7 VALUE L1PFILE [⮫쪮 ���] ;

[FLSNAME - �����, ��� �࠭���� ����� 䠩���, �ᥣ� QFILES < MQFILES,
           �� ����� 䠩� �⢮����� L1PFILE (�� 0)]
MQFILES L1PFILE * BYTE 1- VCTR FLSNAME

[FLSPTR - �����, ��� �࠭���� 㪠��⥫� �� FLSNAME -
          �ᯮ����� � �� ���஢�� � �� ����㯥 (1 .. QFILES)]
WORD MQFILES VCTR FLSPTR
%FI

UNDEF

PROGRAM $FKODER B10 ."
NORTON ����p��騪.                 ��p�� 1.3.  2 ���     1995
�����஢�� ��� ����� 3.3 � ���� ���ᨨ 5.0."

:: FIX BYTE VAR LENS     [����� ��p��� �� 2 ��p�����]
:: FIX WORD VAR LPAGE    [����p ��p����� ���⠥��� ᫥��]        1 ! LPAGE
:: FIX BYTE VAR NSTROK   [��᫮ ��p�� � ����]                   67 ! NSTROK
:: FIX BYTE VAR RAZDSYM  [��� ᨬ���� p������⥫� ��p�����]      12 ! RAZDSYM
:: FIX BYTE VAR PAGESIZE [���ᨬ��쭠� ����� ��p����� � ��p����] 72 ! PAGESIZE
:: FIX WORD VAR TERMPOS                                          80 ! TERMPOS
   [������ � ���ன ��稭����� ���� �������� ��ப� ����]

:: FIX BYTE VAR TIPNUM   [��� �ᯮ��㥬�� �㬥p�樨]
:: FIX BYTE VAR KOD?     [�p����� ��p�����p������ ��室���� 䠩��]
:: FIX BYTE VAR KOLMASK  [�������⢮ ���஥���� ��᮪] 10 ! KOLMASK
       BYTE VAR FLAGERR
       BYTE VAR KONEC    [�p����� ���� ��p���⪨ 䠩��]
   FIX LONG VAR BLOKSIZE [�����p ����砥��� �p� ������� 䠩���]
   FIX BYTE VAR RAZRPRE  [���� p��p�襭�� �p���p�������� �����]
   FIX BYTE VAR NUMMASK  [����p �ᯮ��㥬�� ��᪨ �p���p��������] 1 ! NUMMASK
       LONG VAR STRPOS   [��砫�� ���� ��४���஢�� 䠩��]
       LONG VAR BEGWORD  [���� ��砫� ᫮��]
       LONG VAR SIZEOBR  [�����p ��p����뢠����� 䠩��]
       WORD VAR VOLBUFF  [��쥬 ���p� ��p�����p��騪�]
       WORD VAR MAXBUFF  [���ᨬ���� ��쥬 ���p� ��p�����p��騪�]
       WORD VAR REGKLAV  [������p ᨬ����: 0-��p孨� 1-����訩]
       WORD VAR NOMFILE  [����p ����� ᮧ��������� 䠩��]

         4 BYTE VCTR FRAS   [���ᨢ ��� ����祭�� p���p���� ����� 䠩���]
:: FIX 255 BYTE VCTR IBMDVK [���ᨢ ����� ᮮ⢥������� ᨬ����� ���]
   FIX ACT VAR  SYMOBR [����塞�� �p�楤�p� �p���p��������]
   CHANNEL IF  [����� �����]
   CHANNEL OF  [����� �뢮��]
13 FIX STRING FMSKNAME [��� 䠩�� ��� ��࠭���� ��᪨] "MASK1" ! FMSKNAME

[������� ��楤��]
:: : NM !0 L BEGNM MouON ggg MouOFF ENDNM ;

: KEYSELECT INKEY BR 3849 CONTROL [] [ 11776 MODMEN [] ELSE NOP ;
: ggg '' KODFILE ! SYMOBR 1 ! REGKLAV NUMMASK KOLMASK MIN ! NUMMASK
  ON ?SPALL 11  ON KEYPRESS KEYSELECT  ON MF10 EX  ON MESC EXSV  ON ?Shadow 1
  '' ISPOLN '' INFORM '' VALUES 1 41 16 18 20 16 GENMEN2 [] ;
  : INFORM [i]
    BR 1 "����p������"
       2 "�롮p ��᪨"
       3 "SINCLAIR CATALOG"
       4 "����� ��᪠"
       5 "���p����� ����"
       6 "���p㧨�� ����"
       7 "�����p������"
       8 "���樫�����"
       9 "����஢����"
      10 "��������� 䠩�"
      11 "������ ����"
      12 "SCAN DIRECTORY"
      14 "����㧨�� XLT 䠩�"
      15 "������஢��� �� FW3"
   ELSE EMSTR ;

   : ISPOLN [i] BR 1 STARTIMG  2 SETMASK     4 MODVECT  5 SAVEMASK  6 LOADMASK
     7 DIRF  8 INIUST  9 BOOK  10 FILDIVIDE 11 RESVCTR  12 DIRSCAN  3 SINCLCAT
     13 PDP 14 LOADXLT 15 IMPFW3 ELSE NOP ;

: SETMASK NUMMASK 1+ C KOLMASK = IF+ T1 C ! NUMMASK
  BR 1 MASK1  2 MASK2  3 MASK3  4 MASK4  5 MASK5 6 MASK6 7 MASK7 8 MASK8
     9 MASK9  ELSE MASK1 ;
: SAVEMASK EON MESC NOP ON ?SPALL 6 '' NAMFILE 2 20 MNIN_IJ GENVVOD FMSKNAME
  FMSKNAME C BR+ YESSMASK DD ;
  : YESSMASK ON .RAS ".MSK" CONNECT CH WOPEN CH 0 ' IBMDVK 256 OS CH CLOSE CH ;
  : NAMFILE BR 1 "������ ��� 䠩��" 2 "��᪨ ����஢騪�" ELSE EMSTR ;

: LOADMASK EON MESC DELWIND ON ?SPALL 7 ON MF10 NOP
  MENCPOS ! XORD 2+ ! YORD "*.MSK" SEL
  CONNECT CH OPEN CH 0 ' IBMDVK 256 IS CH CLOSE CH ;

: LOADXLT EON MESC DELWIND ON ?SPALL 7 ON MF10 NOP
  MENCPOS ! XORD 2+ ! YORD "*.XLT" SEL
  CONNECT CH OPEN CH LENB CH SHR DO INKODXLT CLOSE CH 4 ! NUMMASK ;
  : INKODXLT IB CH IB CH E2 ! IBMDVK ;

: INIUST ON ?SPALL 1 EON MESC DELWIND ON MF10 NOP
  '' INISPOLN '' ININFORM '' NOP 6 34 0 MN2_IJ 10 GENMEN2 [] ;
  : ININFORM [i]
    BR 1 "�����p ��᪠"
       2 "��p�����p��騪"
       3 "�����p ��p⮭�"
       4 "������ p�����騪"
       5 "����⥫� 䠩���"
       6 "���࠭��� �������"
       7 "��ࠢ��"
    ELSE EMSTR ;
: INISPOLN [i] BR 1 USTSCAN  3 ININORT  4 INIBOOK  5 INIDIV
  6 SAVEPAL 7 XREN ELSE NOP ;
:: : SAVEPAL "OSPAL.SET" CONNECT CH WOPEN CH 0 ' PALLETS DIM? PALLETS OS CH
     CLOSE CH [] ;

[*****************************************************]
[***                                               ***]
[***  ���樫����p ��p����p�� �������� p�����騪�  ***]
[***                                               ***]
[*****************************************************]

B10
: INIBOOK [] ON ?SPALL 2 EON MESC DELWIND
  '' BOOKISP '' BOOKINF '' VALBOOK 13 20 7 26 16 16 GENMEN2 [] ;
  : BOOKINF [i]
    BR 1 "��� ᨬ���� p������⥫�"
       2 "��� �㬥p�樨"
       3 "��砫�� ����p ��p�����"
       4 "��p�����p����"
       5 "��᫮ ��p�� � ����"
       6 "��᫮ ��p�� � ��p����"
       7 "��砫� ��ன ��������"
    ELSE EMSTR ;
  : BOOKISP [i]
    BR 1 SYMVRAZD  2 TIPNUMERS  3 BEGHUMPAGE  4 ?KODIR  5 MODNSTROK
       6 MODPAGE   7 MODTERM    ELSE NOP [] ;
: VALBOOK [i] BR 1 SHOWRAZDSYM  2 SHOWNUM  3 VALNUMER  4 SPKODIR
  5 SHNSTROK     6 SHPAGE       7 VALTERM  ELSE EMSTR ;

: SHOWRAZDSYM S( BASE@ ) B16 RAZDSYM 0 ' TIMES C2 C2 !TB 1+ #  C2 !TB 1+
  PUSH 2 TON0 POP E2 DO TOVV D 0 ' TIMES 4 ;
: SHOWNUM TIPNUM BR 1 "�������㠫쭠�" 2 "�����������" 3 "��������"
  ELSE "��� �㬥p�樨" ;
: TIPNUMERS TIPNUM 1+ 3 & ! TIPNUM ;
: SPKODIR KOD? BR0 "��p�����p�����" "��� ����p������" ;
: ?KODIR KOD? NOT ! KOD? ;
: VALNUMER LPAGE    VALSKSS ;
: SHNSTROK NSTROK   VALSKSS ;
: SHPAGE   PAGESIZE VALSKSS ;
: VALTERM  TERMPOS  VALSKSS ;

: SYMVRAZD   PODGOT1 NUMVVOD RAZDSYM  [] ;
: MODNSTROK  PODGOT1 NUMVVOD NSTROK   [] ;
: MODPAGE    PODGOT1 NUMVVOD PAGESIZE [] ;
: MODTERM    PODGOT1 NUMVVOD TERMPOS  [] ;
: BEGHUMPAGE PODGOT1 NUMVVOD LPAGE    [] ;
  : PODGOT1 ON ?SPALL 3 '' DEMSTR MENCPOS 26 + 0 6 ;

: STprig [i] BR  1 STS01  2 "�᫠ ᨬ����� � ��ப�"  ELSE EMSTR [A,DL] ;
: PGprig [i] BR  1 STS01  2 "�᫠ ��p�� � ��p����"   ELSE EMSTR [A,DL] ;
  : STS01 "������ ����� ���祭��" ;

[*****************************************]
[***                                   ***]
[***  ���樫����p ��p����p�� ᪠��p�  ***]
[***                                   ***]
[*****************************************]

B10
: USTSCAN ON ?SPALL 2 EON MESC DELWIND
  '' ISPSCAN '' INFISCN '' ZNISCAN 5 18 13 MN1_IJ E4 4- E4 GENMENU [] ;
  : ISPSCAN BR 1 USTPATH 2 USTFTOM 3 USTFDIR 4 ?NEWLAB 5 DEFLAB ELSE NOP ;
  : INFISCN BR 1 "���� �뢮��" 2 "��砫�� ⮬" 3 "���� DIRSCAN"
               4 "���������� ��⪨" 5 "������ ��⪨" ELSE EMSTR ;
  : ?NEWLAB ?IZMLAB NOT ! ?IZMLAB ;
  : ZNISCAN
    BR 1 PROTFIL 2 NOMFTOM 3 FSCNDIR 4 ?MODLAB 5 SYSLAB ELSE EMSTR ;
  : USTFTOM EON MESC NOP '' DEMSTR MENCPOS 18 + 0  6 NUMVVOD FILENUM [] ;
  : DEFLAB  ON ?SPALL 3 EON MESC NOP '' DEMSTR
    MENCPOS 18 + 0 13 GENVVOD SYSLAB [] ;
    : ?MODLAB ?IZMLAB BR+ "��"  "���" ;


: USTFDIR ON ?SPALL 3 ON ?SJust 1 '' DEMSTR 5 24 GENVVOD FSCNDIR [] ;
: USTPATH ON ?SPALL 3 ON ?SJust 1 '' MENCPOS DEMSTR 5 24 GENVVOD PROTFIL [] ;
: ININORT ON ?SPALL 7 EON MESC DELWIND
  '' ISPNORT '' INFNORT '' STRSPATH 4 12 30 MN2_IJ E4 20 - E4 4 GENMEN2 [] ;
  : INFNORT BR 1 "VIEW PATH"  2 "EDIT PATH" ELSE EMSTR ;
  : ISPNORT BR 1 MODVIEW      2 MODEDIT     ELSE NOP ;
  : STRSPATH BR 1 VIEWSPEC 2 EDITSPEC ELSE EMSTR ;

: MODVIEW ON ?SJust 1 ON ?SPALL 6
  '' DEMSTR MENCPOS 10 + 0 30 GENVVOD VIEWSPEC [] ;
: MODEDIT ON ?SJust 1 ON ?SPALL 6
  '' DEMSTR MENCPOS 10 + 0 30 GENVVOD EDITSPEC [] ;

   : VALUES [i] 2 = BR+ MASKA EMSTR ;
     : MASKA NUMMASK
       BR 1 "IBM ---> DVK MIM"
          2 "�����쪨� �㪢�"
          3 "ASTRO FORTH -- TXT"
          4 "��᪠ ���짮��⥫�"
          5 "EDIK --> TXT"
          6 "���᪨� �����쪨�"
          7 "DVK MIM ---> IBM"
          8 "����訥 ������᪨�"
          9 "FIDO - ����஢��"
         10 "Frame Work III --> TXT"
       ELSE EMSTR ;

'' ggg ! FUNCTION

: PSTART [] ONAME CICL [] ;
  : CICL ON ?SPALL 3 ON ?SJust 1
    '' TEXTING 10 10 5 20 GENINF RP SYMOBR CLOSE OF DELWIND [] ;

 : TEXTING [i] 9 ! CLTXT BR 1 " ��p����뢠����"  2 " 䠩�"  3 STEKNAME
    ELSE EMSTR ;
    : STEKNAME TEKNAME E2 1+ E2 1- #  C3 C3 SRCHB MIN ;
    : PERSENT POS IF 10000 * SIZEOBR / D [Persent]
      #  !!! TIMES 10000 + 4 TON0 D
      0 ' TIMES TOVV TOVV #. E2 TOVV TOVV TOVV D [] 0 ' TIMES 5 [ADR.DL] ;

: INIDIV EON MESC NOP ON ?SPALL 18 ON ?SJust 1
  '' prigreg 14 45 4 22 NUMVVOD BLOKSIZE [] ;

: prigreg [i] BR
  1 STS01
  2 "���ᨬ��쭮�� p����p�"
  3 "����� ᮧ��������"
  4 "�p� ������� 䠩���"
  ELSE EMSTR [A,DL] ;

[*****************************************************]
[***                                               ***]
[***  ����⥫� 䠩�� �� ������� �� p����p� 䠩��  ***]
[***                                               ***]
[*****************************************************]

B10
: FILDIVIDE LSATRIBUT IF0 FDIVi [] ;
  : FDIVi [] 0 ! NTF INAME LENB IF C ! SIZEOBR
    KBUF BBUF - ! MAXBUFF YESVOL []
    CLOSE IF DELWIND 10 DO ZVOOK SCANSUBS ;
    : YESVOL FREEVOL 10000 + > BR0 DIVSTART ZAKONCH [] ;
      : DIVSTART [] ON ?SPALL 19
        '' TEXTING 10 10 8 20 GENINF 1 ! NOMFILE RP CICLDIV [] ;

  : CICLDIV S( BLOKSIZE ) ONAMEi RP DIVFILE CLOSE OF SIZEOBR EX0 [] ;

: DIVFILE [] [���p��塞 ��� ��⮢��⠭������� �� ᫥����� �p�室��]
  SIZEOBR BLOKSIZE MIN MAXBUFF MIN [�������쭮 ��������� ��p�� ��p���⪨]
  BLOKSIZE C2 - ! BLOKSIZE [PORCIA] SIZEOBR C2 - ! SIZEOBR [PORCIA]
  BBUF C2 IS IF [PORCIA] BBUF E2 OS OF [] BLOKSIZE EX0 SIZEOBR EX0 [] ;

: ONAMEi TEKNAME 1- E2 1+ E2 #. C3 C3 SRCHB MIN 0 MAX
  ON .RAS OCHRAS  ON ISF KORNAME CONNECT OF WOPEN OF ;
  : OCHRAS #. 0 ! FRAS NOMFILE 3 TON0 1 E2 DO TORAS D 0 ' FRAS 4
    NOMFILE 1+ ! NOMFILE ;
    : TORAS [i] E2 C #  = IF+ ZAMEN [i,B] C2 ! FRAS 1+ [i'] ;
      : ZAMEN D #0 ;

  : KORNAME [] ;

[*********************************************]
[***                                       ***]
[***  ����p������ 䠩�� �� �������� ��᪥  ***]
[***                                       ***]
[*********************************************]

B10
: SINCLCAT EON NOTFILE DNOWORK INAME ONAME
  ON ?SPALL 4 ON ?SJust 1 '' TEXTING 10 10 5 20 GENINF
  39 SPOS IF ON EOF EX RP STRCAT CLOSEALL DELWIND ;
  : DNOWORK ON ?SPALL 21
    "������ �㭪�� �믮������ ⮫쪮 � 䠩����!" InfErr ;
  : STRCAT 0 ' VVB 17 C2 C2 IS IF
    0 C3 C3 PROVZERRO C PUSH IF0 DD POP IF0 EOF
    OS OF 13 OB OF 10 OB OF ;
    : PROVZERRO [Prizn,Adr,Dl] DO SPTEST D ;
      : SPTEST [Prizn,Adr] C @B #  = IF0 USTPRIZN 1+ ;
        : USTPRIZN E2 T1 E2 ;

B10
: STARTIMG KOLVO BR0 ONEFILE MANY SCANSUBS [ 10 DO ZVOOK [] ;
  : ONEFILE L ATRIBUT 16 & IF0 FKOD ;
  : MANY S( L ) 0 ! NTF !0 L KOLVO DO ONERABOT [] ;
    : ONERABOT NOMER DO POISK FKOD [ DELWIND ]
      L UNSELF D [SORT-�������� �����] SHOWFILE !1+ L [] ;
      : POISK L SORT ATRIBUT 128 & EX+ !1+ L [] ;

: FKOD [] !0 REGKLAV INAME STRPOS SPOS IF LENB IF C ! SIZEOBR
  KBUF BBUF - ! MAXBUFF YESISFILE [] CLOSE IF ;
  : YESISFILE FREEVOL 1000 + > BR0 PSTART ZAKONCH [] ;

: KODFILE [] SIZEOBR C MAXBUFF MIN C ! VOLBUFF - ! SIZEOBR
  BBUF VOLBUFF IS IF BBUF VOLBUFF DO MODEKOD D
  BBUF VOLBUFF OS OF SIZEOBR EX0 [] ;

  FIX ACT VAR MODEKOD
  [��楤�� ��४���஢�� ������ �� ����� �� �⥪� ������� ����
   ��४���஢��� ��� � ������� �� ��஥ ���� �᫨ ����,
   ��᫥ �⮣� ������஢��� ���� � �⥪� ��� ᫥���饣� 横��]

  : PEREKOD [ADR] C @B IBMDVK C2 !TB 1+ [ADR"] ;

  : MASK1 INIDVK  '' PEREKOD  NORMKOD [] ;
  : MASK2         '' LITLKOD  NORMKOD [] ;
  : MASK3         '' ASTROBR  NORMKOD [] ;
  : MASK4         '' PEREKOD  NORMKOD [] ;
  : MASK5 SPECDVK '' EDIKPRK  NORMKOD [] ;
  : MASK6         '' CTERMIN  NORMKOD [] ; [���⠭����]
  : MASK7 INIIBM  '' PEREKOD  NORMKOD [] ;
  : MASK8         '' BIGENGL  NORMKOD [] ;
  : MASK9 INIFIDO '' PEREKOD  NORMKOD [] ;
  : MASK10        '' PEREVERT NORMKOD 130 ! STRPOS [] ;
    : NORMKOD '' KODFILE ! SYMOBR ! MODEKOD 0 ! STRPOS [] ;

  [�p���p�����⥫� �p��� 䠩�� ASTRO-FORTH � ����� ⥪�⮢� 䠩�]
  : ASTROBR [B] C 10 = IF+ CRDOBAV [] ;
    : CRDOBAV 13 OB OF [] ;

: LITLKOD [ADR] C @B [B] REGKLAV IF+ PEREKBYTE [B']
  C #A #Z SEG IF+ USTKLAV  C #a #z SEG IF+ USTKLAV
  C #� #� SEG IF+ USTKLAV  C #p #� SEG IF+ USTKLAV
  C #. = IF+ SBROSKLAV C2 !TB 1+ [] ;
  : PEREKBYTE C #A #Z SEG IF+ TOLIT1
    C #� #� SEG IF+ TOLIT1 C #� #� SEG IF+ TOLIT2 ;
  : USTKLAV   1 ! REGKLAV [] ;
  : SBROSKLAV 0 ! REGKLAV [] ;
  : TOLIT1 [B] 32 + [Blitle] ;
  : TOLIT2 [B] 80 + [Blitle] ;

: INAME TEKNAME E2 1+ E2 1- #  C3 C3 SRCHB MIN CONNECT IF OPEN IF ;
: ONAME TEKNAME 1- E2 1+ E2 #. C3 C3 SRCHB MIN 0 MAX
  ON .RAS ".PRK" CONNECT OF WOPEN OF ;

B10
BYTE VAR PRIZNBEG [�ਧ��� ��४���஢�� ��ࢮ� �㪢� � �������]
: CTERMIN [Adr] RP TOBEGIN C ! BEGWORD
  0 ! REGKLAV [� ��砫� ��ᨬ �ਧ��� ��४���஢�� ᫮��]
  [Adr] RP TESTWORD [Adr'] REGKLAV IF+ IZMWORD
  PRIZNBEG IF+ KODFIRST [Adr']
  C 2- @B BR #. SETPRIZN #! SETPRIZN #? SETPRIZN ELSE NOP
  C BBUF VOLBUFF + < EX0 ;
  : TESTWORD [Adr] C 1+ E2 @B
    C #� #� SEG IF+ USTKLAV [�᫨ ᨬ��� ���᪨� � ��⠭����� �ਧ���]
    C #p #� SEG IF+ USTKLAV [�᫨ ᨬ��� ���᪨� � ��⠭����� �ਧ���]
    BR 10 EX 13 EX #  EX ELSE NOP [Adr+1] C BBUF VOLBUFF + < EX0 ;
    : SETPRIZN 1 ! PRIZNBEG ;
    : KODFIRST [] BEGWORD C @B 0 ! PRIZNBEG
      C #� #� SEG BR+ -32 0 +
      C #� #� SEG BR+ -80 0 + E2 !TB [] ;
    : TOBEGIN [Adr] C @B C BR #. SETPRIZN #! SETPRIZN #? SETPRIZN ELSE NOP
      C  #A #Z SEG C2 #a #z SEG &0 C2 #� #� SEG &0
      E2 #� #� SEG &0 [Adr,Prizn-symv] EX+ C BBUF VOLBUFF + < EX0 1+ [Adr'] ;

: IZMWORD [] BEGWORD [Adr] RP IZMSIMV D [] ;
  : IZMSIMV [Adr] C @B
    [Adr,B] C BR #  EXIZMW ELSE CHNGR
    [Adr,B,Sm] + [B] C2 !TB 1+ [Adr+1]
    [Adr+1] C BBUF VOLBUFF + < EX0 [Adr'] ;
    : EXIZMW [Adr,B] D EX [Adr] ;
      : CHNGR C BR #A 95     #B 96     #C 158    #E 96   #p 112
        #P 32      #O 95     #K 95     #M 95     #T 142
        #X 141     #H 101    #� 32     #� 32     #� 32
        #� 32      #� 32     #� 32     #� 32     #� 32
        #� 32      #� 32     #� 32     #� 32     #� 32
        #� 32      #� 32     #� 32     #� 80     #� 80
        #� 80      #� 80     #� 80     #� 80     #� 80
        #� 80      #� 80     #� 80     #� 80     #� 80
        #� 80      #� 80     #� 80     #� 80     ELSE 0 ;

B10
[��४���஢�� �ᥣ� ᫮�� � ����訥 ������᪨� �᫨ ���� ��� 1 ������᪠�]
: BIGENGL [Adr] C ! BEGWORD
  0 ! REGKLAV [� ��砫� ��ᨬ �ਧ��� ��४���஢�� ᫮��]
  [Adr] RP TSTWengl [Adr'] REGKLAV IF+ toengl C BBUF VOLBUFF + < EX0 ;
  : TSTWengl [Adr] C 1+ E2 @B
    C #A #Z SEG IF+ USTKLAV [�᫨ ᨬ��� ENGL � ��⠭����� �ਧ���]
    C #a #z SEG IF+ USTKLAV [�᫨ ᨬ��� ENGL � ��⠭����� �ਧ���]
    BR 10 EX 13 EX #  EX #. EX #! EX #? EX ELSE NOP
    [Adr+1] C BBUF VOLBUFF + < EX0 ;

: toengl [] BEGWORD [Adr] RP IZMengl D [] ;
  : IZMengl [Adr] C @B
    [Adr,B] C BR #  EXIZMW 10 EXIZMW 13 EXIZMW ELSE englCHNGR
    [Adr,B,Sm] - [B] C2 !TB 1+ [Adr+1]
    [Adr+1] C BBUF VOLBUFF + < EX0 [Adr'] ;
    : englCHNGR C BR  #� 95  #� 96  #� 158  #� 96  #p 112
      #� 32   #� 95   #� 95  #� 95  #� 142  #� 141 #� 101 ELSE 0 ;

B8
[��४���஢�� 䠩�� �� EDIK� � IBM]
: EDIKPRK [adr] C @B C BR 16 USTFLAG 17 SBROSFLAG ELSE TOMIM [B'] C2 !TB 1+ ;
  : USTFLAG   [B] D #  !1 REGKLAV [] ;
  : SBROSFLAG [B] D #  !0 REGKLAV [] ;
  : TOMIM [B] C 100 176 SEG REGKLAV * IF+ LM [B'] ;
    : LM [B] 200 + IBMDVK [B'] ;

B10
[���樠������ ���ᨢ� ��p�����p���� ��� ��� -> IBM]
: INIDVK [] RESVCTR INIRUS 64 DO TOVCTR [] MODLLAT ;

[���樠������ ���ᨢ� ��p�����p���� ��� IBM -> ���]
: INIIBM [] RESVCTR INIRUS 64 DO ONVCTR [] OBRLLAT [] ;
  [������ ������ ���� �� ���� ��p��� ᨬ����]
  : TOVCTR [Cod oldsym,Cod newsym] E2 ! IBMDVK [] ;
  : ONVCTR [Cod oldsym,Cod newsym]    ! IBMDVK [] ;

[���樠������ ��� ��४���஢�� ��ᥬ FIDO]
: INIFIDO [] RESVCTR #H #� ! IBMDVK  #y #� ! IBMDVK  #p #� ! IBMDVK [] ;

: SPECDVK RESVCTR EINIRUS 64 DO ONVCTR ;

[��砫쭠� ���樫����� ���ᨢ� ��p�����p����]
: RESVCTR 0 256 DO INIVECT D ;
  : INIVECT C C ! IBMDVK 1+ ;

[����� ᨬ����� ���ᨢ� ��p�����p����]
: SHOWVECT 0 256 DO SHOWS D ;
  : SHOWS C IBMDVK C B10 CR 4 TON SP SP TOB 1+ ;

[����䨪��� ᮤ�p������ ����p� ��p�����p����]
: MODVECT [] S( BASE@ ) EON MESC DELWIND ON ?SPALL 4 B16 #  !!! TIMES
  ON MF10 NOP "����䨪��� ��᪨"
  '' ISPVECT '' INFVECT '' VALVECT 3 10 HS 5 - 12 8 256 GLBMEN2 [] ;
  : INFVECT [i] 1- PUSH S( BASE@ ) B10 "    ᨬ���   "
    POP C C4 C4 + 2- !TB 3 TON0  6 CT E2 DO TOVV D ;
  : VALVECT [i] #  !!! TIMES 1- IBMDVK 2 ' TIMES C2 C2 !TB
    2+ PUSH 2 TON0 POP E2 DO TOVV 6 - 6 ;

  BYTE VAR NEWKOD
  : ISPVECT [i] 1- ON ?SPALL 3  EON MESC D ON ?SPlus #  C IBMDVK ! NEWKOD
    '' DEMSTR MENCPOS 16 + 0 4 NUMVVOD NEWKOD NEWKOD 255 & E2 ! IBMDVK [] ;

[���樠������ �p���᪮� ��� ���ᨢ� ��p�����p����]
: INIGRAF []
  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+
  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+
  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+  #� #+
  #� #+  #� #+  #� #+  #� #!  #� #-  #� #!
  #� #+  #� #+  #� #+  #� #!  #� #!  #� #!
  #� #+  #� #+  #� #+  #� ##  #� #*  #� #!
  #� #!  #� #- ;

[����䨪��� �����쪨� ��⨭᪨� �����]
: MODLLAT [] 97 122 C2 - DO IMLITLAT D [] ;
  : IMLITLAT [I] C IBMDVK 96 + C2 ! IBMDVK 1+ [I'] ;
: OBRLLAT [] 97 122 C2 - E2 96 + E2 DO IZLITLAT D [] ;
  : IZLITLAT [I] C IBMDVK 96 - C2 ! IBMDVK 1+ [I'] ;

B8
: EINIRUS []
  #� 301  #� 302  #� 327  #� 307  #� 304  #� 305  #� 326  #� 332
  #� 311  #� 312  #� 313  #� 314  #� 315  #� 316  #� 317  #� 320
  #� 322  #� 323  #� 324  #� 325  #� 306  #� 310  #� 303  #� 336
  #� 333  #� 335  #� 337  #� 331  #� 330  #� 334  #� 300  #� 321
  #� 341  #� 342  #� 367  #� 347  #� 344  #� 345  #� 366  #� 372
  #� 351  #� 352  #� 353  #� 354  #� 355  #� 356  #� 357  #� 360
  #� 362  #� 363  #� 364  #� 365  #� 346  #� 350  #� 343  #� 376
  #� 373  #� 375  #� 337  #� 371  #� 370  #� 374  #� 340  #� 361 ;

: INIRUS []
  #� 341  #� 342  #� 367  #� 347  #� 344  #� 345  #� 366  #� 372
  #� 351  #� 352  #� 353  #� 354  #� 355  #� 356  #� 357  #� 360
  #� 362  #� 363  #� 364  #� 365  #� 346  #� 350  #� 343  #� 376
  #� 373  #� 375  #� 377  #� 371  #� 370  #� 374  #� 340  #� 361
  #� 141  #� 142  #� 167  #� 147  #� 144  #� 145  #� 166  #� 172
  #� 151  #� 152  #� 153  #� 154  #� 155  #� 156  #� 157  #� 160
  #� 162  #� 163  #� 164  #� 165  #� 146  #� 150  #� 143  #� 176
  #� 173  #� 175  #� 137  #� 171  #� 170  #� 174  #� 140  #� 161 ;

B10
: MFW3>TXT KOLVO BR0 FW3ONEFILE FW3MANY SCANSUBS ;
  : FW3ONEFILE L ATRIBUT 16 & IF0 FKOD ;
  : FW3MANY S( L ) 0 ! NTF !0 L KOLVO DO F3ONERABOT [] ;
    : F3ONERABOT NOMER DO POISK FKOD L UNSELF D SHOWFILE !1+ L [] ;

: IMPFW3 [] EON NOTFILE INFNOF INAME ONAME 130 SPOS IF
  RP PEREVERT CLOSEALL [] ;
  : PEREVERT IB IF C BR0 PROPUSK OBOF ;
    : PROPUSK [] D 6 DO IBIFD 13 OB OF 10 OB OF [] ;
      : IBIFD IB IF D [] ;
      : OBOF OB OF [] ;

: INFNOF ON ?SPALL 40 "�� ��࠭ 䠩�!" InfErr ;

"OSPAL.SET" CONNECT CH OPEN CH 0 ' PALLETS LENB CH IS CH CLOSE CH
25000 ! BLOKSIZE  6 ! NUMMASK  MASK6 [��砫쭠� ���樫�����]
UNDEF

PROGRAM $DUMP B10 ."
[26-10-1994 16:00] ��� ����᪠ �ணࠬ�� ������ ���� ����㦥�� ������:
[10-10-1994 12:41] SCRWORK  - �ਬ�⨢� ࠡ��� � �࠭��
[24- 8-1994 12:43] MENU4    - ����� �࣠����樨 ����
[23- 8-1994 16:51] MENUTIL4 - �⨫��� ��� ����
[21- 4-1994  9:56] GENHLP   - �뤠� �����
[19- 4-1994  8:00] QCL      - ���⮢�� ���䨣���� - �����祭
[ 6- 4-1994 14:43] DIASXXXX - �����ᥬ���� - ����易⥫쭮
[ 5- 4-1994  7:59] [ 5- 4-1994  9:04] [25- 3-1994 15:01] 4005 ����
[10-12-1993 10:48] �ணࠬ�� DUMP."

       BYTE VAR BOXMEM
:: FIX BYTE VAR YDIA
:: FIX BYTE VAR XDIA      [���न���� ���� �����ᥬ����]        39 ! XDIA
:: FIX BYTE VAR BASE      [���⥬� ��᫥��� �� �����]          16 ! BASE
:: FIX BYTE VAR CLSYM     [���� �뢮����� ᨬ���쭮� ���]      14 ! CLSYM
:: FIX BYTE VAR CLMOD     [���� ������஢������ 㦥 ᨬ����]   10 ! CLMOD
:: FIX LONG VAR DMLABEL   [����-��⪠ ��� ����ண� �த�������]  0 ! DMLABEL
::     WORD VAR CHANGE    [�ਧ��� ��������� � ०��� DUMP]
::     WORD VAR BATRVM    [��६����� �࠭�⥫� ��ਡ��]
::     WORD VAR LABLATR   [��६����� �࠭�⥫� ��ਡ��]
       LONG VAR ADRUPS    [��砫�� ���� ���孥� ��ப� � ���� DUMP�]
       LONG VAR ADRTEK    [����騩 ���� DUMP�]
       LONG VAR ADREND    [���� ����  ������ DUMP�]
       BYTE VAR COMLEN    [����� ����ᥬ����㥬�� �������]
   FIX BYTE VAR CLSFON    [��� ᨬ���� � ᨬ��쭮� ���]     11 ! CLSFON
   FIX BYTE VAR CLTTDIA   [����� � ���� �����ᥬ���� � ��]   12 ! CLTTDIA
   FIX BYTE VAR CLTNDIA   [����� � ���� �����ᥬ���� ��� ��] 09 ! CLTNDIA
:: FIX BYTE VAR CLTDUMP   [���� ᨬ����]                       7 ! CLTDUMP
:: FIX BYTE VAR TCLTDUMP  [���� ⥪�饣� ᨬ����]             12 ! TCLTDUMP
:: FIX BYTE VAR TCLFDUMP  [���� 䮭� ⥪�饣� ᨬ����]         7 ! TCLFDUMP
::     BYTE VAR FLAGZOOM  [���� ࠧ�襭�� �뤠� �㬬��]
:: FIX LONG VAR NABEGOBL  [������ ���� ������]          0    ! NABEGOBL
:: FIX LONG VAR NAENDOBL  [��砫�� ���� ������]         3840 ! NAENDOBL
:: FIX LONG VAR NNOVVAL   [����� ����ᨬ�� ���祭��]        0    ! NNOVVAL
:: FIX BYTE VAR DCORNXU   [�⮫��� ���孥�� ������ 㣫�]       3 ! DCORNXU
:: FIX BYTE VAR DCORNYU   [��ப�  ���孥�� ������ 㣫�]       4 ! DCORNYU
:: FIX BYTE VAR DCORNXD   [�⮫��� ������� �ࠢ��� 㣫�]      61 ! DCORNXD
:: FIX BYTE VAR DCORNYD   [��ப�  ������� �ࠢ��� 㣫�]      21 ! DCORNYD
::     BYTE VAR INFSTRD   [��ப�  ������� �ࠢ��� 㣫�]
:: FIX BYTE VAR ?ONSYMV   [0-������ ०��, 1-ᨬ����� ०��] 0 ! ?ONSYMV
:: FIX ACT  VAR PROVTRP   [�஢�ઠ ������ �窨 ��⠭���]
:: FIX ACT  VAR ACTHELP   [��६����� ��ࠡ��稪 �뤠� ���᪠���]
:: FIX ACT  VAR ALTHELP   [��६����� �뤠� ����ୠ⨢��� ���᪠���]
:: FIX ACT  VAR ASSHELP   [�뤠� ���᪠��� �� ��⥬� ������ ��ᥬ����]
:: FIX ACT  VAR ALTKEYS   [��६����� ��ࠡ��稪 ����ୠ⨢��� ������]

DEFINE? CODNUMB NOT %IF
:: FIX  LONG VAR CODNUMB  [���� ࠧ��饭�� ���� � ����� ��� �����ᥬ����]
                    %FI
DEFINE? VVB NOT %IF :: 80 BYTE VCTR VVB  [��㦥���� ��ப�] %FI
DEFINE? NAMEHLP NOT %IF
80 FIX STRING NAMEHLP "" ! NAMEHLP
                    %FI

DEFINE? TMEMORY NOT %IF 20000 BYTE VCTR TMEMORY %FI

DEFINE? NACHCOD NOT %IF
:: FIX  LONG VAR NACHCOD  [���� ��砫� ������ ࠧ��饭�� ���� � �����]
DEFINE? MEMORY %IF 0 ' MEMORY ! NACHCOD %FI
                    %FI

DEFINE? NACHLO NOT %IF
:: FIX  LONG VAR NACHALO [���� ��砫� ������ DUMP�-���筮 �� ����� 祬
                          ����稭� NACHCOD]
                   %FI
   DEFINE? NOPKOD  %IF NOPKOD ! NNOVVAL %FI
'' NOP C C ! ACTHELP ! ALTHELP ! ASSHELP '' T0 ! PROVTRP
DEFINE? DETEC   NOT %IF BYTE CNST DETEC 0 ; %FI
DEFINE? LONGCOM NOT %IF COPYW T1 LONGCOM %FI
DEFINE? PR      NOT %IF COPYW D  PR      %FI

[ DEFINE? !VW NOT %IF :: : !VW !T ; %FI ]
DEFINE? !VW NOT %IF :: COPYW !T !VW %FI

[��楤�� �����஢���� ������ �����]
:: : NDUMP [���� ��ப� ���������,�����,Adr of begin,Adr of end,Adr tek]
     S( NPALLET DCORNYU BROW BCOL BASE@ )
     B16 ETXOFF ! ADRTEK ! ADREND ! NACHALO
     PROVADRUPS [����ᠫ� � �஢�ਫ� ����]
     HS DCORNYU - 1- C ! DCORNYD ! INFSTRD
     ON ?SJust 1  ON ?SPALL 17  EON MESC DELWIND
     DCORNYD DCORNYU + 1- ! BROW DCORNXU 1+ ! BCOL
     72 ! DCORNXD !0 CHANGE [��������� �� �� �뫮]
     NCUR [����ᨫ� �����] DCORNYU 2- ! YDIA
     [Adr,Dl] DCORNXD DCORNXU - LJUST
     '' DMPTEXT DCORNYU DCORNXU DCORNYD DCORNXD GLBINF  17 SETPALL
     DCORNYU 3- DCORNXU DCORNXD C2 - SHR + #� CLFON CLKANT FORMSYMB RSYMB
     DCORNYU 2- DCORNXU DCORNXD C2 - SHR + #� CLFON CLKANT FORMSYMB RSYMB
     DCORNYU 1- DCORNXU DCORNXD C2 - SHR + #� CLFON CLKANT FORMSYMB RSYMB
     DCORNYU 1+ ! DCORNYU
     SHOWBASE SHOWLABEL ADRTEK DVIGAEM [� ⥪�饩 �祩��]
     FIRSTSCR INFDIAS
     SHOWTEK MOULIMIT MouON RP SDV MouFREE MouOFF DELWIND ;
     [�஢�ઠ ���� �� ���४⭮���]
     : PROVADRUPS [] NACHALO [C] ADREND C2 - [Voliume] 15 &0 + ! ADREND
       [AdrBeg] [ADRTEK C2 - -16 & 128 - MAX
       ADREND 16 HS * - 1- MIN ! ADRUPS] ;
     : MOULIMIT DCORNXU DCORNYU 1- DCORNXD 2+ DCORNYD 3+ MouRANGE ;
     : DMPTEXT BR
1  "      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ������쭠� �ଠ"
INFSTRD
   "    ESC F1 F2 F3 F5 F6 F7 F8 F9 F10 PgDn PgUp Enter Tab Insert    "
ELSE EMSTR ;

  : INFDIAS ADRTEK C NACHCOD - ! CODNUMB @B C LONGCOM 15 & ! COMLEN
    PR CLFON CODNUMB PROVTRP BR0 CLTNDIA CLTTDIA
    YDIA XDIA 0 ' VVB 34 CLTOS [] ;
    DEFINE? DEMSTR NOT %IF :: : DEMSTR D EMSTR ; %FI

  [�뤠� �࠭� � ������ �祥�]
  : FIRSTSCR CLFON 4 SHT CLTDUMP &0 SWB ! BATRVM
    DCORNYU DCORNXU 6 + OPADRVM [Ptrvm] ADRUPS
    DCORNYD DCORNYU - 3+ DO STRSHOW DD ;
    : STRSHOW [Ptrvm,Adres] DLABEL CLFON 4 SHT CLSYM &0 SWB C3 98 +
      C3 16 DO POSYM DDD 16 DO ELEMSHOW E2 64 + E2 ;
      : POSYM [Atrib,AdrVm,AdrSourse] C @B C4 &0 C3 !VW [����ᠫ� ᨬ���]
        1+ E2 2+ E2 ;

  : ELEMSHOW [Ptrvm,Tadri] C @B -4 SHT LEGALSYM E2 2+ E2
    C @B LEGALSYM 1+ E2 4+ E2 [Ptrvm+6,Tadri+1] ;
  : DLABEL [Ptrvm,Adress] BATRVM
    CLFON 4 SHT 12 &0 SWB ! BATRVM
    C3 C3 NACHCOD - E2 4- E2 #: BATRVM &0 C3 !VW
    E2 2- E2        C LEGALSYM
    E2 2- E2 -4 SHT C LEGALSYM
    E2 2- E2 -4 SHT C LEGALSYM
    E2 2- E2 -4 SHT C LEGALSYM DD ! BATRVM [] ;
::  : LEGALSYM [Ptrvm,Adr,Sym] 15 & C 9 > BR0 #0 #7 + BATRVM &0 C3 !VW
      [Ptr,AdR] ;

[�ப��⪠ ���� �� 1 ��ப� ����]
:: : WNDUP [] 0 MROLL [] ;

[�ப��⪠ ���� �� 1 ��ப� �����]
: WNDDOWN [] 1 MROLL [] ;
  : MROLL [0 - UP, 1 - DOWN] PUSH
    DCORNYU DCORNXU DCORNYD C3 - 3+ DCORNXD C3 - 2+
    CLFON CLTXT FORMATR 1 POP MouOFF BR0 ROLLUP ROLLDN MouON [] ;

  : MASKLIT 223 & ;

: SCRDUHL MouFREE ON ?SPALL 41
  140 NAMEHLP GENHELP MOULIMIT [] ; '' SCRDUHL ! ACTHELP
  [���� �������� ������]
  : SDV [] TRBMou C BR0 OBRMOUS OBRKEYB [] ;
    : OBRMOUS D MouPosI DCORNYD DCORNYU + 2- = BR0 VERHDUMP NIZDUMP
      DEFPALL INFDIAS MouON [] ;
B16   : NIZDUMP [] MouPosJ DCORNXU - [���न��� �⭮�⥫쭮 ��� ����]
        0E C2 < IF0 NOP
        C  0  4 SEG IF+ BASEMOD  [MOD BASE]
        C  5  9 SEG IF+ DEX      [Exit-ESC]
        C 0A 0C SEG IF+ ACTHELP  [F1-HELP]
        C 0D 0F SEG IF+ OBLWORK  [F2-AREA]
        C 10 12 SEG IF+ BASEMOD  [F3-MOD BASE]
        C 13 15 SEG IF+ USTADR   [F5-UST ADRES]
        C 16 18 SEG IF+ TOLABEL  [F6]
        C 19 1B SEG IF+ ASSHELP  [F7]
        C 1C 1E SEG IF+ MODTRP   [F8]
        C 1F 21 SEG IF+ MODDIAF  [F9]
        C 22 25 SEG IF+ AVTDIAS  [F10]
        C 26 2A SEG IF+ PAGEDNC  [PageUp]
        C 2B 2F SEG IF+ PAGEUPC  [PageDn]
        C 30 35 SEG IF+ NORMODIF [Enter - ����䨪���]
        C 36 39 SEG IF+ TABULYC  [TAB]
        C 3A 3F SEG IF+ INSSDIA  [Insert]
        C 40 41 SEG IF+ BEGSTRC  [HOME]
        C 42 43 SEG IF+ DDSTEP   [Down]
        C 44 45 SEG IF+ DUSTEP   [Up]
          46 47 SEG IF+ ENDSTRC  [END] ;
      DEFINE? DEX NOT %IF : DEX D EX ; %FI
      : VERHDUMP [] MouPosJ DCORNXU - [���न��� �⭮�⥫쭮 ��� ����]
        4 C2 < BR0 USTADR NOUSTADR D [] ;
        : NOUSTADR 5 - C C [������ �⭮�⥫쭮 ���� ��⪨]
          30 < BR+ MouKOD MouSYM [] ;
 : MouKOD [Pos] 3 / D [Nstr] SHOWNORM 0 ! ?ONSYMV USTMOU ;
   : USTMOU MouPosI DCORNYU - 4 SHT + [N-㧫�]
     ADRUPS + [���� ����] C ADRTEK = BR0 NEWATEK DMODIF [] ;
   : NEWATEK [AdrNew] VYPSTEP ;
   : DMODIF [] D NORMODIF [] ;
 : MouSYM [Pos] SHOWNORM 1 ! ?ONSYMV C 31 > BR+ OBRSYM D SHOWTEK ;
   : OBRSYM 1 ! ?ONSYMV 32 - USTMOU ;

    [��ࠡ�⪠ ����������]
    : OBRKEYB [cod] BR
      4700 BEGSTRC  [Home]
      0F09 TABULYC  [Tab]
      5100 PAGEDNC  [PageDn]
      1C0D NORMODIF [Enter]
      4900 PAGEUPC  [PageUp]
      4F00 ENDSTRC  [End]
      4D00 DRSTEP   [Right]
      4B00 DLSTEP   [Left]
       11B EX       [Esc]
      4800 DUSTEP   [Up]
      5000 DDSTEP   [Down]

      3B00 ACTHELP  [F1 - key �맮� �����]
      3C00 OBLWORK  [F2 - key �맮� ���� ࠡ��� � ��������]
      3D00 BASEMOD  [F3 - key ��������� ��⥬� ��᫥��� ��� �����]
      3F00 USTADR   [F5 - key ���� ��⪨ � �������� � ���]
      4000 TOLABEL  [F6 - key ��।������� � ��⠭�������� ��⪥]
      4100 HELPF7   [F7 - key �맮� ����� �� ��⥬� ������]
      4200 MODTRP   [F8 - key ���⠭���� ����஫쭮� �窨]
      4300 MODDIAF  [F9 - key ���� ����� 䠩�� ��⮪��� �����ᥬ���஢����]
      4400 AVTDIAS  [F0 - auto deasm]
      5200 INSSDIA  [INS - key �뢮� ��ப� �����ᥬ���� � 䠩�]
      6300 HELPF6   [CntRlF6-key �맮� ����ୠ⨢��� �����]
      7700 CntrHOME
      7500 CntrEND
      ELSE ALTVVOD DEFPALL INFDIAS ;
    : HELPF6 MouFREE ALTHELP MOULIMIT ;
    : HELPF7 MouFREE ASSHELP MOULIMIT ;
    : CntrHOME SHOWNORM NACHALO NACHCOD - DVIGAEM ;
    : CntrEND  SHOWNORM ADREND DVIGAEM ;

B10
: OBLWORK EON MESC FIRDEL ON ?SPALL 5 ON MHELP AREAHLP
  "    ���������� ������"
  '' IZMENEN '' OBLTEXT '' VALTEXT 4 6 6 20 6 GLBMENU DELWIND [] ;
: FIRDEL DELWIND DEFPALL FIRSTSCR SHOWTEK [] ;
: IZMENEN ON ?SPALL 6
  BR 1 VYPZAPL 2 INVALN 3 INABEG 4 INAEND 5 USTTRAP 6 SBRTRAP ELSE NOP ;
  : USTTRAP 1 SETPOINT ;
    : SETPOINT 0 ' TMEMORY NABEGOBL NAENDOBL C2 - 1+ DO SETVTRP DDD
      !1 CHANGE MESC ;
      : SETVTRP [adr] C3 C3 C3 !BI 1+ [adr'] ;
  : SBRTRAP 0 SETPOINT ;
  : AREAHLP ON ?SPALL 41 138 NAMEHLP GENHELP ;

: OBLTEXT BR
  1 "��������� ���祭���"
  2 "�������� ���祭�� -"
  3 "��砫�� ����   -"
  4 "������ ����    -"
  5 "��������� ����誠��"
  6 "������ ����誨"
  ELSE EMSTR ;
: VALTEXT BR 3 AOBLBEG 4 AOBLEND 2 VNOSVAL ELSE EMSTR ;
: AOBLBEG NABEGOBL STRWORD ;
: AOBLEND NAENDOBL STRWORD ;
: VNOSVAL NNOVVAL  STRBYTE ;
:: : STRWORD SCLR VVODSTR 4 VAL>STR ;
:: : STRBYTE SCLR VVODSTR 2 VAL>STR ;


[�뤠� ᮮ�饭�� � 室� ����������]
: VYPZAPL ON ?SPALL 7 '' ZAPINFO 21 33 1 15 GENINF NABEGOBL NAENDOBL
  C2 - 1+ E2 NACHCOD + E2 C BR+ VZV DDD DELWIND !1 CHANGE MESC ;
::  : ZAPINFO BR 1 "�������� ������" ELSE EMSTR ;
   : VZV [ADR,DL] DO VPIS D [] ;
     : VPIS [ADR] NNOVVAL C2 !TB 1+ ;

: INABEG PODGVVOD NUMVVOD NABEGOBL NABEGOBL NACHALO NACHCOD - MAX
  ADREND NACHCOD - MIN ! NABEGOBL !1 CHANGE ;
: INAEND PODGVVOD NUMVVOD NAENDOBL NAENDOBL NACHALO NACHCOD - MAX
  ADREND NACHCOD - MIN ! NAENDOBL !1 CHANGE ;
: INVALN PODGVVOD NUMVVOD NNOVVAL !1 CHANGE ;
  : PODGVVOD '' DEMSTR MENCPOS 24 + 0 5  ;

[���� ���祭�� ���⮢ �����।�⢥��� � ᨬ���쭮� ����������]
: VODKBD [kod] INKEY 255 & ADRTEK !TB SHOWNORM ADRTEK 1+
  PROVEND SHOWTEK !1 CHANGE ;

B16
[���� ������ ���祭�� ���� � ����������]
: TABULYC [] SHOWNORM ?ONSYMV NOT ! ?ONSYMV SHOWTEK [] ;
  : SHOWNORM  ?ONSYMV BR0 REKODSHOW RETEKSYMB [] ;
  : SHOWTEK   ?ONSYMV BR0 CTEKSHOW  TEKSYMB [] ;
    : ALTVVOD ?ONSYMV BR0 KODVVOD   VODKBD [] ;

  : TEKSYMB             SYMCOOR ADRTEK @B CLSFON TCLTDUMP FORMSYMB RSYMB [] ;
  : RETEKSYMB REKODSHOW SYMCOOR ADRTEK @B CLFON  CLSYM    FORMSYMB RSYMB [] ;

: MODTRP 0 ' TMEMORY ADRTEK NACHCOD - C2 C2 @BI NOT E3 E2 !BI INFDIAS
  !1 CHANGE [] ;

B16
: BEGSTRC [Home] SHOWNORM ADRTEK    OPRUZ E2D -   PROVEND SHOWTEK ;
: ENDSTRC [End]  SHOWNORM ADRTEK 0F OPRUZ E2D - + PROVEND SHOWTEK ;
: PAGEUPC [PageUp]
  ADRUPS 10 DCORNYD DCORNYU - 2+ * - NACHALO MAX ! ADRUPS
  ADRTEK 10 DCORNYD DCORNYU - 2+ * - NACHALO MAX ! ADRTEK
  FIRSTSCR SHOWTEK ;
: PAGEDNC [PageDn] ADREND ADRUPS ?STROK C [SM] ADRUPS +
  ADREND 10 DCORNYD DCORNYU - 2+ * - MIN ! ADRUPS
  ADRTEK + PROVEND FIRSTSCR SHOWTEK ;
  : PROVEND ADREND MIN ! ADRTEK ;
  [�ᥣ� ������� ��ப]
  : ?STROK - 1+ 10 / D DCORNYD DCORNYU - 2+ MIN 10 * ;

B10
DEFINE? UNDFKEY NOT %IF
TRAP UNDFKEY NOP
                    %FI
[�뤠� �㬥� �p� ���ࠢ��쭮� �����]
:: : ZUMER FLAGZOOM IF+ IGOGO UNDFKEY ;
     : IGOGO 4 DO ZVOOK ;

:: : KODVVOD 1 ! FLAGZOOM INKEY MASKLIT
     C 16 25 SEG IF+ DVIG-#0 [B]
     C #A #F SEG BR+ DVIG-#A D [B] ;
  [����䨪��� ��p��� ���祭�� ���� ᨬ�����]
  : DVIG-#0 [B'] C 16 - VDVIGZN ;
  : DVIG-#A [B']   #7 - VDVIGZN ;
::  : VDVIGZN [���ࠤ� �᫠] 0 ! FLAGZOOM
      ADRTEK @B [����稫� ��p�� ���祭��]
      4 SHT &0 [������� ��ࠤ� �᫠ ᮥ������ � ��p�� ���襩]
      ADRTEK !TB [����ᠫ� ����� ���祭�� �� ���� ��p���]
      DTEKTOS [B] NORMZVOOK !1 CHANGE ;
      [�뤠� ��᪠ �p� ��ଠ�쭮� �����]
      :: : NORMZVOOK 1000 40 BEEP ;

[�뢮� ⥪�饣� ���������]
: DTEKTOS S( BASE@ ) B16 SHOWTEK SYMCOOR ADRTEK @B CLMOD SWB &0 RSYMB [] ;

: DLSTEP []  -1 STEP ;
: DRSTEP []   1 STEP ;
: DUSTEP [] -16 STEP ;
: DDSTEP []  16 STEP ;
  : STEP [SM] ADRTEK + NACHALO MAX [NewAdr] SPROVEND BR+ VYPSTEP D [] ;
    : SPROVEND [NewAdr] C ADREND > NOT [NewAdr,Sign] ;
    : VYPSTEP [NewAdr] SHOWNORM ?ONSCREEN ! ADRTEK SHOWTEK [] ;
    : ?ONSCREEN [NewAdr] ADRUPS 16 DCORNYD DCORNYU - 3+ * 1- +
      C2 < BR+ UPROLL ?DOWNROLL [NewAdr] ;
    : UPROLL [] ADRUPS 16 + ! ADRUPS WNDUP
      DCORNYD 2+ [1- DCORNYU +] DCORNXU 6 + OPADRVM ADRUPS
      16 DCORNYD DCORNYU - 2+ * + STRSHOW DD ;
    : ?DOWNROLL C ADRUPS < IF+ DOWNROLL ;
      : DOWNROLL ADRUPS 16 - ! ADRUPS WNDDOWN
        DCORNYU DCORNXU 6 + OPADRVM ADRUPS STRSHOW DD ;

[��।������ ���न��� ������� ��� ����]
: SYMCOOR OPRUZ E2 DCORNYU + E2 58 + [Nstr,Pstr] ;
[��।������ ���न��� ᨬ���쭮� ��� ����]
: OPRCOORDUMP [] OPRUZ 3 * 09 + E2 DCORNYU + E2 [Nstr,Pstr] ;
  [��।������ �� ⥪�饬� ����� ����� 㧫�]
  : OPRUZ [] ADRTEK ADRUPS - ABS 16 / [Nstr,Pstr] ;

[�⮡ࠦ���� ����� ��� ⥪�饣�]
: CTEKSHOW TCLFDUMP TCLTDUMP KODCLTOS [] ;

[�⮡ࠦ���� ����� ��� ���筮��]
: REKODSHOW CLFON CLTDUMP KODCLTOS [] ;
  : KODCLTOS OPRCOORDUMP ADRTEK @B STRBYTE CLTOS ;

[�/� ����䨪�樨 ���筮� ��࠭��� �祩��]
: NORMODIF [] S( BASE@ ) EON MESC NOP BASE ! BASE@
  modif SHOWTEK DEFPALL [] ;
  [��楤�� ����� �᫠]
  : modif [] ON ?SPALL 9  ADRTEK EQU BOXMEM [ᮤ�ন��� �祩��]
    '' prigreg 21 30 1 24 NUMVVOD BOXMEM [] !1 CHANGE [] ;
  :: : prigreg [i] BR 1 "������ ����� ���祭��" ELSE EMSTR [A,DL] ;
  :: : STRVALUE 0 ' VVB BASE@ BR 2 8  8 3  10 3  16 2 ELSE 4 ;

B10
[��楤�� ��⠭���� ��������� ���� �����]
: USTADR [] S( BASE@ ) B16 ON ?SPALL 8  1234 10 BEEP
  '' prigadr 21 30 2 28 NUMVVOD DMLABEL DEFPALL
  [����� 㦥 ������� ���� � �㦭� � ���� ���������] TOLABEL [] ;
  : TOLABEL DMLABEL DVIGAEM [] ;
  : DVIGAEM [Adr]
    NACHCOD + [] C ADREND MIN ! ADRTEK [�஢�ਫ� �� ��室 �� �࠭���]
    NACHALO - -16 & 128 - NACHALO C E3 + MAX
    ADREND 16 HS 7 - * 1- - MIN ! ADRUPS [᪮�४�஢��� �� ����]
    FIRSTSCR SHOWLABEL
    [����ᮢ뢠�� �࠭] SHOWTEK ;
  : STRADR DMLABEL STRWORD ;
  : prigadr [i] BR 1 "������ �ॡ㥬� ��� HEX - "
                   2 "���� ������  �����஢����." ELSE EMSTR ;
[�⮡ࠦ���� ���祭�� ��⪨]
: SHOWLABEL CLFON 13 DCORNYU 1- DCORNXU STRADR #: C3 C3 + !TB 1+ CLTOS [] ;

[############## ��������� ��⥬� ���᫥��� ##################]
B10
   BYTE VAR BL   [��६����� ��� ����஫� ��������� ��⥬� ���᫥���]
:: BYTE VAR BCOL [������� �⮡ࠦ���� ��⥬� ���᫥���]
:: BYTE VAR BROW [��ப� �⮡ࠦ����  ��⥬� ���᫥���]

[����ணࠬ�� ����䨪�樨 ��⥬� ���᫥���]
:: : BASEMOD [] BL 1+ 3 & C ! BL [i] BR 0 2 1 8 2 10 ELSE 16 ! BASE SHOWBASE ;

   [����� ����䨪�樨 ��⥬�]
   : INFBASE [i] BR 0 "BIN" 1 "OCT" 2 "DEC" ELSE "HEX" [Alr,Dl] ;
:: : SHOWBASE CLFON 15 BROW BCOL
     BASE BR 2 0 8 1 10 2 ELSE 3 C ! BL INFBASE CLTOS ;

FIX 50 STRING FILEDIA [��� 䠩�� �����ᥬ���஢����] "DIAS-MEM.TXT" ! FILEDIA
:: : MODDIAF EON MESC NOP
     ON ?SPALL 3 '' INFDIAF 21 15 1 50 GENVVOD FILEDIA [] ;
     : INFDIAF D "������ ��� 䠩�� ��⮪��� �����ᥬ���஢����:" ;

: INSSDIA OPENDIA OUDIAS CLOSE CH ;
  : OPENDIA ON NOF SOZDAT FILEDIA CONNECT CH OPEN CH LENB CH SPOS CH ;
  : SOZDAT WOPEN CH ;
  : OUDIAS 0 ' VVB 40 C2 C2 + E2 DO ISCSYMV C2 - 1+
    OS CH 2573 OW CH COMLEN ADRTEK NACHALO - + DETEC IF0 1+ DVIGAEM ;
    : ISCSYMV C @B #  = EX0 1- ;

B16
: AVTDIAS OPENDIA RP ONEDIAS CLOSE CH ;
  : ONEDIAS OUDIAS ADRTEK ADREND 1- > EX+ INFDIAS TTI IF+ ??ESC ;
    : ??ESC TRB 1B = EX+ [] ;

MENUINIT UNDEF
10 C [1 - �⫠���\ 0 - ࠡ���]
:: : STT "DUMP ���� ।����" BBUF C ! NACHCOD KBUF 5 CT C3 - NDUMP ;
[:: : STTN "DUMP ���� ।����" BBUF C ! NACHCOD KBUF 0 NDUMP ;]
%FI
NOT %IF CLEAR $DUMP %FI
CLEAR $DUMP
MENUINIT

PROGRAM $SCNR B10 ."
�����p ��᪮�.                     ��p�� 1.1  10 ���p�   1997."

           30 VALUE FPP      [����稭� ����㯠 �� ������ ���]
           CHANNEL  FD       [����� ��⮪��� ᪠��஢����]
:: FIX    BYTE VAR  XSCAN    [���न��� � ���� ᪠���]
:: FIX    BYTE VAR  YSCAN    [���न��� Y ���� ᪠���]
:: FIX    BYTE VAR  WEISCAN  [��ਭ� ���� ᪠���]
:: FIX    BYTE VAR  CLFONSC  [���� ���� ᪠���]
:: FIX    BYTE VAR  CLTXTSC  [���� ���� ᪠���]
:: FIX    BYTE VAR  LENTXT1  [����� ����� 䠩��]
:: FIX    WORD VAR  FILENUM  [����� 䠩������ ⮬�] 0 ! FILENUM
:: FIX    BYTE VAR  LENFDIR  [����� ����� ᪠��஢����]
:: FIX    BYTE VAR  ?IZMLAB  [����襭�� �� ������ ��⪨ ��᪠]
:: FIX    BYTE VAR  FIXFILES [�ਧ��� ����祭�� � ᯨ᮪ 䠩��� ���ᠭ��]
:: FIX    BYTE VAR  FULLOUT  [�ਧ��� ������� �뢮�� ���ᠭ�� 䠩��]
:: FIX    BYTE VAR  BLNTREE  [�ਧ��� �뢮�� �ॢ�������� ������] !0 BLNTREE
          WORD VAR  FLAGOPEN [�ਧ��� ������ 䠩�� ��⮪���] !0 FLAGOPEN
          LONG VAR  VOLDIR   [���������� ��饣� ���� 䠩��� ��४��]
          WORD VAR  LEVEL    [��p������� ����p� �p����]
      120 BYTE VCTR TREE     [���ᨢ ���⮯��������]

        14 STRING TIMES   [���ᨢ ���� ��� p�ᯥ�⪨ ���祭�� TIME]
       255 STRING PATHOLD [����� ��� ��࠭���� ��� �室�]
:: FIX  40 STRING PROTFIL [��� 䠩�� �p�⮪���]
:: FIX  40 STRING NSCNDSK [��ப� � ����஬ ᪠���㥬��� ��᪠]
:: FIX 255 STRING FSCNDIR [���ᨢ ����� ᪠��஢���� ��४�ਨ]
:: FIX  13 STRING SYSLAB  [���ᨢ ����� ����� ��⪨ ⮬�]

   "SCANDIR.FLS" ! PROTFIL  "LNM VOLIUME" ! SYSLAB
   "DIR.TXT"     ! FSCNDIR

:: : DIRF [] S( BASE@ ) B10 ON ?SJust 1 GCDIR ! PATHOLD
     [���࠭��� ���ன�⢮ � ���� � ���ண� �ந������ �室 � ᪠���]
     EON MESC DELWBACK ON MF10 EX ON ?SPALL 4 !0 FLAGOPEN OPENPROT
     '' PROGINFO 6 C 4+ ! YSCAN 46 C ! XSCAN 10 29 C ! WEISCAN GENINF
     CLFON ! CLFONSC CLTXT ! CLTXTSC
     ON ?SPALL 23 ON KEYPRESS IZMNOM? OBRDISK DELWBACK [] ;
     : DELWBACK [DELWIND] CLOSEALL PATHOLD 1- CDIR SCANSUBS ;

     : OPENPROT [] !1 FLAGOPEN EON NOF OBNOVL
       PROTFIL CONNECT FD OPEN FD [] ;
       : OBNOVL [] WOPEN FD ;

[����⮢� �p�� �p� ����䨪�樨 ���p���⢠]
: INFSUBS [i] PUSH "A" POP #@ + C3 !TB [Alr,Dl] ;

       : SCANINFO BR 1 "�����p����" 2 "���  �����" 3 NOMFTOM
         4 WAITING ELSE EMSTR ;
         : WAITING 13 ! CLFON "���� !!!" ;
         :: : NOMFTOM FILENUM 1+ FIN ;

      : PROGINFO
        BR 1 "�������� ᪠��p������"
           2 "��p������ ���ன�⢠"
           3 "��������� � 䠩�"
           4 PROTFIL
           5 PREDISK
           6 "�롥p�� ���p���⢮"
           7 "��� ᪠��p������"
        ELSE EMSTR ;
        : PREDISK "  ��� ��� ����� -       "
          "    " C4 20 + !SB NOMFTOM E2 C2 5 CT 17 + !SB D ;
        : IZMNOM? INKEY BR 18432 INCNOM 20480 DECNOM ELSE MUSIC ;
          : INCNOM                !1+ FILENUM SHOWDISK ;
          : DECNOM FILENUM 1- 0 MAX ! FILENUM SHOWDISK ;

: MODSUBS [i] EON DERR PNOFIND
  C 1- ! NUMDRIVE BR 1 "A:\" 2 "B:\" 4 "D:\" 5 "E:\" ELSE "C:\" WORKINF
  CDIR [�����﫨 �p��� � ��⠭����� ��p����� ��⠫��] OTRAB DELWIND SHOWDISK ;
: OBRDISK [i] EON DERR REACTION EON MESC PREKRATIT MENDRIVE "." CDIR
  [�����﫨 �p��� � ��⠭����� ��p����� ��⠫��] OTRAB DELWIND SHOWDISK ;
  : SHOWDISK CLFONSC CLTXTSC YSCAN XSCAN PREDISK WEISCAN CJUST CLTOS [] ;
  : WORKINF ON ?SJust 1 ON ?SPALL 10 '' SCANINFO 11 51 4 15 GENINF ;
  : PREKRATIT DELWIND CLOSEALL ;

DEFINE? MENDRIVE NOT %IF
: MENDRIVE [i] '' MODSUBS '' INFSUBS 14 49 5 3 1 GENMEN3
  C 1- ! NUMDRIVE BR 1 "A:\" 2 "B:\" 4 "D:\" 5 "E:\" ELSE "C:\" WORKINF ;
                     %FI

: MUSIC MUSIC1 [] ;
  : MUSIC1 1234 23 BEEP 2345 45 BEEP 3456 23 BEEP [] ;

B16
: OTRAB ON ?SPALL 1 SCANFIX [] 4 DO MUSIC ;
  [�����p������ ���p���⢠ � �뢮� p�����⮢ ᪠��p������ � 䠩�]
  : SCANFIX [] S( L ) FLAGOPEN IF0 UOPENPROT
    [��p뫨 䠩� ��� ����� ��� ᮧ���� ��� �᫨ ���������]
    SCANSUBS [���뢠�� ���p���� FAT, �p�p㥬 � �뢮���]
    LENB FD C IF+ PROVEOF SPOS FD
    [�᫨ � ���� ᨬ�� ���� 䠩�� ���p��� ���]
    OPROSDRV !1 FLAGOPEN [] !0 VOLDIR
    " ᢮����� - " FTOS FREEVOL FIN [VALSKSS] FTOS FCR FCR [�뢥��]
    IZVLDESCR
    !0 LEVEL DIRFIX [�뢥�� ��⠢ ��p�� � �㦭� ��� �� ��������] TREESCAN
    [����p� �� �뢥�� ��� ���p���� � 䠩� � ����� ���p��� ������] ;
    : PROVEOF C 1- SPOS FD IB FD 26 = IF+ -1 [] ;
    : OPROSDRV [] FCR FCR "***** ��� ����p - " FTOS !1+ FILENUM
      FILENUM FIN [VALSKSS] FTOS " *****" FTOS [ RBOOT [] ;

[��稭��� � ��p����� ��p���p�� ���p���⢠.
 ����砥� p������� ᪠��p������ � ����� ��᪠����
 �� ��p��� ᪠��p������ ���p���⢠
 ��� ��᫥������� ���⮯�������� �� ��p��� ������ ���ᨢ TREE
 ������樨 �p���� �⭮�⥫쭮 ��p����� ��p���p��
 0-� ����� ���ᨢ� �p���� ����p �����p���p�� ��p������ ��⠫���
 � ���p�� �p�������� ��� �� ��p���
 1-� ����� �p���� ����p �����p���p�� � ��p���p�� 0-�� �����
 �� ���p�� �p�室�� ���쭥�訩 ��� ᪠��p�
 � ⠪ ����� ��� ��᫥����� ����⮢ � ��᫥����� �஢��� ]

  : TREESCAN [] ALPNAME E2D 3 = NOT
    [��p������� ��p����� ��⠫�� ��� ��� � � ᮮ⢥��⢨� � �⨬
     �뤠�� ��砫�� ����p ᪠��p������ ��⠫���]
    LEVEL ! TREE [����ᠫ� ��砫�� ����p ��� �p����] RP LEVELSCAN [] ;

  : DIRSCAN [] FREEVOL 4000 > BR+ YESSCAN ZAKONCH ;
    : YESSCAN S( L ) FSCNDIR CONNECT FD WOPEN FD UOPENPROT [RBOOT]
      3 ! LEVEL !0 VOLDIR DIRFIX FCR CLOSE FD [] SCANSUBS [] ;
      : DIRFIX BLNTREE IF+ PREDIRBLANK ALPNAME FTOS !0 L NOMER DO ALLVOLF
        NOMER 2 > IF+ SUBDVOL FCR !0 L NOMER DO OUTFILE ;
        [������ ��饣� ��ꥬ� ��४�ਨ]
        : PREDIRBLANK FPP LEVEL 50 MIN SHL - DO FSP F� LEVEL IF+ F�
          LEVEL 1- 50 MIN DO F� [] ;
        : ALLVOLF L SORT SIZEF VOLDIR + ! VOLDIR !1+ L ;

 : ZAKONCH ON ?SPALL 2 '' TEXTAVAR 10 40 4 34 GENINF TRB D [] ;
   : TEXTAVAR BR
     1 "�������� ��� ��������� �����������"
     2 "���������� ������� �� ����������"
     3 "���������� �����"
     4 "������ ������ ��������� ����� !!!"
     ELSE EMSTR ;

     : SUBDVOL FSP NOMER LEVEL IF+ 1- FIN [VALSKSS] FTOS "-䠩��� ��ꥬ�� -" FTOS
       VOLDIR FIN [VALSKSS] FTOS " ����" FTOS [] ;

    13 STRING TEKFNAME
    [����p������ ᯥ�䨪��p� ��� FAT � 䠩��]
    : OUTFILE [] L SORT ATRIBUT 10 & IF0 OUTOPIS !1+ L [] ;
      : OUTOPIS FNAMNORT ! TEKFNAME
        0 0 REGDESCR DO ?ISDESCR D IF0 YESOUTOPIS ;
        : ?ISDESCR [Prizn,I] C FDESCR SCMP TEKFNAME BR0 FINDESCR 1+ [Pr,I] ;
          : FINDESCR [Pr,I] E2 T1 E2 [1,I] ;
        : YESOUTOPIS BLNTREE IF+ PREBLANK
          FNAMNORT FULLOUT IF+ OUTEXTEND
          LENDESCR IF+ ISKFDESCR C 0D MAX LJUST [FNAMNORT E2D]
          FULLOUT IF+ IZMOPIS FTOS FCR [] ;
          : PREBLANK FPP LEVEL 50 MIN SHL - DO FSP LEVEL 1+ 50 MIN DO F� [] ;
          : OUTEXTEND SPECFILE FTOS ;
          [��⠥� �� � ������ ��� �஡����]
          : IZMOPIS [Adr,Dl] #  C3 C3 SRCHB [Adr,Dl,Sm]
            C2 C2 = BR+ MINUS0D UKOROT ;
            : MINUS0D [Adr,Dl,Sm] D E2 0D + E2 0D - [Adr,Dl] ;
            : UKOROT [Adr,Dl,Sm] E3 C3 + E3 - [Adr,Dl] C DO IUBRSP [Adr,Dl] ;
              : IUBRSP [Adr,Dl] C2 1+ @B #  = BR+ E21+E2 EX [Adr,Dl] ;
                : E21+E2 E2 1+ E2 ;

        : ISKFDESCR [Adr,Dl] C2 C2 ! TIMES 0A 0 SINS TIMES
          TIMES BBUF LENDESCR C3 C2 < BR+ SOVP T0 [1/0]
          BR+ ZAMNADESCR ISKFBIG DDD [Adr,Dl] ;
          : ISKFBIG D TOBIGL TIMES BBUF LENDESCR C3 C2 < BR+ SOVP T0 [1/0]
            IF+ ZAMNADESCR [Adr,Dl] ;
          : ZAMNADESCR [Adr,Dl,Adr,Dl,FAdr] 0D C2 100 SRCHB [LenDescr]
            NACHBDESCR LENDESCR + C3 - MIN [Adr,Dl,Adr,Dl,FAdr,LenDescr]
            5 ET D 5 ET [Adr,Dl] ;
          [��p�����p���� � ����訥 �㪢�]
          : TOBIGL [ADR,DL] DO BPERECOD D [] ;
            : BPERECOD [ADR] C @B C #a #z SEG IF+ BMASK C2 !TB 1+ [ADR+1] ;
              : BMASK 0DF & ;
          : F� " �" OS FD ;
          : F� " �" OS FD ;
          : F� "Ĵ" OS FD ;

    [�����p����� ⥪�饣� �p����]
    : LEVELSCAN []
      NOMER IF0 EXOTUP
      TTI IF+ PROVESC
      NOMER LEVEL IF+ 1- LEVEL TREE = [�� �� 䠩�� � ��४�ਨ ��ࠡ�⠭�]
      NOMER 1 = []
      LEVEL NOT NOT & & IF+ EXOTUP [��p���p�� ����]
      LEVEL TREE SORT ATRIBUT 10 & NOT NOT [����稫� ��p���� ᫥���饣� 䠩��]
      LEVEL &0 IF0 EXOTUP

      LEVEL TREE SORT ATRIBUT [����稫� ��p���� ᫥���饣� 䠩��]
      10 & NOT NOT [1-dir]
      NOMER 1 = NOT [directory is empty] &
      NOMER LEVEL TREE = NOT [all files is worked] &
      BR0 UPTREE DWNTREE
      [�᫨ �� ���⮩ 䠩� �,
      �����⢫���� ��室 �� ����� ��᮪�� �p����� ��p���,
      ���� �� ��p���p�� � �㦭� ���� � ��� ᤥ��� �����⮢��] ;
      : PROVESC TRBMou 11B = IF+ MESC ;

      : DWNTREE [] LEVEL TREE ! L !1+ LEVEL 1 LEVEL ! TREE
        [�� ��p��� �� 㦥 ���⨫���]
        FNAMNORT SADD PATHNAME []
        PATHNAME [Adr,Dl] E2D C DO DELSPST D
        PATHNAME CDIR SCANSUBS IZVLDESCR
        [����p� �� ���⨫��� �� ��p��� �� 1 �p����� � �p�᪠��p�����
         ��p���p��, ����� �㦭� �뢥�� � 䠩� �p�⮪��� � ����� �믮�����
         ������ ᮤ�p������ ����� ��p���p��]
        [BLNTREE IF+ PREBLANK FCR] DIRFIX [] ;
        : DELSPST [i] 1- C ST@B PATHNAME #  = IF+ SDEL#SP [i] ;
          : SDEL#SP [i] C SDEL PATHNAME [i] ;

      [�믮������ ����� 䠩� �� ���� ��p���p���]
      [�㦭� ��� � p���⥫�᪨� ��⠫�� � 㢥����� ����p ᪠��p������ �� 1]
      : EXOTUP [] UPTREE EX [] ;
        : UPTREE LEVEL IF+ DECREASE [] ;
        : DECREASE UPSUB !1- LEVEL LEVEL TREE 1+ LEVEL ! TREE [] !0 VOLDIR ;

    : UOPENPROT [] FCR "���  ᪠��p������ - " FTOS
      DATE 2 FTON#0 #: OBF 2 FTON#0 #: OBF 4 FTON#0 FCR
      "�p��� ᪠��p������ - " FTOS
      TIME 2 FTON#0 #: OBF 2 FTON#0 #: OBF 2 FTON #: OBF 2 FTON#0 FCR [] ;
    : NOTFOUND FCR "  ������ �� �������" FTOS [] FCR ;

B10
: FSP [] #  OB FD [] ;
: FCR [] 2573 OW FD [] ;
: FTOS [Adr,L] OS FD [] ;
: FTON [N] TON0 DO OBF [] ;
: FTON#0 [N] TON0 DO OBFSP [] ;
  : OBFSP [B] C #  = IF+ ZAM#0 OBF ;
    : ZAM#0 [B] D #0 ;
    : OBF [B] OB FD [] ;

[����樨 �� p����筮�� p��� �訡�� �p� p���� � 䠩����� ��⥬��]
: REACTION K_FS_M BR 2 FNOFIND 3 PNOFIND  14 BADSUBS  15 NOREADY
  ELSE NOTHING ;

[����������� ����]
: PNOFIND '' PSTINFO 17 29 PATHNAME E2D 13 MAX 2 / D - 3 13 VYDINF ;
  : PSTINFO BR 1 "� � � �"  2 PATHNAME  3 "�� ������" ELSE EMSTR ;
    : FNPPRSCH FPPRS CH FNPRSCH E2D + ;


[���� �� ������]
: FNOFIND '' STRINFO 17 29 MAXLSTR 3 13 VYDINF ;
  : STRINFO BR 1 "� � � �"  2 FNPPRSCH 3 "�� ������" ELSE EMSTR ;
    : FNPRSCH FNPRS CH ;

: VYDINF FNPPRSCH E2D MAX ON ?SPALL 2 6 + GENINF 2 DO ZVOOK TRB D DELWIND
  DELWIND ;
  : MAXLSTR FNPPRSCH E2D 13 MAX 2 / D - ;

[����p������ � ���p���⢮�]
: BADSUBS "���p���⢮ ����� ��䥪�!" InfErr ;
[���p���⢮ �� ��⮢�]
: NOREADY "���p���⢮ �� ��⮢�!"    InfErr ;
[�������� ����]
: NOTHING "�� � � ���p���⢮�!"    InfErr ;

BYTE VAR  NUMDRIVE 2 ! NUMDRIVE
0 %IF
B10 512 BYTE VCTR WBOOT [] B16
[��⠥� ������ ��������]
: RBOOT []
  [int 25h
   AL   - ����� ���ன�⢠
   CX   - �᫮ ᥪ�஢ � ����樨
   DX   - ��砫�� ᥪ�� ����樨
   DS:BX- ����� ���� ����]
  [ ?IZMLAB ?PROTECT & IF+ CHLABEL ]
  ALPNAME D @B #A - ! NUMDRIVE
  REDBOOT
  0 ' WBOOT [Adr] B16
  "��਩�� ����� ��᪠ - " FTOS
  C 02A + 2 DO INCOBF #- OBF 2 DO INCOBF D B10
  "  ��⪠ ��᪠ - " FTOS 2B + 0B FTOS FCR []
  "   ��騩 ���� ��᪠ - " FTOS
  1B00 ! RAX 21 INTERR RDX RCX RAX 0FF & * * [������ ��᪮��� ��-��]
  FIN [VALSKSS] FTOS [] ;
  : INCOBF [Adr] C @B 2 TON0 DO OBF 1- [Adr-1] ;


CODE REDBOOT
53 ,B              [push    bx      ; ���࠭塞 �ᯮ��㥬� ॣ�����]
51 ,B              [push    cx      ;]
0A0 ,B ' NUMDRIVE ,
                   [mov al,(numdrive)   ; ������ ����� ���ன�⢠]
0BA ,B 0000 ,      [mov     dx,0    ; ����� ᥪ�ࠤ�� �⥭��]
0B9 ,B 0001 ,      [mov     cx,1    ; ��᫮ �⠥��� ᥪ�஢]
1E8D , 0 ' WBOOT , [LEA bx,advvod   ; ����㧨�� 㪠��⥫� �� ����]
0CD ,B 25 ,B       [int     25h     ; ���뢠��� �⥭�� ��᮫�⭮�� ᥪ��]
5A ,B              [pop     dx      ; ����⠭����� �⥪]
59 ,B              [pop     cx      ; ���⠭���� �ᯮ�祭�� ॣ�����]
5B ,B              [pop     bx      ;]
,NEXT

CODE WRIBOOT
53 ,B              [push    bx      ; ���࠭塞 �ᯮ��㥬� ॣ�����]
51 ,B              [push    cx      ;]
0A0 ,B ' NUMDRIVE ,
                   [mov al,(numdrive)   ; ������ ����� ���ன�⢠]
0BA ,B 0000 ,      [mov     dx,0    ; ����� ᥪ�ࠤ�� �⥭��]
0B9 ,B 0001 ,      [mov     cx,1    ; ��᫮ �⠥��� ᥪ�஢]
1E8D , 0 ' WBOOT , [LEA bx,advvod   ; ����㧨�� 㪠��⥫� �� ����]
0CD ,B 26 ,B       [int     26h     ; ���뢠��� ����� ��᮫�⭮�� ᥪ��]
5A ,B              [pop     dx      ; ����⠭����� �⥪]
59 ,B              [pop     cx      ; ���⠭���� �ᯮ�祭�� ॣ�����]
5B ,B              [pop     bx      ;]
,NEXT

'' DIRF ! FUNCTION

[�஢�ઠ ����� �� ��� �� �����]
: ?PROTECT [] 0 EON DERR T1 "00000000.XXX" CONNECT CH WOPEN CH CLOSE CH
  "00000000.XXX" DELFILE [0] ;

B16
: CHLABEL [] NEWLAB [WRIBOOT] ["C:\SYS\LABEL.EXE" EXEFLCL] [] ;
  : NEWLAB SYSLAB 0B LJUST NOMFTOM B10 4 TON0
    8 CT 8 CT + E2 DO TOLABSTRN D 2B ' WBOOT !SB [] ;
    : TOLABSTRN [B,ADR] E2 C2 !TB 1- [ADR'] ;
%FI

0 %IF
B10
: FIG S( BASE@ ) B10 1 "BC31-D00\" 20 DO INCMDIR DDD ;
  : INCMDIR [N,A,D] C3 2 TON0 D C #  = IF+ ZAMNA0
    C4 6 + !TB C3 7 + !TB C2 C2 MKDIR D E3 1+ E3 ;
    : ZAMNA0 D #0 ;
  %FI
MENUINIT
UNDEF
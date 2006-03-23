\ 1999 - Loktev N M ����⮢�� ��� ᮧ����� ��p��� y��p����

anew task-pr.fth
decimal

variable selfownadr  \ ����⢥��� ��p�� ��� y������� ��⮬���
variable parentadr   \ ��p�� p���⥫�
variable dataadr     \ ��p�� ᯨ᪠ ᮡ�⢥���� ������
variable propertyadr \ ��p�� ᯨ᪠ ᢮���
variable nameadr     \ ��p�� ��p��� � ���稪�� ����� ����


\ �p� �믮������ ������ y����⥫� � ��p������
: makenode ( string, ptr, ptr, ptr ptr ---  )
  create , , , , , ( �������� ��砫� ��p���p��p������� ������ )
  does>     ." PFA=" dup .hex dup
            ." CFA=" body> dup .hex
            ." NFA=" >name dup .hex
            ." LFA=" n>link dup .hex
            drop

            dup @ parentadr   !
        cell+ dup @ dataadr     !
        cell+ dup @ propertyadr !
        cell+ dup @ selfownadr  !
        cell+     @ nameadr     !
;

c" Master node" latest 0 0 0 makenode root

: makeproperty ( " name" N-bytes for data ---  )
  create 0 , ( ���� ��� ��p�� �p���y饣� �-�� )
         0 , ( ���� ��� ��p�� ᫥�y�饣� �-�� ��� 0 )
         0 , ( ���� ��� ��p�� �p��-p� ����. �-�� )
         0 , ( ���� ��� ��p�� �p��-p� ����� �-�� )
         dup c, allot ( --- )
  does>  ( --- pfa ) cell 2 lshift + ( �p��y�⨫� ��p-p� )
         dup c@ swap 1+ swap ( --- adr, dl )
;


31 makeproperty name
20 makeproperty datecreation
 4 makeproperty version


: parent ( AdrStrC --- ) find ( pfa --- ) bl word find ( pfa pfa --- )

;

: child ( AdrStrC --- ) find ( pfa --- ) bl word find ( pfa pfa --- )

;



variable instflag       \ Flag of instance var
variable pack-data-base \ Adress of corunt packet's
variable lastinst	\ Adress of previos instance var

: instance 1 instflag ! ;
: instvariable 
  latest
  create 0 , \ make header and place 0 value
  4 , \ Lenght of data
  lastinst , \ Pointer for last instance var
  pack-data-base - , \ Offset of var from pack-data-base
  latest lastinst !
  0 instflag !
  does> ( pfa -- )
  cell+
  cell+
  cell+ @ pack-data-base @ +  ( --- adr )
;

\ Definition of instance var
: var instflag @ if instvariable else variable then
;

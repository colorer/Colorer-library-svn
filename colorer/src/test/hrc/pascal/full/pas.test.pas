(***************************************************
ssssssssssssssssssssssssssssssssss
ssssssssssssssssssssssssssssssssss
***************************************************)

******* �������� � test.pas ******* -----------------------------8<------------

  if Pos(ReplaceDialog1.FindText,  Memo1.Text) <> 0 then


program Test_Colors;

{
�������� � ������ ����������������� ���� Delphi (Pascal.Hrc):
  cdecl
  stdcall
  pascal
  register
  safecall
  default
  nodefault
  message
  read
  write
}

{ ������� � asm-������ � Pascal'e
  ����, ������ ��� ��� Borland �������� ���������! :-) }
procedure TestAsmBlock1; {�������� asm-�����}
begin
  asm
   loop1:   {��� �����}
   @@loop2: {��� �����}
   @loop:   {��� ���� �����, � ������ �� ����������}
;      inc dx      {��� ������ �� ���������������, � ������ ����������}
                   {����� ';' - "������" ��������}
//      inc dx      ��� ������ ���������������, ������ �� ����������
    mov ah, 1       {��� ���������� � asm-�����, ������ �� ����������}
    int 16h
    or ah, ah
    je @end         {����� '@end' �����, � �� ����������������� �����}
    jmp @loop
   @end:            {��� �� �� �����}
    mov Variable, 2
  end;
  WriteLn(Variable);
end;

procedure TestAsmBlock21; {��� ���������}
begin
  if Variable = 1 then
  begin
    Variable := 0;
    asm
     @loop:
      mov ah, 1; int 16h;  {��� ���� ����� ������, � ���������� �����������}
      or ah, ah
      je @end
      jmp @loop
     @end:
    end            {����� ����� 'end' - ';' �� �����������, �� ��� ����� �����}
  end else         {��� ����� ��� �� asm-����}
    Variable := 1;
  Inc(Variable); WriteLn(Variable);
end;

procedure TestAsmBlock22; {�� �� ���������� � ��������� ����������}
begin
  if Variable = 1 then
  begin
    Variable := 0;
    asm
     @loop:
      mov ah, 1
      int 16h
      or ah, ah
      je @end
      jmp @loop
     @end:
    end           {��� ���� ����� �����}
  end else
    Variable := 1;
  Inc(Variable); WriteLn(Variable);
end;

begin
  Ch := #32;  {��� ������ �������}
  Ch := #$20; {��� ���� ������ �������, �� �� �� ���������}
  f := 4.93e33;
  i := $3fa33;
  i :=$3fa33;   // ? :=
  i := 23..33;
  i := 984;
  5.4 0.  $3Fg
  5.4 0.
  safecall 23
  // from tst.asm
  1.  .1
  1. .1         // <?
  1..1
  1. .....1...1
  0...1e2
  1.1.1e-1
  ).3
.3 .3()
3.
3.(dfd)
    3e3
end.



// Tested on Colorer 2.8

//  ���� � "end"

 begin

    asm
        xor eax,eax
        // sdffffdsfdfds
 end;
  //
 end;
  //

begin
 asm
     xor ax,ax
     //  xor ax,ax
 end;
end;


// � ����� ��� ��. ����� �� ����������� �� ������� ( ��� ������ "end" �����/������ ).

 begin

    asm
        xor eax,eax
        // sdffffdsfdfds
  end;

 end;


begin
 asm
     xor ax,ax
     //  xor ax,ax
  end;
end;

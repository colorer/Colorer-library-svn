******* �������� � test.asm ******* -------------------------------8<----------
; lala
;��������� ������ � Pascal, Assmebler, C � �.�. ���������� ��� �����!
1..1
1......1...1
0...1e2
1.1.1e-1

%macro lala
 safdsafd 234
%endmacro

;�������� � ������ ����������������� ���� ���� �� ��� �������� (Asm.Hrc):
codeseg
dataseg
startupcode
org
uses
returns
ifdef
ifndef
else
endif
model
struc
ends
union
******* �������� � test.asm ******* -------------------------------8<----------

;��������� ������ � Pascal, Assmebler, C � �.�. ���������� ��� �����!
1..1
1......1...1
0...1e2
1.1.1e-1

;�������� � ������ ����������������� ���� ���� �� ��� �������� (Asm.Hrc):
codeseg
dataseg
startupcode
org
uses
returns
ifdef
ifndef
else
endif
model
struc
ends
union

; ------------------------------------------------------------------------------
;   64-bit shift left
; ------------------------------------------------------------------------------

;
; target (EAX:EDX) count (ECX)
;
__llshl     PROC    near

        cmp     cl, 32
        jl      @__llshl@below32

        cmp     cl, 64
        jl      @__llshl@below64

        xor     edx, edx
        xor     eax, eax
        ret

@__llshl@below64:

        mov     edx, eax
        shl     edx, cl
        xor     eax, eax
        ret

@__llshl@below32:

        shld    edx, eax, cl
        shl     eax, cl
        ret

__llshl     ENDP


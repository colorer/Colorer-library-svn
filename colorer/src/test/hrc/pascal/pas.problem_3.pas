// ��� �奬�: ...\hrc\main\pascal.hrc
//
// �஡����. ���祢�� ᫮�� inherited �� �뤥���� ��� ���祢��.
// ����� , �� �஡���� � ��ࠬ����� ᪮����...
//
class function F(x: Integer): Integer;
function G(x: Integer): Integer;
class procedure P(x: Integer);
procedure Q(x: Integer);

function TRect.LoadFromStream(S: IStream): TBool;
begin
    Result := ( inherited LoadFromStream(S)) and  // Problem !!!
              //^^^^^^^^^
              A.LoadFromStream(S) and B.LoadFromStream(S)
    ;
end;


function TRect.LoadFromStream(S: IStream): TBool;
begin
    Result := inherited LoadFromStream(S) // OK !!!
    ;        //^^^^^^^^
end;


unit MyClasses;

interface

uses
  Classes, Tags;

{
  ���� � ��᪠�� �.�. ����ᮢ� �㭪樨/��楤��� � �����
   ��ࠡ�⪨ �᪫�祭��. ��� ��� ��� ����୮ ���ᢥ稢�����
   (������� � ���� ����������� ���ᢥ⪨ ����� ᪮���/���)
   ������ ᥩ�� ᠬ ࠧ�������, ��� ����� ������஢���
   hrc, �� ���� ������� ����.
}
(*
sdfsadf
*)

(*$I+*) //!! <-- ����, �� ⮦� ��४⨢� ���������. ��� �
{$I+}
        //       � ��� �� ⮦� ������ ���� �� 㤠����.

type         //�� !!���ᢥ稢����� ᠬ �� ᥡ�, ��� ����
  TClass1 = class(TMyObject)
    class function TagSupported(const Tag: TagType): Boolean;
  end;//��� !!���ᢥ稢����� � ���

function CopyList(List: TList): TList;

implementation//<��� !!���ᢥ稢����� ᠬ �� ᥡ�, ��� ����

uses
  SysUtils;

function CopyList(List: TList): TList;
var
  i: Integer;
begin//<������������������������������������������������������������������Ŀ
  Result := TList.Create;                       //                         �


  try
     try
     except
     end

  except
  end

  try //�������� !!����. � �>�������������������������������������Ŀ      �
    if List <> nil then                         //                  �      �
      for i := 0 to List.Count - 1 do           //                  �      �
        Result.Add(TMyObject.CreateAssign(TMyObject(List[i])));//   �      �
  except                                        //                  �      �
    Result.Free;                                //                  �      �
    raise;                                      //                  �      �
  end;                                          //                  �      �
  Result.Capacity := Result.Count;              //                  �      �
  //       �������������������� !! � �� - ��� ���� ���ᢥ��     �      �
  //                   ���� � ����� � ��⮤�� ��ꥪ⮢ � ����ᥩ?  �      �
  //                   � �譮� ⥪�� ⠪��� ���...                �      �
end;//<��������������������������������������������������������������      �
//�������������� !!����. � �>����������������������������������������������

{ TClass1 }

//�����<�� !!����. � ��>��������������������������������������������Ŀ
class function TClass1.TagSupported(const Tag: TagType): Boolean; //  �
begin                                                             //  �
  Result := TagIn(Tag, [TClass1Tag, TClass11Tag]);                //  �
end;                                                              //  �
                                                                  //  �
initialization //<���� !!����� �� ���ᢥ稢�����                 //  �
  RegisterClass(TClass1);                                         //  �
finalization //<���� !!����� �� ���ᢥ稢�����                   //  �
  UnRegisterClass(TClass1);                                       //  �
end.//��<�� !!����. � �>����������������������������������������������

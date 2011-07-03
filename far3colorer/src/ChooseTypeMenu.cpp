#include "ChooseTypeMenu.h"

ChooseTypeMenu::ChooseTypeMenu(const wchar_t *AutoDetect,const wchar_t *Favorites)
{
  ItemCount = 0;
  Item = nullptr;
  ItemSelected = 0;

  StringBuffer s;
  s.append(DString("&A ")).append(DString(AutoDetect));
  AddItem(s.getWChars(), 0, NULL,0);
  AddItem(Favorites, MIF_SEPARATOR, NULL, 1);
}

ChooseTypeMenu::~ChooseTypeMenu()
{
  for (size_t idx = 0; idx < ItemCount; idx++){
    if (Item[idx].Text){
      free((void*) Item[idx].Text);
    }
  }
  free(Item);
}

void ChooseTypeMenu::DeleteItem(size_t index)
{
  if (Item[index].Text){
    free((void*) Item[index].Text);
  }
  memmove(Item+index,Item+index+1,sizeof(FarMenuItem)*(ItemCount-(index+1)));
  ItemCount--;
  if (ItemSelected>=index) ItemSelected--;
}

FarMenuItem *ChooseTypeMenu::getItems()
{
  return Item;
}

int ChooseTypeMenu::AddItem(const wchar_t *Text, const MENUITEMFLAGS Flags, const FileType* UserData, size_t PosAdd)
{
  if (PosAdd > ItemCount)
    PosAdd = ItemCount;

  if (!(ItemCount & 255))
  {
    FarMenuItem *NewPtr;
    if (!(NewPtr=(FarMenuItem *)realloc(Item, sizeof(FarMenuItem)*(ItemCount+256+1))))
      return -1;

    Item=NewPtr;
  }

  if (PosAdd < ItemCount)
    memmove(Item+PosAdd+1,Item+PosAdd,sizeof(FarMenuItem)*(ItemCount-PosAdd));

  ItemCount++;

  Item[PosAdd].Flags = Flags;
  Item[PosAdd].Text = _wcsdup(Text);
  Item[PosAdd].UserData = (DWORD_PTR) UserData;
  Item[PosAdd].Reserved = 0;
  Item[PosAdd].AccelKey = 0;

  return PosAdd;
}

int ChooseTypeMenu::AddGroup(const wchar_t *Text)
{
  return AddItem(Text, MIF_SEPARATOR, NULL);
}

int ChooseTypeMenu::AddItem(const FileType* fType, size_t PosAdd)
{
  const String *v;
  v=((FileTypeImpl*)fType)->getParamValue(DHotkey);
  StringBuffer s;
  if (v!=NULL && v->length()){
    s.append(DString("&")).append(v);
  }else{
    s.append(DString(" "));
  }
  s.append(DString(" ")).append(((FileType*)fType)->getDescription());
  return AddItem(s.getWChars(), 0, fType, PosAdd);
}

void ChooseTypeMenu::SetSelected(size_t index)
{
  if (index<ItemCount){
    Item[ItemSelected].Flags &= ~MIF_SELECTED;
    Item[index].Flags |= MIF_SELECTED;
    ItemSelected =index;
  }
}

int ChooseTypeMenu::GetNext(size_t index)
{
  size_t p;
  for (p=index+1;p<ItemCount;p++){
    if (!(Item[p].Flags &MIF_SEPARATOR)) break;
  }
  if (p<ItemCount){
    return p;
  }else{
    for (p=favorite_idx; p<ItemCount && !(Item[p].Flags&MIF_SEPARATOR);p++);
    return p+1;
  }
}

FileType* ChooseTypeMenu::GetFileType( size_t index)
{
  return (FileType*)Item[index].UserData;
}

void ChooseTypeMenu::MoveToFavorites(size_t index)
{
  FileType* f=(FileType*)Item[index].UserData;
  DeleteItem(index);
  size_t k=AddFavorite(f);
  SetSelected(k);
  HideEmptyGroup();
  if (f->getParamValue(DFavorite)==null){
    ((FileTypeImpl*)f)->addParam(&DFavorite);
  }
  f->setParamValue(DFavorite,&DTrue);
}

int ChooseTypeMenu::AddFavorite(const FileType* fType)
{
  size_t i;
  for (i=favorite_idx;i<ItemCount && !(Item[i].Flags&MIF_SEPARATOR);i++);
  size_t p=AddItem(fType,i=ItemCount?i:i+1);
  if (ItemSelected>=p) ItemSelected++;
  return p;
}

void ChooseTypeMenu::HideEmptyGroup()
{
  for (size_t i=favorite_idx;i<ItemCount-1;i++){
    if ((Item[i].Flags&MIF_SEPARATOR)&&(Item[i+1].Flags&MIF_SEPARATOR)) Item[i].Flags|=MIF_HIDDEN;
  }
}

void ChooseTypeMenu::DelFromFavorites(size_t index)
{
  FileType* f=(FileType*)Item[index].UserData;
  DeleteItem(index);
  AddItemInGroup(f);
  if (Item[index].Flags&MIF_SEPARATOR)  SetSelected(GetNext(index));
  else  SetSelected(index);
  f->setParamValue(DFavorite,&DFalse);
}

int ChooseTypeMenu::AddItemInGroup(FileType* fType)
{
  size_t i;
  const String* group =fType->getGroup();
  for (i=favorite_idx;i<ItemCount && !((Item[i].Flags&MIF_SEPARATOR) && (group->compareTo(DString(Item[i].Text))==0) );i++);
  if (Item[i].Flags&MIF_HIDDEN) Item[i].Flags &= ~MIF_HIDDEN;
  size_t k=AddItem(fType,i+1);
  if (ItemSelected>=k) ItemSelected++;
  return k;
}

bool ChooseTypeMenu::IsFavorite(size_t index)
{
  size_t i;
  for (i=favorite_idx;i<ItemCount && !(Item[i].Flags&MIF_SEPARATOR);i++);
  i=ItemCount?i:i+1;
  if (i>index) return true;
  return false;
}

void ChooseTypeMenu::RefreshItemCaption(size_t index)
{
  if (Item[index].Text){
    free((void*) Item[index].Text);
  }
  const String *v;
  v=((FileTypeImpl*)GetFileType(index))->getParamValue(DHotkey);
  StringBuffer s;
  if (v!=NULL && v->length()){
    s.append(DString("&")).append(v);
  }else{
    s.append(DString(" "));
  }
  s.append(DString(" ")).append(((FileType*)GetFileType(index))->getDescription());

  Item[index].Text= _wcsdup(s.getWChars());
}

BOOL SetClipData(void *Data, int Size)
{
HANDLE hGlob;
char *ptr;

  hGlob = GlobalAlloc(GHND|GMEM_DDESHARE, Size + 1);
  if (!hGlob) return FALSE;
  ptr = (char*)GlobalLock(hGlob);
  if (!ptr) return FALSE;
  MoveMemory(ptr,Data,Size+1);
  ptr[Size] = 0;
  GlobalUnlock(hGlob);
  if (OpenClipboard(NULL)){
    EmptyClipboard();
    SetClipboardData(CF_OEMTEXT,hGlob);
    CloseClipboard();
    return TRUE;
  };
  return FALSE;
};

BOOL GetClipData(void *Data, int Size)
{
HANDLE hGlob;
char *ptr;
  if (OpenClipboard(NULL)){
    hGlob = GetClipboardData(CF_OEMTEXT);
    ptr = (char*)GlobalLock(hGlob);
    if (!ptr) return FALSE;
    for(int i = 0;ptr[i] && i < Size;i++)
      ((char*)Data)[i] = ptr[i];
    ((char*)Data)[i] = 0;
    GlobalUnlock(hGlob);
    CloseClipboard();
    return TRUE;
  };
  return FALSE;
};

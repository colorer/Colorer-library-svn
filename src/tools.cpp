#include "tools.h"

wchar_t *rtrim(wchar_t* str)
{
  wchar_t *ptr = str;
  str += wcslen(str);

  while (iswspace(*(--str))) *str = 0;

  return ptr;
}

wchar_t *ltrim(wchar_t* str)
{
  while (iswspace(*(str++)));

  return str - 1;
}

wchar_t *trim(wchar_t* str)
{
  return ltrim(rtrim(str));
}

/* 
Function converts a path in the UNC path. 
Source path can be framed by quotes, be a relative, or contain environment variables 
*/
wchar_t *PathToFool(const wchar_t *path, bool unc)
{
  int len=wcslen(path);
  if (!len)
    return NULL;

  wchar_t *new_path = NULL;
  // we remove quotes, if they are present, focusing on the first character
  // if he quote it away and the first and last character.
  // If the first character quote, but the latter does not - well, it's not our 
  // problem, and so and so error
  if (*path==L'"')
  {
    len--;
    new_path=new wchar_t[len];
    wcsncpy(new_path, &path[1],len-1);
  }
  else
  {
    len++;
    new_path=new wchar_t[len];
    wcscpy(new_path, path);
  }

  // replace the environment variables to their values
  int i=ExpandEnvironmentStrings(new_path,NULL,0);
  if (i>len)
    len = i;
  {
    wchar_t *temp = new wchar_t[len];
    ExpandEnvironmentStrings(new_path,temp,i);
    delete[] new_path;
    new_path = temp;
  }

  // take the full path to the file, converting all kinds of ../ ./ etc
  int p=FSF.ConvertPath(CPM_FULL, new_path, NULL, 0);
  if (p>len)
  {
    len = p;
    wchar_t *temp = new wchar_t[len];
    wcscpy(temp,new_path);
    delete[] new_path;
    new_path = temp;
  }
  FSF.ConvertPath(CPM_FULL, new_path, new_path, len);

  if (unc)
  {
    // for normal work with long paths, the path must be converted to UNC
    if (wcsstr(new_path,L"\\\\?\\")==NULL){
      len+=4;
      wchar_t *temp = new wchar_t[len];
      wcscpy(temp,L"\\\\?\\");
      wcscat(temp,new_path);
      delete[] new_path;
      new_path = temp;
    }
  }

  // reduce the length of the buffer
  i = wcslen(new_path)+1;
  if (i!=len)
  {
    wchar_t *temp = new wchar_t[i];
    wcscpy(temp,new_path);
    delete[] new_path;
    return temp;
  }

  return new_path;
}


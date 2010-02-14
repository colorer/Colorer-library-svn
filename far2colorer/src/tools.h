#ifndef _TOOLS_H_
#define _TOOLS_H_

#include"pcolorer.h"

wchar_t *rtrim(wchar_t* str);
wchar_t *ltrim(wchar_t* str);
wchar_t *trim(wchar_t* str);

wchar_t *PathToFool(const wchar_t *path, bool unc);

#endif
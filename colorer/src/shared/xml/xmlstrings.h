
//  #123  #xABCD
bool getXMLNumber(const String &str, int *res)
{
int type, num;
int s, e, i, j, k;
long r;

  e = str.length();
  if (!e) return false;

  if (str[0] != '#') return false;

  s = 1;
  type = 0;

  if(str[1] == 'x'){
    s = 2;
    type = 1;
  };

  switch(type){
    case 0:
      num = 0;
      i = e-1;
      while(i >= s){
        j = str[i];
        if((j < '0') || (j > '9'))
          return false;
        j &= 15;
        k = e-i-1;
        r = (long)j;
        while(k){
          k--;
          r *= 10;
        };
        num += r;
        i--;
      };
      *res = num;
      break;
    case 1:
      num = 0;
      i = e-1;
      while(i >= s){
        j = str[i];
        if(((j < 0x30) || (j > 0x39)) &&
          ((j < 'a') || (j > 'f')) &&
          ((j < 'A') || (j > 'F')))
            return false;
        if (j > 0x60) j -= 0x27;
        if (j > 0x40) j -= 0x7;
        j &= 15;
        if(i > e-9)
          num |= (j << ((e-i-1)*4) );
        i--;
      };
      *res = num;
      break;
  };
  return true;
};

/** String with xml entities recognition.
    Extends next entities:
     - &amp;  -> &
     - &lt;   -> <
     - &gt;   -> >
     - &quot; -> "
     - &apos; -> '
    @ingroup xml
*/
class EntityString : public String{
protected:
  wchar *wstr;
  int len;
public:
  EntityString(const String *cstring, int s, int l){
    static struct {char ent[5]; char val;} entities[] = {
      {"amp", '&'},
      {"lt", '<'},
      {"gt", '>'},
      {"quot", '"'},
      {"apos", '\''}
    };
    if (l == -1) l = cstring->length();
    wstr = new wchar[l];
    int pos;
    for(pos = 0, len = 0; pos < l; pos++, len++){
      wstr[len] = (*cstring)[s+pos];
      if (wstr[len] != '&') continue;
      bool subst_done = false;
      for(int ep = 0; !subst_done && ep < sizeof(entities)/sizeof(entities[0]); ep++){
        int ws_ep = 0;
        while ((*cstring)[s+pos+1+ws_ep] == entities[ep].ent[ws_ep]) ws_ep++;
        if (entities[ep].ent[ws_ep] || (*cstring)[s+pos+1+ws_ep] != ';') continue;
        wstr[len] = entities[ep].val;
        pos += ws_ep+1;
        subst_done = true;
      };
      if(subst_done) continue;
      int ent_end = 0;
      while((*cstring)[s + pos + (++ent_end)] != ';' && ent_end < 20);
      int numval = 0;
      if (!getXMLNumber(DString(cstring, s+pos+1, ent_end-1), &numval)) continue;
      wstr[len] = (wchar)numval;
      pos += ent_end;
    };
  };
  ~EntityString(){
    if (wstr) delete[] wstr;
  };
  wchar operator[](int i) const{
    if (i >= len) return 0xFFFF; //!!!error
    return wstr[i];
  };
  int length() const{
    return len;
  };
  String *substring(int s, int l = -1) const{
    return new SString(this, s, l);
  };
};

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

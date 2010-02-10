#ifndef _COLORER_JSTRING_H_
#define _COLORER_JSTRING_H_

#include<unicode/String.h>

#define env_NewString(s) env->NewString((jchar*)s->getWChars(), s->length())

#define env_NewString_or_null(string) ((string == null) ? null : env_NewString(string))

class JString : public String{
public:
  JString(JNIEnv *env, jstring jdstring){
    this->env = env;
    this->jdstring = (jstring)env->NewGlobalRef(jdstring);
    jboolean copied;
    chars = env->GetStringChars(this->jdstring, &copied);
    len = env->GetStringLength(this->jdstring);
  };
  ~JString(){
    env->ReleaseStringChars(jdstring, chars);
    env->DeleteGlobalRef(jdstring);
  };

  int length() const{
    return len;
  };
  wchar operator[](int idx) const{
    if (idx < 0 || idx >= len) throw OutOfBoundException(StringBuffer("JString: ")+SString(idx));
    return chars[idx];
  };
  JNIEnv *env;
private:
  jstring jdstring;
  const jchar *chars;
  int len;
};

#endif
#ifndef _COLORER_JSTRING_H_
#define _COLORER_JSTRING_H_

#include<unicode/String.h>

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
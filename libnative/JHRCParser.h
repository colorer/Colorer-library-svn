#ifndef _COLORER_JHRCPARSER_H_
#define _COLORER_JHRCPARSER_H_

#include<common/Hashtable.h>
#include<colorer/HRCParser.h>

class JHRCParser{
public:
  HRCParser *hrcParser;
  Hashtable<jobject> regions;
  Hashtable<jobject> fileTypes;

  jobject getRegion(JNIEnv *env, const String *regname){
    jobject reg = regions.get(regname);
    if (reg == null){
      const Region *nreg = hrcParser->getRegion(regname);
      if (nreg == null) return null;

      jclass cRegion = env->FindClass("net/sf/colorer/Region");
      jmethodID idRegionConstr = env->GetMethodID(cRegion, "<init>", "(Ljava/lang/String;Ljava/lang/String;Lnet/sf/colorer/Region;I)V");

      reg = env->NewObject(cRegion, idRegionConstr,
                          env->NewString(nreg->getName()->getWChars(), nreg->getName()->length()),
                          nreg->getDescription() ? env->NewString(nreg->getDescription()->getWChars(), nreg->getDescription()->length()) : null,
                          nreg->getParent() ? getRegion(env, nreg->getParent()->getName()) : null,
                          nreg->getID()
                        );
      reg = env->NewGlobalRef(reg);
      regions.put(regname, reg);
    }
    return reg;
  };

  jobject enumerateFileTypes(JNIEnv *env, int idx){
    FileType *filetype = hrcParser->enumerateFileTypes(idx);
    if (filetype == null) return null;
    return getFileType(env, filetype);
  }

  jobject getFileType(JNIEnv *env, FileType *filetype){
    jobject jtype = fileTypes.get(filetype->getName());
    if (jtype == null){
      jclass cFileType = env->FindClass("net/sf/colorer/FileType");
      jmethodID idFileTypeConstr = env->GetMethodID(cFileType, "<init>", "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
printf("inn0\n");
printf("inn %d %d %d\n", filetype->getName()->length(),filetype->getGroup()->length(),filetype->getDescription()->length());
      jtype = env->NewObject(cFileType, idFileTypeConstr, filetype,
                          env->NewString(filetype->getName()->getWChars(), filetype->getName()->length()),
                          filetype->getGroup() ? env->NewString(filetype->getGroup()->getWChars(), filetype->getGroup()->length()) : null,
                          filetype->getDescription() ? env->NewString(filetype->getDescription()->getWChars(), filetype->getDescription()->length()) : null
                        );
      jtype = env->NewGlobalRef(jtype);
      fileTypes.put(filetype->getName(), jtype);
printf("out\n");
    }
    return jtype;
  }

};

#endif
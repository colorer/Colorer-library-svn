#ifndef _COLORER_JHRCPARSER_H_
#define _COLORER_JHRCPARSER_H_

#include<common/Hashtable.h>
#include<colorer/HRCParser.h>

class JHRCParser{
  Hashtable<jobject> regions;
  Hashtable<jobject> fileTypes;

public:
  HRCParser *hrcParser;
  jobject jHRCParser;

  jobject getRegion(JNIEnv *env, int index){
    CLR_TRACE("NSC:JHRCParser:getRegion", "index:%d, hp=%d", index, hrcParser);
    const Region *nreg = hrcParser->getRegion(index);
    if (nreg == null){
      return null;
    }
    CLR_TRACE("NSC:JHRCParser:getRegion", "region:%s", nreg->getName()->getChars());
    return getRegion(env, nreg->getName());
  }

  jobject getRegion(JNIEnv *env, const String *regname){
    jobject reg = regions.get(regname);
    if (reg == null){
      const Region *nreg = hrcParser->getRegion(regname);
      if (nreg == null) return null;

      jclass cRegion = env->FindClass("net/sf/colorer/Region");
      jmethodID idRegionConstr = env->GetMethodID(cRegion, "<init>", "(Ljava/lang/String;Ljava/lang/String;Lnet/sf/colorer/Region;IJ)V");

      reg = env->NewObject(cRegion, idRegionConstr,
                          env->NewString(nreg->getName()->getWChars(), nreg->getName()->length()),
                          nreg->getDescription() ? env->NewString(nreg->getDescription()->getWChars(), nreg->getDescription()->length()) : null,
                          nreg->getParent() ? getRegion(env, nreg->getParent()->getName()) : null,
                          nreg->getID(),
                          (jlong)nreg
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
      jtype = env->NewObject(cFileType, idFileTypeConstr, (jlong)filetype,
                          env->NewString(filetype->getName()->getWChars(), filetype->getName()->length()),
                          filetype->getGroup() ? env->NewString(filetype->getGroup()->getWChars(), filetype->getGroup()->length()) : null,
                          filetype->getDescription() ? env->NewString(filetype->getDescription()->getWChars(), filetype->getDescription()->length()) : null
                        );
      jtype = env->NewGlobalRef(jtype);
      fileTypes.put(filetype->getName(), jtype);
    }
    return jtype;
  }

  void finalize(JNIEnv *env){
      for(jobject region = regions.enumerate(); region != null; region = regions.next()) {
        env->DeleteGlobalRef(region);
      }
      for(jobject filetype = fileTypes.enumerate(); filetype != null; filetype = fileTypes.next()) {
        env->DeleteGlobalRef(filetype);
      }
      env->DeleteGlobalRef(jHRCParser);
      delete this;
  }

private:
  ~JHRCParser(){}

};

#endif
#ifndef _COLORER_JHRCPARSER_H_
#define _COLORER_JHRCPARSER_H_

#include<common/Hashtable.h>
#include<colorer/HRCParser.h>

class JHRCParser{
public:
  HRCParser *hrcParser;
  Hashtable<jobject> regions;

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

};

#endif
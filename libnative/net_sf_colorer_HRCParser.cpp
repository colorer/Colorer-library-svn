
#include"net_sf_colorer_HRCParser.h"
#include"JHRCParser.h"

extern "C" {

JNIEXPORT jobject JNICALL Java_net_sf_colorer_HRCParser_getRegion(JNIEnv *env, jobject obj, jlong iptr, jstring qname){
  JHRCParser *hp = (JHRCParser*)iptr;
  return hp->getRegion(env, &JString(env, qname));
};

JNIEXPORT void JNICALL Java_net_sf_colorer_HRCParser_finalize(JNIEnv *env, jobject obj, jlong iptr){
  printf("clr:HRCParser finalize\n");
  JHRCParser *hp = (JHRCParser*)iptr;
  for(int idx = 0; idx < hp->regions.size(); idx++)
    env->DeleteGlobalRef(hp->regions.get(hp->regions.key(idx)));
  delete hp;
}


}

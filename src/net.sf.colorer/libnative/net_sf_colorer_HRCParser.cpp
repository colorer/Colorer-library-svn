
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
  for(jobject region = hp->regions.enumerate(); region != null; region = hp->regions.next())
    env->DeleteGlobalRef(region);
  for(jobject filetype = hp->fileTypes.enumerate(); filetype != null; filetype = hp->fileTypes.next())
    env->DeleteGlobalRef(filetype);
  delete hp;
}

JNIEXPORT jobject JNICALL Java_net_sf_colorer_HRCParser_enumerateFileTypes
  (JNIEnv *env, jobject obj, jlong iptr, jint idx){
  JHRCParser *hp = (JHRCParser*)iptr;
  return hp->enumerateFileTypes(env, idx);
}

}
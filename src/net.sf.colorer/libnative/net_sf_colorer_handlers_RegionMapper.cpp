
#include"net_sf_colorer_handlers_RegionMapper.h"


JNIEXPORT jobject JNICALL Java_net_sf_colorer_handlers_RegionMapper_getRegionDefine__JLnet_sf_colorer_Region_2
  (JNIEnv *env, jobject obj, jlong iptr, jobject region)
{
    RegionMapper *rm = (RegionMapper*)iptr;

    jclass jClass = env->FindClass("net/sf/colorer/Region");
//    jmethodID jmInit = env->GetFieldID(jClass, "iptr");
//    jobject object = env->Get(jClass, jmInit);


    return null;
}

JNIEXPORT jobject JNICALL Java_net_sf_colorer_handlers_RegionMapper_getRegionDefine__JLjava_lang_String_2
  (JNIEnv *env, jobject obj, jlong iptr, jstring name)
{
    RegionMapper *rm = (RegionMapper*)iptr;
    return null;
}

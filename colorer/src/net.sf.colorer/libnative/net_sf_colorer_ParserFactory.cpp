
#include"net_sf_colorer_ParserFactory.h"

#include"JParserFactory.h"

extern "C" {

JNIEXPORT jlong JNICALL Java_net_sf_colorer_ParserFactory_init(JNIEnv *env, jobject obj, jstring catalogPath){
  JParserFactory *jpf = null;
  try{
    if (catalogPath == null) jpf = new JParserFactory(null);
    else jpf = new JParserFactory(&JString(env, catalogPath));
  }catch(Exception &e){
    jclass cException = env->FindClass("net/sf/colorer/ParserFactoryException");
    env->ThrowNew(cException, e.getMessage()->getChars());
    return 0;
  };
  // HRCParser wrapper
  jpf->jhp = new JHRCParser();
  jpf->jhp->hrcParser = jpf->getHRCParser();
  jclass jClass = env->FindClass("net/sf/colorer/HRCParser");
  jmethodID jmInit = env->GetMethodID(jClass, "<init>", "(J)V");
  jpf->jHRCParser = env->NewGlobalRef(env->NewObject(jClass, jmInit, (jlong)jpf->jhp));

  return (jlong)jpf;
};

JNIEXPORT void JNICALL Java_net_sf_colorer_ParserFactory_finalize(JNIEnv *env, jobject obj, jlong iptr){
  printf("clr:ParserFactory finalize iptr:%d\n", (int)iptr);
  JParserFactory *jpf = (JParserFactory*)iptr;
  if (jpf == null) return;
  env->DeleteGlobalRef(jpf->jHRCParser);
  delete jpf;
};

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_getVersion(JNIEnv *env, jobject obj, jlong iptr){
  JParserFactory *jpf = (JParserFactory*)iptr;
  return env->NewStringUTF(jpf->getVersion());
};

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_enumerateFileTypes
(JNIEnv *env, jobject obj, jlong iptr, jint idx){
  ParserFactory *pf = (JParserFactory*)iptr;
  FileType *ft = pf->getHRCParser()->enumerateFileTypes(idx);
  if (ft == null) return null;
//printf("%d %s\n", idx, ft->getName()->getChars());
  return env->NewString(ft->getName()->getWChars(), ft->getName()->length());
};

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_getFileTypeDescription
(JNIEnv *env, jobject obj, jlong iptr, jstring ftypename){
  ParserFactory *pf = (JParserFactory*)iptr;
  FileType *ft = pf->getHRCParser()->getFileType(&JString(env, ftypename));
  if (ft == null) return null;
  return env->NewString(ft->getDescription()->getWChars(), ft->getDescription()->length());
};

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_getFileTypeGroup
(JNIEnv *env, jobject obj, jlong iptr, jstring ftypename){
  ParserFactory *pf = (JParserFactory*)iptr;
  FileType *ft = pf->getHRCParser()->getFileType(&JString(env, ftypename));
  if (ft == null) return null;
  return env->NewString(ft->getGroup()->getWChars(), ft->getGroup()->length());
};

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_enumerateHRDClasses(JNIEnv *env, jobject obj, jlong iptr, jint idx){
  ParserFactory *pf = (JParserFactory*)iptr;
  const String *str = pf->enumerateHRDClasses(idx);
  if (str == null) return null;
  return env->NewString(str->getWChars(), str->length());
};

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_enumerateHRDInstances
(JNIEnv *env, jobject obj, jlong iptr, jstring hrdClass, jint idx){
  ParserFactory *pf = (JParserFactory*)iptr;
  const String *str = pf->enumerateHRDInstances(JString(env, hrdClass), idx);
  if (str == null) return null;
  return env->NewString(str->getWChars(), str->length());
}

JNIEXPORT jstring JNICALL Java_net_sf_colorer_ParserFactory_getHRDescription
(JNIEnv *env, jobject obj, jlong iptr, jstring hrdClass, jstring hrdName){
  ParserFactory *pf = (JParserFactory*)iptr;
  const String *str = pf->getHRDescription(JString(env, hrdClass), JString(env, hrdName));
  if (str == null) return null;
  return env->NewString(str->getWChars(), str->length());
};

JNIEXPORT jobject JNICALL Java_net_sf_colorer_ParserFactory_getHRCParser(JNIEnv *env, jobject obj, jlong iptr){
  JParserFactory *jpf = (JParserFactory*)iptr;
  return jpf->jHRCParser;
};



















JNIEXPORT jobject JNICALL Java_net_sf_colorer_ParserFactory_createTextParser(JNIEnv *env, jobject obj, jlong iptr){
  ParserFactory *pf = (JParserFactory*)iptr;
  TextParser *hp = pf->createTextParser();

  jclass jClass = env->FindClass("net/sf/colorer/TextParser");
  jmethodID jmInit = env->GetMethodID(jClass, "<init>", "()V");
  jobject object = env->NewObject(jClass, jmInit);
  return object;
};








JNIEXPORT jobject JNICALL Java_net_sf_colorer_ParserFactory_createStyledMapper(JNIEnv *env, jobject obj, jlong iptr, jstring hrdClass, jstring hrdName){
  ParserFactory *pf = (JParserFactory*)iptr;
  StyledHRDMapper *rm = pf->createStyledMapper(&JString(env, hrdClass), &JString(env, hrdName));

  jclass jClass = env->FindClass("net/sf/colorer/handlers/RegionMapper");
  jmethodID jmInit = env->GetMethodID(jClass, "<init>", "()V");
  jobject object = env->NewObject(jClass, jmInit);
  return object;
}

JNIEXPORT jobject JNICALL Java_net_sf_colorer_ParserFactory_createTextMapper(JNIEnv *env, jobject obj, jlong iptr, jstring hrdName){
  ParserFactory *pf = (JParserFactory*)iptr;
  TextHRDMapper *rm = pf->createTextMapper(&JString(env, hrdName));

  jclass jClass = env->FindClass("net/sf/colorer/handlers/RegionMapper");
  jmethodID jmInit = env->GetMethodID(jClass, "<init>", "()V");
  jobject object = env->NewObject(jClass, jmInit);
  return object;
};

};
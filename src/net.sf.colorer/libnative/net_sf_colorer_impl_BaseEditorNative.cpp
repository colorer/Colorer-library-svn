
#include"net_sf_colorer_impl_BaseEditorNative.h"

#include"JBaseEditor.h"

jclass cStyledRegion;
jclass cLineRegion;

jmethodID idStyledRegionConstr;
jmethodID idLineRegionConstr;
int jbe_count = 0;



jobject createStyledRegion(JNIEnv *env, const RegionDefine *rd){
  if (rd == null) return null;
  const StyledRegion *styled = StyledRegion::cast(rd);
  if (styled == null) return null;
  return env->NewObject(cStyledRegion, idStyledRegionConstr, styled->bfore, styled->bback, styled->fore, styled->back, styled->style);
}



extern "C"{

JNIEXPORT jlong JNICALL Java_net_sf_colorer_impl_BaseEditorNative_init(JNIEnv *env, jobject obj, jobject pf, jobject ls){

  if (pf == null || ls == null){
    throw_exc(env, "Bad BaseEditor constructor parameters");
    return 0;
  };

  printf("clr:BaseEditor init: %d\n", jbe_count+1);

  if (jbe_count == 0){
    cStyledRegion = (jclass)env->NewGlobalRef(env->FindClass("net/sf/colorer/handlers/StyledRegion"));
    cLineRegion = (jclass)env->NewGlobalRef(env->FindClass("net/sf/colorer/handlers/LineRegion"));

    idStyledRegionConstr = env->GetMethodID(cStyledRegion, "<init>", "(ZZIII)V");
    idLineRegionConstr = env->GetMethodID(cLineRegion, "<init>", "(Lnet/sf/colorer/Region;Lnet/sf/colorer/handlers/RegionDefine;ZIILnet/sf/colorer/Scheme;)V");
  };
  JBaseEditor *jbe = null;
  try{
    jbe_count++;
    jclass cPF = env->FindClass("net/sf/colorer/ParserFactory");
    jfieldID idIptr = env->GetFieldID(cPF, "iptr", "J");
    JParserFactory *parserFactory = (JParserFactory *)env->GetLongField(pf, idIptr);
    JavaLineSource *lineSource = new JavaLineSource(env, ls);

    jbe = new JBaseEditor(parserFactory, lineSource);
    jbe->jpf = env->NewGlobalRef(pf);
    jbe->lineSource = lineSource;
  }catch(Exception &e){
    throw_exc(env, e.getMessage()->getChars()); return 0;
  }
  return (jlong)jbe;
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_finalize(JNIEnv *env, jobject obj, jlong iptr){
  JBaseEditor *be = JBaseEditor::get(env, iptr);

  if (be == null) return;

  int idx;
  for(idx = 0; idx < be->lrCache.size(); idx++)
    if (be->lrCache.elementAt(idx) != null) env->DeleteGlobalRef(be->lrCache.elementAt(idx));
  for(idx = 0; idx < be->regionHandlers.size(); idx++)
    delete be->regionHandlers.elementAt(idx);

  env->DeleteGlobalRef(be->jpf);
  delete be->lineSource;

  if (jbe_count == 1){
    env->DeleteGlobalRef(cStyledRegion);
    env->DeleteGlobalRef(cLineRegion);
  };
  delete be;
  jbe_count--;
  printf("clr:BaseEditor finalize: %d\n", jbe_count);
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_setRegionCompact(JNIEnv *env, jobject obj, jlong iptr, jboolean compact)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->setRegionCompact(compact);
}


JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_setFileType(JNIEnv *env, jobject obj, jlong iptr, jstring typeName)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->setFileType(JString(env, typeName));
}

JNIEXPORT jstring JNICALL Java_net_sf_colorer_impl_BaseEditorNative_chooseFileType(JNIEnv *env, jobject obj, jlong iptr, jstring filename)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->chooseFileType(&JString(env, filename));
  FileType *filetype = be->getFileType();
  const String *ftname = filetype->getName();
  return env->NewString(ftname->getWChars(), ftname->length());
}

JNIEXPORT jstring JNICALL Java_net_sf_colorer_impl_BaseEditorNative_getFileType(JNIEnv *env, jobject obj, jlong iptr)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  FileType *ft = be->getFileType();
  if (ft == null) return null;
  return env->NewString(ft->getName()->getWChars(), ft->getName()->length());
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_setRegionMapper(JNIEnv *env, jobject obj, jlong iptr, jstring cls, jstring name)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->setRegionMapper(&JString(env, cls), &JString(env, name));
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_addRegionHandler(JNIEnv *env, jobject obj, jlong iptr, jobject rh, jobject filter){
  JBaseEditor *be = JBaseEditor::get(env, iptr);

  jmethodID gnID = env->GetMethodID(env->GetObjectClass(filter), "getName", "()Ljava/lang/String;");
  jstring filter_name = (jstring)env->CallObjectMethod(filter, gnID);
  JWrapRegionHandler *jwrh = new JWrapRegionHandler(env, be->pf->jhp, rh, be->pf->getHRCParser()->getRegion(&JString(env, filter_name)));
  be->addRegionHandler(jwrh);
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_removeRegionHandler(JNIEnv *env, jobject obj, jlong iptr, jobject rh){
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  JWrapRegionHandler *jwrh = null;
  for(int idx = 0; idx < be->regionHandlers.size(); idx++){
    JWrapRegionHandler *check = be->regionHandlers.elementAt(idx);
    if (env->IsSameObject(check->regionHandler, rh)){
      be->removeRegionHandler(check);
      break;
    };
  };
}


JNIEXPORT jobject JNICALL Java_net_sf_colorer_impl_BaseEditorNative_getBackground(JNIEnv *env, jobject obj, jlong iptr)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  return createStyledRegion(env, be->rd_def_Text);
}
JNIEXPORT jobject JNICALL Java_net_sf_colorer_impl_BaseEditorNative_getVertCross(JNIEnv *env, jobject obj, jlong iptr)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  return createStyledRegion(env, be->rd_def_VertCross);
}
JNIEXPORT jobject JNICALL Java_net_sf_colorer_impl_BaseEditorNative_getHorzCross(JNIEnv *env, jobject obj, jlong iptr)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  return createStyledRegion(env, be->rd_def_HorzCross);
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_setBackParse
  (JNIEnv *env, jobject obj, jlong iptr, jint backParse)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->setBackParse(backParse);
}

JNIEXPORT jobjectArray JNICALL Java_net_sf_colorer_impl_BaseEditorNative_getLineRegions(JNIEnv *env, jobject obj, jlong iptr, jint lno)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);

  if (lno >= be->lrCache.size()){
    int newsize = be->lrCache.size()*2;
    if (newsize < lno+1) newsize = lno+1;
    be->lrCache.setSize(newsize);
  }

  LineRegion *lregion = null;
  try{
    lregion = be->getLineRegions(lno);
  }catch(Exception &e){
    throw_exc(env, e.getMessage()->getChars());
    return null;
  }catch(...){
    throw_exc(env, "PANIC");
    return null;
  };

  jobjectArray cachedLR = be->lrCache.elementAt(lno);
  if (cachedLR != null && lno <= be->validLine){
    return cachedLR;
  }else{
    if (cachedLR != null) env->DeleteGlobalRef(cachedLR);
    for(int idx = be->validLine+1; idx < lno; idx++){
      if (be->lrCache.elementAt(idx) != null)
        env->DeleteGlobalRef(be->lrCache.elementAt(idx));
      be->lrCache.setElementAt(null, idx);
    };
  }

  int arrSize = 0;
  LineRegion *next = null;
  for(next = lregion; next != null; next = next->next) arrSize++;
  jobjectArray lrArray = env->NewObjectArray(arrSize, cLineRegion, null);

  int idx = 0;
  for(next = lregion; next != null; next = next->next, idx++){
    jobject sr = createStyledRegion(env, next->styled());
    jobject region = null;
    if (next->region) region = be->pf->jhp->getRegion(env, next->region->getName());
    jobject lr = env->NewObject(cLineRegion, idLineRegionConstr, region, sr, next->special, next->start, next->end, null);
    env->SetObjectArrayElement(lrArray, idx, lr);
    if (sr != null) env->DeleteLocalRef(sr);
    env->DeleteLocalRef(lr);
  };
  be->lrCache.setElementAt((jobjectArray)env->NewGlobalRef(lrArray), lno);
  be->validLine = lno;
  return lrArray;
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_validate
  (JNIEnv *env, jobject obj, jlong iptr, jint lno)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->validate(lno);
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_idleJob
  (JNIEnv *env, jobject obj, jlong iptr, jint time)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->idleJob(time);
}

JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_modifyEvent(JNIEnv *env, jobject obj, jlong iptr, jint topLine)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->modifyEvent(topLine);
  if (be->validLine > topLine-1) be->validLine = topLine-1;
}


JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_modifyLineEvent(JNIEnv *env, jobject obj, jlong iptr, jint line)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->modifyLineEvent(line);
  if (be->validLine > line-1) be->validLine = line-1;
}


JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_visibleTextEvent(JNIEnv *env, jobject obj, jlong iptr, jint wStart, jint wSize)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->visibleTextEvent(wStart, wSize);
}


JNIEXPORT void JNICALL Java_net_sf_colorer_impl_BaseEditorNative_lineCountEvent(JNIEnv *env, jobject obj, jlong iptr, jint newLineCount)
{
  JBaseEditor *be = JBaseEditor::get(env, iptr);
  be->lineCountEvent(newLineCount);
}


};

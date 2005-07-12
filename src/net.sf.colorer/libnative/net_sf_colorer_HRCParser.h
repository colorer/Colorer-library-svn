/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class net_sf_colorer_HRCParser */

#ifndef _Included_net_sf_colorer_HRCParser
#define _Included_net_sf_colorer_HRCParser
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     net_sf_colorer_HRCParser
 * Method:    finalize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_net_sf_colorer_HRCParser_finalize
  (JNIEnv *, jobject, jlong);

/*
 * Class:     net_sf_colorer_HRCParser
 * Method:    getRegion
 * Signature: (JLjava/lang/String;)Lnet/sf/colorer/Region;
 */
JNIEXPORT jobject JNICALL Java_net_sf_colorer_HRCParser_getRegion
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     net_sf_colorer_HRCParser
 * Method:    getRegionByIndex
 * Signature: (JI)Lnet/sf/colorer/Region;
 */
JNIEXPORT jobject JNICALL Java_net_sf_colorer_HRCParser_getRegionByIndex
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     net_sf_colorer_HRCParser
 * Method:    enumerateFileTypes
 * Signature: (JI)Lnet/sf/colorer/FileType;
 */
JNIEXPORT jobject JNICALL Java_net_sf_colorer_HRCParser_enumerateFileTypes
  (JNIEnv *, jobject, jlong, jint);

#ifdef __cplusplus
}
#endif
#endif

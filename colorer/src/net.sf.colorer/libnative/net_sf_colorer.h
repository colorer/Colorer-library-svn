
#include<jni.h>

#include<common/Common.h>
#include"JavaLineSource.h"
#include"JString.h"

void throw_exc(JNIEnv *env, const char *msg);

#define TRACE(comp, s) printf("[N][" comp "] " s "\n")
#define TRACE1(comp, s, p1) printf("[N][" comp "] " s "\n", p1)
#define TRACE2(comp, s, p1, p2) printf("[N][" comp "] " s "\n", p1, p2)

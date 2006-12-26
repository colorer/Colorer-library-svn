
shared_path = ../../src/shared/
obj_output_slash = objs/
obj = obj

CPP = cl
DEBUG1 = -Zi
DEBUG2 = /debug /fixed:no
#DEBUG1 = /O2gtiyb2 /GX /wd4101 /W3
#DEBUG2 =
DEFINES = -DCOLORER_FEATURE_HTTPINPUTSOURCE=0

CPPFLAGS = $(DEBUG1) /nologo /Fo$*.$(obj) -c -EHsc -I../../src/shared -I../../src/zlib $(DEFINES)


all: xml2bin.exe xmlbintest.exe

include ../../src/shared/makefile.objs
include ../../src/shared/makefile.zlib

OBJS =  $(coreobjects)
#include ../../shared/_dependencies.dep

xmlbintest.exe: $(OBJS) $(obj_output_slash)xmlbintest.obj
	link /OUT:xmlbintest.exe $(OBJS) $(obj_output_slash)xmlbintest.obj $(DEBUG2)

xml2bin.exe: $(OBJS) $(obj_output_slash)xml2bin.obj
	link /OUT:xml2bin.exe $(OBJS) $(obj_output_slash)xml2bin.obj $(DEBUG2)

$(obj_output_slash)xmlbintest.obj: xmlbintest.cpp xmlbin.cpp xmlbin.h build.xml xmlbin.params.xsl xmlbin.xsl
	$(CPP) $(CPPFLAGS) xmlbintest.cpp

$(obj_output_slash)xml2bin.obj: xml2bin.cpp xmlbin.h
	$(CPP) $(CPPFLAGS) xml2bin.cpp

test: xml2bin.exe
	echo.>> timing
	xmlbintest.exe text.hrc >> timing

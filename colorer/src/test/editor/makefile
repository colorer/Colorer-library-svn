
TARGET=editor_test.exe
all: $(TARGET)

CYGWIN=-mno-cygwin

shared_path=../../shared/
obj_output=objs

include ../../shared/makefile.colorer.cygwin

LIBINC = -lwininet -lstdc++ -lmingw32
LIBFLAGS = $(objs) $(FLAGS) -o $(TARGET) $(LIBINC)
CPPFLAGS_CUSTOM = -DCOLORER_FEATURE_LOGLEVEL=COLORER_FEATURE_LOGLEVEL_FULL
CPPFLAGS_CUSTOM += -g

objs = $(obj_output_slash)editor_test.$(obj) $(coreobjects)

$(obj_output_slash)editor_test.$(obj): editor_test.cpp
	$(CPP) $(CPPFLAGS) editor_test.cpp

$(TARGET): $(objs)
	$(LINK) $(LIBFLAGS)

clean:
	$(RM) $(objs)

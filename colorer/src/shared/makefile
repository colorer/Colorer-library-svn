
all: lines
	echo \#..>_dependencies.dep
	make colorer
	perl _makefile.deps.pl
	rm -f *.d *.o .o

lines:
	perl _buildlines.pl


#generate dependencies:
CPPFLAGS_CUSTOM=-MD -E

shared_path=../shared/
obj_output=.


include makefile.colorer.gcc

LIBINC = -lc -lm
LIBFLAGS = $(objs) $(FLAGS) -o colorer $(LIBINC)

colorer:: $(coreobjects) ./ConsoleToolsRunner.o ./FarEditor.o  ./FarEditorSet.o

./ConsoleToolsRunner.o:
	$(CPP) $(CPPFLAGS) $(shared_path)../colorer/ConsoleToolsRunner.cpp
./FarEditor.o:
	$(CPP) $(CPPFLAGS) $(shared_path)../pcolorer/FarEditor.cpp
./FarEditorSet.o:
	$(CPP) $(CPPFLAGS) $(shared_path)../pcolorer/FarEditorSet.cpp

clean:
	$(RM) $(objs)


#define COLORER_FEATURE_ASSERT TRUE

#include<math.h>
#include<stdlib.h>
#include<common/Logging.h>
#include<colorer/viewer/TextLinesStore.h>
#include<colorer/editor/BaseEditor.h>

TextLinesStore tls;

void compare(int lno, LineRegion *lr_o, LineRegion *lr_1);
void test_Incremental(BaseEditor &editor_o, BaseEditor &editor_1);
void test_Random(BaseEditor &editor_o, BaseEditor &editor_1);

#define rand_range(a)  double(rand()) / RAND_MAX * (a)


int main(int argc, char *argv[])
{
    try {

        ParserFactory pf(&DString("../../../catalog.xml"));
        
        tls.loadFile(&DString(argv[1]), null, false);

        BaseEditor editor_o(&pf, &tls);
        BaseEditor editor_1(&pf, &tls);


        editor_o.chooseFileType(&DString(argv[1]));
        editor_1.chooseFileType(&DString(argv[1]));
        editor_o.lineCountEvent(tls.getLineCount());
        editor_1.lineCountEvent(tls.getLineCount());

        editor_o.visibleTextEvent(0, tls.getLineCount());

//        test_Incremental(editor_o, editor_1);

        test_Random(editor_o, editor_1);

    }catch(Exception &e){
        printf(e.getMessage()->getChars());
        return 1;
    }

    return 0;
}

// Incremental check
void test_Incremental(BaseEditor &editor_o, BaseEditor &editor_1)
{
    printf("Incremental\n");
    for (int sizeunit = 2; sizeunit < tls.getLineCount(); sizeunit *= 2) {

        editor_1.visibleTextEvent(0, sizeunit);
        editor_1.modifyEvent(0);

        for (int lno = 0; lno < tls.getLineCount(); lno++) {
            LineRegion *lr_o = editor_o.getLineRegions(lno);
            LineRegion *lr_1 = editor_1.getLineRegions(lno);

            printf("%03d: %s\n", lno, tls.getLine(lno)->getChars());

            compare(lno, lr_o, lr_1);
        }
    }
}


// Random checks
void test_Random(BaseEditor &editor_o, BaseEditor &editor_1)
{
    printf("Random\n");

    for (int sizeunit = 2; sizeunit < tls.getLineCount(); sizeunit *= 2) {

        editor_1.visibleTextEvent(sizeunit, sizeunit*2+20);
        //editor_1.modifyEvent(0);

        for (int iterator = 0; iterator < tls.getLineCount(); iterator++) {
            
            int lno = rand_range(tls.getLineCount());

            editor_1.modifyEvent(rand_range(tls.getLineCount()));
            
            LineRegion *lr_o = editor_o.getLineRegions(lno);
            LineRegion *lr_1 = editor_1.getLineRegions(lno);

            printf("%03d: %s\n", lno, tls.getLine(lno)->getChars());

            compare(lno, lr_o, lr_1);
        }
    }
}


void compare(int lno, LineRegion *lr_o, LineRegion *lr_1)
{
    while (lr_o != null && lr_1 != null) {
        
        if (lr_o->region != lr_1->region ||
            lr_o->start  != lr_1->start  ||
            lr_o->end    != lr_1->end
           )
        {      
            // Cail Lomecb is a random sequence - protect test against it
            if (strnicmp(tls.getLine(lno)->getChars()+lr_1->start, "Cail Lomecb", 11) != 0)
            {
                printf("VALID : region:%s, %d:%d\n", lr_o->region->getName()->getChars(), lr_o->start, lr_o->end);
                printf("FAILED: region:%s, %d:%d\n", lr_1->region->getName()->getChars(), lr_1->start, lr_1->end);
                throw Exception();
            }
        }
        
        lr_o = lr_o->next;
        lr_1 = lr_1->next;
    }
}
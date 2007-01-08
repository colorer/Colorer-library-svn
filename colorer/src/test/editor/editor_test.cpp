
#define COLORER_FEATURE_ASSERT TRUE

#include<common/Logging.h>
#include<colorer/viewer/TextLinesStore.h>
#include<colorer/editor/BaseEditor.h>

int main(int argc, char *argv[])
{
try {

    ParserFactory pf(&DString("../../../catalog.xml"));
    
    TextLinesStore tls;
    tls.loadFile(&DString(argv[1]), null, false);

    BaseEditor editor_o(&pf, &tls);
    BaseEditor editor_1(&pf, &tls);


    editor_o.chooseFileType(&DString(argv[1]));
    editor_1.chooseFileType(&DString(argv[1]));
    editor_o.lineCountEvent(tls.getLineCount());
    editor_1.lineCountEvent(tls.getLineCount());

    editor_o.visibleTextEvent(0, tls.getLineCount());

    for (int sizeunit = 10; sizeunit < tls.getLineCount(); sizeunit += 1) {

        editor_1.visibleTextEvent(0, sizeunit);
        editor_1.modifyEvent(0);

        for (int lno = 0; lno < tls.getLineCount(); lno++) {
            
            LineRegion *lr_o = editor_o.getLineRegions(lno);
            LineRegion *lr_1 = editor_1.getLineRegions(lno);

            printf("%03d: %s\n", lno, tls.getLine(lno)->getChars());

            while (lr_o != null && lr_1 != null) {
                
                assert(lr_o->region == lr_1->region);
                assert(lr_o->start == lr_1->start);
                assert(lr_o->end == lr_1->end);
                
                lr_o = lr_o->next;
                lr_1 = lr_1->next;
            }

        }

    }
}catch(Exception &e){
    printf(e.getMessage()->getChars());
}

}

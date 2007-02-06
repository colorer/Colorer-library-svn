
import std.stdio;

void main(char[][] args)
{
    writefln("Hello World, Reloaded");

    // auto type inference and built-in foreach
    foreach (argc, argv; args)
    {
        // Object Oriented Programming
        CmdLin cl = new CmdLin(argc, argv);
        // Improved typesafe printf
        writefln(cl.argnum, cl.suffix, " arg: %s", cl.argv);
        // Automatic or explicit memory management
        delete cl;
    }

    // Nested structs and classes
    struct specs
    {
        // all members automatically initialized
        int count, allocated;
    }

    // Nested functions can refer to outer
    // variables like args
    specs argspecs()
    {
        specs* s = new specs;
        // no need for '->'
        s.count = args.length;		   // get length of array with .length
        s.allocated = typeof(args).sizeof; // built-in native type properties
        foreach (argv; args)
            s.allocated += argv.length * typeof(argv[0]).sizeof;
        return *s;
    }

    // built-in string and common string operations
    writefln("argc = %d, " ~ "allocated = %d",
	argspecs().count, argspecs().allocated);
}

class CmdLin
{
    private int _argc;
    private char[] _argv;

public:
    this(int argc, char[] argv)	// constructor
    {
        _argc = argc;
        _argv = argv;
    }

    int argnum()
    {
        return _argc + 1;
    }

    char[] argv()
    {
        return _argv;
    }

    char[] suffix()
    {
        char[] suffix = "th";
        switch (_argc)
        {
          case 0:
            suffix = "st";
            break;
          case 1:
            suffix = "nd";
            break;
          case 2:
            suffix = "rd";
            break;
          default:
	    break;
        }
        return suffix;
    }
}


class Test
{
       static this()
       {
       }
}


void func1(int x)
{   int x;	// illegal, x shadows parameter x

    int y;

    { int y; }	// illegal, y shadows enclosing scope's y

    void delegate() dg;
    dg = { int y; };	// ok, this y is not in the same function

    struct S
    {
	int y;		// ok, this y is a member, not a local
    }

    { int z; }
    { int z; }	// ok, this z is not shadowing the other z

    { int t; }
    { t++;   }	// illegal, t is undefined
}

char[] a;


foreach (int i, char c; a)
{
    printf("a[%d] = '%c'\n", i, c);
}


char[] a = "\xE2\x89\xA0";	// \u2260 encoded as 3 UTF-8 bytes

foreach (dchar c; a)
{
    printf("a[] = %x\n", c);	// prints 'a[] = 2260'
}

dchar[] b = "\u2260";

foreach (char c; b)
{
    printf("%x, ", c);	// prints 'e2, 89, a0'
}


int gethardware()
{
    asm {
	    mov	EAX, dword ptr 0x1234;
    }
}

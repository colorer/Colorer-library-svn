
#ifdef Test
#error Test :(
#endif

const int test=12345;

func(
    int arg
    )
{
}



unsigned char ucl_rotr8(unsigned char value, int shift);
extern __inline__ unsigned char ucl_rotr8(unsigned char value, int shift)
{
    unsigned char result;

    __asm__ __volatile__ ("movb %b1, %b0; rorb %b2, %b0"
                        : "=a"(result) : "g"(value), "c"(shift));
    return result;
}


0x2322u
//

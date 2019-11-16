#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "mpn.h"
#include "bigint.h"

int main()
{
    char str[]="-12345678901234567890";
    BigInteger a(str);
    BigInteger b=a.multiply(a);
    BigInteger c= b.multiply(a);
    c.setNegative();
    c.show();
    return 0;
}

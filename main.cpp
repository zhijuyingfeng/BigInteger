#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "mpn.h"
#include "bigint.h"

int main()
{
    BigInteger a("413");
    BigInteger b("780");
    BigInteger c=a.modInverse(b);
    c.show();
    return 0;
}

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "mpn.h"
#include "bigint.h"

int main()
{
    BigInteger a("12345678901234567890");
    BigInteger b("1234567889");
    BigInteger c=a.mod(b);
    c.show();
    return 0;
}

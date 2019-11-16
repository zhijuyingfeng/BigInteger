#include "mpn.h"

int32_t MPN::NEGATIVE_ONE=-1;
int32_t MPN::CHARS_PER_WORD=9;
int64_t MPN::NEGATIVE_ONE_64=0xFFFFFFFF;

int32_t MPN::add_1(int32_t *dest, int32_t *x, const int32_t &size, const int32_t &y)
{
    int64_t carry=y;
    for(int i=0;i<size;i++)
    {
        carry+=static_cast<int64_t>(x[i])&NEGATIVE_ONE_64;
        dest[i]=carry&NEGATIVE_ONE;
        carry>>=32;
    }
    return carry&NEGATIVE_ONE;
}


int32_t MPN::mul_1(int32_t *dest, int32_t *x, const int32_t &len, const int32_t &y)
{
    int64_t carry=0;
    int64_t ywords=static_cast<int64_t>(y)&NEGATIVE_ONE_64;
    for(int i=0;i<len;i++)
    {
        carry+=(NEGATIVE_ONE_64&static_cast<int64_t>(x[i]))*ywords;
        dest[i]=carry&NEGATIVE_ONE;
        uint64_t temp=static_cast<uint64_t>(carry);
        temp>>=32;
        carry=static_cast<int64_t>(temp);
    }
    return NEGATIVE_ONE&carry;
}

int32_t MPN::set_str(int32_t *dest, int8_t *str, const int32_t &str_len)
{
    int32_t pos=0;
    int32_t base=10;
    int32_t size=0;

    while(pos<str_len)
    {
        int32_t temp=0;
        int32_t bigbase=1;
        for(int32_t i=0;i<CHARS_PER_WORD&&pos<str_len;i++,pos++)
        {
            temp=temp*base+str[pos];
            bigbase*=base;
        }
        int cy_limb;
        if(!size)
        {
            cy_limb=temp;
        }
        else
        {
            cy_limb=mul_1(dest,dest,size,bigbase);
            cy_limb+=add_1(dest,dest,size,temp);
        }
        if(cy_limb)
            dest[size++]=cy_limb;
    }
    return size;
}

int32_t MPN::add_n(int32_t *dest, int32_t *x, int32_t *y, const int32_t &len)
{
    int64_t carry=0;
    for(int i=0;i<len;i++)
    {
        carry+=(static_cast<int64_t>(x[i])&NEGATIVE_ONE_64)+
                (static_cast<int64_t>(y[i])&NEGATIVE_ONE_64);
        dest[i]=carry&NEGATIVE_ONE;
        uint64_t temp=static_cast<uint64_t>(carry);
        temp>>=32;
        carry=static_cast<int64_t>(temp);
    }
    return carry&NEGATIVE_ONE;
}

void MPN::mul(int32_t *dest, int32_t *x, const int32_t &xlen, int32_t *y, const int32_t &ylen)
{
    dest[xlen]=mul_1(dest,x,xlen,y[0]);

    for(int32_t i=1;i<ylen;i++)
    {
        int64_t carry=0,yword=static_cast<int64_t>(y[i])&NEGATIVE_ONE_64;

        for(int32_t j=0;j<xlen;j++)
        {
            int64_t temp=static_cast<int64_t>(x[j])&NEGATIVE_ONE_64;
            temp*=yword;
            temp+=static_cast<int64_t>(dest[i+j])&NEGATIVE_ONE_64;

            carry+=temp;
            dest[i+j]=carry&NEGATIVE_ONE;

            uint64_t tmp=static_cast<uint64_t>(carry);
            tmp>>=32;
            carry=static_cast<int64_t>(tmp);
        }
        dest[i+xlen]=carry&NEGATIVE_ONE;
    }
}

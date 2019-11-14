#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bigint.h"
#include "mpn.h"

BigInteger::BigInteger()
{
    *this=ZERO;
}

BigInteger::BigInteger(const int&val)
{
    this->ival=val;
    this->words=NULL;
}

BigInteger::BigInteger(const char* val)
{
    if(!val)
    {
        *this=ZERO;
        return;
    }
    int32_t len=static_cast<int32_t>(strlen(val));
    if(len<=15)
    {
        *this=valueOf(atoll(val));
        return;
    }
    int8_t* bytes;
    bool negative=false;
    int start=0;
    if(val[0]=='-')
    {
        len--;
        negative=true;
        start++;
    }
    bytes=new int8_t[len];
    for(int i=0;i<len;i++)
    {
        bytes[i]=val[i+start]-'0';
    }
    BigInteger ans=valueOf(bytes,len,negative);
    delete [] bytes;
    this->words=NULL;
    *this=ans;
}

BigInteger::BigInteger(const BigInteger&val)
{
    this->ival=val.ival;
    if(val.words)
    {
        this->words=new int32_t[this->ival];
        memcpy(this->words,val.words,sizeof(int32_t)*static_cast<size_t>(this->ival));
    }
    else
    {
        this->words=NULL;
    }
}

BigInteger BigInteger::operator=(const BigInteger &val)
{
    this->ival=val.ival;
    if(val.words)
    {
        delete [] this->words;
        this->words=new int32_t[this->ival];
        memcpy(this->words,val.words,sizeof(int32_t)*static_cast<size_t>(this->ival));
    }
    else
    {
        this->words=NULL;
    }
    return *this;
}

BigInteger::~BigInteger()
{
    delete [] this->words;
}

BigInteger BigInteger::ONE(1);
BigInteger BigInteger::ZERO(0);
BigInteger BigInteger::TEN(10);
int32_t BigInteger::NEGATIVE_ONE=-1;
int32_t BigInteger::CHARS_PER_WORD=9;
int64_t BigInteger::NEGATIVE_ONE_64=0xFFFFFFFFLL;

bool BigInteger::isNegative() const
{
    if(this->words)
        return this->words[this->ival-1]<0;
    else
        return this->ival<0;
}

void BigInteger::set(const int *words, const int &len)
{
    this->ival=len;
    delete [] this->words;
    this->words=new int[len];
    memcpy(this->words,words,sizeof(int)*static_cast<size_t>(len));
}

BigInteger::BigInteger(const int64_t&val)
{
    this->ival=2;
    this->words=new int32_t[2];
    this->words[0]=val&NEGATIVE_ONE;
    this->words[1]=(val>>32)&NEGATIVE_ONE;
}

BigInteger BigInteger::valueOf(int8_t *digits, const int &byte_len, const bool &negative)
{
    int32_t *words=new int32_t[byte_len/CHARS_PER_WORD+1];
    int32_t size=MPN::set_str(words,digits,byte_len);
    if(!size)
        return ZERO;
    if(words[size-1]<0)
        words[size++]=0;
    if (negative)
        negate(words, words, size);
    BigInteger ans = make(words, size);
    delete[] words;
    return ans;
}

BigInteger BigInteger::valueOf(const int64_t &val)
{
    BigInteger ans=ZERO;
    int32_t i=val&NEGATIVE_ONE;
    if(val==static_cast<int64_t>(i))
    {
        ans.ival=i;
        ans.words=NULL;
    }
    else
    {
        ans.ival=2;
        ans.words=new int32_t[ans.ival];
        ans.words[0]=val&NEGATIVE_ONE;
        ans.words[1]=(val>>32)&NEGATIVE_ONE;
    }
    return ans;
}

void BigInteger::show()
{
    for(int i=0;i<this->ival;i++)
        printf("%08X\n",this->words[this->ival-i-1]);
}

bool BigInteger::negate(int32_t *dest, int32_t *src, const int32_t &len)
{
    bool negative=src[len-1]<0;
    int64_t carry=1;
    for(int i=0;i<len;i++)
    {
        carry+=static_cast<int64_t>((~src[i]))&NEGATIVE_ONE_64;
        dest[i]=carry&NEGATIVE_ONE;
        carry>>=32;
    }
    return negative&&dest[len-1]<0;
}

BigInteger BigInteger::make(int32_t *words, const int32_t &len)
{
    if(words==NULL)
        return BigInteger(len);
    int32_t Len=wordsNeeded(words,len);
    if(Len<=1)
        return !Len?ZERO:BigInteger(words[0]);
    BigInteger ans=ZERO;
    ans.ival=Len;
    ans.words=new int32_t[Len];
    memcpy(ans.words,words,sizeof(int32_t)*static_cast<size_t>(Len));
    return ans;
}

int32_t BigInteger::wordsNeeded(const int *words, const int &len)
{
    int i = len;
    if (i>0)
    {
        int word = words[--i];
        if (word == -1)
        {
            while (i>0 && (word = words[i - 1])<0)
            {
                i--;
                if (word != -1)
                    break;
            }
        }
        else
        {
            while (word == 0 && i>0 && (word = words[i - 1]) >= 0)
                i--;
        }
    }
    return i + 1;
}

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

const BigInteger BigInteger::ONE(1);
const BigInteger BigInteger::ZERO(0);
const BigInteger BigInteger::TEN(10);

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
    memcpy(this->words,words,sizeof(int32_t)*static_cast<size_t>(len));
}

BigInteger::BigInteger(const int64_t&val)
{
    this->ival=2;
    this->words=new int32_t[2];
    this->words[0]=val&NEGATIVE_ONE;
    this->words[1]=(val>>32)&NEGATIVE_ONE;
}

BigInteger BigInteger::valueOf(int8_t *digits, const int32_t &byte_len, const bool &negative)
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
        printf("%08x",this->words[this->ival-i-1]);
    printf("\n");
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
    if(!words)
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

int32_t BigInteger::wordsNeeded(const int32_t *words, const int32_t &len)
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

BigInteger BigInteger::add(const BigInteger &x, const int32_t &y)
{
    if(!x.words)
        return BigInteger::add(x.ival,y);
    BigInteger ans=ZERO;
    ans.setAdd(x,y);
    return ans.canonicalize();
}

BigInteger BigInteger::add(const int32_t &x, const int32_t &y)
{
    return valueOf(static_cast<int64_t>(x)+static_cast<int64_t>(y));
}

void BigInteger::setAdd(const BigInteger &x, const int32_t &y)
{
    delete [] this->words;
    if(!x.words)
    {
        *this=BigInteger::add(x.ival,y);
        return;
    }
    this->words=new int32_t[x.ival+1];
    int32_t temp=MPN::add_1(this->words,x.words,x.ival,y);
    if(x.words[x.ival-1]<0)
        temp--;
    this->words[x.ival]=temp;
    this->ival=wordsNeeded(words,x.ival+1);
}

BigInteger BigInteger::canonicalize()
{
    if(this->words&&
            (this->ival=this->wordsNeeded(this->words,this->ival))<=1)
    {
        if(this->ival==1)
            this->ival=this->words[0];
        delete [] this->words;
        this->words=NULL;
    }
    return *this;
}

BigInteger BigInteger::add(BigInteger x, BigInteger y, const int32_t &k)
{
    if(!x.words&&!y.words)
    {
        int64_t temp=static_cast<int64_t>(k)*static_cast<int64_t>(x.ival);
        temp*=static_cast<int64_t>(y.ival);
        return valueOf(temp);
    }
    if(k!=1)
    {
        if(-1==k)
        {
            y=BigInteger::neg(y);
        }
        else
            y=BigInteger::times(y,valueOf(k));
    }
    if(!x.words)
        return BigInteger::add(y,x.ival);
    else if(!y.words)
        return BigInteger::add(x,y.ival);

    //Both are big
    if(y.ival>x.ival)
    {
        //swap so that x is longer than y
        int32_t temp=x.ival;
        x.ival=y.ival;
        y.ival=temp;

        int32_t* tmp=x.words;
        x.words=y.words;
        y.words=tmp;
    }

    int32_t i=y.ival;
    BigInteger ans=ZERO;
    ans.words=new int32_t[x.ival+1];
    int32_t cy_limb=MPN::add_n(ans.words,x.words,y.words,i);

    int64_t carry=cy_limb;
    int64_t y_ext=y.words[i-1]<0?NEGATIVE_ONE_64:0;
    for(;i<x.ival;i++)
    {
        carry+=(static_cast<int64_t>(x.words[i])&NEGATIVE_ONE_64)+y_ext;
        ans.words[i]=carry&NEGATIVE_ONE;
        uint64_t temp=static_cast<uint64_t>(carry);
        temp>>=32;
        carry=static_cast<int64_t>(temp);
    }
    if(x.words[i-1]<0)
        y_ext--;
    ans.words[x.ival]=(carry+y_ext)&NEGATIVE_ONE;
    ans.ival=x.ival+1;
    return ans.canonicalize();
}

BigInteger BigInteger::add(const BigInteger &val) const
{
    return BigInteger::add(*this,val,1);
}

BigInteger BigInteger::subtract(const BigInteger &val) const
{
    return BigInteger::add(*this,val,-1);
}

BigInteger BigInteger::neg(const BigInteger &x)
{
    if(!x.words&&x.ival!=MIN_INT32)
        return valueOf(-x.ival);
    BigInteger result=ZERO;
    result.setNegative(x);
    return result.canonicalize();
}

void BigInteger::setNegative(const BigInteger &x)
{
    if(!x.words)
    {
        delete [] this->words;
        if(x.ival==MIN_INT32)
        {
            *this=valueOf(MIN_INT32);
        }
        else
        {
            this->words=NULL;
            this->ival=-x.ival;
        }
        return;
    }

    int32_t* xwords=x.words;
    if(x.words==this->words)
    {
        xwords=new int32_t[x.ival];
        memcpy(xwords,x.words,sizeof(int32_t)*static_cast<size_t>(x.ival));
    }
    delete [] this->words;
    int32_t len=x.ival;
    this->words=new int32_t[len+1];
    if(negate(words,xwords,len))
        words[len++]=0;
    this->ival=len;
    if(xwords!=x.words)
        delete [] xwords;
}

BigInteger BigInteger::times(const BigInteger &x, int32_t y)
{
    if(!y)
        return ZERO;
    else if(1==y)
        return x;
    else if(!x.words)
    {
        return valueOf(static_cast<int64_t>(x.ival)*static_cast<int64_t>(y));
    }
    bool negative=false;
    BigInteger ans=ZERO;
    ans.words=new int32_t[x.ival+1];
    int32_t *xwords;
    if(x.words[x.ival-1]<0)
    {
        negative=true;
        xwords=new int32_t[x.ival+1];
        negate(xwords,x.words,x.ival);
    }
    else
    {
        xwords=x.words;
//        xwords=new int32_t[x.ival];
//        memcpy(xwords,x.words,static_cast<size_t>(x.ival)*sizeof(int32_t));
    }
    if(y<0)
    {
        negative=!negative;
        y=-y;
    }
    ans.words[x.ival]=MPN::mul_1(ans.words,xwords,x.ival,y);
    ans.ival=x.ival+1;
    if(negative)
        ans.setNegative();
    if(xwords!=x.words)
        delete [] xwords;
    return ans.canonicalize();
}

void BigInteger::setNegative()
{
    this->setNegative(*this);
}

BigInteger BigInteger::times(const BigInteger &x, const BigInteger &y)
{
    if(!y.words)
        return times(x,y.ival);
    else if(!x.words)
        return times(y,x.ival);

    bool negative=false;
    int32_t* xwords=x.words,*ywords=y.words;
    if(x.words[x.ival-1]<0)
    {
        negative=true;
        xwords=new int32_t[x.ival];
        negate(xwords,x.words,x.ival);
    }
    if(y.words[y.ival-1]<0)
    {
        negative=!negative;
        ywords=new int32_t[y.ival];
        negate(ywords,y.words,y.ival);
    }
    //swap if x is shorter than y
    int32_t xlen=x.ival,ylen=y.ival;
    if(xlen<ylen)
    {
        int32_t* twords=xwords;
        xwords=ywords;
        ywords=twords;
        int32_t tlen=xlen;
        xlen=ylen;
        ylen=tlen;
    }
    BigInteger ans=ZERO;
    ans.ival=xlen+ylen;
    ans.words=new int32_t[ans.ival];
    MPN::mul(ans.words,xwords,xlen,ywords,ylen);

    if(negative)
        ans.setNegative();

    if(xwords!=x.words&&xwords!=y.words)
        delete [] xwords;
    if(ywords!=y.words&&ywords!=x.words)
        delete [] ywords;
    return ans.canonicalize();
}
BigInteger BigInteger::multiply(const BigInteger &val) const
{
    return times(*this,val);
}

#ifndef BIGINT_H
#define BIGINT_H

#include <cstdint>

class BigInteger
{
private:
    int32_t ival;
    int32_t *words;

    BigInteger(const int32_t&val);
    BigInteger(const int64_t&val);
    void set(const int32_t *words, const int32_t &len);

    static BigInteger ONE;
    static BigInteger ZERO;
    static BigInteger TEN;
    static int32_t NEGATIVE_ONE;
    static int64_t NEGATIVE_ONE_64;
    static int32_t CHARS_PER_WORD;

    static BigInteger valueOf(int8_t *digits, const int &byte_len, const bool &negative);
    static int32_t wordsNeeded(const int *words, const int &len);
    static BigInteger valueOf(const int64_t& val);

    /** Set dest[0:len-1] to the negation of src[0:len-1].
    * Return true if overflow (i.e. if src is -2**(32*len-1)).
    * Ok for src==dest. */
    static bool negate(int32_t *dest, int32_t *src, const int32_t &len);

    /** Make a canonicalized BigInteger from an array of words.
    * The array may be reused (without copying). */
    static BigInteger make(int32_t *words, const int32_t &len);
public:
    BigInteger();
    ~BigInteger();
    BigInteger(const BigInteger&val);
    BigInteger(const char* val);
    BigInteger operator=(const BigInteger&val);
    bool isNegative()const;
    void show();
};

#endif // BIGINT_H

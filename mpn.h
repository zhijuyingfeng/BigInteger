#ifndef MPN_H
#define MPN_H

#include <cstdint>

class MPN
{
    static int32_t NEGATIVE_ONE;
    static int64_t NEGATIVE_ONE_64;
    static int32_t CHARS_PER_WORD;
public:
    static int32_t set_str(int32_t*dest, int8_t*str, const int32_t& str_len);

    /** Add x[0:size-1] and y, and write the size least
    * significant words of the result to dest.
    * Return carry, either 0 or 1.
    * All values are unsigned.
    * This is basically the same as gmp's mpn_add_1. */
    static int32_t add_1(int32_t *dest, int32_t*x, const int32_t&size, const int32_t&y);

    /** Multiply x[0:len-1] by y, and write the len least
    * significant words of the product to dest[0:len-1].
    * Return the most significant word of the product.
    * All values are treated as if they were unsigned
    * (i.e. masked with 0xffffffffL).
    * OK if dest==x (not sure if this is guaranteed for mpn_mul_1).
    * This function is basically the same as gmp's mpn_mul_1.
    */
    static int32_t mul_1(int32_t*dest, int32_t*x, const int32_t&len, const int32_t&y);

    /** Add x[0:len-1] and y[0:len-1] and write the len least
      * significant words of the result to dest[0:len-1].
      * All words are treated as unsigned.
      * Return the carry, either 0 or 1
      * This function is basically the same as gmp's mpn_add_n.*/
    static int32_t add_n(int32_t*dest,int32_t*x,int32_t*y,const int32_t &len);


    /**
     * Multiply x[0:xlen-1] and y[0:ylen-1], and
     * write the result to dest[0:xlen+ylen-1].
     * The destination has to have space for xlen+ylen words,
     * even if the result might be one limb smaller.
     * This function requires that xlen >= ylen.
     * The destination must be distinct from either input operands.
     * All operands are unsigned.
     * This function is basically the same gmp's mpn_mul. */
    static void mul(int32_t*dest,int32_t*x,const int32_t &xlen,int32_t*y,const int32_t &ylen);
};

#endif // MPN_H

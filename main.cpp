#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "mpn.h"
#include "bigint.h"

int main()
{
    const char p_string[] =  "473775733099111458095976402892151243213040090086618881"
                             "031224856589919883111175432182210784004484707192524499"
                             "542529027682867198116142629463026716198471009819494687"
                             "419717246926437103127982384979803193954685843127713049"
                             "424082085738653899178130804288754453401240826773288925"
                             "74545183253774970952621120608285211907";
    const char q_string[] =  "179763108085962807080093919657307409975718428269269435"
                             "318693176520197803112687952722248946497029842707955098"
                             "616584161059182132899857985791259621959619387353664424"
                             "449358830496413557281341725462360322134010042282012642"
                             "274593164792155394683891626920676409771537318196968561"
                             "566914528483955014073455660937229038263";
    BigInteger p("12345678901234567890"),q("12345678889");
    BigInteger c=q.modInverse(p);
//    BigInteger c=p.divide(q);
    c.show();
    return 0;
}

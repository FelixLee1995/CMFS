#include <iostream>
#include "test_suit.h"
#include "api/ctp_ext/ctp_ext.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"

using namespace ctp_ftd;

TEST(test, test_decode)
{


    char debuf[2048] = {};
    unsigned int decode_len = 0;
    
    unsigned char * pData = bLoginRsp;
    auto p_ftd_header = (ftd_header *) pData; 

    auto len = p_ftd_header->ftd_content_len - sizeof(ftdc_crpheader);
    auto p_content = pData + 6;

    //DecodeZero(p_content, len, debuf, &decode_len);


}



int main()
{

    testing::InitGoogleTest();
    return RUN_ALL_TESTS();

}

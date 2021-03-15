#include <iostream>
#include "test_suit.h"
#include "api/ctp_ext/ctp_ext.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "core/plantformtools.h"

using namespace ctp_ftd;

TEST(test, test_decode)
{


    char debuf[2048] = {};
    unsigned int decode_len = 0;
    
    auto pData = (char *)bLoginRsp;
    auto p_ftd_header = (ftd_header *) pData; 

    auto len = ntohs(p_ftd_header->ftd_content_len) - sizeof(ftdc_crpheader);
    auto p_content = pData + 6;

    DecodeZero(p_content, len, debuf, &decode_len);


    auto p_ftdc_header = (ftdc_header *)debuf;

    // uint16_t fCount = ntohs(p_ftdc_header->ftdc_field_count);
    // uint32_t topic_id = ntohl(p_ftdc_header->ftdc_topic_id);
	auto pContent = (char*)((char*)p_ftdc_header+sizeof(ftdc_header));

	ftdc_field_header* pField = (ftdc_field_header*)pContent;

    std::cout << "fieldID: " << pField->field_id << ", Len: " << ntohs(pField->field_len) << std::endl;

    pContent += sizeof(ftdc_field_header);
    

    if (ntohs(pField->field_id) == ftdc_fid_RspInfoField)
    {
        CThostFtdcRspInfoField *pInfo = (CThostFtdcRspInfoField *)pContent;
        //auto errorid = ntohl(pInfo->ErrorID);

        std::cout << "ErrorMsg: " << pInfo->ErrorMsg << std::endl;
    }
  

    pContent += ntohs(pField->field_len);

}





TEST(test, test_decode_market_data)
{


    char debuf[2048] = {};
    unsigned int decode_len = 0;
    
    auto pData = (char *)bRtnMarketSnap;
    auto p_ftd_header = (ftd_header *) pData; 

    auto len = ntohs(p_ftd_header->ftd_content_len) - sizeof(ftdc_crpheader);
    auto p_content = pData + 6;

    DecodeZero(p_content, len, debuf, &decode_len);

    std::cout << "decoded size is " << decode_len <<std::endl;


    auto p_ftdc_header = (ftdc_header *)debuf;

    // uint16_t fCount = ntohs(p_ftdc_header->ftdc_field_count);
    // uint32_t topic_id = ntohl(p_ftdc_header->ftdc_topic_id);
	auto pContent = (char*)((char*)p_ftdc_header+sizeof(ftdc_header));

	ftdc_field_header* pField = (ftdc_field_header*)pContent;

    std::cout << "fieldID: " << pField->field_id << ", Len: " << ntohs(pField->field_len) << std::endl;

    pContent += sizeof(ftdc_field_header);
    

    if (ntohs(pField->field_id) == ftdc_fid_DepthMarketDataField)
    {
        CThostFtdcDepthMarketDataField *marketData = (CThostFtdcDepthMarketDataField *)pContent;
        //auto errorid = ntohl(pInfo->ErrorID);

        std::cout << "UpdateTime: " << marketData->UpdateTime << std::endl;
    }


    for (unsigned int i =0 ;i< decode_len ; ++i)
    {
        printf("0x%02x, ", debuf[i]);
        //std::cout << std::hex << static_cast<int>(debuf[i]) << ", " << std::endl; 
    }
  

    pContent += ntohs(pField->field_len);

}



int main()
{

    testing::InitGoogleTest();
    return RUN_ALL_TESTS();

}

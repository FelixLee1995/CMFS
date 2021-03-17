#include "apiWrapper/ctp/MyMarketApi.h"
#include "core/singleton.h"

MyMarketApi::MyMarketApi(const nlohmann::json& configJson)
    : m_rid(1)
    , m_front_connect_stautus(false)
    , m_login_stautus(false)
    , m_api(nullptr)
    , m_spi(nullptr)
{



    m_brokerid = configJson["brokerid"].get<std::string>();
    m_front_addr = configJson["path"].get<std::string>();
    m_userid = configJson["username"].get<std::string>();
    m_pwd = configJson["pwd"].get<std::string>();

}

void MyMarketApi::Dispose() {}

MyMarketApi::~MyMarketApi()
{
    std::cout << "MyMarketApi deConstruction!" << std::endl;

    if (m_api)
    {
        m_api->RegisterSpi(nullptr);
        m_api->Release();
        m_api = nullptr;
    }

    if (m_spi)
    {
        delete m_spi;
        m_spi = nullptr;
    }
}

void MyMarketApi::Init()
{
    m_api = CThostFtdcMdApi::CreateFtdcMdApi();
    m_spi = new MyMarketSpi(this);
    m_api->RegisterSpi(m_spi);

    std::cout << "api version is " << m_api->GetApiVersion();

    m_api->RegisterFront(const_cast<char *>(m_front_addr.c_str()));

    m_api->Init();
}

int MyMarketApi::ReqUserLogin()
{
    auto check_flag = m_front_connect_stautus.load();

    if (!check_flag)
    {
        std::cout << "please connect fisrt!" << std::endl;
        return SYS_FAIL;
    }

    CThostFtdcReqUserLoginField reqUserLoginField = {};
    memcpy(&reqUserLoginField.BrokerID, m_brokerid.c_str(), sizeof(reqUserLoginField.BrokerID));
    memcpy(&reqUserLoginField.UserID, m_userid.c_str(), sizeof(reqUserLoginField.UserID));
    memcpy(&reqUserLoginField.Password, m_pwd.c_str(), sizeof(reqUserLoginField.Password));

    m_api->ReqUserLogin(&reqUserLoginField, 0);

    return SYS_OK;
}

int MyMarketApi::ReqSubscribeMarketData(const std::vector<std::string> instrVec) 
{


    auto check_flag = m_login_stautus.load();
    if (!check_flag) {
        std::cout << "please login first!" << std::endl;
        return -1;
    }

    int res = 0;
    int cnt = instrVec.size();
    char **instrument = new char *[cnt];

    for (int i = 0; i < cnt; i++)
    {
        instrument[i] = new char[31];
        std::strcpy(instrument[i], instrVec[i].data());
    }

    res = m_api->SubscribeMarketData(instrument, cnt);



    for (int i = 0 ; i< cnt ; i++)
    {
        delete[] instrument[i];
    }

    delete[] instrument;

    return res;
}

int MyMarketApi::ReqSubscribeMarketData(const std::string &instrumentID)
{
    int res = SYS_FAIL;
    char **instrument = new char *[1];
    instrument[0] = new char[sizeof(TThostFtdcInstrumentIDType)];
    std::memcpy(instrument[0], instrumentID.c_str(), sizeof(TThostFtdcInstrumentIDType));
    res = m_api->SubscribeMarketData(instrument, 1);

    delete[] instrument[0];
    delete[] instrument;
    return res;
}

int MyMarketApi::ReqUnSubscribeMarketData(const std::string &instrumentID)
{
    int res = SYS_FAIL;
    char **instrument = new char *[1];
    instrument[0] = new char[sizeof(TThostFtdcInstrumentIDType)];
    std::memcpy(instrument[0], instrumentID.c_str(), sizeof(TThostFtdcInstrumentIDType));
    res = m_api->UnSubscribeMarketData(instrument, 1);

    delete[] instrument[0];
    delete[] instrument;

    return res;
}

const char *MyMarketApi::GetTradingDay()
{
    return m_api->GetTradingDay();
}

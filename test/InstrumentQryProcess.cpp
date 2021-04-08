#include <iostream>
#include <fstream>
#include <assert.h>
#include "core/plantformtools.h"
#include "apiWrapper/ctp/MyCtpApi.h"



using namespace std;



int main()
{


    std::ifstream ifs("./config/CtpInstrQryAddress.ini");


    std::string address, brokerid, userid, pwd, appid, authcode;

    if (!ifs)
    {
        std::cerr << "Failed to find QryAddress config file!" << std::endl;
        return -1;
    }

    ifs >> address >> brokerid >> userid >> pwd >> appid >> authcode;

    auto p_api = new MyCtpApi(brokerid, userid, pwd, address, appid, authcode);

    p_api->Init();
    CommonSleep(2000);
    assert(p_api->GetConnectStatus());
    p_api->ReqAuth();
    CommonSleep(2000);
    assert(p_api->GetAuthStatus());
    p_api->ReqUserLogin();
    CommonSleep(2000);
    assert(p_api->GetLoginStatus());

    p_api->ReqQryInstrument();

    CommonSleep(1000*120);

    return 0;
}
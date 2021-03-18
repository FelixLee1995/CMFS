#include "adapter/CAdapterManager.h"
#include "adapter/CUDPAdapter.h"
#include "adapter/CMockAdapter.h"
#include "adapter/CCtpMarketAdapter.h"


CAdapterManager::CAdapterManager()
{
    m_ConfigPtr.reset(Singleton<CConfig>::GetInstance());
}


void CAdapterManager::Init()
{


    auto configJson = m_ConfigPtr->GetConfigJson();
    auto adapters =  configJson["adapters"];
    if (!adapters.is_array())
    {
        SPDLOG_ERROR("config.json error, no array for market adapters!");
        return;
    }
    
    for (auto adapterConfig:adapters)
    {
        auto IfInUse = adapterConfig["InUse"].get<bool>();
        if (IfInUse)
        {
            auto type = adapterConfig["type"].get<std::string>();
            Initdapter(type, adapterConfig);
        }

    }
    
}

void CAdapterManager::Initdapter(const std::string& type, const nlohmann::json& configJson)
{

    if (type == "CTP") {
        Singleton<CCtpMarketAdapter>::Instance(configJson)->Init();
    }
    else if (type == "UDP") {
        Singleton<CUdpMarketAdapter>::Instance(configJson)->Init();
    }
    else if (type == "MOCK") {
        Singleton<CMockAdapter>::Instance()->Init();
    }


}

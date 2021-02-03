/**
*  File: singleton.h
*  Desc: 单例模板
*  Created by felix on 2020/09/04
*/
#pragma once 

#include <core/nocopyable.h>

template <typename T>
class Singleton: Nocopyable {

public:
template<typename... Args>
static T* Instance(Args&&... args) {
    if (m_pInstance == nullptr) {
        m_pInstance = new T(std::forward<Args>(args)...); 
    }
    return m_pInstance;
};


static T * GetInstance() {
    if (m_pInstance == nullptr) {
        throw std::logic_error("error, no instance available, please initialize first");
    }
    return m_pInstance;
}


static void DestroyInstance() {
    delete m_pInstance;
    m_pInstance = nullptr;
}

private:

static T* m_pInstance;

};

template <typename T>
T* Singleton<T>::m_pInstance = nullptr;
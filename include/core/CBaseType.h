#ifndef _CBASE_H
#define _CBASE_H

#include <string>
#include <cstring>
#ifndef WIN32
#include <unistd.h>
#endif // !WIN32


#ifdef WIN32
#define PMS_DLLEXPORT __declspec(dllexport)
#define PMS_SLEEP(time) Sleep(time)
#define PMS_SOCKLEN int
#define PMS_GET_DYNAMIC_LIB(libName) LoadLibraryA(libName)
#define PMS_FREE_DYNAMIC_LIB(libHandle) FreeLibrary((HMODULE)libHandle)
#define PMS_GET_DYNAMIC_METHOD(libHandle, methodName) GetProcAddress((HMODULE)libHandle, methodName)
#define PMS_STRTOK  strtok_s
#else
#define PMS_DLLEXPORT
#define PMS_SLEEP(time) usleep(time * 1000)
#define PMS_SOCKLEN socklen_t
#define PMS_GET_DYNAMIC_LIB(libName)  dlopen(libName, RTLD_LAZY)
#define PMS_FREE_DYNAMIC_LIB(libHandle) dlclose(libHandle)
#define PMS_GET_DYNAMIC_METHOD(libHandle, methodName) dlsym(libHandle, methodName)
#define PMS_STRTOK  strtok_r
#endif

//set  byte align
#pragma pack(push, 1)



class CTBaseString
{
};

template <int TLength> class CTString: public CTBaseString
{
public:

    CTString(void)
    {
        Clear();
    }


    CTString(const CTString<TLength>& value)
    {
        SetValue(value.GetValue());
    }


    CTString(const char * value)
    {
        SetValue(value);
    }

    const CTString & operator =(const CTString<TLength>& value)
    {
        SetValue(value.GetValue());
        return value;
    }

    char& operator [](int k){
        return m_data[k];
    }


    const char * operator = (const char *value)
    {
        SetValue(value);
        return value;
    }


    const char * operator = (std::string& value)
    {
        SetValue(value.c_str());
        return value.c_str();
    }


    const int operator =(const int value)
    {
        char tmp[100];
        sprintf(tmp,"%*d", TLength, value);
        SetValue(tmp);
        return value;
    }


    operator const char *(void) const
    {
        return m_data;
    }


    bool operator < (const char *value) const
    {
        return strcmp(m_data,value)<0;
    }


    bool operator > (const char *value) const
    {
        return strcmp(m_data, value)>0;
    }


    bool operator <= (const char *value) const
    {
        return strcmp(m_data,value)<=0;
    }


    bool operator >= (const char *value) const
    {
        return strcmp(m_data, value)>=0;
    }


    bool operator == (const char *value) const
    {
        return strcmp(m_data, value)==0;
    }


    bool operator != (const char *value) const
    {
        return strcmp(m_data, value)!=0;
    }


    void Clear(void)
    {
        m_data[0]='\0';
    }

    const char *GetValue(void) const
    {
        return m_data;
    }

    void GetString(char *outStrBuffer) const
    {
        if (outStrBuffer != nullptr)
        {
            memcpy(outStrBuffer, m_data, TLength);
            outStrBuffer[TLength - 1]='\0';
        }
    }


    void SetValue(const char *value)
    {
        if (value == nullptr)
            m_data[0]='\0';
        else
        {
            strncpy(m_data, value, TLength);
            m_data[TLength - 1]='\0';
        }
    }


    bool IsEmpty(void) const
    {
        const char *p = m_data;
        while (*p)
        {
            if (*p!=' ')
            {
                return false;
            }
            p++;
        }
        return true;
    }


    bool IsValid(void) const
    {
        return true;
    }


    void TrimRight(void)
    {
        char *p = m_data + strlen(m_data) -1;
        while (p >= m_data)
        {
            if (*p != ' ')
            {
                break;
            }
            *p = '\0';
            p--;
        }
    }


    unsigned int Hash(unsigned int seed) const
    {
        unsigned long ret = seed;
        const char *str = m_data;

        if (*str == '\0')
        {
            return(ret);
        }
        long n = 0x100;
        while (*str)
        {
            unsigned long v = n|(*str);
            n += 0x100;
            int r = (int)((v>>2)^v)&0x0f;
            ret = (ret<<r)|(ret>>(32-r));
            ret &= 0xFFFFFFFFL;
            ret ^= v*v;
            str++;
        }
        return ((ret>>16)^ret);
    }

protected:
    char m_data[TLength];
};

typedef CTString<4> FIELD_4;
typedef CTString<5> FIELD_5;
typedef CTString<7> FIELD_7;
typedef CTString<8> FIELD_8;
typedef CTString<9> FIELD_9;
typedef CTString<10> FIELD_10;
typedef CTString<11> FIELD_11;
typedef CTString<12> FIELD_12;
typedef CTString<13> FIELD_13;
typedef CTString<16> FIELD_16;
typedef CTString<17> FIELD_17;
typedef CTString<18> FIELD_18;
typedef CTString<19> FIELD_19;
typedef CTString<20> FIELD_20;
typedef CTString<21> FIELD_21;
typedef CTString<24> FIELD_24;
typedef CTString<30> FIELD_30;
typedef CTString<31> FIELD_31;
typedef CTString<32> FIELD_32;
typedef CTString<33> FIELD_33;
typedef CTString<36> FIELD_36;
typedef CTString<40> FIELD_40;
typedef CTString<41> FIELD_41;
typedef CTString<50> FIELD_50;
typedef CTString<51> FIELD_51;
typedef CTString<61> FIELD_61;
typedef CTString<64> FIELD_64;
typedef CTString<65> FIELD_65;
typedef CTString<80> FIELD_80;
typedef CTString<81> FIELD_81;
typedef CTString<100> FIELD_100;
typedef CTString<128> FIELD_128;
typedef CTString<255> FIELD_255;
typedef CTString<256> FIELD_256;
typedef CTString<300> FIELD_300;
typedef CTString<350> FIELD_350;
typedef CTString<360> FIELD_360;
typedef CTString<400> FIELD_400;
typedef CTString<512> FIELD_512;
typedef CTString<2048> FIELD_2048;
typedef CTString<1000> FIELD_1000;
typedef CTString<2000> FIELD_2000;

typedef CTString<30> FIELD_TIME;

typedef CTString<50>  FIELD_TEMPLATE_NAME;
typedef CTString<100> FIELD_TEMPALTE_PATH;
typedef CTString<10>  PROCESS_NODE_ROLE;
typedef CTString<10>  PROCESS_NODE_TYPE;
typedef CTString<10>  FIELD_PROCESS_ID;
typedef CTString<10>  FIELD_PROCESS_TYPE;
typedef CTString<50>  FIELD_RSP_NODE_NAME;
typedef CTString<50>  FIELD_PROCESS_NODE_DESC;
typedef CTString<50>  FIELD_PROCESS_NAME;

#pragma pack(pop)

#endif
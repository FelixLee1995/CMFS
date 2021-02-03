/**
*  File: ProcSingletonCheck.h
*  Desc: ����Ӧ�ó���ĵ�·����ʵ���������
*  Created by felix on 2021/02/03
*/

#ifndef _PROCSINGLETONCHECK_H_
#define _PROCSINGLETONCHECK_H_

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#if defined(__linux__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif


#include <cstring>
#include <string>

extern int Errno;
namespace CommonTools{


#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
    void ProcSingletonCheck(const char * appName) {

        HANDLE mutex;
        mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT(appName));
        if (mutex == NULL)
        {
            printf("null and create\n");
            mutex = CreateMutex(NULL, FALSE, TEXT(appName));
        }

        else {
            printf("Failed to create process, one instance has been created!");
            throw std::logic_error( "Failed to create process, one instance has been created!" );
        }
    }
#endif


#ifdef __linux__

    void ProcSingletonCheck(const char *appName)
    {
        char szPidFile[128];
        sprintf(szPidFile, "/var/run/%s.pid", appName);
        /* ��ȡ�ļ������� */
        char str[32];


        int pidfile = open(szPidFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (pidfile < 0)
        {
            printf("pidfile is %d", pidfile);
            char * mesg = strerror(Errno);
            printf("Mesg:%s\n",mesg);
            exit(1);
        }

        /* �����ļ������ʧ����˵���ļ��ѱ���������һ���������еĽ��̣�����ֱ���˳� */
        if (lockf(pidfile, F_TLOCK, 0) < 0)
        {
            printf("Failed to create process, one instance has been created!");
            exit(0);
        }

        /* �����ļ��ɹ��󣬻�һֱ�����������֪�������˳��������ֶ� close �ļ�
       Ȼ�󽫽��̵Ľ��̺�д�뵽 pid �ļ�*/
        sprintf(str, "%d\n", getpid()); // \n is a symbol.
        ssize_t len = std::strlen(str);
        ssize_t ret = write(pidfile, str, len);
        if (ret != len)
        {
            fprintf(stderr, "Can't Write Pid File: %s", szPidFile);
            exit(0);
        }
    }

#endif


}



#endif //_PROCSINGLETONCHECK_H_

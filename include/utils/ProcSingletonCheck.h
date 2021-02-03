/**
*  File: ProcSingletonCheck.h
*  Desc: 用于应用程序的单路径单实例启动检查
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
        /* 获取文件描述符 */
        char str[32];


        int pidfile = open(szPidFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (pidfile < 0)
        {
            printf("pidfile is %d", pidfile);
            char * mesg = strerror(Errno);
            printf("Mesg:%s\n",mesg);
            exit(1);
        }

        /* 锁定文件，如果失败则说明文件已被锁，存在一个正在运行的进程，程序直接退出 */
        if (lockf(pidfile, F_TLOCK, 0) < 0)
        {
            printf("Failed to create process, one instance has been created!");
            exit(0);
        }

        /* 锁定文件成功后，会一直持有这把锁，知道进程退出，或者手动 close 文件
       然后将进程的进程号写入到 pid 文件*/
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

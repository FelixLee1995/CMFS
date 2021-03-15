#ifndef __NETTRANS_H__
#define __NETTRANS_H__
#include <stdint.h>

#define SWAP16(a) (((a)>>8)|((a)<<8))
#define SWAP32(a) ((a=(((a)>>16)|((a)<<16))),(((a)>>8)&0x00ff00ff)|(((a)<<8)&0xff00ff00))
#define SWAP64(a) ((a=(((a)>>32)|((a)<<32))),(a=((((a)>>16)&0x0000ffff0000ffff)|(((a)<<16)&0xffff0000ffff0000))),((((a)>>8)&0x00ff00ff00ff00ff)|(((a)<<8)&0xff00ff00ff00ff00)))

#ifdef __GLIBC__
#if __GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 9)
# if defined(__BYTE_ORDER) && defined(__BIG_ENDIAN)
#  if __BYTE_ORDER == __LITTLE_ENDIAN
#   define LITTLEENDIAN 1
#  else
#   define LITTLEENDIAN 0
#  endif
# else
#  error "Couldn't determine byte-order on this platform."
# endif
#endif
#endif

#ifndef LITTLEENDIAN
#define LITTLEENDIAN 1
#endif

#if LITTLEENDIAN
inline uint16_t MY_NTOHS(uint16_t a){ return SWAP16(a);}
inline uint16_t MY_HTONS(uint16_t a){ return SWAP16(a);}
#ifdef WIN32
inline uint32_t MY_NTOHL(uint32_t a){ return SWAP32(a);}
inline uint32_t MY_HTONL(uint32_t a){ return SWAP32(a);}
//inline uint32_t MY_NTOHL(uint32_t a){ return ((a<<24)&0xff000000)|((a<<8)&0x00ff0000)|((a>>24)&0x000000ff)|((a>>8)&0x0000ff00);}
inline uint64_t MY_NTOHLL(uint64_t a){ return SWAP64(a);}
inline double MY_NTOHF(double a){  union{double f;uint64_t n;} t={a};t.n=SWAP64(t.n);return t.f;}
inline double MY_HTONF(double a){  union{double f;uint64_t n;} t={a};t.n=SWAP64(t.n);return t.f;}
/*inline uint64_t MY_NTOHLL(uint64_t a){ return 
	((a<<56)&0xff00000000000000)|
	((a<<40)&0x00ff000000000000)|
	((a<<24)&0x0000ff0000000000)|
	((a<<8)&0x000000ff00000000)|
	((a>>56)&0x00000000000000ff)|
	((a>>40)&0x000000000000ff00)|
	((a>>24)&0x0000000000ff0000)|
	((a>>8)&0x00000000ff000000);
}*/
inline uint64_t MY_HTONLL(uint64_t a){ return SWAP64(a);}
#else

#include <arpa/inet.h>
//#define MY_NTOHS(a) __builtin_bswap16(a)
//#define MY_HTONS(a) __builtin_bswap16(a)
#define MY_NTOHL(a) ((uint32_t)__builtin_bswap32(a))
#define MY_HTONL(a) ((uint32_t)__builtin_bswap32(a))
inline uint64_t MY_NTOHLL(uint64_t a){return __builtin_bswap64(a);}
inline uint64_t MY_HTONLL(uint64_t a){return __builtin_bswap64(a);}
inline double MY_NTOHF(double a){  union{double f;uint64_t n;} t={a};t.n=MY_NTOHLL(t.n);return t.f;}
inline double MY_HTONF(double a){  union{double f;uint64_t n;} t={a};t.n=MY_HTONLL(t.n);return t.f;}
#endif

#else
#define MY_NTOHS(a) (a)
#define MY_NTOHL(a) (a)
#define MY_NTOHLL(a) (a)
#define MY_HTONS(a) (a)
#define MY_HTONL(a) (a)
#define MY_HTONLL(a) (a)
#endif//LITTLEENDIAN
#endif //__NETTRANS_H__

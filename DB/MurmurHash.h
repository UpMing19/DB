#pragma once

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;


#else

#include<stdint.h>

#endif


void MurmurHash3_x86_32(const void *key,int len,uint32_t seed,void *out);
void MurmurHash3_x86_128(const void *key,int len,uint32_t seed,void *out);
void MurmurHash3_x64_128(const void *key,int len,uint32_t seed,void *out);

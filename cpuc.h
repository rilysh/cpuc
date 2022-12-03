#include <cpuid.h>

#ifndef _CPUC_H_
#define _CPUC_H_
/**
 * Note: There are other similar bit flags exist om cpuid.h.
*/
#ifndef bit_FMA3
#   define bit_FMA3 (1 << 12)
#endif
#ifndef bit_AVX512VPCLMUL
#   define bit_AVX512VPCLMUL (1 << 10)
#endif
#ifndef bit_X64
#   define bit_X64 (1 << 29)
#endif
#ifndef bit_PREFETCHW
#   define bit_PREFETCHW (1 << 8)
#endif
/* clang doesn't have bit_PREFETCHWT1, so we've define that. */
#ifndef bit_PREFETCHWT1
#   define bit_PREFETCHWT1 (1 << 0)
#endif

#define _XFEATURE_ENABLED_MASK 0

void x86_cpuid(signed int out[4], signed int eax, signed int ecx)
{
    __cpuid_count(eax, ecx, out[0], out[1], out[2], out[3]);
}

unsigned long __xgetbv(unsigned int idx)
{
    unsigned int eax, edx;
    __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(idx));
    return ((unsigned long)edx << 32) | eax;
}

static char *wrap_ret(int ret)
{
    if (ret)
        return "\033[0;92mYes\033[0m";
    else
        return "\033[0;91mNo\033[0m";
}

static char auth_vendor[13];
static int HW_MMX;
static int HW_SSE;
static int HW_SSE2;
static int HW_SSE3;
static int HW_SSSE3;
static int HW_SSE41;
static int HW_SSE42;
static int HW_AES;
static int HW_AVX;
static int HW_FMA3;
static int HW_RDRAND;
static int HW_XSAVE;
static int HW_XSAVEOPT;
static int HW_F16C;

static int HW_AVX2;
static int HW_BMI;
static int HW_BMI2;
static int HW_ADX;
static int HW_MPX;
static int HW_SHA;
static int HW_RDSEED;
static int HW_PREFETCHWT1;
static int HW_RDPID;
static int HW_AVX512_F;
static int HW_AVX512_CD;
static int HW_AVX512_PF;
static int HW_AVX512_ER;
static int HW_AVX512_VL;
static int HW_AVX512_BW;
static int HW_AVX512_DQ;
static int HW_AVX512_IFMA;
static int HW_AVX512_VBMI;
static int HW_AVX512_VPOPCNTDQ;
static int HW_AVX512_4FMAPS;
static int HW_AVX512_4VNNIW;
static int HW_AVX512_VNNI;
static int HW_AVX512_VBMI2;
static int HW_GFNI;
static int HW_VAES;
static int HW_AVX512_VPCLMUL;
static int HW_AVX512_BITALG;
static int HW_AVX512_BF16;

static int HW_x64;
static int HW_ABM;
static int HW_SSE4a;
static int HW_FMA4;
static int HW_XOP;
static int HW_PREFETCHW;
static int HW_LAHFLM;
#endif /* _CPUC_H_ */

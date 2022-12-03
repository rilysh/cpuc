#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#   include "cpuc.h"
#else
#   error "CPU architecture doesn't support X86 instruction set"
#endif

#define OS_AVX_OK       0x6
#define OS_AVX512_OK    0xE6
#define AMD64_LIMIT     0xFFFFFFFFFFFFFFFF

static void menu_help()
{
    fprintf(stdout,
        "CPUC\n\nUsage:\n"
        "cpuc --all (Prints all detectable CPU extensions)\n"
        "cpuc --info (Prints detectable general CPU extensions)\n"
        "cpuc --features (Prints detectable CPU features)\n"
        "cpuc --simd128 (Prints detectable SIMD 128-bit extension)\n"
        "cpuc --simd256 (Prints detectable SIMD 256-bit extension)\n"
        "cpuc --simd512 (Prints detectable SIMD 512-bit extension)\n"
    );
}

static int os_amd64()
{
    if (UINTPTR_MAX == AMD64_LIMIT)
        return 1;
    else
        return 0;
}

static int os_AVX()
{
    int avx_capable = 0;
    int cpu_info[4];
    
    x86_cpuid(cpu_info, 1, 0);

    int os_hv_XSAVE_XRSTORE = (cpu_info[2] & (1 << 27));
    int cpu_avx_capable = (cpu_info[2] & (1 << 28));

    if (os_hv_XSAVE_XRSTORE && cpu_avx_capable) {
        unsigned long xcr_mask = __xgetbv(_XFEATURE_ENABLED_MASK);
        avx_capable = xcr_mask & OS_AVX_OK;
    }

    return avx_capable == OS_AVX_OK;
}

static int os_AVX512()
{
    if (!os_AVX()) {
        return -1;
    }

    int avx512_capable = 0;
    unsigned long xcr_mask = __xgetbv(_XFEATURE_ENABLED_MASK);

    avx512_capable = xcr_mask & OS_AVX512_OK;
    return avx512_capable == OS_AVX512_OK;
}

static char *cpu_vendor()
{
    signed int cpu_info[4];
    x86_cpuid(cpu_info, 0, 0);
    memcpy(auth_vendor + 0, &cpu_info[1], 4);
    memcpy(auth_vendor + 4, &cpu_info[3], 4);
    memcpy(auth_vendor + 8, &cpu_info[2], 4);

    /* pad with '\0' at the end */
    auth_vendor[12] = '\0';

    return auth_vendor;
}

static void host_info()
{
    int info[4];
    x86_cpuid(info, 0, 0);
    
    int info0 = info[0];

    x86_cpuid(info, 0x80000000, 0);

    unsigned int linfo0 = info[0];
    
    if (info0 >= 0x00000001) {
        x86_cpuid(info, 0x00000001, 0);
        
        HW_MMX      = (info[3] & bit_MMX);

        HW_SSE      = (info[3] & bit_SSE);
        HW_SSE2     = (info[3] & bit_SSE2);
        HW_SSE3     = (info[3] & bit_SSE3);
        HW_SSSE3    = (info[2] & bit_SSSE3);
        HW_SSE41    = (info[2] & bit_SSE4_1);
        HW_SSE42    = (info[2] & bit_SSE4_2);
        
        HW_AES      = (info[2] & bit_AES);
        HW_AVX      = (info[2] & bit_AVX);
        HW_FMA3     = (info[2] & bit_FMA3);
        HW_RDRAND   = (info[2] & bit_RDRND);
        HW_XSAVE    = (info[2] & bit_XSAVE);
        HW_XSAVEOPT = (info[2] & bit_XSAVEOPT);

        HW_F16C     = (info[2] & bit_F16C);
    }

    if (info0 >= 0x00000007) {
        x86_cpuid(info, 0x00000007, 0);
        
        HW_AVX2                 = (info[1] & bit_AVX2);
        HW_BMI                  = (info[1] & bit_BMI);
        HW_BMI2                 = (info[1] & bit_BMI2);
        HW_ADX                  = (info[1] & bit_ADX);
        HW_MPX                  = (info[1] & bit_MPX);
        HW_SHA                  = (info[1] & bit_SHA);
        HW_RDSEED               = (info[1] & bit_RDSEED);
        HW_PREFETCHWT1          = (info[2] & bit_PREFETCHWT1);
        HW_RDPID                = (info[2] & bit_RDPID);

        HW_AVX512_F             = (info[1] & bit_AVX512F);
        HW_AVX512_CD            = (info[1] & bit_AVX512CD);
        HW_AVX512_PF            = (info[1] & bit_AVX512PF);
        HW_AVX512_ER            = (info[1] & bit_AVX512ER);
        HW_AVX512_VL            = (info[1] & bit_AVX512VL);
        HW_AVX512_BW            = (info[1] & bit_AVX512BW);
        HW_AVX512_DQ            = (info[1] & bit_AVX512DQ);
        HW_AVX512_IFMA          = (info[1] & bit_AVX512IFMA);
        HW_AVX512_VBMI          = (info[2] & bit_AVX512VBMI);
        HW_AVX512_VPOPCNTDQ     = (info[2] & bit_AVX512VPOPCNTDQ);
        HW_AVX512_4FMAPS        = (info[3] & bit_AVX5124FMAPS);
        HW_AVX512_4VNNIW        = (info[3] & bit_AVX5124VNNIW);
        HW_AVX512_VNNI          = (info[2] & bit_AVX512VNNI);
        HW_AVX512_VBMI2         = (info[2] & bit_AVX512VBMI2);

        HW_GFNI                 = (info[2] & bit_GFNI);
        HW_VAES                 = (info[2] & bit_VAES);
        HW_AVX512_VPCLMUL       = (info[2] & bit_AVX512VPCLMUL);
        HW_AVX512_BITALG        = (info[2] & bit_AVX512BITALG);

        x86_cpuid(info, 0x00000007, 1);
        
        HW_AVX512_BF16          = (info[0] & bit_AVX512BF16);
    }

    /* These are extended features. See: https://github.com/gcc-mirror/gcc/blob/e55251f3d3598ca6e001c4afa51b2ec913b4cac1/gcc/config/i386/cpuid.h#L66*/
    if (linfo0 >= 0x80000001) {
        x86_cpuid(info, 0x80000001, 0);

        HW_x64          = (info[3] & bit_X64);
        HW_ABM          = (info[2] & bit_ABM);
        HW_SSE4a        = (info[2] & bit_SSE4a);
        HW_FMA4         = (info[2] & bit_FMA4);
        HW_XOP          = (info[2] & bit_XOP);
        HW_PREFETCHW    = (info[2] & bit_PREFETCHW);
        HW_LAHFLM       = (info[2] & bit_LAHF_LM);
    }
}

static void flag_info()
{
    host_info();
    fprintf(stdout,
        "Info:\n"
        "   CPU Vendor  \033[0;97m%s\033[0m\n"
        "   64-bit      [%s]\n"
        "   AVX Prop    [%s]\n"
        "   AVX512 Prop [%s]\n"
        , cpu_vendor(), wrap_ret(os_amd64()),
        wrap_ret(os_AVX() && HW_AVX), wrap_ret(os_AVX512() && HW_AVX512_F)
    );
}

static void flag_features()
{
    host_info();
    fprintf(stdout,
        "CPU Features:\n"
        "   AVX         [%s]\n"
        "   AVX512      [%s]\n"
        "   MMX         [%s]\n"
        "   x64         [%s]\n"
        "   ABM         [%s]\n"
        "   RDRAND      [%s]\n"
        "   RDSEED      [%s]\n"
        "   BMI1        [%s]\n"
        "   BMI2        [%s]\n"
        "   ADX         [%s]\n"
        "   MPX         [%s]\n"
        "   PREFETCHW   [%s]\n"
        "   PREFETCHWT1 [%s]\n"
        "   LAHF_LM     [%s]\n"
        "   RDPID       [%s]\n"
        "   GFNI        [%s]\n"
        "   F16C        [%s]\n"
        "   VAES        [%s]\n"
        "   XSAVE       [%s]\n"
        "   XSAVEOPT    [%s]\n"
        , wrap_ret(os_AVX()), wrap_ret(os_AVX512()),
        wrap_ret(HW_MMX), wrap_ret(HW_x64),
        wrap_ret(HW_ABM), wrap_ret(HW_RDRAND),
        wrap_ret(HW_RDSEED), wrap_ret(HW_BMI),
        wrap_ret(HW_BMI2), wrap_ret(HW_ADX),
        wrap_ret(HW_MPX), wrap_ret(HW_PREFETCHW),
        wrap_ret(HW_PREFETCHWT1), wrap_ret(HW_LAHFLM),
        wrap_ret(HW_RDPID), wrap_ret(HW_F16C),
        wrap_ret(HW_GFNI), wrap_ret(HW_VAES),
        wrap_ret(HW_XSAVE), wrap_ret(HW_XSAVEOPT)
    );
}

static void flag_simd128()
{
    host_info();
    fprintf(stdout,
        "SIMD 128-bit:\n"
        "   SSE         [%s]\n"
        "   SSE2        [%s]\n"
        "   SSE3        [%s]\n"
        "   SSSE3       [%s]\n"
        "   SSE4a       [%s]\n"
        "   SSE4.1      [%s]\n"
        "   SSE4.2      [%s]\n"
        "   AES-NI      [%s]\n"
        "   SHA         [%s]\n"
        , wrap_ret(HW_SSE), wrap_ret(HW_SSE2),
        wrap_ret(HW_SSE3), wrap_ret(HW_SSSE3),
        wrap_ret(HW_SSE4a), wrap_ret(HW_SSE41),
        wrap_ret(HW_SSE42), wrap_ret(HW_AES),
        wrap_ret(HW_SHA)
    );
}

static void flag_simd256()
{
    host_info();
    fprintf(stdout,
        "SIMD 256-bit:\n"
        "   AVX         [%s]\n"
        "   XOP         [%s]\n"
        "   FMA3        [%s]\n"
        "   FMA4        [%s]\n"
        "   AVX2        [%s]\n"
        , wrap_ret(HW_AVX), wrap_ret(HW_XOP),
        wrap_ret(HW_FMA3), wrap_ret(HW_FMA4),
        wrap_ret(HW_AVX2)
    );
}

static void flag_simd512()
{
    host_info();
    fprintf(stdout,
        "SIMD 512-bit:\n"
        "   AVX512-F            [%s]\n"
        "   AVX512-CD           [%s]\n"
        "   AVX512-PF           [%s]\n"
        "   AVX512-ER           [%s]\n"
        "   AVX512-VL           [%s]\n"
        "   AVX512-BW           [%s]\n"
        "   AVX512-DQ           [%s]\n"
        "   AVX512-IFMA         [%s]\n"
        "   AVX512-VBMI         [%s]\n"
        "   AVX512-VPOPCNTDQ    [%s]\n"
        "   AVX512-4FMAPS       [%s]\n"
        "   AVX512-4VNNIW       [%s]\n"
        "   AVX512-VBMI2        [%s]\n"
        "   AVX512-VPCLMUL      [%s]\n"
        "   AVX512-VNNI         [%s]\n"
        "   AVX512-BITALG       [%s]\n"
        "   AVX512-BF16         [%s]\n"
        , wrap_ret(HW_AVX512_F), wrap_ret(HW_AVX512_CD),
        wrap_ret(HW_AVX512_PF), wrap_ret(HW_AVX512_ER),
        wrap_ret(HW_AVX512_VL), wrap_ret(HW_AVX512_BW),
        wrap_ret(HW_AVX512_DQ), wrap_ret(HW_AVX512_IFMA),
        wrap_ret(HW_AVX512_VBMI), wrap_ret(HW_AVX512_VPOPCNTDQ),
        wrap_ret(HW_AVX512_4FMAPS), wrap_ret(HW_AVX512_4VNNIW),
        wrap_ret(HW_AVX512_VBMI2), wrap_ret(HW_AVX512_VPCLMUL),
        wrap_ret(HW_AVX512_VNNI), wrap_ret(HW_AVX512_BITALG),
        wrap_ret(HW_AVX512_BF16)
    );
}

static void flag_all()
{
    host_info();
    fprintf(stdout,
        "Info:\n"
        "   CPU Vendor          \033[0;97m%s\033[0m\n"
        "   64-bit              [%s]\n"
        "   AVX Prop            [%s]\n"
        "   AVX512 Prop         [%s]\n\n"
        "CPU Features:\n"
        "   AVX                 [%s]\n"
        "   AVX512              [%s]\n"
        "   MMX                 [%s]\n"
        "   x64                 [%s]\n"
        "   ABM                 [%s]\n"
        "   RDRAND              [%s]\n"
        "   RDSEED              [%s]\n"
        "   BMI1                [%s]\n"
        "   BMI2                [%s]\n"
        "   ADX                 [%s]\n"
        "   MPX                 [%s]\n"
        "   PREFETCHW           [%s]\n"
        "   PREFETCHWT1         [%s]\n"
        "   LAHF_LM             [%s]\n"
        "   RDPID               [%s]\n"
        "   GFNI                [%s]\n"
        "   F16C                [%s]\n"
        "   VAES                [%s]\n"
        "   XSAVE               [%s]\n"
        "   XSAVEOPT            [%s]\n\n"
        "SIMD 128-bit:\n"
        "   SSE                 [%s]\n"
        "   SSE2                [%s]\n"
        "   SSE3                [%s]\n"
        "   SSSE3               [%s]\n"
        "   SSE4a               [%s]\n"
        "   SSE4.1              [%s]\n"
        "   SSE4.2              [%s]\n"
        "   AES-NI              [%s]\n"
        "   SHA                 [%s]\n\n"
        "SIMD 256-bit:\n"
        "   AVX                 [%s]\n"
        "   XOP                 [%s]\n"
        "   FMA3                [%s]\n"
        "   FMA4                [%s]\n"
        "   AVX2                [%s]\n\n"
        "SIMD 512-bit:\n"
        "   AVX512-F            [%s]\n"
        "   AVX512-CD           [%s]\n"
        "   AVX512-PF           [%s]\n"
        "   AVX512-ER           [%s]\n"
        "   AVX512-VL           [%s]\n"
        "   AVX512-BW           [%s]\n"
        "   AVX512-DQ           [%s]\n"
        "   AVX512-IFMA         [%s]\n"
        "   AVX512-VBMI         [%s]\n"
        "   AVX512-VPOPCNTDQ    [%s]\n"
        "   AVX512-4FMAPS       [%s]\n"
        "   AVX512-4VNNIW       [%s]\n"
        "   AVX512-VBMI2        [%s]\n"
        "   AVX512-VPCLMUL      [%s]\n"
        "   AVX512-VNNI         [%s]\n"
        "   AVX512-BITALG       [%s]\n"
        "   AVX512-BF16         [%s]\n"
        , cpu_vendor(), wrap_ret(os_amd64()),
        wrap_ret(os_AVX() && HW_AVX), wrap_ret(os_AVX512() && HW_AVX512_F),

        wrap_ret(os_AVX()), wrap_ret(os_AVX512()),
        wrap_ret(HW_MMX), wrap_ret(HW_x64),
        wrap_ret(HW_ABM), wrap_ret(HW_RDRAND),
        wrap_ret(HW_RDSEED), wrap_ret(HW_BMI),
        wrap_ret(HW_BMI2), wrap_ret(HW_ADX),
        wrap_ret(HW_MPX), wrap_ret(HW_PREFETCHW),
        wrap_ret(HW_PREFETCHWT1), wrap_ret(HW_LAHFLM),
        wrap_ret(HW_RDPID), wrap_ret(HW_GFNI),
        wrap_ret(HW_F16C), wrap_ret(HW_VAES),
        wrap_ret(HW_XSAVE), wrap_ret(HW_XSAVEOPT),

        wrap_ret(HW_SSE), wrap_ret(HW_SSE2),
        wrap_ret(HW_SSE3), wrap_ret(HW_SSSE3),
        wrap_ret(HW_SSE4a), wrap_ret(HW_SSE41),
        wrap_ret(HW_SSE42), wrap_ret(HW_AES),
        wrap_ret(HW_SHA),

        wrap_ret(HW_AVX), wrap_ret(HW_XOP),
        wrap_ret(HW_FMA3), wrap_ret(HW_FMA4),
        wrap_ret(HW_AVX2),

        wrap_ret(HW_AVX512_F), wrap_ret(HW_AVX512_CD),
        wrap_ret(HW_AVX512_PF), wrap_ret(HW_AVX512_ER),
        wrap_ret(HW_AVX512_VL), wrap_ret(HW_AVX512_BW),
        wrap_ret(HW_AVX512_DQ), wrap_ret(HW_AVX512_IFMA),
        wrap_ret(HW_AVX512_VBMI), wrap_ret(HW_AVX512_VPOPCNTDQ),
        wrap_ret(HW_AVX512_4FMAPS), wrap_ret(HW_AVX512_4VNNIW),
        wrap_ret(HW_AVX512_VBMI2), wrap_ret(HW_AVX512_VPCLMUL),
        wrap_ret(HW_AVX512_VNNI), wrap_ret(HW_AVX512_BITALG),
        wrap_ret(HW_AVX512_BF16)
    );
}

int main(int argc, char **argv)
{
    if (argc >= 2) {
        if (strcmp(argv[1], "--h") == 0
            || strcmp(argv[1], "--help") == 0) {
            menu_help();
        } else if (strcmp(argv[1], "--info") == 0) {
            flag_info();
        } else if (strcmp(argv[1], "--features") == 0) {
            flag_features();
        } else if (strcmp(argv[1], "--simd128") == 0) {
            flag_simd128();
        } else if (strcmp(argv[1], "--simd256") == 0) {
            flag_simd256();
        } else if (strcmp(argv[1], "--simd512") == 0) {
            flag_simd512();
        } else if (strcmp(argv[1], "--all") == 0) {
            flag_all();
        } else {
            fprintf(stderr, "error: wrong arguments.\n");
            exit(1);
        }
    } else {
        menu_help();
        exit(0);
    }
    exit(0);
}

#ifndef _COMPILER_LIBS_H
#define _COMPILER_LIBS_H

#if defined ( __VSCODE )

        #ifndef __ASM
            #define __ASM
        #endif

        #ifndef __INLINE
            #define __INLINE
        #endif

        #ifndef __WEAK
            #define __WEAK
        #endif

        #ifndef __ALIGN
            #define __ALIGN(n)
        #endif

        #ifndef __PACKED
            #define __PACKED
        #endif

        #ifndef __STATIC_INLINE
            #define __STATIC_INLINE
        #endif

        #ifndef GET_SP
            #define GET_SP()
        #endif

#elif defined ( __CC_ARM )

        #ifndef __ASM
            #define __ASM               __asm
        #endif

        #ifndef __INLINE
            #define __INLINE            __inline
        #endif

        #ifndef __WEAK
            #define __WEAK              __weak
        #endif

        #ifndef __ALIGN
            #define __ALIGN(n)          __align(n)
        #endif

        #ifndef __PACKED
            #define __PACKED            __packed
        #endif

        #ifndef __STATIC_INLINE
            #define __STATIC_INLINE static __inline
        #endif

        #ifndef GET_SP
            #define GET_SP()                __current_sp()
        #endif

#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

        #ifndef __ASM
            #define __ASM               __asm
        #endif

        #ifndef __INLINE
            #define __INLINE            __inline
        #endif

        #ifndef __WEAK
            #define __WEAK              __attribute__((weak))
        #endif

        #ifndef __ALIGN
            #define __ALIGN(n)          __attribute__((aligned(n)))
        #endif

        #ifndef __PACKED
            #define __PACKED            __attribute__((packed, aligned(1)))
        #endif

        #ifndef __STATIC_INLINE
            #define __STATIC_INLINE static __inline
        #endif

        #ifndef GET_SP
            #define GET_SP()                __current_sp()
        #endif

#elif defined ( __ICCARM__ )

        #ifndef __ASM
            #define __ASM               __asm
        #endif

        #ifndef __INLINE
            #define __INLINE            inline
        #endif

        #ifndef __WEAK
            #define __WEAK              __weak
        #endif

        #ifndef __ALIGN
            #define STRING_PRAGMA(x) _Pragma(#x)
            #define __ALIGN(n) STRING_PRAGMA(data_alignment = n)
        #endif

        #ifndef __PACKED
            #define __PACKED            __packed
        #endif

        #ifndef __STATIC_INLINE
            #define __STATIC_INLINE static inline
        #endif


        #ifndef GET_SP
            #define GET_SP()                __get_SP()
        #endif

#elif defined   ( __GNUC__ )

        #ifndef __ASM
            #define __ASM               __asm
        #endif

        #ifndef __INLINE
            #define __INLINE            inline
        #endif

        #ifndef __WEAK
            #define __WEAK              __attribute__((weak))
        #endif

        #ifndef __ALIGN
            #define __ALIGN(n)          __attribute__((aligned(n)))
        #endif

        #ifndef __PACKED
            #define __PACKED           __attribute__((packed))
        #endif

        #ifndef __STATIC_INLINE
            #define __STATIC_INLINE static inline
        #endif

        #ifndef GET_SP
            #define GET_SP()                __get_MSP()
        #endif

#elif defined   ( __TASKING__ )

        #ifndef __ASM
            #define __ASM               __asm
        #endif

        #ifndef __INLINE
            #define __INLINE            inline
        #endif

        #ifndef __WEAK
            #define __WEAK              __attribute__((weak))
        #endif

        #ifndef __ALIGN
            #define __ALIGN(n)          __align(n)
        #endif

        /* Not defined for TASKING. */
        #ifndef __PACKED
            #define __PACKED
        #endif

        #ifndef GET_SP
            #define GET_SP()                __get_MSP()
        #endif
#endif

#endif //_COMPILER_LIBS_H

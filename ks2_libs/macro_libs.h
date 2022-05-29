
#ifndef __MACRO_LIBS_H
#define __MACRO_LIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#define KS2_POWER_OF_TWO(A) ( ((A) != 0) && ((((A) - 1) & (A)) == 0) )

#define KS2_MAKE_UINT16(p)  ((uint16_t)(*(p))|(((uint16_t)(*((uint8_t*)(p)+1))))<<8)

#define KS2_ARRAY_SIZE(a)  (sizeof(a) / sizeof(a[0]))

#define KS2_CONCAT2_(x,y) x ## y
#define KS2_CONCAT2(x,y) KS2_CONCAT2_(x,y)

#define KS2_CONCAT3_(x,y,z) x ## y ## z
#define KS2_CONCAT3(x,y,z) KS2_CONCAT3_(x,y,z)

#ifdef __cplusplus
}
#endif

#endif    /*__MACRO_LIBS_H*/



















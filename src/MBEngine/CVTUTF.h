/* CVTUTF.H - Unicode, Inc. conversions between UCS-4, UTF-16 and UTF-8 (Mark E. Davis, 1994-1996). */
#ifndef CVTUTF_H
#define CVTUTF_H

typedef unsigned long UCS4;
typedef unsigned short UTF16;
typedef unsigned char UTF8;

enum {
    cvtok = 0,              /* conversion successful */
    sourceExhausted = 1,    /* partial character in source, but hit end */
    targetExhausted = 2     /* insuff. room in target for conversion */
};
typedef int ConversionResult;

extern const UCS4 kReplacementCharacter;
extern const UCS4 kMaximumUCS2;
extern const UCS4 kMaximumUTF16;
extern const UCS4 kMaximumUCS4;

#ifdef __cplusplus
extern "C" {
#endif
ConversionResult ConvertUCS4toUTF16(UCS4 **sourceStart, const UCS4 *sourceEnd, UTF16 **targetStart, const UTF16 *targetEnd);
ConversionResult ConvertUTF16toUCS4(UTF16 **sourceStart, UTF16 *sourceEnd, UCS4 **targetStart, const UCS4 *targetEnd);
ConversionResult ConvertUTF16toUTF8(UTF16 **sourceStart, const UTF16 *sourceEnd, UTF8 **targetStart, const UTF8 *targetEnd);
ConversionResult ConvertUTF8toUTF16(UTF8 **sourceStart, UTF8 *sourceEnd, UTF16 **targetStart, const UTF16 *targetEnd);
ConversionResult ConvertUCS4toUTF8(UCS4 **sourceStart, const UCS4 *sourceEnd, UTF8 **targetStart, const UTF8 *targetEnd);
ConversionResult ConvertUTF8toUCS4(UTF8 **sourceStart, UTF8 *sourceEnd, UCS4 **targetStart, const UCS4 *targetEnd);
#ifdef __cplusplus
}
#endif

#endif

#ifndef FFMPEGS_H
#define FFMPEGS_H

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
extern "C" {
#include <stdint.h>
}
#endif

#ifndef   UINT64_C
#define   UINT64_C(value)   __CONCAT(value,ULL)
#endif

extern "C" {
    #include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pixFmt;
    int fps;
} VideoEncodeSpec ;


class FFmpegs
{
public:
    FFmpegs();

    static void h264Encode(VideoEncodeSpec &in,const char * outFilename);
};

#endif // FFMPEGS_H

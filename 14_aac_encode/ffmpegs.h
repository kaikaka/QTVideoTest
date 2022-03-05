#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
    #include <libavformat/avformat.h>
}

typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} AudioEncodeSpc;

class FFmpegs
{
public:
    FFmpegs();

    static void aacEncode(AudioEncodeSpc &in,const char *outFilename);
};

#endif // FFMPEGS_H

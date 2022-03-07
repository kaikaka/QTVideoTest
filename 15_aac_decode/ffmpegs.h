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
} AudioDecodeSpc;

class FFmpegs
{
public:
    FFmpegs();

    static void aacDecode(AudioDecodeSpc &out,const char *inFilename);
};

#endif // FFMPEGS_H

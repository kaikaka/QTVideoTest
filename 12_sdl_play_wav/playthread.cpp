#include "playthread.h"
#include <SDL2/SDL.h>
#include <QDebug>
#include <QFile>
//采样率
#define SAMPLE_RATE 44100
//采样格式
#define SAMPLE_FORMAT AUDIO_S16LSB
//采样大小
#define SAMPLE_SIZE SDL_AUDIO_BITSIZE(SAMPLE_FORMAT)

//声道数
#define CHANNELS 2
//音频缓冲区的样本数量
#define SAMPLES 1024
//每个样本占用多少个字节 (采样大小 * 声道数 / 8)
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE * CHANNELS) >> 3)
//文件缓冲区的大小 (样本 * 采样大小)
#define BUFFER_SIZE (SAMPLES * BYTES_PER_SAMPLE)

#define FILENAME "/Users/yinxiangkai/Desktop/test/01.wav"

typedef struct {
    int len = 0;//全部数据
    int pullLen = 0;//已经读取的数据
    Uint8 *data = nullptr;
} AudioBuffer ;

playThread::playThread(QObject *parent) : QThread(parent)
{
    connect(this, &playThread::finished,
            this,&playThread::deleteLater);
}


playThread::~playThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << this << "析构";
}

void pull_audio_data(void *userdata,
                     Uint8 * stream,
                     int len) {
    //等待音频设备回调(多次)
    //stream 需要往stream中填充pcm数据
    //len 希望填充的大小(samples * format * channels / 8)
    qDebug() << "pull_audio_data" << len;
    //清空stream (静音降噪处理)
    SDL_memset(stream,0,len);

    //取出audiobuffer
    AudioBuffer *buffer = (AudioBuffer *)userdata;
    if (buffer->len <= 0) return;
    //取len、bufferLen的最小值（为了保证数据安全，防止指针越界）
    buffer->pullLen = (len > buffer->len) ? buffer->len : len;
    SDL_MixAudio(stream,buffer->data,buffer->pullLen,SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}

//run 是qthread 虚函数 只需要实现就行了
void playThread::run() {
    /*SDL播放音频有2种模式
     * push(推)：【程序】主动推送数据给【音频设备】
     * pull(拉)：【音频设备】主动向【程序】拉去数据
    */
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug() << "sdl_init error" << SDL_GetError();
        return;
    }
    //加载wav文件
    SDL_AudioSpec spec;

    //存放从文件中读取的数据
    Uint8 *data = nullptr;
    Uint32 len = 0;
    //loadwav数据并给spec，data，len赋值
    if (!SDL_LoadWAV(FILENAME,&spec,&data,&len)) {
        qDebug() << "SDL_LoadWAV error" << SDL_GetError();
                // 清除所有的子系统
                SDL_Quit();
                return;
    }
    //samples，callback 要放到loadwav后面
    spec.samples = SAMPLES;
    spec.callback = pull_audio_data;
    //回调参数
    AudioBuffer buffer;
    buffer.data = data;
    buffer.len = len;

    spec.userdata = &buffer;

    //打开设备
    if (SDL_OpenAudio(&spec,nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        SDL_Quit();
        return;
    }
    //打开文件
    QFile file(FILENAME);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "file open error" << SDL_GetError();
        SDL_Quit();
        return;
    }
    //开始播放(0是取消暂停)
    SDL_PauseAudio(0);

    int sampleSize = SDL_AUDIO_BITSIZE(spec.format);
    int bytesPerSample = (sampleSize * spec.channels) >> 3;
    //如果一直有数据
    while (!isInterruptionRequested()) {
        //只要从文件中读取的音频数据，还没有填充完毕就跳过
        if (buffer.len > 0) continue;
        //文件读取完毕
        if (buffer.len <= 0) {
            //剩余的样本数量
            int samples = buffer.pullLen / bytesPerSample;
            int ms = samples * 1000 / spec.freq;
            SDL_Delay(ms);
            break;
        }
        //读取到文件数据
        buffer.data = data;
    }
    //释放文件数据/关闭设备/清除所有的子系统
    SDL_FreeWAV(data);
    SDL_CloseAudio();
    SDL_Quit();
}

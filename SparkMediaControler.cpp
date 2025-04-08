#include "SparkMediaControler.h"

void SparkMediaControler::setVideoSize(QSize size)
{
    std::lock_guard<std::mutex> lock(play_mutex);
    this->size = size;
    delete image_frame;
    m_codec.setOutVideo(size.width(),size.height());

    if (size.width() <= 0 || size.height() <=0){
        this->size.setWidth(m_codec.getRawVideoSettings().width);
        this->size.setHeight(m_codec.getRawVideoSettings().height);
    }
    image_frame = new QImage(this->size,format);
}

void SparkMediaControler::setVideoSize(int widht, int height)
{
    setVideoSize(QSize(widht,height));
}


void audioCallback(void* userData, Uint8* stream, int len) {
    //qDebug()<<"缓冲区数据长度: "<<len;
    SparkMediaControler* ctrl = static_cast<SparkMediaControler*>(userData);
    emit ctrl->onTimeChange();
    SDL_memset(stream, ctrl->audio_spec.silence, len);  // 默认填充静音

    uint8_t* aud_data[1] = {nullptr};
    int aud_size[1] = {0};
        if (ctrl->m_codec.getFinalAudFrame(aud_data, aud_size)<0) {
            // 无数据则跳出循环
            return;
        }
    int copySize = std::min(aud_size[0], len);
    // 使用音量混合而不是直接拷贝
    // SDL_MixAudioFormat(stream, aud_data[0]+pos, 
    //                   ctrl->audio_spec.format, 
    //                   copySize, 
    //                   ctrl->volume);
    //DL_memcpy(stream, aud_data[0], copySize);
    
    SDL_MixAudioFormat(stream, aud_data[0], ctrl->audio_spec.format, copySize, ctrl->volume);
}
bool SparkMediaControler::isVideo()
{
    return m_codec.getMediaType() == MediaType::video;
}
double SparkMediaControler::getTime()
{
    return m_codec.getTime();
}
int SparkMediaControler::setAudioDevice()
{
    audio_spec.freq=48000;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 2;
    audio_spec.silence = 0;
    audio_spec.callback = audioCallback;
    audio_spec.userdata = this;

    m_codec.setOutAudio(48000,2,16);
    
    if ((audio_device_id = SDL_OpenAudioDevice(nullptr,0,&audio_spec, nullptr,SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2){
        qWarning() << "open audio device failed ";
        closeMedia();
        return -1;
    }
    return 0;
}

void SparkMediaControler::play(int step)
{
    if (!haveFile)
    {
        qWarning() << "没有媒体文件";
        return;
    }
    m_codec.startDecoding();
    isPlay = true;
    audio_spec.samples = 0;
    codec_thead = new std::thread(&SparkMediaControler::playThead, this, step);
    codec_thead->detach();
    
    emit onStatusChange();
}
void SparkMediaControler::pause()
{
    if (!isPlay) {  // 如果已经暂停，直接返回
        return;
    }
    
    isPlay = false;
    m_codec.stopDecoding();
    SDL_PauseAudioDevice(audio_device_id,1);
}


void SparkMediaControler::openMedia(fs::path path){
    m_path = path;
    if(m_codec.openFile(path.c_str())<0){
        m_codec.closeFile();
        haveFile = false;
        return;
    }
    delete image_frame;
    int32_t w = Codec::getTitleImgWidth(path);
    int32_t h = Codec::getTitleImgHeight(path);
    image_frame = new QImage(QSize(w,h),QImage::Format_RGB32);
    uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image_frame->bits()) };
    int linesize[1] = { static_cast<int>(image_frame->bytesPerLine()) };
    int res = Codec::getTitleImg(SparkMediaControler::getInstance()->getPath(),
                       w,h,data,linesize);
    if (res>=0) {
        emit onImageDone();
    } else {
        delete image_frame;
        QImage srcImage(Path::applicationPath("images/icon.png").toString());
        srcImage = srcImage.convertToFormat(QImage::Format_ARGB32);
        image_frame = new QImage(srcImage.size(), QImage::Format_RGB32);
        image_frame->fill(Qt::black);
        QPainter painter(image_frame);
        painter.drawImage(0, 0, srcImage);
        painter.end();
        emit onImageDone();
    }

    haveFile = true;
    if (m_codec.getMediaType() == MediaType::video)
    {
        setVideoSize(0,0);
    }
    
    //codec_thead = new std::thread(&SparkMediaControler::playAudio, this);
    
    setSeekTime(0.0);
    emit onFileOpen();
}
void SparkMediaControler::closeMedia(){
    pause();
    haveFile = false;
    m_codec.closeFile();
}

bool SparkMediaControler::isHaveFile()
{
    return haveFile;
}

void SparkMediaControler::addMedia(fs::path path, int index)
{
    for (auto i = play_list.begin(); i != play_list.end(); i++) {
        if (*i == path) {
            i--;
            play_point = i;
            return;
        }
    }
    
    if (index) {
        if (play_list.empty())
        {
            play_list.push_back(path);
        } else {
            play_list.insert(play_point+1,path);

        }
        
    }
    else { 
        play_list.push_back(path);
    }
    
    if (play_list.size()==1)
    {
        play_point = play_list.begin();
        if (haveFile)
        {
            closeMedia();
        }
        openMedia(play_point->c_str());
    }
    emit onPlayListChange();
}

void SparkMediaControler::removeMedia(fs::path path)
{
    for (auto i = play_list.begin(); i != play_list.end(); i++)
    {
        if (*i != path)
        {
            continue;
        }
        
        if (i==play_point)
        {
            bool st = getStatus();
            pause();
            closeMedia();
            play_point=play_list.erase(play_point);
            if (play_point == play_list.end() && !play_list.empty())
            {
                play_point--;
                openMedia(play_point->c_str());
                if (st) // 若果是播放中那就恢复播放
                {
                    play();
                }
            } else {
                openMedia(play_point->c_str());
                if (st) // 若果是播放中那就恢复播放
                {
                    play();
                }
            }
            
        
        } else {
            play_list.erase(i);
        }
    
    }
    emit onPlayListChange();
}

void SparkMediaControler::removeAllMedia()
{
    while (!play_list.empty())
    {
        play_list.pop_front();
    }
    play_point = play_list.begin();
}

std::deque<fs::path> *SparkMediaControler::getPlayList()
{
    return &play_list;
}

void SparkMediaControler::nextMedia(bool need_play)
{
    if (!play_list.empty())
    {
        bool st = isPlay;
        pause();
        closeMedia();
        play_point++;
        if ( play_point == play_list.end())
        {
            play_point = play_list.begin();
        }
        openMedia(play_point->c_str());
        if (st || need_play)
        {
            play();
        }
        
    }
    
}

void SparkMediaControler::previousmedia()
{
    if (!play_list.empty())
    {
        bool st = isPlay;
        pause();
        closeMedia();
        --play_point;
        if (play_point < play_list.begin())
        {
            play_point = play_list.end()-1;
        }
        openMedia(play_point->c_str());
        if (st)
        {
            play();
        }
        
    }
}

QImage *SparkMediaControler::getImg()
{
    return image_frame;
}

// void SparkMediaControler::playAudio(){
//     auto start = std::chrono::high_resolution_clock::now();
//     SDL_PauseAudioDevice(audio_device_id,0);
//     while (true)
//     {
//         if (!haveFile)
//         {
//             return;
//         }
//         if (isPlay)
//         {
//             if (m_codec.isEnd())
//             {
//                 isPlay = false;
//             }


//             // uint8_t* aud_data[1];
//             // int aud_size[1]={0};
//             // if (!m_codec.getFinalAudFrame(aud_data,aud_size))
//             // {  
//             //     int res = SDL_QueueAudio(audio_device_id, aud_data[0], aud_size[0]);
//             //     if (res == -1)
//             //     {
//             //         qDebug() << "SDL_QueueAudio error:" << SDL_GetError();
//             //     }
//             // }
//             SDL_Delay(5);

//         }
//     }
    
// }

void SparkMediaControler::playThead(int step)
{
    std::lock_guard<std::mutex> lock(codec_mutex);
    qDebug()<<"in";
    int is_step = step;
    // audio_spec.samples = 2048;
    // if ((audio_device_id = SDL_OpenAudioDevice(nullptr,0,&audio_spec, nullptr,SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2){
    //     qWarning() << "open audio device failed ";
    //     closeMedia();
    //     return;
    // }
    SDL_PauseAudioDevice(audio_device_id,0);
    
    while (isPlay)
    {
        //int target = m_codec.getAudioSamples()%2 ? m_codec.getAudioSamples()-1 : m_codec.getAudioSamples();
        if (audio_spec.samples != m_codec.getAudioSamples()) {
            SDL_PauseAudioDevice(audio_device_id,1);
            SDL_CloseAudioDevice(audio_device_id);
            audio_spec.samples = m_codec.getAudioSamples();
            if ((audio_device_id = SDL_OpenAudioDevice(nullptr,0,&audio_spec, nullptr,SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2){
                qWarning() << "open audio device failed ";
                closeMedia();
                return;
            }
            SDL_PauseAudioDevice(audio_device_id,0);
        }
        //qDebug() <<"当前SDL audio_spec.samples: "<< audio_spec.samples;
        if (is_step)
        {
            if (step==0){
                pause();
                break;
            }
            step--;
        }
        
        if (!haveFile)
        {
            return;
        }
        if (m_codec.isEnd())
        {
            isPlay = false;
            DoPlayMod();
        }
        if (m_codec.getMediaType() == MediaType::audio)
        {
            continue;
        }
        
        play_mutex.lock();
        uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image_frame->bits()) };
        int linesize[1] = { static_cast<int>(image_frame->bytesPerLine()) };
        if(!m_codec.getFinalVidFrame(data,linesize)){
            emit onImageDone();
        }
        play_mutex.unlock();
        
    }

    SDL_PauseAudioDevice(audio_device_id,1);
    emit onStatusChange();
    qDebug()<<"nice";
}

bool SparkMediaControler::getStatus()
{
    return isPlay;
}

void SparkMediaControler::setSeekTime(double time)
{
    m_codec.setSeekTime(time);
}

double SparkMediaControler::getSeekTime()
{
    return m_codec.getSeekTime();
}

void SparkMediaControler::setPlaybackSpeed(double speed)
{
    if (haveFile)
    {
        m_codec.setPlaybackSpeed(speed);
    }
    
}

void SparkMediaControler::setVolume(int v)
{
    
    SDL_LockAudioDevice(audio_device_id);
    this->volume = (v < 0 ? 0 : (v > 100 ? 100 : v));
    SDL_UnlockAudioDevice(audio_device_id);
}

void SparkMediaControler::DoPlayMod()
{
    switch (play_mode)
    {
        case PlayMode_List:
            nextMedia(true);
            break;
        case PlayMode_Random:
            nextMedia(true);
            break;
        case PlayMode_One:
            break;
        case PlayMode_OneLoop:
            play();
            break;
        default:
            break;
    }
}

void SparkMediaControler::setPlayMode(PlayMode mode)
{
    play_mode = mode;
    if (mode == PlayMode_Random)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(play_list.begin(), play_list.end(), g);
        play_point = play_list.begin();
    }
}

PlayMode SparkMediaControler::getPlayMode()
{
    return play_mode;
}

SparkMediaControler::SparkMediaControler()
{
    isPlay=false;
    haveFile=false;
    size.setWidth(1920);
    size.setHeight(1080);
    format = QImage::Format_RGB32;
    image_frame = new QImage(size,format);
    play_point = play_list.end();
    volume = 100;
    SDL_Init(SDL_INIT_AUDIO);
    setAudioDevice();
}

SparkMediaControler::~SparkMediaControler()
{
    closeMedia();
    SDL_CloseAudioDevice(audio_device_id);
}

SparkMediaControler *SparkMediaControler::getInstance(){
    static SparkMediaControler instance;
    return &instance;
}

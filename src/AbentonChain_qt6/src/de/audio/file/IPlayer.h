#pragma once

#if 0
struct IPlayer
{
    virtual ~IPlayer() = default;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void setLoop(bool) = 0;
    virtual void setVolume(float) = 0;
    virtual void setPitch(float) = 0;
    virtual bool isPlaying() const = 0;
};

class Player 
{
public:
    enum class Mode {
        Auto,
        ForcePreload,
        ForceStream
    };

    // Load file, decide backend based on Mode + file properties
    bool load(const std::string& path, Mode mode = Mode::Auto);

    void play();
    void stop();
    void pause();

    void setLoop(bool enabled);
    void setVolume(float gain);   // linear gain
    void setPitch(float ratio);   // resampling ratio
    bool isPlaying() const;

private:

    std::unique_ptr<IPlayer> m_impl;
};

void AudioPlayer::play()        { if (m_impl) m_impl->play(); }
void AudioPlayer::stop()        { if (m_impl) m_impl->stop(); }
void AudioPlayer::pause()       { if (m_impl) m_impl->pause(); }
void AudioPlayer::setLoop(bool b)   { if (m_impl) m_impl->setLoop(b); }
void AudioPlayer::setVolume(float g){ if (m_impl) m_impl->setVolume(g); }
void AudioPlayer::setPitch(float r) { if (m_impl) m_impl->setPitch(r); }
bool AudioPlayer::isPlaying() const { return m_impl && m_impl->isPlaying(); }


bool AudioPlayer::load(const std::string& path, Mode mode)
{
    if (mode == Mode::ForcePreload) {
        m_impl = std::make_unique<PreloadedSound>(path);
        return true;
    }

    if (mode == Mode::ForceStream) {
        m_impl = std::make_unique<StreamingMusic>(path);
        return true;
    }

    // Auto
    FileInfo info = probeFile(path);

    constexpr double  MAX_PRELOAD_SECONDS = 2.0;
    constexpr uint64_t MAX_PRELOAD_PCM    = 20ull * 1024 * 1024; // 20 MB

    const bool smallEnough =
        info.durationSeconds <= MAX_PRELOAD_SECONDS &&
        info.pcmBytes        <= MAX_PRELOAD_PCM &&
        info.channels        <= 2;

    if (smallEnough)
        m_impl = std::make_unique<PreloadedSound>(path);
    else
        m_impl = std::make_unique<StreamingMusic>(path);

    return true;
}



class PreloadedSound : public IPlayer 
{
public:
    explicit PreloadedSound(const std::string& path);
    void play() override;
    void stop() override;
    void pause() override;
    void setLoop(bool) override;
    void setVolume(float) override;
    void setPitch(float) override;
    bool isPlaying() const override;

private:
    std::vector<float> m_pcm; // interleaved
    // playback state, cursor, etc.
};

class StreamingMusic : public IPlayer 
{
public:
    explicit StreamingMusic(const std::string& path);
    void play() override;
    void stop() override;
    void pause() override;
    void setLoop(bool) override;
    void setVolume(float) override;
    void setPitch(float) override;
    bool isPlaying() const override;

private:
    // decoder handle, ring buffer, streaming thread / callback, etc.
};


struct FileInfo 
{
    double durationSeconds;
    uint64_t pcmBytes;   // decoded
    uint32_t channels;
    uint32_t sampleRate;
};

AudioFileInfo probeFile(const std::string& path);

#endif
#pragma once
#include <de/session/Clip.h>
#include <de/audio/plugin/IPlugin.h>
//#include <de/audio/dsp/DspTrack.h>
//#include <gui/track/PluginWidget.h>

class PluginWidget;
class TrackWidget;

namespace de {
namespace session {

class Session;

// typedef std::shared_ptr<Clip> SharedClip;

// typedef std::vector<SharedClip> SharedClips;

// typedef std::shared_ptr<de::audio::IPlugin> SharedPlugin;

// ============================================================================
struct Track :  public QObject,
                public de::audio::IDspChainElement
// ============================================================================
{
    Q_OBJECT

    static int
    GetFreeTrackId();

    static QColor
    GetNextTrackColor();
public:

    enum eType
    {
        Master = 0,
        Send,
        User
    };

    Session* m_session;
    de::audio::IDspChainElement* m_inputSignal;
    de::audio::IPlugin* m_chainStart;
    de::audio::IPlugin* m_chainEnd;
    TrackWidget* m_trackWidget;
    int m_trackId;
    int m_trackType; // 0 = master, 1 = send, 2 = user
    QColor m_trackColor;
    QString m_trackName;
    int m_width;    // in vertical mode
    int m_height;   // in horizontal mode
    QRect m_rect;
    int m_activeClipId;
    std::vector<Clip*> m_clips;
    std::vector<de::audio::IPlugin*> m_plugins;
    //std::vector<de::audio::IPlugin*> m_trashBin;
    std::vector<PluginWidget*> m_pluginWidgets;

    Track();
    ~Track();
    void shutdown();

    void setSession(Session* session) { m_session = session; }
    Session* getSession() const { return m_session; }

    void setTrackType(int typ) { m_trackType = typ; }
    int getTrackType() const { return m_trackType; }

    void setTrackId(int id) { m_trackId = id; }
    int getTrackId() const { return m_trackId; }

    void setTrackName(const QString& name) { m_trackName = name; }
    const QString& getTrackName() const { return m_trackName; }

    // void clear();
    // void reset();
    void newClip();

    int getActiveClipId() const { return m_activeClipId; }

    Clip* getActiveClip() const { return getClip(m_activeClipId); }

    // const SharedClips& getClips() const { return m_clips; }
    // SharedClips& getClips() { return m_clips; }

    Clip* getClip(int clipId) const
    {
        auto it = std::find_if(m_clips.begin(), m_clips.end(),
                    [clipId](const Clip* clip) { return clip->m_clipId == clipId; });
        if (it == m_clips.end())
        {
            return nullptr;
        }
        else
        {
            return *it;
        }
    }

    //const std::vector<SharedPlugin>& getPlugins() const { return m_plugins; }
    //std::vector<SharedPlugin>& getPlugins() { return m_plugins; }

    //void setPlugins( std::vector<SharedPlugin> plugins );

    // void removePlugin( SharedPlugin plugin );

    //de::audio::IPlugin* createPlugin( std::string uri, int index = -1);



    // ----------------------------------------
    // PluginWidget hinzufügen
    // ----------------------------------------
    void addPlugin(const QString &name);
    void insertPlugin(int index, const QString &name);
    void removePlugin(PluginWidget* w);
    bool swapPlugins(int dragIndex, int dropIndex);

    void destroyPlugins();
    //void cleanupTrash();
    //void setAudioCentral( IAudioCentral* audioCentral);

    // const std::vector<SharedPlugin>& getPlugins() const { return m_plugins; }
    // std::vector<SharedPlugin>& getPlugins() { return m_plugins; }

    //void setPlugins( std::vector<SharedPlugin> plugins );

    // void removePlugin( SharedPlugin plugin );

    //SharedPlugin createPlugin( std::string uri, int index = -1);

    // void deregisterMidiListeners();

    // void dumpChain();

    std::string
    dsp_name() const override { return std::string("Track - ") + getTrackName().toStdString(); }

    void
    dsp_read(f64 pts, u32 frames, u32 sampleRate,
                f32* __restrict__ L,
                f32* __restrict__ R ) override;

    void
    dsp_init(u64 frames, u32 channels, u32 sampleRate) override;

    u32
    dsp_getInputSignalCount() const override;

    de::audio::IDspChainElement*
    dsp_getInputSignal(int i = 0) override;

    void
    dsp_setInputSignal(de::audio::IDspChainElement* input, int i = 0) override;

    void
    dsp_clearInputSignals() override;

    // bool swapPlugins(int dragIndex, int dropIndex);

private:
    void updateDspChain();

    std::string debugStr() const
    {
        std::ostringstream o;
        o << "[" << m_plugins.size() << "]{";
        for (size_t i = 0; i < m_plugins.size(); ++i)
        {
            if (i>0) o << ",";
            o << m_plugins[i]->getPluginId();
        }
        o << "}";
        return o.str();
    }


};

} // end namespace session
} // end namespace de.

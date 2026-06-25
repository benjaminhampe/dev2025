#include "Track.h"

namespace de {
namespace session {

// static
// ===========================================================================
int Track::GetFreeTrackId()
// ===========================================================================
{
    static int s_id = 0;
    return ++s_id;
}

Track::Track()
    : m_trackId{ GetFreeTrackId() }
    , m_trackType{ User }
    , m_trackName{ QString("%1 - Audio").arg(m_trackId) }
{
    DE_TRACE(getTrackName().toStdString())
    m_dsp = std::make_shared<audio::DspTrack>(this);
    newClip();

}
Track::~Track()
{
    DE_TRACE(getTrackName().toStdString())
}

void Track::shutdown()
{
    if (m_dsp)
    {
        m_dsp->cleanupAll();
        m_dsp.reset();
    }
    m_clips.clear();
}

void Track::newClip()
{
    auto clip = std::make_shared<Clip>();
    m_clips.emplace_back( std::move( clip ) );
}

/*
void Track::addClip(double timeBeg, double timeEnd, std::string name)
{
    auto clip = std::make_shared<Clip>();
    clip->m_name = std::move( name );
    clip->m_timeBeg = timeBeg;
    clip->m_timeEnd = timeEnd;
    m_clips.emplace_back( std::move( clip ) );
}
*/



} // end namespace session
} // end namespace de.

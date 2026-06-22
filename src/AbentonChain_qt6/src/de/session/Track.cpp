#include "Track.h"

namespace de {
namespace session {

// static
// ===========================================================================
u32 Track::GetFreeTrackId()
// ===========================================================================
{
    static u32 s_id = 0;
    return ++s_id;
}

Track::Track()
    : m_trackId(GetFreeTrackId())
{
    DE_TRACE(trackName().toStdString())
    m_dsp = std::make_shared<audio::DspTrack>(this);
    newClip();

}
Track::~Track()
{
    DE_TRACE(trackName().toStdString())
}

void Track::shutdown()
{
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

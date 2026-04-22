#pragma once

#if 0
struct FpsCounter
{
    struct Item
    {
        double t;
        double y;
    };

    std::vector<Item> m_frames;
    double m_tmin;
    double m_tmax;
    double m_ymin;
    double m_ymax;
    double m_range;
    double m_duration;
    uint64_t m_totalFrames;
    double m_fps;
    double m_average;

    double m_timeStart;

    void push( double t )
    {
        if (m_frames.empty())
        {
            m_tmin = t;
            m_tmax = t;
            m_ymin = 0;
            m_ymax = 0;
            m_range = 0;
            m_duration = 0;
            m_totalFrames = 0;
        }

        m_frames.emplace_back();
        Item & item = m_frames.back();
        item.t = t;
        item.y = m_frames.size();
        m_totalFrames++;

        m_tmin = std::min(m_tmin, item.t);
        m_tmax = std::max(m_tmax, item.t);
        m_ymin = std::min(m_ymin, item.y);
        m_ymax = std::max(m_ymax, item.y);
        m_range = m_ymax - m_ymin;
        m_duration = m_tmax - m_tmin;

        m_fps = double(m_totalFrames) / m_duration;

    }

    double getFPS()
    {
        return m_fps;
    }
};

#endif
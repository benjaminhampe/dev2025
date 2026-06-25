#pragma once
#include <de/Math.h>

namespace de {

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
struct BranchlessIntMinMax
{
    static inline T min(T a, T b) noexcept
    {
        T diff = a - b;
        T mask = diff >> (sizeof(T)*8 - 1); // arithmetic shift
        return b + (diff & mask);
    }

    static inline T max(T a, T b) noexcept
    {
        T diff = a - b;
        T mask = diff >> (sizeof(T)*8 - 1);
        return a - (diff & mask);
    }
};

// =======================================================================
template< typename T > struct Range
// =======================================================================
{
    constexpr static const T Min = std::numeric_limits< T >::lowest();
    constexpr static const T Max = std::numeric_limits< T >::max();

    T m_min;
    T m_max;

    Range()
        : m_min{ Max }
        , m_max{ Min }
    {}

    Range(T range_min, T range_max)
        : m_min{ range_min }
        , m_max{ range_max }
    {}

    T getRange() const { return m_max - m_min; }

    std::string str() const
    {
        std::ostringstream o;
        o << m_min << "," << m_max;
        return o.str();
    }

    void reset()
    {
        m_min = Max;
        m_max = Min;
    }

    void reset(const T range_min, const T range_max)
    {
        m_min = range_min;
        m_max = range_max;
    }

    static T imin(T a, T b);

    void addPoint(const T value) noexcept
    {
        if constexpr (std::is_same_v<T, float>)
        {
            m_min = fminf(m_min, value);
            m_max = fmaxf(m_max, value);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            m_min = fmin(m_min, value);
            m_max = fmax(m_max, value);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            m_min = BranchlessIntMinMax<T>::min(m_min, value);
            m_max = BranchlessIntMinMax<T>::max(m_max, value);
        }
        else
        {
            m_min = std::min(m_min, value);
            m_max = std::max(m_max, value);
        }
    }

};

} // end namespace de.

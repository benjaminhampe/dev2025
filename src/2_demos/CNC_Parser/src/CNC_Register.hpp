#pragma once
#include <CNC_Block.hpp>

namespace de {
namespace cnc {

// Is actually a Axis specific register yet
// Axis can have a speed, acceleration, etc...
struct CNC_Register
{
   char m_name;   // 1 byte with register name 'G', 'E', 'F', etc...
   char m_dummy1; // add 1 byte padding
   u16 m_dummy2;  // add 2 byte padding
   u32 m_dummy3;  // add 4 byte padding -> = 8 byte machine aligned memory (64bit machine).
   double m_currValue;
   double m_lastValue;
   double m_maxValue;
   double m_minValue;
   double m_travelDistance;
   u64 m_callCounter;

   double m_axisSpeed;        // in [mm/min]
   double m_axisAccelration;  // in [mm/min*min]
   double m_timeConsumption;  // in [min]

   CNC_Register( char name )
      : m_name(name)
      , m_currValue(0.0)
      , m_lastValue(0.0)
      , m_maxValue(std::numeric_limits<double>::lowest())
      , m_minValue(std::numeric_limits<double>::max())
      , m_travelDistance(0.0)
      , m_callCounter(0)
      , m_axisSpeed(40.0) // 40mm/min
      , m_axisAccelration(0.0)
      , m_timeConsumption(0.0)
   {

   }

   std::string
   toString() const
   {
      std::ostringstream s;
      s << char(m_name) << "[" << getDistString() << "," << getTimeString() << "]";
      //"(" << m_currValue << "," << m_minValue << "," << m_maxValue << "), "
      //"L(" << (0.001 * m_travelDistance) << "m)";
      return s.str();
   }

   std::string
   getTimeString() const
   {
      std::ostringstream o;
      if ( m_timeConsumption < 1.0 )
      {
         o << m_timeConsumption * 60.0 << "s";
      }
      else
      {
         o << m_timeConsumption << "min";
      }
      return o.str();
   }

   std::string
   getDistString() const
   {
      std::ostringstream o;
      if ( m_travelDistance < 1000.0 )
      {
         o << m_travelDistance << "mm";
      }
      else
      {
         o << (0.001 * m_travelDistance) << "m";
      }
      return o.str();
   }

   // Only meaningful for axis specific register
   void setFeedSpeed( double mm_per_minute )
   {
      m_axisSpeed = mm_per_minute;
   }
   // Only meaningful for axis specific register
   void setAccelration( double mm_per_minute_squared )
   {
      m_axisAccelration = mm_per_minute_squared;
   }
   // Only meaningful for axis specific register
   void move( double value, int relativeMode, double feedSpeed )
   {
      m_axisSpeed = feedSpeed;
      if ( relativeMode == 1 ) { moveRelative( value ); }
      else                     { moveAbsolute( value ); }
   }
   // Only meaningful for axis specific register
   void moveAbsolute( double value_in_mm )
   {
      m_lastValue = m_currValue;
      m_currValue = value_in_mm;
      m_minValue = std::min( m_currValue, m_minValue );
      m_maxValue = std::max( m_currValue, m_maxValue );
      double travelDistance = std::abs(m_currValue - m_lastValue);
      m_travelDistance += travelDistance;
      m_timeConsumption += travelDistance / m_axisSpeed; // v=s/t, s=vt, t=s/v.
      m_callCounter++;
   }
   // Only meaningful for axis specific register
   void moveRelative( double value_in_mm )
   {
      m_lastValue = m_currValue;
      m_currValue += value_in_mm;
      m_minValue = std::min( m_currValue, m_minValue );
      m_maxValue = std::max( m_currValue, m_maxValue );
      double travelDistance = std::abs(value_in_mm);
      m_travelDistance += travelDistance;
      m_timeConsumption += travelDistance / m_axisSpeed; // v=s/t, s=vt, t=s/v.
      m_callCounter++;
   }
};

} // end namespace cnc
} // end namespace de

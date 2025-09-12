#pragma once
#include <cstdint>
#include <cmath>     // std::abs()
#include <de_core/de_PerformanceTimer.h>

namespace de {

//===========================================================
struct FPSComputer
//===========================================================
{
   double m_timeEpoch;
   double m_timeNow;             // relative to m_timeStart!
   double m_timeLastFPSCompute;  // relative to m_timeStart!

   uint64_t m_frameCount;
   uint64_t m_frameCountLast;
   double m_fps;

   double m_timeFrameStart;
   double m_timeFrameEnd;

   double m_timeFrameDurationMin;
   double m_timeFrameDurationMax;

   uint64_t m_drawCalls;
   uint64_t m_drawCallsLastFrame;
   uint64_t m_drawCallsLastSecond;
   uint64_t m_drawCallsPerFrame;
   uint64_t m_drawCallsPerSecond;

public:
   FPSComputer()
   {
      reset();
   }

   ~FPSComputer()
   {
      // Save history... (not yet)
   }

   void reset()
   {
      m_timeEpoch = HighResolutionClock::GetTimeInSeconds(); // the current epoch ( keeps numeric values low and highp )
      m_timeNow = 0.0;           // relative to epoch
      m_timeLastFPSCompute = 0.0;// relative to epoch

      m_frameCount = 0;
      m_frameCountLast = 0;
      m_fps = 0.0;

      m_timeFrameStart = 0.0; // relative to epoch
      m_timeFrameEnd = 0.0;   // relative to epoch

      m_drawCalls = 0;
      m_drawCallsLastFrame = 0;
      m_drawCallsLastSecond = 0;
      m_drawCallsPerFrame = 0;
      m_drawCallsPerSecond = 0;
   }

   void beginFrame()
   {
      m_timeNow = HighResolutionClock::GetTimeInSeconds() - m_timeEpoch;
      m_timeFrameStart = m_timeNow;
   }

   void drawCall()
   {
      // m_timeNow = getTimeInSeconds() - m_timeEpoch;
      m_drawCalls++;
      // Add item to draw call history... (not yet)
   }

   void endFrame()
   {
      // Add item to frame history... (not yet)

      m_timeNow = HighResolutionClock::GetTimeInSeconds() - m_timeEpoch;
      m_timeFrameEnd = m_timeNow;

      m_frameCount++;

      // Compute FPS after one second passed
      double dt = m_timeNow - m_timeLastFPSCompute;
      if ( dt >= 1.0 )
      {
         m_timeLastFPSCompute = m_timeNow;
         m_fps = double( m_frameCount - m_frameCountLast ) / dt; // divide by actual time passed.
         m_frameCountLast = m_frameCount;

         m_drawCallsPerSecond = double( m_drawCalls - m_drawCallsLastSecond ) / dt;
         m_drawCallsLastSecond = m_drawCalls;
      }

      m_drawCallsPerFrame = m_drawCalls - m_drawCallsLastFrame;
      m_drawCallsLastFrame = m_drawCalls;
   }

   double getTime() const { return m_timeNow; }

   uint64_t getFrameCount() const { return m_frameCount; }

   double getFPS() const { return m_fps; }

   // typ = 0 -> MeshBuffers per Frame
   // typ = 1 -> Vertices per Frame
   // typ = 2 -> Indices per Frame
   // typ = 3 -> Primitives per Frame
   // typ = 4 -> Total rendered Frames
   // typ = 5 -> Total rendered MeshBuffers
   // typ = 6 -> Total rendered Vertices
   // typ = 7 -> Total rendered Indices
   // typ = 8 -> Total rendered Primitives

//   uint64_t getDrawCallsPerFrame() const { return m_drawCallsPerFrame; }
//   uint64_t getDrawCallsPerSecond() const { return m_drawCallsPerSecond; }

//   uint64_t getRenderStat( int typ ) const;

};

} // end namespace de
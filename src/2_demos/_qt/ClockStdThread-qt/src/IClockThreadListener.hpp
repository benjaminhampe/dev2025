#pragma once
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////////
struct IClockThreadListener
//////////////////////////////////////////////////////////////////////////////////
{
   virtual ~IClockThreadListener() = default;

   virtual void clockThreadStarted() {}
   virtual void clockThreadStopped() {}

   virtual void clockThreadPaused() {}
   virtual void clockThreadResumed() {}

   virtual void clockWorkOrange( int64_t nanosecondsNow ) { (void)nanosecondsNow; }
};

//////////////////////////////////////////////////////////////////////////////////
struct IClockThread
//////////////////////////////////////////////////////////////////////////////////
{
   virtual ~IClockThread() = default;

   virtual void play() {}
   virtual void pause() {}

   virtual void stop() {}

   virtual void setListener( IClockThreadListener* listener ) {}

   virtual double getTimeInSeconds() const { return 0.0; }
};

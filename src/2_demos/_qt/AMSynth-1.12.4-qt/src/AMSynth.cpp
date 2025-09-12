/*
 *  AMSynth.cpp
 *
 *  Copyright (c) 2001-2020 Nick Dowell <nick@nickdowell.com>
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is part of amsynth.
 *
 *  amsynth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  amsynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with amsynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AMSynth.hpp"

#if 0

void amsynth_midi_input(unsigned char status, unsigned char data1, unsigned char data2);

////////////////////////////////////////////////////////////////////////////////

#if defined(BENNI_USE_MIDI)
static MidiDriver *opened_midi_driver(MidiDriver *driver)
{
   if (driver && driver->open() != 0) {
      delete driver;
      return nullptr;
   }
   return driver;
}

static void open_midi()
{
   const char *alsa_client_name = config.jack_client_name.empty() ? PACKAGE_NAME : config.jack_client_name.c_str();

   if (config.midi_driver == "alsa" || config.midi_driver == "ALSA") {
      if (!(midiDriver = opened_midi_driver(CreateAlsaMidiDriver(alsa_client_name)))) {
         std::cerr << _("error: could not open ALSA MIDI interface") << endl;
      }
      return;
   }

   if (config.midi_driver == "oss" || config.midi_driver == "OSS") {
      if (!(midiDriver = opened_midi_driver(CreateOSSMidiDriver()))) {
         std::cerr << _("error: could not open OSS MIDI interface") << endl;
      }
      return;
   }

   if (config.midi_driver == "auto") {
      midiDriver = opened_midi_driver(CreateAlsaMidiDriver(alsa_client_name)) ?:
                   opened_midi_driver(CreateOSSMidiDriver());
      if (config.current_midi_driver.empty()) {
         std::cerr << _("error: could not open any MIDI interface") << endl;
      }
      return;
   }
}
#endif


unsigned amsynth_timer_callback();

static int signal_received = 0;

static void signal_handler(int signal)
{
   signal_received = signal;
}


unsigned
amsynth_timer_callback()
{
   //amsynth_lash_poll_events();
   return 1;
}



static bool compare(const amsynth_midi_event_t &first, const amsynth_midi_event_t &second) {
   return (first.offset_frames < second.offset_frames);
}



///////////////////////////////////////////////////////////////////////////////

void ptest ()
{
   //
   // test parameters
   //
   const int kTestBufSize = 64;
   const int kTestSampleRate = 44100;
   const int kTimeSeconds = 60;
   const int kNumVoices = 10;

   float *buffer = new float [kTestBufSize];

   VoiceAllocationUnit *voiceAllocationUnit = new VoiceAllocationUnit;
   voiceAllocationUnit->SetSampleRate (kTestSampleRate);

   // trigger off some notes for amsynth to render.
   for (int v=0; v<kNumVoices; v++) {
      voiceAllocationUnit->HandleMidiNoteOn(60 + v, 1.0f);
   }

   struct rusage usage_before;
   getrusage (RUSAGE_SELF, &usage_before);

   long total_samples = kTestSampleRate * kTimeSeconds;
   long total_calls = total_samples / kTestBufSize;
   unsigned remain_samples = total_samples % kTestBufSize;
   for (int i=0; i<total_calls; i++) {
      voiceAllocationUnit->Process (buffer, buffer, kTestBufSize);
   }
   voiceAllocationUnit->Process (buffer, buffer, remain_samples);

   struct rusage usage_after;
   getrusage (RUSAGE_SELF, &usage_after);

   unsigned long user_usec = (usage_after.ru_utime.tv_sec*1000000 + usage_after.ru_utime.tv_usec)
                     - (usage_before.ru_utime.tv_sec*1000000 + usage_before.ru_utime.tv_usec);

   unsigned long syst_usec = (usage_after.ru_stime.tv_sec*1000000 + usage_after.ru_stime.tv_usec)
                     - (usage_before.ru_stime.tv_sec*1000000 + usage_before.ru_stime.tv_usec);

   unsigned long usec_audio = kTimeSeconds * kNumVoices * 1000000;
   unsigned long usec_cpu = user_usec + syst_usec;

   fprintf (stderr, _("user time: %f		system time: %f\n"), user_usec/1000000.f, syst_usec/1000000.f);
   fprintf (stderr, _("performance index: %f\n"), (float) usec_audio / (float) usec_cpu);

   delete [] buffer;
   delete voiceAllocationUnit;
}

#endif

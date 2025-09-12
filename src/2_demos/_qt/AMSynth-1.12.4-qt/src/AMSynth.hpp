/*
 *  AMSynth.hpp
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

#pragma once

#include "config.h"
#include "de_types.hpp"

#include "Configuration.h"
#include "drivers/RtAudioDriver.hpp"
//#include "JackOutput.h"
//#include "lash.h"
//#include "midi.h"
#include "Synthesizer.h"
#include "VoiceAllocationUnit.h"
#include "VoiceBoard/LowPassFilter.h"
#include "Preset.h"

//#if defined(BENNI_USE_MIDI)
#include "drivers/RtMidiDriver.hpp"
#include "MidiController.h"
//#endif

struct AMSynth : public IDspChainElement
{
   Configuration m_config;
   u32 m_frames = 0;
   u32 m_channels = 0;
   f64 m_sampleRate = 48000.0;
   RtAudioDriver m_audioDriver;
   std::vector< float > m_audioBuffer;

   Synthesizer m_synthesizer;

   std::vector<amsynth_short_midi_event_t> m_midi_in;

   // std::vector<amsynth_midi_event_t> m_midi_in;
   std::vector<amsynth_midi_cc_t> m_midi_out;

   RtMidiDriver m_midiDriver;

   #if defined(BENNI_USE_MIDI)
   MidiDriver m_midiDriver;
   std::vector< u8 > m_midiBuffer;
   size_t m_midiBufferSize = 4096;
   int m_gui_midi_pipe[2];
   #endif

   AMSynth()
   {
      printf("sizeof(amsynth_short_midi_event_t) = %d\n", u32(sizeof(amsynth_short_midi_event_t)) );
      printf("sizeof(amsynth_midi_cc_t) = %d\n", u32(sizeof(amsynth_midi_cc_t)) );
      printf("sizeof(amsynth_midi_event_t) = %d\n", u32(sizeof(amsynth_midi_event_t)) );

      if (m_config.current_tuning_file != "default")
      {
         m_synthesizer.loadTuningScale( m_config.current_tuning_file.c_str());
      }
      m_synthesizer.loadBank( m_config.current_bank_file.c_str() );
      m_synthesizer.setPresetNumber( 0 );

      if ( m_config.current_tuning_file != "default")
      {
         amsynth_load_tuning_file( m_config.current_tuning_file.c_str());
      }
   }

   ~AMSynth() override { Stop(); }

   void Start()
   {
      m_audioDriver.setInputSignal( this );
      m_audioDriver.startAudio();
   }

   void Stop()
   {
      m_audioDriver.stopAudio();
   }
/*
   void
   amsynth_save_bank(const char *filename)
   {
      m_synthesizer.saveBank(filename);
   }

   void
   amsynth_load_bank(const char *filename)
   {
      m_synthesizer.loadBank(filename);
   }

   int
   amsynth_get_preset_number()
   {
      return m_synthesizer.getPresetNumber();
   }

   void
   amsynth_set_preset_number(int preset_no)
   {
      m_synthesizer.setPresetNumber(preset_no);
   }

*/

   void sendMidi( u8 status, u8 data1, u8 data2 )
   {
      amsynth_short_midi_event_t event;
      event.channel = 0;
      event.command = status;
      event.midiNote = data1;
      event.velocity = data2;
      m_midi_in.emplace_back( std::move(event) );
/*
      u8 buffer[3] = { status, data1, data2 };
      if ( m_config.midi_channel > 1 )
      {
         buffer[0] |= ((m_config.midi_channel - 1) & 0x0f);
      }
      write( m_gui_midi_pipe[1], buffer, sizeof(buffer) );
*/
   }

   int
   amsynth_load_tuning_file(const char *filename)
   {
      int result = m_synthesizer.loadTuningScale(filename);
      if (result != 0)
      {
         std::cerr << "Error: could not load tuning file " << filename << std::endl;
      }
      return result;
   }


   void
   setupDsp( u32 frames, u32 channels, f64 sampleRate ) override
   {
      printf("SetupDSP( frames = %d, frames = %d, frames = %lf )\n", frames, channels, sampleRate);
      fflush( stdout );

      m_frames = frames;
      m_channels = channels;
      m_sampleRate = sampleRate;
      m_audioBuffer.resize( m_frames*m_channels*4 );

      Preset::setIgnoredParameterNames( m_config.ignored_parameters);

      m_synthesizer.setSampleRate( sampleRate );
      m_synthesizer.setMaxNumVoices( m_config.polyphony );
      m_synthesizer.setMidiChannel( m_config.midi_channel );
      m_synthesizer.setPitchBendRangeSemitones( m_config.pitch_bend_range);

      m_midi_in.clear();
      m_midi_in.reserve( 4096 );

      m_midi_out.clear();
      m_midi_out.reserve( 4096 );
   }

   void
   readDspSamples( f32* dst, u32 frames, u32 channels ) override
   {
//      auto buffer = m_audioBuffer.data();
//      int bufsize = m_config.buffer_size;
//      amsynth_audio_callback( buffer+bufsize*2,
//                              buffer+bufsize*3,
//                              bufsize,
//                              1,
//                              m_midi_in,
//                              m_midi_out);

      //std::vector<amsynth_midi_event_t> midi_in_merged = m_midi_in;

#if defined(BENNI_USE_MIDI)
      u8* buffer = m_midiBuffer.data();
      size_t bufferSize = m_midiBuffer.size();
      memset(buffer, 0, bufferSize);

      if (m_gui_midi_pipe[0]) {
         ssize_t bytes_read = read(m_gui_midi_pipe[0], buffer, bufferSize);
         if (bytes_read > 0) {
            amsynth_midi_event_t event = {0};
            event.offset_frames = num_frames - 1;
            event.length = (unsigned int) bytes_read;
            event.buffer = buffer;
            midi_in_merged.push_back(event);
            buffer += bytes_read;
            bufferSize -= bytes_read;
         }
      }

      if (m_midiDriver) {
         int bytes_read = m_midiDriver->read(buffer, (unsigned) bufferSize);
         if (bytes_read > 0) {
            amsynth_midi_event_t event = {0};
            event.offset_frames = num_frames - 1;
            event.length = bytes_read;
            event.buffer = buffer;
            midi_in_merged.push_back(event);
         }
      }

      std::sort(midi_in_merged.begin(), midi_in_merged.end(), compare);
#endif

      f32* l = m_audioBuffer.data();
      f32* r = m_audioBuffer.data() + 1;


      m_midi_in.insert( m_midi_in.end(),
                        m_midiDriver.m_midiInBuffer.begin(),
                        m_midiDriver.m_midiInBuffer.end() );

      m_midiDriver.m_midiInBuffer.clear();

      m_synthesizer.process(frames, m_midi_in, m_midi_out, l, r, m_channels);

      // m_synthesizer.process(frames, midi_in_merged, m_midi_out, l, r, m_channels);

#if defined(BENNI_USE_MIDI)
      if (midi_out.size() > 0)
      {
         std::vector<amsynth_midi_cc_t>::const_iterator out_it;
         for ( out_it = midi_out.begin();
               out_it != midi_out.end(); ++out_it )
         {
            m_midiDriver.write_cc(out_it->channel, out_it->cc, out_it->value);
         }
      }
#endif


//      for (int i=0; i<bufsize; i++) {
//         buffer[2*i]   = buffer[bufsize*2+i];
//         buffer[2*i+1] = buffer[bufsize*3+i];
//      }

//      if (m_audioDriver.write( buffer, bufsize * m_channels) < 0)
//      {
//         break;
//      }

       DspUtils::DSP_COPY( m_audioBuffer.data(), dst, m_frames * m_channels );

       m_midi_in.clear();
   }

};

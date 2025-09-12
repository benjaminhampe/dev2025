/*
 *  Synthesizer.cpp
 *
 *  Copyright (c) 2014-2019 Nick Dowell
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
#include "de_types.hpp"
#include "controls.h"
#include <vector>

//
// Property names
//
#define PROP_KBM_FILE "tuning_kbm_file"
#define PROP_SCL_FILE "tuning_scl_file"

class MidiController;
class PresetController;
class VoiceAllocationUnit;

struct ISynthesizer
{
    virtual ~ISynthesizer() = default;
    virtual int loadTuningKeymap(const char *filename) = 0;
    virtual int loadTuningScale(const char *filename) = 0;
};

class Synthesizer : ISynthesizer
{
   double _sampleRate;
   MidiController *_midiController;
   PresetController *_presetController;
   VoiceAllocationUnit *_voiceAllocationUnit;
   bool needsResetAllVoices_ = false;

public:
   Synthesizer();
   ~Synthesizer() override;

   void loadBank(const char *filename);
   void saveBank(const char *filename);

   void loadState(char *buffer);
   int  saveState(char **buffer);

   int  getPresetNumber();
   void setPresetNumber(int number);

   float getParameterValue(Param parameter);
   void  setParameterValue(Param parameter, float value);

   float getNormalizedParameterValue(Param parameter);
   void  setNormalizedParameterValue(Param parameter, float value);

   void getParameterName(Param parameter, char *buffer, size_t maxLen);
   void getParameterLabel(Param parameter, char *buffer, size_t maxLen);
   void getParameterDisplay(Param parameter, char *buffer, size_t maxLen);

   void setPitchBendRangeSemitones(int value);

   int getMaxNumVoices();
   void setMaxNumVoices(int value);

   static constexpr unsigned char kMidiChannel_Any = 0;
   unsigned char getMidiChannel();
   void setMidiChannel(unsigned char);

   int loadTuningKeymap(const char *filename) override;
   int loadTuningScale(const char *filename) override;

   void setSampleRate(int sampleRate);

   void process(  u32 nframes,
                  std::vector<amsynth_short_midi_event_t> const & midi_in,
                  std::vector<amsynth_midi_cc_t> &midi_out,
                  f32 *audio_l,
                  f32 *audio_r,
                  u32 audio_stride );

//   void process(  u32 nframes,
//                  std::vector<amsynth_midi_event_t> const & midi_in,
//                  std::vector<amsynth_midi_cc_t> &midi_out,
//                  f32 *audio_l,
//                  f32 *audio_r,
//                  u32 audio_stride );

   MidiController *getMidiController() { return _midiController; }
   PresetController *getPresetController() { return _presetController; }

};

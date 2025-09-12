/*
 *  editor_pane.h
 *
 *  Copyright (c) 2001-2012 Nick Dowell
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

#include "../controls.h"

#include <QWidget>

#include "AMSynth.hpp"

#include "BigImage.hpp"

#include "bitmap_knob.h"


//////////////////////////////////////////////////////////////////////////////////
struct EditorPanel : public QWidget
//////////////////////////////////////////////////////////////////////////////////
{
   Q_OBJECT
   AMSynth* m_amsynth;
   BigImage m_imgBackground;
   BigImage m_imgKnobSpot;
   BigImage m_imgKnobBoost;
   BigImage m_imgKnobBoostCut;
   BigImage m_imgKnobMix;
   BigImage m_imgKnobWidth;
   BigImage m_imgKnobOscOctave;
   BigImage m_imgKnobOscPitch;
   BigImage m_imgWaveformOsc;
   BigImage m_imgWaveformLfo;
   BigImage m_imgButtonSimple;
   BigImage m_imgPortamentoModes;
   BigImage m_imgKeybmode;
   BigImage m_imgFilterSlopeRes;
   BigImage m_imgFilterTypeRes;
   BigImage m_imgOscSelect;

   BitmapKnob* m_popup_osc1_waveform;
   BitmapKnob* m_knob_osc1_pulsewidth;
   BitmapKnob* m_popup_osc2_waveform;
   BitmapKnob* m_knob_osc2_pulsewidth;
   BitmapKnob* m_knob_osc2_range;
   BitmapKnob* m_knob_osc2_pitch;
   BitmapKnob* m_knob_osc2_detune;
   //BitmapButton* m_btn_osc2_sync;
   BitmapKnob* m_knob_osc_mix;
   BitmapKnob* m_knob_osc_mix_mode;

   BitmapKnob* m_knob_amp_attack;
   BitmapKnob* m_knob_amp_decay;
   BitmapKnob* m_knob_amp_sustain;
   BitmapKnob* m_knob_amp_release;

   BitmapKnob* m_popup_filter_type;
   BitmapKnob* m_popup_filter_slope;
   BitmapKnob* m_knob_filter_resonance;
   BitmapKnob* m_knob_filter_cutoff;
   BitmapKnob* m_knob_filter_kbd_track;
   BitmapKnob* m_knob_filter_env_amount;
   BitmapKnob* m_knob_filter_attack;
   BitmapKnob* m_knob_filter_decay;
   BitmapKnob* m_knob_filter_sustain;
   BitmapKnob* m_knob_filter_release;

   BitmapKnob* m_knob_master_vol;
   BitmapKnob* m_knob_distortion_crunch;
   BitmapKnob* m_popup_lfo_waveform;
   BitmapKnob* m_knob_lfo_freq;
   BitmapKnob* m_knob_freq_mod_amount;
   BitmapKnob* m_popup_freq_mod_osc;
   BitmapKnob* m_knob_filter_mod_amount;
   BitmapKnob* m_knob_amp_mod_amount;

   BitmapKnob* m_knob_reverb_wet;
   BitmapKnob* m_knob_reverb_roomsize;
   BitmapKnob* m_knob_reverb_width;
   BitmapKnob* m_knob_reverb_damp;

   BitmapKnob* m_knob_portamento_time;
   BitmapKnob* m_popup_portamento_mode;
   BitmapKnob* m_popup_keyboard_mode;
   BitmapKnob* m_knob_filter_vel_sens;
   BitmapKnob* m_knob_amp_vel_sens;

public:
   EditorPanel( AMSynth* amsynth, QWidget* parent )
      : QWidget(parent)
      , m_amsynth( amsynth )
   {
      setMinimumSize( 600, 400 );
      setMaximumSize( 600, 400 );

      QString dir = "media/AMSynth/skins/default/";
      loadImage( m_imgBackground, 600,400,1, "background", dir + "background.png" );

      loadImage( m_imgKnobSpot,     49,49,49, "knob_spot", dir + "knob_spot.png" );
      loadImage( m_imgKnobBoost,    49,49,49, "knob_boost", dir + "knob_boost.png" );
      loadImage( m_imgKnobBoostCut, 49,49,49, "knob_boost_cut", dir + "knob_boost_cut.png" );
      loadImage( m_imgKnobMix,      49,49,49, "knob_mix", dir + "knob_mix.png" );
      loadImage( m_imgKnobWidth,    49,49,49, "knob_width", dir + "knob_width.png" );
      loadImage( m_imgKnobOscOctave,49,49,8, "knob_osc_octave", dir + "knob_osc_octave.png" );
      loadImage( m_imgKnobOscPitch, 49,49,25, "knob_osc_pitch", dir + "knob_osc_pitch.png" );

      loadImage( m_imgWaveformOsc,  49,49,5, "waveform_osc", dir + "waveform_pics.png" );
      loadImage( m_imgWaveformLfo,  49,49,7, "waveform_lfo", dir + "waveform_lfo.png" );
      loadImage( m_imgButtonSimple, 21,21,2, "button_simple", dir + "button_simple.png" );
      loadImage( m_imgPortamentoModes,55,15,2, "portamento_modes", dir + "portamento_modes.png" );
      loadImage( m_imgKeybmode,     55,15,3, "keybmode", dir + "keybmode.png" );
      loadImage( m_imgFilterSlopeRes,45,15,2, "filter_slope_res", dir + "filter_slope.png" );
      loadImage( m_imgFilterTypeRes,65,15,5, "filter_type_res", dir + "filter_type.png" );
      loadImage( m_imgOscSelect,    55,15,3, "osc_select", dir + "osc_select.png" );

      PresetController* presetController = m_amsynth->m_synthesizer.getPresetController();
      Preset & preset = presetController->getCurrentPreset();
      printf("CurrentPreset = %s\n", preset.getName().c_str() ); fflush(stdout);

      m_popup_osc1_waveform = new BitmapKnob( m_amsynth, "osc1_waveform", 20, 20, &m_imgWaveformOsc, this );
      m_knob_osc1_pulsewidth = new BitmapKnob( m_amsynth, "osc1_pulsewidth", 80, 20, &m_imgKnobSpot, this );
      m_popup_osc2_waveform = new BitmapKnob( m_amsynth, "osc2_waveform", 20, 110, &m_imgWaveformOsc, this );
      m_knob_osc2_pulsewidth = new BitmapKnob( m_amsynth, "osc2_pulsewidth", 80, 110, &m_imgKnobSpot, this );
      m_knob_osc2_range = new BitmapKnob( m_amsynth, "osc2_range", 20, 170, &m_imgKnobOscOctave, this );
      m_knob_osc2_pitch = new BitmapKnob( m_amsynth, "osc2_pitch", 80, 170, &m_imgKnobOscPitch, this );
      m_knob_osc2_detune = new BitmapKnob( m_amsynth, "osc2_detune", 140, 170, &m_imgKnobBoostCut, this );
      //BitmapButton* m_btn_osc2_sync;
      m_knob_osc_mix = new BitmapKnob( m_amsynth, "osc_mix", 170, 20, &m_imgKnobMix, this );
      m_knob_osc_mix_mode = new BitmapKnob( m_amsynth, "osc_mix_mode", 170, 80, &m_imgKnobBoost, this );
      m_knob_amp_attack = new BitmapKnob( m_amsynth, "amp_attack", 260, 20, &m_imgKnobBoost, this );
      m_knob_amp_decay = new BitmapKnob( m_amsynth, "amp_decay", 320, 20, &m_imgKnobBoost, this );
      m_knob_amp_sustain = new BitmapKnob( m_amsynth, "amp_sustain", 380, 20, &m_imgKnobBoost, this );
      m_knob_amp_release = new BitmapKnob( m_amsynth, "amp_release", 440, 20, &m_imgKnobBoost, this );

      m_popup_filter_type = new BitmapKnob( m_amsynth, "filter_type", 288, 92, &m_imgFilterTypeRes, this );
      m_popup_filter_slope = new BitmapKnob( m_amsynth, "filter_slope", 355, 92, &m_imgFilterSlopeRes, this );

      m_knob_filter_resonance = new BitmapKnob( m_amsynth, "filter_resonance", 260, 110, &m_imgKnobBoost, this );
      m_knob_filter_cutoff = new BitmapKnob( m_amsynth, "filter_cutoff", 320, 110, &m_imgKnobSpot, this );
      m_knob_filter_kbd_track = new BitmapKnob( m_amsynth, "filter_kbd_track", 380, 110, &m_imgKnobWidth, this );
      m_knob_filter_env_amount = new BitmapKnob( m_amsynth, "filter_env_amount", 440, 110, &m_imgKnobBoostCut, this );
      m_knob_filter_attack = new BitmapKnob( m_amsynth, "filter_attack", 260, 170, &m_imgKnobBoost, this );
      m_knob_filter_decay = new BitmapKnob( m_amsynth, "filter_decay", 320, 170, &m_imgKnobBoost, this );
      m_knob_filter_sustain = new BitmapKnob( m_amsynth, "filter_sustain", 380, 170, &m_imgKnobBoost, this );
      m_knob_filter_release = new BitmapKnob( m_amsynth, "filter_release", 440, 170, &m_imgKnobBoost, this );

      m_knob_master_vol = new BitmapKnob( m_amsynth, "master_vol", 530, 20, &m_imgKnobBoost, this );
      m_knob_distortion_crunch = new BitmapKnob( m_amsynth, "distortion_crunch", 530, 80, &m_imgKnobBoost, this );

      m_popup_lfo_waveform = new BitmapKnob( m_amsynth, "lfo_waveform", 20, 260, &m_imgWaveformLfo, this );
      m_knob_lfo_freq = new BitmapKnob( m_amsynth, "lfo_freq", 80, 260, &m_imgKnobSpot, this );
      m_knob_freq_mod_amount = new BitmapKnob( m_amsynth, "freq_mod_amount", 140, 260, &m_imgKnobBoost, this );
      m_popup_freq_mod_osc = new BitmapKnob( m_amsynth, "freq_mod_osc", 137, 302, &m_imgOscSelect, this );
      m_knob_filter_mod_amount = new BitmapKnob( m_amsynth, "filter_mod_amount", 200, 260, &m_imgKnobBoost, this );
      m_knob_amp_mod_amount = new BitmapKnob( m_amsynth, "amp_mod_amount", 260, 260, &m_imgKnobBoost, this );

      m_knob_reverb_wet = new BitmapKnob( m_amsynth, "reverb_wet", 350, 260, &m_imgKnobBoost, this );
      m_knob_reverb_roomsize = new BitmapKnob( m_amsynth, "reverb_roomsize", 410, 260, &m_imgKnobWidth, this );
      m_knob_reverb_width = new BitmapKnob( m_amsynth, "reverb_width", 470, 260, &m_imgKnobWidth, this );
      m_knob_reverb_damp = new BitmapKnob( m_amsynth, "reverb_damp", 530, 260, &m_imgKnobBoost, this );

      m_knob_portamento_time = new BitmapKnob( m_amsynth, "portamento_time", 20, 340, &m_imgKnobBoost, this );
      m_popup_portamento_mode = new BitmapKnob( m_amsynth, "portamento_mode", 111, 356, &m_imgPortamentoModes, this );
      m_popup_keyboard_mode = new BitmapKnob( m_amsynth, "keyboard_mode", 205, 356, &m_imgKeybmode, this );
      m_knob_filter_vel_sens = new BitmapKnob( m_amsynth, "filter_vel_sens", 470, 340, &m_imgKnobSpot, this );
      m_knob_amp_vel_sens = new BitmapKnob( m_amsynth, "amp_vel_sens", 530, 340, &m_imgKnobSpot, this );



/*
      //// Create controls

      for (i=0; i<kAmsynthParameterCount; i++)
      {
         const gchar *control_name = parameter_name_from_index ((int) i);

         if (!g_key_file_has_group (gkey_file, control_name)) {
            g_warning ("layout.ini contains no entry for control '%s'", control_name);
            continue;
         }

         gint pos_x = g_key_file_get_integer (gkey_file, control_name, KEY_CONTROL_POS_X, &gerror); HANDLE_GERROR (gerror);
         gint pos_y = g_key_file_get_integer (gkey_file, control_name, KEY_CONTROL_POS_Y, &gerror); HANDLE_GERROR (gerror);
         gchar *type = g_key_file_get_string (gkey_file, control_name, KEY_CONTROL_TYPE, &gerror); HANDLE_GERROR (gerror); g_strstrip (type);
         gchar *resn = g_key_file_get_string (gkey_file, control_name, KEY_CONTROL_RESOURCE, &gerror); HANDLE_GERROR (gerror); g_strstrip (resn);

         /////////////////////////

         GtkWidget *widget = NULL;
         resource_info *res = g_datalist_get_data (&resources, resn);
         if (!res) {
            g_warning ("layout.ini error: control '%s' references a non-existent resource '%s'", control_name, resn);
            continue;
         }
         GdkPixbuf *subpixpuf = gdk_pixbuf_new_subpixbuf (editor_pane_bg, pos_x, pos_y, res->fr_width, res->fr_height);
         GtkAdjustment *adj = adjustments[i];

         if (g_strcmp0 (KEY_CONTROL_TYPE_KNOB, type) == 0)
         {
            widget = bitmap_knob_new (adj, res->pixbuf, res->fr_width, res->fr_height, res->fr_count, editor_scaling_factor);
            bitmap_knob_set_bg (widget, subpixpuf);
            bitmap_knob_set_parameter_index (widget, i);
         }
         else if (g_strcmp0 (KEY_CONTROL_TYPE_BUTTON, type) == 0)
         {
            widget = bitmap_button_new (adj, res->pixbuf, res->fr_width, res->fr_height, res->fr_count, editor_scaling_factor);
            bitmap_button_set_bg (widget, subpixpuf);
         }
         else if (g_strcmp0 (KEY_CONTROL_TYPE_POPUP, type) == 0)
         {
            const char **value_strings = parameter_get_value_strings((int) i);
            widget = bitmap_popup_new (adj, res->pixbuf, res->fr_width, res->fr_height, res->fr_count, editor_scaling_factor);
            bitmap_popup_set_strings (widget, value_strings);
            bitmap_popup_set_bg (widget, subpixpuf);
         }

         g_signal_connect_after(G_OBJECT(widget), "button-press-event", G_CALLBACK (on_control_press), GINT_TO_POINTER(i));
         gtk_fixed_put (GTK_FIXED (fixed), widget, pos_x * editor_scaling_factor, pos_y * editor_scaling_factor);

#if ENABLE_LAYOUT_EDIT
         gtk_buildable_set_name (GTK_BUILDABLE (widget), control_name);
         g_object_set (G_OBJECT (widget), "name", resn, "tooltip-text", type, NULL);
         g_signal_connect(widget, "motion-notify-event", G_CALLBACK(control_move_handler), NULL);
         gtk_widget_add_events(widget, GDK_BUTTON2_MOTION_MASK);
#endif

         g_object_unref (G_OBJECT (subpixpuf));
         g_free (resn);
         g_free (type);
      }

      g_key_file_free (gkey_file);
      g_datalist_clear (&resources);
   }
*/

   }

   //   ~EditorPanel() override
   //   {
   //   }

public slots:
   void updateFromPreset()
   {
      m_popup_osc1_waveform->updateFromPreset();
      m_knob_osc1_pulsewidth->updateFromPreset();
      m_popup_osc2_waveform->updateFromPreset();
      m_knob_osc2_pulsewidth->updateFromPreset();
      m_knob_osc2_range->updateFromPreset();
      m_knob_osc2_pitch->updateFromPreset();
      m_knob_osc2_detune->updateFromPreset();
      //BitmapButton* m_btn_osc2_sync->updateFromPreset();
      m_knob_osc_mix->updateFromPreset();
      m_knob_osc_mix_mode->updateFromPreset();
      m_knob_amp_attack->updateFromPreset();
      m_knob_amp_decay->updateFromPreset();
      m_knob_amp_sustain->updateFromPreset();
      m_knob_amp_release->updateFromPreset();

      m_popup_filter_type->updateFromPreset();
      m_popup_filter_slope->updateFromPreset();

      m_knob_filter_resonance->updateFromPreset();
      m_knob_filter_cutoff->updateFromPreset();
      m_knob_filter_kbd_track->updateFromPreset();
      m_knob_filter_env_amount->updateFromPreset();
      m_knob_filter_attack->updateFromPreset();
      m_knob_filter_decay->updateFromPreset();
      m_knob_filter_sustain->updateFromPreset();
      m_knob_filter_release->updateFromPreset();

      m_knob_master_vol->updateFromPreset();
      m_knob_distortion_crunch->updateFromPreset();

      m_popup_lfo_waveform->updateFromPreset();
      m_knob_lfo_freq->updateFromPreset();
      m_knob_freq_mod_amount->updateFromPreset();
      m_popup_freq_mod_osc->updateFromPreset();
      m_knob_filter_mod_amount->updateFromPreset();
      m_knob_amp_mod_amount->updateFromPreset();

      m_knob_reverb_wet->updateFromPreset();
      m_knob_reverb_roomsize->updateFromPreset();
      m_knob_reverb_width->updateFromPreset();
      m_knob_reverb_damp->updateFromPreset();

      m_knob_portamento_time->updateFromPreset();
      m_popup_portamento_mode->updateFromPreset();
      m_popup_keyboard_mode->updateFromPreset();
      m_knob_filter_vel_sens->updateFromPreset();
      m_knob_amp_vel_sens->updateFromPreset();

   }

protected:
   void paintEvent( QPaintEvent* event ) override
   {
      QPainter dc( this );
      dc.drawImage( rect(), m_imgBackground.img, m_imgBackground.img.rect() );

      QFont font("Tahoma", 12, 400, true );
      dc.setFont( font );
      dc.setPen( QColor(255,128,20) );

      dc.drawText( 300,360, "Win64 port 2023 by" );
      dc.drawText( 280,380, "benjaminhampe@gmx.de" );


      QWidget::paintEvent(event);
   }

};


/*
#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget *
editor_pane_new(void *synthesizer, GtkAdjustment **adjustments, gboolean is_plugin, int scaling_factor);

void modal_midi_learn(Param param_index);

G_END_DECLS
*/

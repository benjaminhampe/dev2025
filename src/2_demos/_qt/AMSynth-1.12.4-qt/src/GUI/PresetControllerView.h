/*
 *  PresetControllerView.h
 *
 *  Copyright (c) 2001-2019 Nick Dowell
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

#include "PresetController.h"
#include "AMSynth.hpp"
#include "filesystem.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

inline void
snprintf_truncate(char *str, size_t size, const char *format, ...)
{
   va_list va_args;
   va_start(va_args, format);
   size_t count = vsnprintf(str, size, format, va_args);
   va_end(va_args);
   if (count >= size - 1)
      strcpy(str + size - 4, "...");
}

/*
inline QPushButton*
button_with_image( const gchar *stock_id, const gchar *label)
{
   if (!gtk_icon_factory_lookup_default (stock_id)) {
      return gtk_button_new_with_label (label);
   }
   GtkWidget *button = gtk_button_new ();
   gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_stock (stock_id, GTK_ICON_SIZE_SMALL_TOOLBAR));
   atk_object_set_name (gtk_widget_get_accessible (button), label);
   return button;
}

inline bool
on_output(GtkSpinButton *spin, gpointer user_data)
{
   static const char *names[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
   gchar text[12];
   gint value = gtk_spin_button_get_value_as_int (spin);
   sprintf (text, "%s%d", names[value % 12], value / 12 - 1);
   gtk_entry_set_text (GTK_ENTRY (spin), text);
   return TRUE;
}
*/

struct PresetControllerView : public QWidget, public UpdateListener
{
   Q_OBJECT
   AMSynth* m_amsynth;
   PresetController* m_presetController;
   QComboBox* m_bank_combo;
   QComboBox* m_preset_combo;
   QPushButton* m_btnSave;
   QPushButton* m_btnPlay;
   QSpinBox* m_audition_spin;
   QPushButton* m_btnPanic;
   u8 m_audition_note; // test note to play
   bool m_inhibit_combo_callback;
   bool m_audition_button_pressed;
public:
   static PresetControllerView*
   create( AMSynth* amsynth, PresetController* presetController, QWidget* parent )
   {
       return new PresetControllerView( amsynth, presetController, parent );
   }

   PresetControllerView( AMSynth* amsynth, PresetController* presetController, QWidget* parent )
      : m_amsynth( amsynth )
      , m_presetController(presetController)
      , m_bank_combo(nullptr)
      , m_preset_combo(nullptr)
      , m_audition_spin(nullptr)
      , m_audition_note(0)
      , m_inhibit_combo_callback(false)
   {
      //this->widget = gtk_hbox_new (FALSE, 0);
      //GtkBox *hbox = GTK_BOX (this->widget);
      auto hbox = new QHBoxLayout();
      hbox->setContentsMargins(0,0,0,0);

      //bank_combo = gtk_combo_box_text_new ();
      m_bank_combo = new QComboBox( this );
      //g_signal_connect (G_OBJECT (bank_combo), "changed", G_CALLBACK (&on_combo_changed), this);
      //gtk_box_pack_start (hbox, bank_combo, FALSE, FALSE, 0);
      hbox->addWidget( m_bank_combo );

      //combo = gtk_combo_box_text_new ();
      m_preset_combo = new QComboBox( this );
      //gtk_combo_box_set_wrap_width (GTK_COMBO_BOX (combo), 4);
      //g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK (&on_combo_changed), this);
      //g_signal_connect (G_OBJECT (combo), "notify::popup-shown", G_CALLBACK (&on_combo_popup_shown), this);
      //gtk_box_pack_start (hbox, combo, TRUE, TRUE, 0);
      hbox->addWidget( m_preset_combo );

      //save_button = button_with_image (GTK_STOCK_SAVE, _("Save"));
      m_btnSave = new QPushButton( "Save", this );
      //g_signal_connect (G_OBJECT (save_button), "clicked", G_CALLBACK (&on_save_clicked), this);
      //gtk_box_pack_start (hbox, save_button, FALSE, FALSE, 0);
      hbox->addWidget( m_btnSave );

      //gtk_box_pack_start (hbox, gtk_label_new ("  "), FALSE, FALSE, 0);

      //GtkWidget *widget = nullptr;
      //widget = button_with_image (GTK_STOCK_MEDIA_PLAY, _("Audition"));
      m_btnPlay = new QPushButton( "Audition", this );
      //widget = button_with_image (GTK_STOCK_MEDIA_PLAY, _("Audition"));
      //g_signal_connect (G_OBJECT (widget), "pressed", G_CALLBACK (&on_audition_pressed), this);
      //g_signal_connect (G_OBJECT (widget), "released", G_CALLBACK (&on_audition_released), this);
      //g_signal_connect (G_OBJECT (widget), "key-press-event", G_CALLBACK (&on_audition_key_press_event), this);
      //g_signal_connect (G_OBJECT (widget), "key-release-event", G_CALLBACK (&on_audition_key_release_event), this);
      //gtk_box_pack_start (hbox, widget, FALSE, FALSE, 0);
      hbox->addWidget( m_btnPlay );

      //GtkAdjustment *audition_adj = (GtkAdjustment *) gtk_adjustment_new(60.0, 0.0, 127.0, 1.0, 5.0, 0.0);
      //audition_spin = gtk_spin_button_new(audition_adj, 1.0, 0);
      m_audition_spin = new QSpinBox( this );
      //gtk_editable_set_editable (GTK_EDITABLE (audition_spin), false);
      //gtk_widget_set_can_focus (audition_spin, FALSE);
      //gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (audition_spin), false);
      //g_signal_connect (G_OBJECT (audition_spin), "output", G_CALLBACK (&on_output), NULL);
      //gtk_box_pack_start (hbox, audition_spin, FALSE, FALSE, 0);
      hbox->addWidget( m_audition_spin );

      //widget = button_with_image (GTK_STOCK_MEDIA_STOP, _("Panic"));
      m_btnPanic = new QPushButton( "Panic", this );
      //g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (&on_panic_clicked), this);
      //gtk_box_pack_start (hbox, widget, FALSE, FALSE, 0);
      hbox->addWidget( m_btnPanic );

      setLayout( hbox );

      populateBankCombo();

      connect( m_btnPlay, SIGNAL(pressed()), this, SLOT(onBtnPlayPressed()) );
      connect( m_btnPlay, SIGNAL(released()), this, SLOT(onBtnPlayReleased()) );

      connect( m_btnPanic, SIGNAL(pressed()), this, SLOT(onBtnPanicPressed()) );
      //updateMe();

      connect( m_bank_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onBankComboIndexChanged(int)) );
      connect( m_preset_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onPresetComboIndexChanged(int)) );
   }

/*
   unsigned char getAuditionNote()
   {
      audition_note = (unsigned char) gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(audition_spin));
      return audition_note;
   }
*/

signals:
   void requestEditorPanelUpdate();

public slots:

   void onBtnPlayPressed()
   {
      //printf("onBtnPlayPressed\n"); fflush(stdout);
      m_amsynth->sendMidi( MIDI_STATUS_NOTE_ON, 57, 127);
   }

   void onBtnPlayReleased()
   {
      //printf("onBtnPlayReleased\n"); fflush(stdout);
      m_amsynth->sendMidi( MIDI_STATUS_NOTE_OFF, 57, 57);
   }

   void onBtnPanicPressed()
   {
      //printf("onBtnPanicPressed\n"); fflush(stdout);
      m_amsynth->sendMidi( MIDI_STATUS_CONTROLLER, MIDI_CC_ALL_SOUND_OFF, 0);
   }

   void onBankComboIndexChanged(int index)
   {
      printf("onBankComboIndexChanged(%d)\n", index ); fflush(stdout);

      std::vector<BankInfo> const & banks = PresetController::getPresetBanks();
      //   that->presetController->loadPresets(banks[bank].file_path.c_str());

      m_presetController->loadPresets( banks[ index ].file_path.c_str() );
      //that->presetController->selectPreset (preset);

      populatePresetCombo();
   }

   void onPresetComboIndexChanged(int index)
   {
      printf("onPresetComboIndexChanged(%d)\n", index ); fflush(stdout);

      //const std::vector<BankInfo> &banks = PresetController::getPresetBanks();
      //   that->presetController->loadPresets(banks[bank].file_path.c_str());

      m_presetController->selectPreset( index );
      //that->presetController->selectPreset (preset);

      emit requestEditorPanelUpdate();
   }

   void populateBankCombo()
   {
      m_bank_combo->clear();

      std::vector<BankInfo> const & banks = PresetController::getPresetBanks();

      //auto presetController = m_amsynth->m_synthesizer.getPresetController();

      //m_bank_combo->addItem( QString::fromStdString( presetController->getFilePath() ) );

      printf("populateBankCombo() :: Bank.Count = %d\n", int(banks.size()) );

      for ( size_t i = 0; i < banks.size(); ++i )
      {
         std::string bankName = Filesystem::fileBase( banks[ i ].file_path );
         if ( bankName.empty() )
         {
            break;
         }
         m_bank_combo->addItem( QString::number(i) + " - " + QString::fromStdString( bankName ), QVariant( int(i) ) );

         printf("Bank[%d] %s\n", int(i), bankName.c_str() );
      }

      fflush(stdout);

      populatePresetCombo();
   }

   void populatePresetCombo()
   {
      m_preset_combo->clear();

      auto presetController = m_amsynth->m_synthesizer.getPresetController();

      for ( size_t i = 0; i < 128; ++i )
      {
         std::string presetName = presetController->getPreset(i).getName();
         if ( presetName.empty() )
         {
            break;
         }
         m_preset_combo->addItem( QString::number(i) + " - " + QString::fromStdString( presetName ), QVariant( int(i) ) );
      }

      //Preset & currentPreset = presetController->getCurrentPreset();

      //std::vector<BankInfo> const & banks = m_amsynth->m_synthesizer.getPresetController()->getPresetBanks();

      printf("populatePresetCombo()\n" );

      for ( size_t i = 0; i < 128; ++i )
      {
         std::string presetName = presetController->getPreset(i).getName();
         if ( presetName.empty() )
         {
            break;
         }
         printf("Preset[%d] %s\n", int(i), presetName.c_str() );
      }
      fflush(stdout);

      /*
      char text [48] = "";

      // bank combo

      gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (bank_combo))));
      for (size_t i=0; i<banks.size(); i++) {
         snprintf_truncate (text, sizeof(text), "[%s] %s", banks[i].read_only ? _("F") : _("U"), banks[i].name.c_str());
         gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (bank_combo), (gint) i, text);
      }

      const std::string current_file_path = presetController->getFilePath();
      for (size_t i=0; i<banks.size(); i++) {
         if (current_file_path == banks[i].file_path) {
            gtk_combo_box_set_active (GTK_COMBO_BOX (bank_combo), (gint) i);
            gtk_widget_set_sensitive (save_button, !banks[i].read_only);
         }
      }

      // preset combo

      gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (combo))));

      for (gint i = 0; i < PresetController::kNumPresets; i++) {
         snprintf_truncate (text, sizeof(text), "%d: %s", i, presetController->getPreset(i).getName().c_str());
         gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo), i, text);
      }
      gtk_combo_box_set_active (GTK_COMBO_BOX (combo), presetController->getCurrPresetNumber());

      // allow the preset combo to be narrower than the longest preset name, prevents the window getting wider than the editor
      gtk_widget_set_size_request (combo, 100, -1);

      inhibit_combo_callback = false;
      */
   }


   //~PresetControllerView() override {}

   //   static void on_combo_changed (GtkWidget *widget, PresetControllerViewImpl *);
   //   static void on_combo_popup_shown (GObject *gobject, GParamSpec *pspec, PresetControllerViewImpl *);
   //   static void on_save_clicked (GtkWidget *widget, PresetControllerViewImpl *);
   //   static void on_audition_pressed (GtkWidget *widget, PresetControllerViewImpl *);
   //   static void on_audition_released (GtkWidget *widget, PresetControllerViewImpl *);
   //   static void on_panic_clicked (GtkWidget *widget, PresetControllerViewImpl *);

   //   static gboolean on_audition_key_press_event (GtkWidget *widget, GdkEventKey *event, PresetControllerViewImpl *);
   //   static gboolean on_audition_key_release_event (GtkWidget *widget, GdkEventKey *event, PresetControllerViewImpl *);

   /*
   void on_combo_changed (GtkWidget *widget, PresetControllerViewImpl *that)
   {
      if (that->inhibit_combo_callback)
         return;

      if (widget == that->bank_combo) {
         gint bank = gtk_combo_box_get_active (GTK_COMBO_BOX (that->bank_combo));
         const std::vector<BankInfo> &banks = PresetController::getPresetBanks();
         that->presetController->loadPresets(banks[bank].file_path.c_str());
      }

      gint preset = gtk_combo_box_get_active (GTK_COMBO_BOX (that->combo));
      that->presetController->selectPreset (PresetController::kNumPresets - preset - 1);
      that->presetController->selectPreset (preset);
   }

   void on_combo_popup_shown (GObject *gobject, GParamSpec *pspec, PresetControllerViewImpl *that)
   {
      that->presetController->loadPresets();
   }

   void on_save_clicked (GtkWidget *widget, PresetControllerViewImpl *that)
   {
      that->presetController->loadPresets(); // in case another instance has changed any of the other presets
      that->presetController->commitPreset();
      that->presetController->savePresets();
      that->update();
   }


   void updateMe()
   {
      inhibit_combo_callback = true;

      const std::vector<BankInfo> &banks = PresetController::getPresetBanks();

      char text [48] = "";

      // bank combo

      gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (bank_combo))));
      for (size_t i=0; i<banks.size(); i++) {
         snprintf_truncate (text, sizeof(text), "[%s] %s", banks[i].read_only ? _("F") : _("U"), banks[i].name.c_str());
         gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (bank_combo), (gint) i, text);
      }

      const std::string current_file_path = presetController->getFilePath();
      for (size_t i=0; i<banks.size(); i++) {
         if (current_file_path == banks[i].file_path) {
            gtk_combo_box_set_active (GTK_COMBO_BOX (bank_combo), (gint) i);
            gtk_widget_set_sensitive (save_button, !banks[i].read_only);
         }
      }

      // preset combo

      gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (combo))));

      for (gint i = 0; i < PresetController::kNumPresets; i++) {
         snprintf_truncate (text, sizeof(text), "%d: %s", i, presetController->getPreset(i).getName().c_str());
         gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo), i, text);
      }
      gtk_combo_box_set_active (GTK_COMBO_BOX (combo), presetController->getCurrPresetNumber());

      // allow the preset combo to be narrower than the longest preset name, prevents the window getting wider than the editor
      gtk_widget_set_size_request (combo, 100, -1);

      inhibit_combo_callback = false;
   }



   */

   //GtkWidget * getWidget() override { return widget; }


};



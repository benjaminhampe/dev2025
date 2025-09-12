/*
 *  main.cpp
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

#ifdef WITH_GUI
#include "GUI/gui_main.h"
#include "GUI/MainWindow.h"
#endif

void fatal_error(const std::string & msg)
{
   std::cerr << msg << std::endl;
#ifdef WITH_GUI
   ShowModalErrorMessage(msg);
#endif
   exit(1);
}

//#include <fcntl.h>
//#include <getopt.h>
//#include <unistd.h>

#include <stdlib.h>
#include <sys/time.h>
//#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>

int main( int argc, char *argv[] )
{
   srand((unsigned) time(nullptr));

   bool no_gui = false; // (getenv("AMSYNTH_NO_GUI") != nullptr);
   int gui_scale_factor = 0;

#ifdef WITH_GUI
   //if (!no_gui)
      //gui_kit_init(&argc, &argv);
#endif

   AMSynth amsynth;
   amsynth.Start();

/*
   open_midi();
   midiBuffer = (unsigned char *)malloc(midiBufferSize);

   // prevent lash from spawning a new jack server
   setenv("JACK_NO_START_SERVER", "1", 0);

   if (config.alsa_seq_client_id != 0 || !config.jack_client_name.empty())
      // LASH only works with ALSA MIDI and JACK
      amsynth_lash_init();

   if (config.alsa_seq_client_id != 0) // alsa midi is active
      amsynth_lash_set_alsa_client_id((unsigned char) config.alsa_seq_client_id);

   if (!config.jack_client_name.empty())
      amsynth_lash_set_jack_client_name(config.jack_client_name.c_str());

   // give audio/midi threads time to start up first..
   // if (jack) sleep (1);

   if (pipe(gui_midi_pipe) != -1) {
      fcntl(gui_midi_pipe[0], F_SETFL, O_NONBLOCK);
   }
*/
   if (!no_gui)
   {
      //main_window_show(s_synthesizer, out, gui_scale_factor);
      //gui_kit_run(&amsynth_timer_callback);

      QApplication app( argc, argv );

      auto mainWindow = new MainWindow( &amsynth, gui_scale_factor);
      //g_signal_connect(G_OBJECT(mainWindow->window), "delete-event", G_CALLBACK(delete_event), mainWindow);
      //g_signal_connect(G_OBJECT(mainWindow->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

      //midiLearnDialog = new MIDILearnDialog(
      //      synthesizer->getMidiController(),
      //      synthesizer->getPresetController(),
      //      GTK_WINDOW(mainWindow->window));

      //g_idle_add(startup_check, mainWindow);
      //return mainWindow->window;
      //gtk_widget_show_all();
      mainWindow->show();

      app.exec();
   }

   amsynth.Stop();

   if (amsynth.m_config.xruns)
   {
      std::cerr << amsynth.m_config.xruns << " audio buffer underruns occurred" << std::endl;
   }

   return 0;
}

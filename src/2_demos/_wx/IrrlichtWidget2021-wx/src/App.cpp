/***************************************************************
 * Name:      App.cpp
 * Purpose:   Code for Application Class
 * Author:    BenjaminHampe@gmx.de (BenjaminHampe@gmx.de)
 * Created:   2011-07-12
 * Copyright: BenjaminHampe@gmx.de (BenjaminHampe@gmx.de)
 * License:
 **************************************************************/

#include "App.hpp"

bool App::OnInit()
{
   // Print CommandLine Commands wxCmdLineEntryDesc
   size_t c = sizeof(commands)/sizeof(commands[0])-1;

   wxString s = wxT("");
   for ( size_t i = 0; i < c; ++i )
   {
      s += wxString::Format(
      wxT("Option [%d]\t%s, %s, %s, %d, %d\n"),
      i,
      //commands[i].kind,
      commands[i].shortName,
      commands[i].longName,
      commands[i].description,
      commands[i].type,
      commands[i].flags);
   }

   std::cout << s.mb_str(wxConvUTF8) << std::endl;

   // Set CommandLine Parser
   wxCmdLineParser parser(commands, argc, argv);
   parser.SetDesc(commands);
   parser.SetLogo(wxT("Irrlicht2011"));
   parser.SetSwitchChars(wxT("-"));
   parser.Parse(false);

   // Parse CommandLine
   bool showHelp = parser.Found(wxT("h"));
   if (showHelp)
      wxMessageBox(s,wxT("wx-Irrlicht2011 CommandLineOptions"));

   // Print CommandLine Arguments
   for ( size_t i = 0; i < parser.GetParamCount(); ++i )
   {
      wxString p = parser.GetParam( i );
      wxLogMessage( p.c_str() );
      wxMessageBox( p, wxT("Param") + i );
   }

   // Create GUI
   Window* win = new Window(0L, wxID_ANY, wxT("Irrlicht2011"));
   win->SetIcon(wxICON(aaaa)); // To Set App Icon
   win->Show();
   return true;
}

int App::OnExit()
{
   // clean up
   return 0;
}

int App::OnRun()
{
   int exitcode = wxApp::OnRun();
   if ( exitcode!=0 ) 
   {
      // error
   }
   return exitcode;
}

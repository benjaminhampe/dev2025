#include "Window_wx.hpp"

// ===================================================================
// Window
// ===================================================================
BEGIN_EVENT_TABLE(Window, wxFrame)
   EVT_CLOSE(Window::closeEvent)
   //EVT_DROP_FILES(Window::dropFilesEvent)
   //EVT_SLIDER(id, func):
   EVT_MENU(MenuID_FileExit, Window::onMenuFileExit)
   EVT_MENU(MenuID_FileLoad, Window::onMenuFileOpen)
   // EVT_MENU(MenuID_FileSAVE, Window::OnSave)
   // EVT_MENU(MenuID_FileSAVE_AS, Window::OnSaveAs)
   EVT_MENU(MenuID_HelpAbout, Window::onMenuHelpAbout)
END_EVENT_TABLE()

Window::Window(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos,
      wxSize const & size,
      long style )
   : wxFrame(parent, id, wxT("VLC Player wx + ffmpeg"), pos, size, style)
   , m_MenuBar( nullptr )
   , m_MenuFile( nullptr )
   , m_MenuHelp( nullptr )
{
   //   wxFont fontAwesome( 16, wxFONTFAMILY_UNKNOWN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _("fontawesome") );
   //   SetFont( fontAwesome );
   SetSize(800, 600);
   Center();
   DragAcceptFiles( true );
   createMenuBar();
   createStatusBar();

   m_MidiKeyboard = new PianoKeyboardCtrl( this, wxID_ANY );

   wxBoxSizer* v = new wxBoxSizer( wxVERTICAL );
   v->Add( m_MidiKeyboard, wxSizerFlags().Proportion(1).Expand() );
   SetSizer( v );

}

Window::~Window()
{
}

void Window::createStatusBar()
{
   CreateStatusBar(1);
   SetStatusText(_("RtAudioConfigDialog-wx (c) 2021 Benjamin Hampe <benjamin.hampe@gmx.de> | "),0);
}

void Window::createMenuBar()
{
   m_MenuFile = new wxMenu(_T(""));
   m_MenuFile->Append(MenuID_FileLoad, _("&Load\tAlt-O"), _("Laden von Dateien und Projekten"));
   m_MenuFile->Append(MenuID_FileExit, _("&Exit VLC\tAlt-F4"), _("Programm beenden"));

   m_MenuHelp = new wxMenu(_T(""));
   m_MenuHelp->Append(MenuID_HelpAbout, _("&About\tF12"), _("Infos"));

   m_MenuBar = new wxMenuBar();
   m_MenuBar->Append( m_MenuFile, _("&File"));
   m_MenuBar->Append( m_MenuHelp, _("&Help"));
   SetMenuBar( m_MenuBar );
}

// void Window::dropFilesEvent( wxDropFilesEvent& event )
// {
   // int fileCount = event.GetNumberOfFiles();
   // DE_DEBUG("FileCount = ", fileCount )
   // for ( int i = 0; i < fileCount; ++i )
   // {
      // wxString const & uri = event.GetFiles()[ i ];
      // DE_DEBUG("File[",i,"] ", uri.ToStdString() )
   // }
   // if ( fileCount > 0 )
   // {
      // std::string uri = event.GetFiles()[ 0 ].ToStdString();
      // openFile( uri );
   // }
// }

void Window::closeEvent( wxCloseEvent & event )
{
   Destroy();
}

void Window::onMenuFileExit( wxCommandEvent & event )
{
   Destroy();
}

void Window::onMenuHelpAbout( wxCommandEvent & event )
{
   wxMessageBox( wxT("wxIrrlicht (c) 2008-2011 by BenjaminHampe@gmx.de\n\nVersion 1.0\n"), wxT("About"));
}

void Window::onMenuFileOpen( wxCommandEvent & event )
{
   wxFileDialog fileDialog( this );
   wxString uri;
   if ( fileDialog.ShowModal() == wxID_OK )
   {
      uri = fileDialog.GetPath();
   }

//   openFile( uri.ToStdString() );

   event.Skip();
}

// void Window::openFile( std::string uri )
// {
   // if ( uri.empty() )
   // {
      // DE_ERROR("Empty uri")
      // return;
   // }

   // if ( !m_File.open( uri ) )
   // {
      // DE_ERROR("Cant open uri ",uri )
      // return;
   // }

   // DE_DEBUG("Opened VLC uri ",uri )

   // //std::string saveName = uri.ToStdString() + ".coverart.jpg";
   // //dbSaveImage( m_File.m_CoverArt, saveName );
   // //DE_DEBUG("Saved CoverArt ",saveName)

   // m_imgVideo->setImage( m_File.m_CoverArt );
   // updateMenuBarFromMediaFile();

   // if ( !de::audio::BufferLoader::load( m_Buffer, uri, de::audio::ST_S16I, -1 ) )
   // {
      // DE_ERROR("Cant load waveform uri ", uri)
   // }
   // else
   // {
      // DE_DEBUG("Loaded waveform uri = ", uri)
      // DE_DEBUG("Loaded waveform buffer = ",m_Buffer.toString())

      // de::Image waveform( 1024,64 );
      // de::audio::BufferDraw::drawChannelWaveform( m_Buffer, 0, waveform, waveform.getRect() );
      // m_imgWaveform->setImage( waveform );
   // }

   //de::fileconverter::convert2wav( uri );

   // m_File.hasDurationFast();
// }

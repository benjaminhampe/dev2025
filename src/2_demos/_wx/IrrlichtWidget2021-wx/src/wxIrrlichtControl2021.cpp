#include "wxIrrlichtControl2021.hpp"
#include <wx/filename.h>

BEGIN_EVENT_TABLE(wxIrrlichtControl, wxControl)
   EVT_TIMER(ID_IRR_TIMER, wxIrrlichtControl::timerEvent)
   EVT_SIZE(wxIrrlichtControl::resizeEvent)
   EVT_PAINT(wxIrrlichtControl::paintEvent)
   EVT_ERASE_BACKGROUND(wxIrrlichtControl::eraseBackgroundEvent)
   EVT_MOUSE_EVENTS(wxIrrlichtControl::mouseEvent)
   EVT_KEY_DOWN(wxIrrlichtControl::keyPressEvent)
   EVT_KEY_UP(wxIrrlichtControl::keyReleaseEvent)
   EVT_ENTER_WINDOW(wxIrrlichtControl::enterEvent)
   EVT_LEAVE_WINDOW(wxIrrlichtControl::leaveEvent)
END_EVENT_TABLE()

wxIrrlichtControl::wxIrrlichtControl(
         wxWindow* parent,
         wxWindowID id,
         wxPoint const & pos,
         wxSize const & size,
         long style,
         wxString const & name)
   : wxControl( parent, id, pos, size, style, wxDefaultValidator, name )
   , m_Device( nullptr )
   , m_Camera( nullptr )
   , m_Timer( this, ID_IRR_TIMER )
   , m_FPSLabel( nullptr )
   , m_ShowFPSLabel( true )
{
   // std::cout << "wxIrrlichtControl()" << std::endl;
   SetBackgroundStyle( wxBG_STYLE_CUSTOM );
   init();
   startTimer();
}

wxIrrlichtControl::~wxIrrlichtControl()
{
   // std::cout << "~wxIrrlichtControl()" << std::endl;
   stopTimer();
   if ( m_Device )
   {
      m_Device->drop();
      m_Device = nullptr;
   }
}

void wxIrrlichtControl::enterEvent( wxMouseEvent & event )
{
   std::cout << __func__ << "\n";
}
void wxIrrlichtControl::leaveEvent( wxMouseEvent & event )
{
   std::cout << __func__ << "\n";
}

void wxIrrlichtControl::startTimer(int milliseconds)
{
   // std::cout << "wxIrrlichtControl::startTimer(" << milliseconds << ")" << std::endl;
   m_Timer.Start( milliseconds );
}

void wxIrrlichtControl::stopTimer()
{
   // std::cout << "wxIrrlichtControl::stopTimer()" << std::endl;
   m_Timer.Stop();
}

void wxIrrlichtControl::init()
{
   if ( m_Device )
   {
      return; // error
   }

   //std::cout << "wxIrrlichtControl::init() " << std::endl;

   // NullDevice:
   //uint32_t desktop = m_Device->getVideoModeList()->getDesktopResolution().Width;
   //uint32_t desktop = m_Device->getVideoModeList()->getDesktopResolution().Width;
   uint32_t w = uint32_t( GetClientSize().GetX() );
   uint32_t h = uint32_t( GetClientSize().GetY() );
   m_CreateParams.DriverType = irr::video::EDT_OPENGL;
   m_CreateParams.WindowSize = irr::core::dimension2du( w, h );
   m_CreateParams.Bits = 32;
   m_CreateParams.LoggingLevel = irr::ELL_NONE;
#ifdef LINUX
//    wxStaticText* pnl = new wxStaticText(this, -1, wxEmptyString);
//    pnl->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
//    GtkWidget* handle = pnl->GetHandle();
//    if (!handle) return;
//    XID xHandle = GDK_WINDOW_XWINDOW(handle->window);
//    Display* mDisp = GDK_WINDOW_XDISPLAY(handle->window);
//    stringc dName = (long)mDisp;
//    dName += ':';
//    dName += (int)xHandle;
//    actual_params->WindowId = (long)dName.c_str();
#else
   m_CreateParams.WindowId = reinterpret_cast< void* >( this->GetHandle() );
#endif
   m_Device = irr::createDeviceEx(m_CreateParams);
   if ( !m_Device )
   {
      throw "Can't create Irrlicht m_Device!";
   }
   m_Device->setResizable( true );

   irr::video::IVideoDriver* driver = m_Device->getVideoDriver();

   std::wstringstream s;
   irr::IOSOperator* ios = m_Device->getOSOperator();
   s << ios->getOperationSystemVersion() << "\n";
   uint32_t CPU_MHz = 0, RAM_Total = 0, RAM_Free = 0;
   ios->getProcessorSpeedMHz(&CPU_MHz);
   ios->getSystemMemory(&RAM_Total, &RAM_Free);
   s << "CPU " << CPU_MHz << " MHz\n";
   s << "RAM " << RAM_Free << " / " << RAM_Total << "\n";
   s << "GPU " << driver->getVendorInfo().c_str() << "\n";
   std::wcout << s.str() << std::endl;

   irr::gui::IGUIEnvironment* guienv = m_Device->getGUIEnvironment();
   m_FPSLabel = guienv->addStaticText(L"FPS: 0", irr::core::rect<irr::s32>(50, 50, 150, 100), false);
   m_FPSLabel->setOverrideColor( irr::video::SColor(255,255,255,255) );

   irr::scene::ISceneManager* smgr = m_Device->getSceneManager();
   irr::scene::ISceneNode* root = smgr->getRootSceneNode();
   m_Camera = smgr->addCameraSceneNode(root);
   if ( m_Camera )
   {
      m_Camera->setPosition(irr::core::vector3df(0,50,0));
      m_Camera->setTarget(irr::core::vector3df(0,0,0));
      irr::scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(irr::core::vector3df(0,15,0), 30.0f);
      m_Camera->addAnimator(anim);
      anim->drop();
   }

   irr::scene::ISceneNode* cube = smgr->addCubeSceneNode(20);
    if (cube)
    {
    #if 0
        cube->setMaterialTexture(0, driver->getTexture("../media/mauer.png"));
        cube->setMaterialTexture(1, driver->getTexture("../media/hyper01.jpg"));
        cube->setMaterialFlag( irr::video::EMF_LIGHTING, false );
        cube->setMaterialType( irr::video::EMT_REFLECTION_2_LAYER );
   #else
        std::string uri = "media/wxIrrlicht/tex.png";
/*
        wxString a0_path;
        wxString a0_name;
        wxString a0_ext;
        wxFileName::SplitPath( wxTheApp->argv[0], &a0_path, &a0_name, &a0_ext, wxPATH_NATIVE );
        //a0_path.Replace( "\\", "/" );
        std::cout << "a0_path = " << a0_path.ToUTF8().data() << std::endl;
        std::cout << "a0_name = " << a0_name.ToUTF8().data() << std::endl;
        std::cout << "a0_ext = " << a0_ext.ToUTF8().data() << std::endl;

        std::string uri2 = a0_path.ToUTF8().data();
        uri2 += "\\media\\wxIrrlicht\\tex.png";
        //a0_path.Replace( "\\", "/" );
*/
        auto tex = driver->getTexture( uri.c_str() );
         if ( !tex )
         {
            std::cout << "No texture " << uri << std::endl;
         }
        cube->setMaterialTexture(0, tex );
        cube->setMaterialFlag( irr::video::EMF_LIGHTING, false );
        cube->setMaterialType( irr::video::EMT_SOLID );
   #endif
    }

//			smgr->addSkyBoxSceneNode(
//			driver->getTexture("irrlicht2_up.jpg"),
//			driver->getTexture("irrlicht2_dn.jpg"),
//			driver->getTexture("irrlicht2_lf.jpg"),
//			driver->getTexture("irrlicht2_rt.jpg"),
//			driver->getTexture("irrlicht2_ft.jpg"),
//			driver->getTexture("irrlicht2_bk.jpg"));


}//InitIrr()


void wxIrrlichtControl::render()
{
   //std::cout<<"wxIrrlichtControl::Render()"<<std::endl;
   if ( !m_Device ) { return; }
   if ( !m_Device->run() )
   {
      std::cout << "Device not running" << std::endl;
      return;
   }

   irr::video::IVideoDriver* driver = m_Device->getVideoDriver();
   irr::scene::ISceneManager* smgr = m_Device->getSceneManager();
   irr::gui::IGUIEnvironment* guienv = m_Device->getGUIEnvironment();

   driver->beginScene(true, true, irr::video::SColor(255,155,155,150));
   smgr->drawAll();
   guienv->drawAll();
   driver->endScene();

   if ( m_FPSLabel && m_ShowFPSLabel)
   {
      int fps = driver->getFPS();
      static int lastFPS = 0;
      if (lastFPS != fps)
      {
         irr::core::stringw str = L"FPS: ";
         str += fps;
         m_FPSLabel->setText(str.c_str());
         lastFPS = fps;
      }
   }
}

void wxIrrlichtControl::eraseBackgroundEvent(wxEraseEvent& event)
{
   std::cout<< __func__ << " :: Do i need this event?\n";
}

void wxIrrlichtControl::paintEvent(wxPaintEvent& event)
{
   //std::cout<<"wxIrrlichtControl::paintEvent()"<<std::endl;
   wxPaintDC paint_dc(this);
   render();
}

void wxIrrlichtControl::resizeEvent(wxSizeEvent& event)
{
   uint32_t w = event.GetSize().x;
   uint32_t h = event.GetSize().y;

   std::cout << "wxIrrlichtControl::resizeEvent(" << w << "," << h << ")" << std::endl;

   if ( w < 1 ) { return; }
   if ( h < 1 ) { return; }

   if ( m_Device )
   {
      irr::video::IVideoDriver* driver = m_Device->getVideoDriver();
      irr::scene::ISceneManager* smgr = m_Device->getSceneManager();
      irr::gui::IGUIEnvironment* guienv = m_Device->getGUIEnvironment();

      driver->OnResize( irr::core::dimension2du( w,h ) );
      driver->setViewPort( irr::core::recti( 0,0,w,h ) );
      if ( smgr->getActiveCamera() )
      {
         smgr->getActiveCamera()->setAspectRatio( float( w ) / float( h ) );
      }

      m_Device->run();
   }
}

void wxIrrlichtControl::timerEvent(wxTimerEvent& event)
{
   //std::cout<<"wxIrrlichtControl::timerEvent()"<<std::endl;
   render();
}

void wxIrrlichtControl::mouseEvent(wxMouseEvent& event)
{
   //std::cout<<"wxIrrlichtControl::mouseEvent()"<<std::endl;
   irr::SEvent sevt;
   sevt.EventType = irr::EET_MOUSE_INPUT_EVENT;

   if (event.IsButton())
   {
      if (event.LeftDown()) sevt.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
      else if (event.LeftUp()) sevt.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
      else if (event.MiddleDown()) sevt.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
      else if (event.MiddleUp()) sevt.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
      else if (event.RightDown()) sevt.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
      else if (event.RightUp()) sevt.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
   }
   else if (event.GetWheelRotation() != 0)
   {
      sevt.MouseInput.Wheel = (float)event.GetWheelRotation();
      sevt.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
   }
   else if (event.Moving() || event.Dragging())
   {
      sevt.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
   }

   sevt.MouseInput.X = event.GetX();
   sevt.MouseInput.Y = event.GetY();

   if ( m_Device )
   {
      m_Device->postEventFromUser(sevt);
   }

   if ( !m_Timer.IsRunning() )
   {
      render();
   }

   event.Skip();
}

void wxIrrlichtControl::keyPressEvent(wxKeyEvent& event)
{
   // std::cout<<"wxIrrlichtControl::OnKey()"<<std::endl;
   irr::SEvent sevt;
   sevt.EventType = irr::EET_KEY_INPUT_EVENT;
   sevt.KeyInput.Key = (irr::EKEY_CODE)event.GetRawKeyCode();
   sevt.KeyInput.PressedDown = event.GetEventType() == wxEVT_KEY_DOWN;
   sevt.KeyInput.Shift = event.ShiftDown();
   sevt.KeyInput.Control = event.CmdDown();
   #if wxUSE_UNICODE
   sevt.KeyInput.Char = event.GetUnicodeKey();
   #else
   sevt.KeyInput.Char = event.GetKeyCode();
   #endif

   if ( m_Device )
   {
      m_Device->postEventFromUser(sevt);
   }

   if ( !m_Timer.IsRunning() )
   {
      render();
   }

   event.Skip();
}

void wxIrrlichtControl::keyReleaseEvent(wxKeyEvent& event)
{
   // std::cout<<"wxIrrlichtControl::OnKey()"<<std::endl;
   irr::SEvent sevt;
   sevt.EventType = irr::EET_KEY_INPUT_EVENT;
   sevt.KeyInput.Key = (irr::EKEY_CODE)event.GetRawKeyCode();
   sevt.KeyInput.PressedDown = !(event.GetEventType() == wxEVT_KEY_UP);
   sevt.KeyInput.Shift = event.ShiftDown();
   sevt.KeyInput.Control = event.CmdDown();
   #if wxUSE_UNICODE
   sevt.KeyInput.Char = event.GetUnicodeKey();
   #else
   sevt.KeyInput.Char = event.GetKeyCode();
   #endif

   if ( m_Device )
   {
      m_Device->postEventFromUser(sevt);
   }

   if ( !m_Timer.IsRunning() )
   {
      render();
   }

   event.Skip();
}

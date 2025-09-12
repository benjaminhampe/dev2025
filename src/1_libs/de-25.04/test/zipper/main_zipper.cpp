#include <DarkImage.hpp>
#include <iostream>
#include <fstream>
#include <Irrlicht/irrlicht.h>

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   irr::IrrlichtDevice* device = irr::createDevice( irr::video::EDT_SOFTWARE );

   std::string uri;
   if ( argc > 1 )
   {
      uri = argv[ 1 ];
   }

   if ( device )
   {
      irr::io::IFileArchive* archive = nullptr;
      device->getFileSystem()->addFileArchive(
         "example.zip",
         true, // ignoreCase
         true, // ignorePaths
         irr::io::EFAT_ZIP,
         "",
         &archive );

      if ( !archive )
      {
         std::cout << "no archive" << std::endl;
      }
      else
      {
         std::cout << "got archive" << std::endl;
         archive->drop();
      }

      //irr::io::IReadFile* file = device->getFileSystem()->createAndOpenFile( uri.c_str() );

//      if ( device->getFileSystem()->addZipFileArchive( uri.c_str(), true, true ) )
//      {

//      }

      device->drop();
   }


}

/*
 *
 * #include <irrlicht.h>
#include <driverChoice.h>
#include <SMeshBuffer.h>
#include <vector>
#include <sstream>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#include <Matrix3D_v1.h>
#include <Matrix3D_v2.h>
#include <Matrix3D_v3.h>
#include <Waves3D_v1.h>
#include <Waves3D_v2.h>
#include <Waves3D_v3.h>
#include <Orbit3D_v1.h>
#include <Orbit3D_v2.h>

int main()
{
   // ask user for driver
   E_DRIVER_TYPE driverType=EDT_OPENGL; //driverChoiceConsole();
   if (driverType==EDT_COUNT) return 1;

   // create device
   IrrlichtDevice* device = createDevice( driverType, dimension2d<u32>(1024, 768), 32, false);
   if (!device) return 1; // could not create selected driver.
   device->setResizable( true );
   device->setWindowCaption(L"Custom Scene Node - Irrlicht Engine Demo");

   IVideoDriver* driver = device->getVideoDriver();
   ISceneManager* smgr =  device->getSceneManager();
   ISceneNode* root = smgr->getRootSceneNode();

   // --> create camera:
   //smgr->addCameraSceneNode(0, vector3df(0,-40,0), vector3df(0,0,0));

   SKeyMap km[ 10 ];
   km[ 0 ].Action = EKA_MOVE_FORWARD;
   km[ 0 ].KeyCode = KEY_KEY_W;
   km[ 1 ].Action = EKA_MOVE_BACKWARD;
   km[ 1 ].KeyCode = KEY_KEY_S;
   km[ 2 ].Action = EKA_STRAFE_LEFT;
   km[ 2 ].KeyCode = KEY_KEY_A;
   km[ 3 ].Action = EKA_STRAFE_RIGHT;
   km[ 3 ].KeyCode = KEY_KEY_D;
   km[ 4 ].Action = EKA_JUMP_UP;
   km[ 4 ].KeyCode = KEY_SPACE;
   km[ 5 ].Action = EKA_CROUCH;
   km[ 5 ].KeyCode = KEY_KEY_C;

   km[ 6 ].Action = EKA_MOVE_FORWARD;
   km[ 6 ].KeyCode = KEY_UP;
   km[ 7 ].Action = EKA_MOVE_BACKWARD;
   km[ 7 ].KeyCode = KEY_DOWN;
   km[ 8 ].Action = EKA_STRAFE_LEFT;
   km[ 8 ].KeyCode = KEY_LEFT;
   km[ 9 ].Action = EKA_STRAFE_RIGHT;
   km[ 9 ].KeyCode = KEY_RIGHT;

   scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS(
      smgr->getRootSceneNode(),  // parent
      100,                       // rotate speed
      0.5f,                      // move speed
      -1,                        // id
      km,                        // SKeyMap[]
      10,                        // SKeyMap size
      false,                     // noVerticalMovement
      1.0f,                      // jump speed
      false,                     // invert mouse
      true                       // make active
   );

   cam->setPosition( vector3df(300,500,0) );
   cam->setTarget( vector3df(301,-40,200) );

   Matrix3D_v1* node = new Matrix3D_v1( root, smgr, 666 );
   Matrix3D_v2* node2 = new Matrix3D_v2( root, smgr, 667 );
   Matrix3D_v3* node3 = new Matrix3D_v3( root, smgr, 668 );
   Waves3D_v1* node4 = new Waves3D_v1( root, smgr, 701 );
   Waves3D_v2* node5 = new Waves3D_v2( root, smgr, 702 );
   Waves3D_v3* node6 = new Waves3D_v3( root, smgr, 703 );
   Orbit3D_v1* node7 = new Orbit3D_v1( root, smgr, 707 );
//   Orbit3D_v1* node8 = new Orbit3D_v1( root, smgr, 708 );
//   Orbit3D_v1* node9 = new Orbit3D_v1( root, smgr, 709 );
   node2->setPosition( vector3df(700,0,0) );
   node3->setPosition( vector3df(1400,0,0) );
   node4->setPosition( vector3df(0,0,800) );
   node5->setPosition( vector3df(700,0,800) );
   node6->setPosition( vector3df(1400,0,800) );
   node7->setPosition( vector3df(0,0,-1000) );
//   node8->setPosition( vector3df(1100,0,-1000) );
//   node9->setPosition( vector3df(2200,0,-1000) );
   IImage* img = driver->createImageFromFile( "../../benni.ico_entry0_256x256_24.png" );
   node->setImage( img );
   node2->setImage( img );
   node3->setImage( img );
   //node4->setImage( img );
   node5->setImage( img );
   node6->setImage( img );
//   ISceneNodeAnimator*
//   anim = smgr->createRotationAnimator( vector3df(0.21f, 0, 0.11f));
//   if (anim)
//   {
//      node->addAnimator(anim);
//      anim->drop();
//      anim = 0;
//   }
   node->drop();
   node2->drop();
   node3->drop();
   node4->drop();
   node5->drop();
   node6->drop();
   node7->drop();
//   node8->drop();
//   node9->drop();
   node = 0; // As I shouldn't refer to it again, ensure that I can't
   node2 = 0; // As I shouldn't refer to it again, ensure that I can't
   node3 = 0; // As I shouldn't refer to it again, ensure that I can't

   // Now draw everything and finish.
   u32 frames=0;

   size_t nTime0 = device->getTimer()->getTime();
   size_t msWait = 31;

   while (device->run())
   {
      size_t nTime1 = device->getTimer()->getTime();
      if (nTime1 - nTime0 > msWait)
      {
         driver->beginScene(true, true, SColor(0,100,100,200));

         smgr->drawAll();

         driver->endScene();

         nTime0 = nTime1;
      }
      else
      {
         device->yield();
      }

      if (++frames==100)
      {
         stringw str = L"Mandelbrot 3D screensaver [";
         str += driver->getName();
         str += L"] FPS: ";
         str += (s32)driver->getFPS();
         str += L" | ";
         auto cam = smgr->getActiveCamera();
         if ( cam )
         {
            auto pos = cam->getAbsoluteTransformation().getTranslation();
            str += L"CamPos(";
            str += int(pos.X);
            str += L",";
            str += int(pos.Y);
            str += L",";
            str += int(pos.Z);
            str += L"), ";

            auto deg = cam->getAbsoluteTransformation().getRotationDegrees();
            str += L"CamAng(";
            str += int(deg.X);
            str += L",";
            str += int(deg.Y);
            str += L",";
            str += int(deg.Z);
            str += L"), ";

            auto focus = cam->getTarget();
            str += L"CamFocus(";
            str += int(focus.X);
            str += L",";
            str += int(focus.Y);
            str += L",";
            str += int(focus.Z);
            str += L")";
         }

         device->setWindowCaption(str.c_str());
         frames=0;
      }
   }
   device->drop();
   return 0;
}

*/

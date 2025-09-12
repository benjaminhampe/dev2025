/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
#include <RtEnumerator.hpp>
#include <iostream>
#include <sstream>

#include <TestEngine1.hpp>
#include <TestEngine2.hpp>

int main( int argc, char** argv )
{
   RtEnumerator enumerator;
   enumerator.enumerate( RtAudio::WINDOWS_WASAPI );
   std::cout << enumerator.toString() << std::endl;

   std::cout << "=========== =" << std::endl;
//   enumerator.enumerate( RtAudio::WINDOWS_DS );
//   std::cout << enumerator.toString() << std::endl;

//   enumerator.enumerate( RtAudio::WINDOWS_ASIO );
//   std::cout << enumerator.toString() << std::endl;

//   enumerator.enumerate( RtAudio::RTAUDIO_DUMMY );
//   std::cout << enumerator.toString() << std::endl;

   TestEngine1 t1;

   std::cout << "=========== ==" << std::endl;

   t1.play();

   std::cout << "=========== ===" << std::endl;


   dbSleepMs( 2000 );

   std::cout << "=========== ====" << std::endl;

   t1.close();

   std::cout << "=========== =====" << std::endl;
//   dbSleep( 1000 );

//   TestEngine2 t2;
//   t2.play();
//   dbSleep( 2000 );
//   t2.close();
   return 0;
}

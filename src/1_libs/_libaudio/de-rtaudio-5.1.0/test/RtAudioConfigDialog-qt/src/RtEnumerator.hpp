#pragma once
#include <RtEnumItem.hpp>

// ===========================================================================
struct RtEnumerator
// ===========================================================================
{
   //DE_CREATE_LOGGER("RtEnumerator")
   RtAudio::Api m_api;

   int32_t m_defaultInput;
   int32_t m_defaultOutput;

   std::vector< RtEnumItem > m_devices;
   std::vector< RtEnumItem > m_outputs;
   std::vector< RtEnumItem > m_inputs;
   std::vector< RtEnumItem > m_duplex;


   static RtEnumerator& getSingleton()
   {
      static RtEnumerator s_enumerator;
      return s_enumerator;
   }

   RtEnumerator();
   ~RtEnumerator();


   std::string toString() const
   {
      std::string apiStr = getRtApiString(m_api);
      std::ostringstream s;
      s << "Api = " << apiStr << "\n";
      s << "Count = " << m_devices.size() << "\n";
      for ( size_t i = 0; i < m_devices.size(); ++i )
      {
         auto const & dev = m_devices[ i ];
         s << "Device[" << i << "] " << dev.toString() << "\n";
      }
      return s.str();
   }


   void enumerate( RtAudio::Api api = RtAudio::WINDOWS_WASAPI );
   bool load( std::string const & uri );
   bool save( std::string const & uri ) const;
   // void play();
   // void stop();
//   int32_t getDeviceCount() const { return m_database.size(); }
//   RtEnumItem & getDevice( int32_t i ) { return m_database[ i ]; }
//   RtEnumItem const & getDevice( int32_t i ) const { return m_database[ i ]; }

   int32_t getOutputCount() const { return m_outputs.size(); }
   RtEnumItem & getOutput( int32_t i ) { return m_outputs[ i ]; }
   RtEnumItem const & getOutput( int32_t i ) const { return m_outputs[ i ]; }

   int32_t getInputCount() const { return m_inputs.size(); }
   RtEnumItem & getInput( int32_t i ) { return m_inputs[ i ]; }
   RtEnumItem const & getInput( int32_t i ) const { return m_inputs[ i ]; }

   int32_t getDefaultInputDeviceIndex() const { return m_defaultInput; }
   int32_t getDefaultOutputDeviceIndex() const { return m_defaultOutput; }

   RtEnumItem const*
   getDefaultOutputDevice() const
   {
      for ( size_t i = 0; i < m_outputs.size(); ++i )
      {
         if ( m_outputs[ i ].m_deviceIndex == m_defaultOutput )
         {
            return &m_outputs[ i ];
         }
      }
      //DE_ERROR("No default output device")
      return nullptr;
   }

   RtEnumItem const*
   getDefaultInputDevice() const
   {
      for ( size_t i = 0; i < m_inputs.size(); ++i )
      {
         if ( m_inputs[ i ].m_deviceIndex == m_defaultInput )
         {
            return &m_inputs[ i ];
         }
      }
      //DE_ERROR("No default input device")
      return nullptr;
   }

   RtEnumItem const*
   getInputDeviceByName( std::string const & deviceName ) const
   {
      for ( size_t i = 0; i < m_inputs.size(); ++i )
      {
         if ( m_inputs[ i ].name() == deviceName )
         {
            return &m_inputs[ i ];
         }
      }
      //DE_ERROR("No input device, deviceName = ", deviceName)
      return nullptr;
   }

   RtEnumItem const*
   getOutputDeviceByName( std::string const & deviceName ) const
   {
      for ( size_t i = 0; i < m_outputs.size(); ++i )
      {
         if ( m_outputs[ i ].name() == deviceName )
         {
            return &m_outputs[ i ];
         }
      }
      //DE_ERROR("No output device, deviceName = ", deviceName)
      return nullptr;
   }

   void forAllOutputDevices( std::function< void( RtEnumItem const & d) > const & fn = [] ( RtEnumItem const & d) {} )
   {
      for ( auto const& output: m_outputs )
      {
         fn( output );
      }
   }

   void forAllInputDevices( std::function< void( RtEnumItem const & d) > const & fn = [] ( RtEnumItem const & d) {} )
   {
      for ( auto const& input: m_inputs )
      {
         fn( input );
      }
   }

};

inline RtEnumerator& getDeviceEnumerator()
{
   return RtEnumerator::getSingleton();
}


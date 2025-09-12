#include "PluginVst24.h"

PluginVst24&
PluginVst24::getInstance()
{
   static PluginVst24 s_instance;
   return s_instance;
}


PluginVst24::PluginVst24()
{
   m_sampleRate = 44100;   // A default value, overriden by effSetSampleRate
   m_blockSize = 4096;     // A default value, overriden by effSetBlockSize
   m_frameCounter = 0;

   //m_vstEvents.resize( 1 );
   //m_vstMidiEvents.resize( 1 );

#ifdef USE_EDITOR_WINDOW
   m_editorWindow = nullptr;
#endif

   size_t nCollect = 2*1024; // FFT Size?

#ifdef USE_VIDEO_DRIVER
   m_cl3dmbL.setDebugName("m_cl3dmbL");
   m_cl3dmbR.setDebugName("m_cl3dmbR");
   m_mmbLwaveform.setDebugName("m_mmbLwaveform");
   m_mmbRwaveform.setDebugName("m_mmbRwaveform");

   m_cl3dmbL.m_primType = de::PrimitiveType::LineStrip;
   m_cl3dmbL.m_vertices.resize( nCollect );

   m_cl3dmbR.m_primType = de::PrimitiveType::LineStrip;
   m_cl3dmbR.m_vertices.resize( nCollect );

   m_nbL.resize( nCollect );
   m_nbL.setNotifyFullCallback(
      [&] ( float* p, size_t n )
      {
         if ( !p )
         {
            DE_RUNTIME_ERROR("!p in m_nbL","");
         }

         if ( n != m_cl3dmbL.m_vertices.size() )
         {
            DE_RUNTIME_ERROR("n != m_cl3dmbL.m_vertices.size()","");
         }

         for ( size_t i = 0; i < n; ++i )
         {
            float sample = *p; p++;
            m_cl3dmbL.m_vertices[ i ] = de::ColorLine3DVertex(i, 1000.0f*sample, 0, 0xFFFFFFFF );
         }
         m_cl3dmbL.m_shouldUpload = true;

         m_nsmLwaveform.pushRow( p, n );

      } );

   m_nbR.resize( nCollect );
   m_nbR.setNotifyFullCallback(
      [&] ( float* p, size_t n )
      {
         if ( !p )
         {
            DE_RUNTIME_ERROR("!p in m_nbR","");
         }

         if ( n != m_cl3dmbR.m_vertices.size() )
         {
            DE_RUNTIME_ERROR("n != m_cl3dmbR.m_vertices.size()","");
         }

         for ( size_t i = 0; i < n; ++i )
         {
            float sample = *p; p++;
            m_cl3dmbR.m_vertices[ i ] = de::ColorLine3DVertex(i, 1000.0f*sample, 0, 0xFFFFFFFF );
         }
         m_cl3dmbR.m_shouldUpload = true;


         m_nsmRwaveform.pushRow( p, n );

      } );

   //m_nbL.push( nullptr, 0, "" );
   //m_nbR.push( nullptr, 0, "PluginVST24_R" );

   m_nsmLwaveform.resize( nCollect, 512 );
   m_nsmRwaveform.resize( nCollect, 512 );

   m_nsmLwaveform.setCallback(
      [&] ( de::NotifyShiftMatrix< float >* nsm )
      {
         uint32_t rows = nsm->rowCount();
         uint32_t cols = nsm->colCount();

         m_mmbLwaveform.m_primType = de::PrimitiveType::Lines;
         //m_mmbLwaveform.m_vertices.resize( rows * 2 * (cols-1) );

         if ( m_mmbLwaveform.m_vertices.empty() )
         {
            uint32_t k = 0;

            for ( uint32_t r = 0; r < rows; ++r )
            {
               for ( uint32_t c = 1; c < cols; ++c )
               {
                  k++;
                  k++;
               }
            }
            m_mmbLwaveform.m_vertices.resize( k );

            uint32_t expect = rows * 2 * (cols-1);
            if ( m_mmbLwaveform.m_vertices.size() == expect )
            {
               DE_INFO( "k(",k,") == expect(",expect,")")
            }
            else
            {
               DE_ERROR( "k(",k,") != expect(",expect,")")
            }
         }

         uint32_t k = 0;

         for ( uint32_t r = 0; r < rows; ++r )
         {
            std::vector< float > const & row = *nsm->m_data[ r ];
            float sampleA = row[ 0 ];

            for ( uint32_t c = 1; c < cols; ++c )
            {
               float sampleB = row[ c ];
               de::ColorLine3DVertex A( (c-1), 1000.0f*sampleA, 10.0f * r, 0xFFFFFFFF );
               de::ColorLine3DVertex B( c, 1000.0f*sampleB, 10.0f * r, 0xFFFF0000 );
               m_mmbLwaveform.m_vertices[ k ] = A; k++;
               m_mmbLwaveform.m_vertices[ k ] = B; k++;
               sampleA = sampleB;
            }
         }

         m_mmbLwaveform.m_shouldUpload = true;
      });

#endif

   // #define kEffectMagic CCONST ('V', 's', 't', 'P')

   memset( &m_aeffect, 0, sizeof(m_aeffect) );

   m_aeffect.magic = kEffectMagic;
   m_aeffect.object = this;
   m_aeffect.user = this;
   m_aeffect.flags = effFlagsIsSynth | effFlagsHasEditor
                   | effFlagsCanReplacing;
                // | effFlagsCanDoubleReplacing
                // | effFlagsProgramChunks
                // | effFlagsNoSoundInStop;
   m_aeffect.numPrograms = 0;
   m_aeffect.numParams = 0;
   m_aeffect.numInputs = 0;
   m_aeffect.numOutputs = 2;
   m_aeffect.initialDelay = 0;
   m_aeffect.uniqueID = CCONST('B','v','s','t');
   m_aeffect.version = 2400;
   m_aeffect.dispatcher = GlobalVst24_dispatcher; // AEffectDispatcherProc
   m_aeffect.process = nullptr; // GlobalVst24_processAudio; // AEffectProcessProc
   m_aeffect.processReplacing = GlobalVst24_processReplacing; // AEffectProcessProc
   m_aeffect.processDoubleReplacing = GlobalVst24_processDoubleReplacing; // AEffectProcessDoubleProc
   m_aeffect.setParameter = GlobalVst24_setParameter;
   m_aeffect.getParameter = GlobalVst24_getParameter;
   //plugin->bufferSize = 4096;
   //plugin->buffer = new float[plugin->bufferSize*2];
   //plugin->numVoices = NUM_VOICES;
   //plugin->events = new VstEvents;

   printf("PluginVst24::ctr()\n"); fflush( stdout );
}

PluginVst24::~PluginVst24()
{
#ifdef USE_EDITOR_WINDOW
   if ( m_editorWindow )
   {
      delete m_editorWindow;
      m_editorWindow = nullptr;
   }
#endif
}

void
PluginVst24::processReplacing( float** in, float** out, VstInt32 sampleFrames )
{
   DE_RUNTIME_ASSERT(out != nullptr);

   if ( sampleFrames < 1 )
   {
      DE_ERROR( "Got sampleFrames = ", sampleFrames )
      return;
   }

   uint32_t const frameCount = uint32_t( sampleFrames );

   if ( m_blockSize != sampleFrames )
   {
      m_blockSize = sampleFrames;
      //DE_DEBUG("processReplacing(",m_blockSize,")")
      //m_cl3dmbL.m_vertices.resize( m_blockSize );
      //m_cl3dmbR.m_vertices.resize( m_blockSize );
   }

   // Get the left and right inputs
   //float* inL = in[0];
   //float* inR = in[1];

   // Get the left and right outputs
   float* L = out[0];
   float* R = out[1];
   DE_RUNTIME_ASSERT(L != nullptr);
   DE_RUNTIME_ASSERT(R != nullptr);

   de::SampleChecker::check(L,frameCount,"VST_L");
   de::SampleChecker::check(R,frameCount,"VST_R");

   //      // process the audio
   //      for (VstInt32 i = 0; i < sampleFrames; i++)
   //      {
   //         // pan the left and right channels
   //         oL[i] = m_gain * (iL[i] * (1.0f - m_pan));
   //         oR[i] = m_gain * (iR[i] * m_pan);
   //      }

   constexpr double TWO_PI = 2.0*M_PI;

   double freq = 220.0;

   double semitone0 = 1.0;                      // C
   //double semitone1 = pow( 2.0, 1.0 / 12.0 );   // C#
   //double semitone2 = pow( 2.0, 2.0 / 12.0 );   // D
   //double semitone3 = pow( 2.0, 3.0 / 12.0 );   // D#
   //double semitone4 = pow( 2.0, 4.0 / 12.0 );   // E
   double semitone5 = pow( 2.0, 5.0 / 12.0 );   // F
   //double semitone6 = pow( 2.0, 6.0 / 12.0 );   // F#
   //double semitone7 = pow( 2.0, 7.0 / 12.0 );   // G
   //double semitone8 = pow( 2.0, 8.0 / 12.0 );   // G#
   double semitone9 = pow( 2.0, 9.0 / 12.0 );   // A
   //double semitone10 = pow( 2.0, 10.0 / 12.0 ); // A#
   //double semitone11 = pow( 2.0, 11.0 / 12.0 ); // H
   double semitone12 = 2.0;                     // C+1
   double sampleRate = double(m_sampleRate);
   double phase_step0 = double( TWO_PI * (freq * semitone0) / sampleRate ); // C
   double phase_step1 = double( TWO_PI * (freq * semitone5) / sampleRate ); // F
   double phase_step2 = double( TWO_PI * (freq * semitone9) / sampleRate ); // A
   double phase_step3 = double( TWO_PI * (freq * semitone12)/ sampleRate ); // C
   //double phase = double( g_frameCounter ) * phase_step;

   uint64_t frameCounter = m_frameCounter;

   L = out[0];
   R = out[1];

   for (int i = 0; i < sampleFrames; i++)
   {
      double phase0 = fmod( double( frameCounter ) * phase_step0, TWO_PI );
      double phase1 = fmod( double( frameCounter ) * phase_step1, TWO_PI );
      double phase2 = fmod( double( frameCounter ) * phase_step2, TWO_PI );
      double phase3 = fmod( double( frameCounter ) * phase_step3, TWO_PI );

      float sample = float( 0.04 * (sin( phase0 )
                                  + sin( phase1 )
                                  + sin( phase2 )
                                  + sin( phase3 )) );

//      float frequency = m_voices[j].frequency * (1.0f + m_pitchBend * MAX_PITCHBEND);
//      float phase = m_buffer[(int) (frequency * m_bufferSize / sampleRate)];
//      sample += phase * m_voices[j].volume * m_gain;
//      // update the phase
//      phase += (1.0f / sampleRate) * frequency;
//      if (phase >= 2.0f * PI) phase -= 2.0f * PI;
//      m_buffer[(int) (frequency * m_bufferSize / sampleRate)] = phase;

      // write the sample to the outputs
      *L = sample;
      *R = sample;
      L++;
      R++;
      ++frameCounter;
   }

/*
   L = out[0];
   R = out[1];
   for (int i = 0; i < sampleFrames; i++)
   {
      float sampleL = *L;
      float sampleR = *R;
      m_meshBufferL.m_vertices[ i ] = de::S3DVertex(i, 100.0f*sampleL, 0, 0,0,0, 0xFFFFFFFF, 0,0 );
      m_meshBufferR.m_vertices[ i ] = de::S3DVertex(i, 100.0f*sampleR, 10, 0,0,0, 0xFFFFFFFF, 0,0 );
      ++L;
      ++R;
   }
   m_meshBufferL.setDirty();
   m_meshBufferR.setDirty();

   L = out[0];
   R = out[1];
   for ( size_t i = 0; i < size_t(sampleFrames); ++i )
   {
      float sampleL = *L;
      float sampleR = *R;
      m_cl3dmbL.m_vertices[ i ] = de::ColorLine3DVertex(i, 1000.0f*sampleL, 0, 0xFFFFFFFF );
      m_cl3dmbR.m_vertices[ i ] = de::ColorLine3DVertex(i, 1000.0f*sampleR, 10, 0xFFFFFFFF );
      ++L;
      ++R;
   }
   m_cl3dmbL.m_shouldUpload = true;
   m_cl3dmbR.m_shouldUpload = true;
*/

#ifdef USE_VIDEO_DRIVER
   L = out[0];
   R = out[1];
   m_nbL.push( L, size_t( sampleFrames ) );
   m_nbR.push( R, size_t( sampleFrames ) );
#endif

   m_frameCounter = frameCounter;
}

void
PluginVst24::processDoubleReplacing( double** in, double** out, VstInt32 sampleFrames )
{
   if ( m_blockSize != sampleFrames )
   {
      m_blockSize = sampleFrames;
      printf("processDoubleReplacing(%d)\n", m_blockSize); fflush( stdout );
   }
}





// Global VST2 audio producer handler (float, legacy callback)
void
// VSTCALLBACK
GlobalVst24_processAudio(AEffect* vst, float* in, float* out, VstInt32 sampleFrames)
{
   GlobalVst24_processReplacing(vst,&in,&out,sampleFrames);
}

void
// VSTCALLBACK
GlobalVst24_processReplacing(AEffect* vst, float** in, float** out, VstInt32 sampleFrames)
{
   if ( !vst ) return;
   if ( !vst->user ) return;
   auto plugin = reinterpret_cast< PluginVst24* >(vst->object);
   if ( !plugin ) return;
   plugin->processReplacing( in, out, sampleFrames );
}

void
// VSTCALLBACK
GlobalVst24_processDoubleReplacing(AEffect* vst, double** in, double** out, VstInt32 sampleFrames)
{
   if ( !vst ) return;
   if ( !vst->user ) return;
   auto plugin = reinterpret_cast< PluginVst24* >(vst->object);
   if ( !plugin ) return;
   plugin->processDoubleReplacing( in, out, sampleFrames );
}

void // VSTCALLBACK
GlobalVst24_processMidiEvents(AEffect* vst)
{
   if ( !vst ) return;
   if ( !vst->user ) return;
   auto plugin = reinterpret_cast< PluginVst24* >(vst->object);
   if ( !plugin ) return;
   plugin->processMidiEvents();
}

// Global VST2.4 plugin to host info request event handler
VstIntPtr // VSTCALLBACK
GlobalVst24_dispatcher(AEffect* vst, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
   if ( !vst ) return 0;
   if ( !vst->user ) return 0;
   auto plugin = reinterpret_cast< PluginVst24* >(vst->object);
   if ( !plugin ) return 0;
   return plugin->dispatcher( opcode, index, value, ptr, opt );
}

// AEffectSetParameterProc
void // VSTCALLBACK
GlobalVst24_setParameter(AEffect* vst, VstInt32 index, float parameter )
{
   if ( !vst ) return;
   if ( !vst->user ) return;
   auto plugin = reinterpret_cast< PluginVst24* >(vst->object);
   if ( !plugin ) return;
   plugin->setParameter( index, parameter );
}

// AEffectGetParameterProc
float // VSTCALLBACK
GlobalVst24_getParameter(AEffect* vst, VstInt32 index )
{
   if ( !vst ) return 0.0f;
   if ( !vst->user ) return 0.0f;
   auto plugin = reinterpret_cast< PluginVst24* >(vst->object);
   if ( !plugin ) return 0.0f;
   return plugin->getParameter( index );
}


/*

//         case audioMasterVersion:
//            ret = 2400;
//            break;
//         case audioMasterIdle:
//            ret = 0;
//            break;
//         case audioMasterGetTime:
//            //ret = (VstIntPtr) &(plugin->timeInfo);
//            break;
//         case audioMasterProcessEvents:
//            //plugin->events = (VstEvents*) ptr;
//            processMidiEvents(effect);
//            ret = 1;
//            break;
//         case audioMasterSizeWindow:
//            //MoveWindow(plugin->hWnd, 0, 0, LOWORD(value), HIWORD(value), TRUE);
//            ret = 1;
//            break;


   // create a window for the plugin
   HINSTANCE hInstance = GetModuleHandle(NULL);
   WNDCLASSEX wc;
   wc.cbSize        = sizeof(WNDCLASSEX);
   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = DefWindowProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = "SynthEffect";
   wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
   RegisterClassEx(&wc);

   hWnd = CreateWindow("SynthEffect", "SynthEffect", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 200, 100,
      NULL, NULL, hInstance, NULL);
   ShowWindow(hWnd, SW_SHOW);

   RECT r_client;
   GetClientRect(hWnd, &r_client);
   m_rect.left = r_client.left;
   m_rect.top = r_client.top;
   m_rect.right = r_client.right;
   m_rect.bottom = r_client.bottom;

   return &effect;
*/

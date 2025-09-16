#include <de/audio/buffer/BufferQueueTest.hpp>

namespace de {
namespace audio {

// ---------------------------------------------------------
//    m_FrameCount = 0, m_FrameIndex = 0;
// push_back( AAAAAABBBBBB )
//    [AAAAAABBBBBB], m_FrameCount = 12, m_FrameIndex = 0;
// pop_front( KKKKKKKK )
//    [BBBB], m_FrameCount = 12, m_FrameIndex = 8;
// push_back( CCCCCC )
//    [BBBBCCCCCC], m_FrameCount = 18, m_FrameIndex = 8;
// pop_front( KKKKKKKK )
//    [CC], m_FrameCount = 18, m_FrameIndex = 16;
// push_back( DDDDDD )
//    [CCDDDDDD], m_FrameCount = 24, m_FrameIndex = 16;
// pop_front( KKKKKKKK )
//    [], m_FrameCount = 24, m_FrameIndex = 24;
// ---------------------------------------------------------

#ifndef CPP_EQUAL
#define CPP_EQUAL(x,expected) \
if ((x)!=(expected)) { \
   std::ostringstream s; \
   s << "Failed expression: " #x " == " #expected "\n"; \
   s << "Failed line:" << __LINE__ << "\n"; \
   s << "Expected:" << #expected << "\n"; \
   s << "Got:" << (x) << ""; \
   throw std::runtime_error( s.str() ); \
}
#endif

void
BufferQueueTest::test()
{
   DE_DEBUG("TEST BEGIN:" )

   BufferQueue Q; // testObj

   DE_WARN("=======================================")
   DE_WARN("[pushA] testObj" )
   DE_WARN("=======================================")
   Buffer::Ptr A = Buffer::createPtr( ST_S16I, 2, 44100, 3 );
   A->setSampleS16( 0,0, 1 );
   A->setSampleS16( 0,1, 2 );
   A->setSampleS16( 1,0, 3 );
   A->setSampleS16( 1,1, 4 );
   A->setSampleS16( 2,0, 5 );
   A->setSampleS16( 2,1, 6 );
   Q.push_back( std::move( A ), 0.0 );
   Q.dump();

   CPP_EQUAL( Q.getChunkIndex(), 0 )
   CPP_EQUAL( Q.getChunkOffset(), 0 )
   CPP_EQUAL( Q.getFrameCount(), 3 )
   CPP_EQUAL( Q.getFrameIndex(), 0 )

   DE_WARN("=======================================")
   DE_WARN("[pushB] testObj" )
   DE_WARN("=======================================")
   Buffer::Ptr B = Buffer::createPtr( ST_S16I, 2, 44100, 3 );
   B->setSampleS16( 0,0, 7 );
   B->setSampleS16( 0,1, 8 );
   B->setSampleS16( 1,0, 9 );
   B->setSampleS16( 1,1, 10 );
   B->setSampleS16( 2,0, 11 );
   B->setSampleS16( 2,1, 12 );
   Q.push_back( std::move( B ), 0.3 );
   Q.dump();

   CPP_EQUAL( Q.getChunkIndex(), 0 )
   CPP_EQUAL( Q.getChunkOffset(), 0 )
   CPP_EQUAL( Q.getFrameCount(), 6 )
   CPP_EQUAL( Q.getFrameIndex(), 0 )

   DE_WARN("=======================================")
   DE_WARN("[popC] testObj" )
   DE_WARN("=======================================")
   Buffer::Ptr C = Buffer::createPtr( ST_S16I, 2, 44100, 4 );
   Q.pop_front( *C, 0.0 );
   Q.dump();

   CPP_EQUAL( Q.getChunkIndex(), 1 )
   CPP_EQUAL( Q.getChunkOffset(), 1 )
   CPP_EQUAL( Q.getFrameCount(), 6 )
   CPP_EQUAL( Q.getFrameIndex(), 3 )

   DE_WARN("=======================================")
   DE_WARN("[pushD] testObj" )
   DE_WARN("=======================================")
   Buffer::Ptr D = Buffer::createPtr( ST_S16I, 2, 44100, 1 );
   D->setSampleS16( 0,0, 13 );
   D->setSampleS16( 0,1, 14 );
   Q.push_back( std::move( D ), 0.4 );
   Q.dump();

   CPP_EQUAL( Q.getChunkIndex(), 1 )
   CPP_EQUAL( Q.getChunkOffset(), 1 )
   CPP_EQUAL( Q.getFrameCount(), 7 )
   CPP_EQUAL( Q.getFrameIndex(), 3 )

   DE_WARN("=======================================")
   DE_WARN("[popE] testObj" )
   DE_WARN("=======================================")
   Buffer::Ptr E = Buffer::createPtr( ST_S16I, 2, 44100, 3 );
   Q.pop_front( *E, 0.0 );
   Q.dump();

   CPP_EQUAL( Q.getChunkIndex(), 3 )
   CPP_EQUAL( Q.getChunkOffset(), 0 )
   CPP_EQUAL( Q.getFrameCount(), 7 )
   CPP_EQUAL( Q.getFrameIndex(), 7 )

   CPP_EQUAL( Q.getChunkIndex(), 3 )
   CPP_EQUAL( Q.getChunkOffset(), 0 )
   CPP_EQUAL( Q.getFrameCount(), 7 )
   CPP_EQUAL( Q.getFrameIndex(), 7 )

   DE_DEBUG("TEST END:" )
}

} // end namespace audio
} // end namespace de








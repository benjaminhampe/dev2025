#pragma once
#include <CNC_Word.hpp>

namespace de {
namespace cnc {

// A line of commands (if any) and comment (if any)
// e.g. G21 G17 G90 F100 M03 S1000
//
struct CNC_Block
{
   // Keep several parser stages for debugging purposes
   std::string m_originalLine;               // Stage I
   std::vector< CNC_Word > m_originalWords;  // Stage II
   std::string m_originalComment;

   // Parser output
   std::vector< CNC_Word > m_inputWords;
   std::vector< CNC_Word > m_parsedWords;    // Output Stage II
   std::vector< CNC_Word > m_skippedWords;   // Output Stage II
   std::vector< std::string > m_debugLog;    // Output Stage II

   // Util operating on std::vector< CNC_Word >
   static s32
   findWord( std::vector< CNC_Word > const & words, char name )
   {
      for ( size_t i = 0; i < words.size(); ++i )
      {
         CNC_Word const & word = words[i];
         if ( word.m_key == name ) return s32(i);
      }
      return -1;
   }

   // Util operating on std::vector< CNC_Word >
   static CNC_Word*
   getWord( std::vector< CNC_Word > & words, char name )
   {
      for ( size_t i = 0; i < words.size(); ++i )
      {
         CNC_Word & word = words[i];
         if ( word.m_key == name ) return &word;
      }
      return nullptr;
   }

   // Util operating on std::vector< CNC_Word >
   static std::string
   joinWords( std::vector< CNC_Word > const & words )
   {
      std::ostringstream o;
      for ( size_t i = 0; i < words.size(); ++i )
      {
         if ( i > 0 ) o << ' ';
         o << words[ i ].toString();
      }
      return o.str();
   }

   /*
   // Util operating on std::vector< CNC_Word >
   static std::string
   toHtml( std::vector< CNC_Word > const & words )
   {
      std::ostringstream o;
      for ( size_t i = 0; i < words.size(); ++i )
      {
         CNC_Word const & word = words[ i ];
         if ( i > 0 ) o << ' ';
         o << word.toHtml();
      }
      return o.str();
   }

   // Stores comment without the preceeding ;
   bool hasComment() const { return m_comment.size() > 0; }
   bool noComment() const { return m_comment.empty(); }

   bool hasWords() const { return m_words.size() > 0; }
   bool noWords() const { return m_words.empty(); }

   bool isComment() const { return hasComment() && noWords(); }

   void dumpWord( CNC_Word* word )
   {
      if ( !word ) return;
      s64 index = word - m_inputWords.data();
      if ( index < 0 || size_t(index) >= m_inputWords.size() ) return;
      // Erase word from m_words;
      m_processed.emplace_back( *word );
   }
*/

   void addDebugLog( std::string msg )
   {
      if ( msg.empty() ) return;
      m_debugLog.emplace_back( msg );
   }

   std::string
   getCommentString() const
   {
      std::ostringstream o;

      std::string const skippedWords = joinWords( m_skippedWords );

      bool const hasBoth = (skippedWords.size() > 0)
                  && (m_originalComment.size() > 0);

      if ( skippedWords.size() > 0 )
      {
         o << ";" << skippedWords;
      }
      if ( hasBoth )
      {
         o << ' '; // Add space only if (skippedWords and originalComment) exist.
      }
      if ( m_originalComment.size() > 0 )
      {
         o << ";" << m_originalComment;
      }
      return o.str();
   }

   // <parsed-words> ;<skipped-words ><original-comment>
   std::string
   toString() const
   {
      std::ostringstream o;
      o << joinWords( m_parsedWords )
        << " "
        << getCommentString();
      return o.str();
   }

   std::string
   toHtml() const
   {
      std::ostringstream o;
      for ( size_t i = 0; i < m_parsedWords.size(); ++i )
      {
         CNC_Word const & word = m_parsedWords[ i ];
         if ( i > 0 ) o << ' ';
         o << word.toHtml();
      }

      std::string comment = getCommentString();
      if ( comment.size() > 0 )
      {
         o << "<span style=\"color:rgb(150,150,150)\"> " << comment << "</span>";
      }
      return o.str();
   }
};

} // end namespace cnc
} // end namespace de


/*
void addListener( IGCodeParserListener* listener )
{
   if ( !listener ) return;
   m_listeners.emplace_back( listener );
}

void emit_begin( std::string const & uri )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_begin( uri ); }
}

void emit_end( std::string const & uri )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_end( uri ); }
}

void emit_newLine( std::string const & line )
{
   for ( auto listener : m_listeners )
   {
      if ( listener ) listener->on_newLine( line );
   }
}

void emit_commandList( std::vector< std::string > const & commands, std::string const & comment )
{
   for ( auto listener : m_listeners )
   {
      if ( listener ) listener->on_commandList( commands, comment );
   }
}

void emitG00( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G00( x,y,z ); }
}

void emitG01( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G01( x,y,z ); }
}

void emitG02( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G02( x,y,z ); }
}

void emitG03( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G03( x,y,z ); }
}

void emitG04( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G04( x,y,z ); }
}
*/

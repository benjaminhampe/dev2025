#pragma once
#include <CNC_Register.hpp>
#include <CNC_Machine.hpp>

namespace de {
namespace cnc {

// The parser splitts given multiline text into CNC_Word(s)
// Multiline text is a long linear string with linebreak chars '\n','\r'.
// ' '  is space char. Can occur everywhere (linebegin, inside & between words).
//      Splitting CNC words by spaces is therefore a bad approach.
//      We must splitt by every alpha word, an unbroken list of A-Za-z.
//      We can still encounter M103 HiImACommentar... display messages.
// '\n' is linefeed char. Moves type-writer tape a bit for a new line.
// '\r' is carriage return char. Moves type-writer cursor back to linebegin, today reduntant.
//
// A more efficient parser would splitt the text into CNC_Blocks on the fly,
// this 'naive' approach splitts text into lines before parsing, which is memory intensive.
//
// The class CNC_Machine actually interprets the CNC commands by listening to CNC_Parser
// CNC_Machine is a combination of StateMachine/Computer and HtmlWriter.
// CNC_Parser is more a tokenizer that sends CNC_Blocks to its registered listener CNC_Machine.
//
// Usage:
//          std::string gcode = de::cnc::TextUtil::loadText( dbOpenFileA() );
//          de::cnc::CNC_Parser parser;
//          de::cnc::CNC_Machine computer;
//          parser.addListener( &computer );
//          parser.parse( uri, gcode );
//          computer.save(uri + ".CNC_Parser.html",uri);
//          system( (uri + ".CNC_Parser.html").c_str() );
// =======================================================================
struct CNC_Parser
// =======================================================================
{
   DE_CREATE_LOGGER("de.cnc.CNC_Parser")

   IParserListener* m_listener;

   CNC_Parser()
      : m_listener( nullptr )
   {}

   //~CNC_Parser() {}

   void addListener( IParserListener* listener )
   {
      m_listener = listener;
   }
   void emit_parseBlock( CNC_Block block )
   {
      if ( m_listener ) { m_listener->parseBlock( block ); }
   }
   void emit_parserStarted( std::string uri, std::string sourceText )
   {
      if ( m_listener ) { m_listener->parserStarted( uri, sourceText ); }
   }
   void emit_parserEnded()
   {
      if ( m_listener ) { m_listener->parserEnded(); }
   }

   // This function can be heavily optimized. Especially the first stage.
   // This parser splits given text into lines and then words + comment
   bool
   parse( std::string const & uri, std::string const & sourceText )
   {
      emit_parserStarted( uri, sourceText );

      // 1.1. Replace windows linebreaks "\r\n" with linux linebreaks "\n".
      std::string trimmedCode = TextUtil::replaceText( sourceText, "\n\r", "\n" );
      // 1.2. Replace mac linebreaks "\n\r" with linux linebreaks "\n".
      trimmedCode = TextUtil::replaceText( trimmedCode, "\r\n", "\n" );
      // 1.3. Replace tabs '\t' with spaces ' ', if any, probably not.
      trimmedCode = TextUtil::replaceText( trimmedCode, "\t", " " );
      // 1.4. Splitt now text into vector of lines, splitt by linux linebreaks '\n' only.
      std::vector< std::string > m_trimmedLines = TextUtil::splitText( trimmedCode, '\n' );

      // 2. For all splitted lines:
      for ( size_t i = 0; i < m_trimmedLines.size(); ++i )
      {
         // 2.1. Get splitted line:
         std::string const & originalLine = m_trimmedLines[ i ];

         // 2.2. Splitt current line into left and right string parts
         std::string commandPart;
         std::string commentPart;
         // 2.2.I. Find comment start ';', if any.
         std::string::size_type posSemicolon = originalLine.find( ';' );
         // 2.2.II. Found no comment -> Entire line is commands only
         if ( posSemicolon == std::string::npos )
         {
            commandPart = originalLine;
         }
         // 2.2.III. Found comment -> Split line into left(=commandList) and right(=comment).
         else
         {
            commandPart = originalLine.substr( 0, posSemicolon );
            commentPart = originalLine.substr( posSemicolon + 1 );
         }

         // 3. Splitt left string (=commandList) again, splitt by spaces ' ' into cnc words.
         std::vector< std::string > textWords = CNC_Util::splitCommandPart( commandPart );
         //std::vector< std::string > textWords = GCodeParserUtil::splitString( commandPart, ' ' );

         // 4. Compose CNC_Block as parser result -> package what the interpreter gets.
         CNC_Block block;

         // 4.1 Compose CNC_Block.m_originalLine
         block.m_originalLine = originalLine;

         // 4.2 Compose CNC_Block.m_originalLine
         block.m_originalComment = commentPart;

         // 4.3 Compose CNC_Block.m_words:
         // 4.3.1 Are there any command words?
         if ( textWords.size() > 0 )
         {
            // 4.3.2 Loop all available words:
            for ( size_t t = 0; t < textWords.size(); ++t )
            {
               // 4.3.3 Get current word:
               std::string const & textWord = textWords[ t ];

               // 4.3.4 Has the string any chars?
               if ( textWord.empty() )
               {
                  std::cout << "[Error] CNC_Parser.parse(" << i << ") :: "
                   "Got empty token, " << originalLine << std::endl;
                  continue;
               }

               // 4.3.4 Has the string enough chars?
               if ( textWord.size() < 2)
               {
                  std::cout << "[Error] CNC_Parser.parse(" << i << ") :: "
                   "Too few chars, " << originalLine << std::endl;
                  continue;
               }

               // TODO: makeUpper() to ensure A-Z, never a-z.

               // Do we have a CNC register A-Z,a-z ?
               char machineRegister = textWord[ 0 ];

               if ( !CNC_Util::isWord( machineRegister )  )
               {
                  std::cout << "[Error] CNC_Parser.parse(" << i << ") :: "
                   "Not a register(" << machineRegister << "), " << originalLine << std::endl;

                  //block.m_broken.emplace_back( textWord );
                  std::ostringstream o;
                  o << "Not a register(" << machineRegister << ").";
                  block.addDebugLog( o.str() );
                  continue;
               }

               // Splitt the rest of string to the value part. We have the commandPart already.
               std::string valueText( textWord.begin() + 1, textWord.end() );


               CNC_Word word;
               word.m_key = machineRegister;             // MachineRegister A-Z
               word.m_value = atof( valueText.c_str() ); // New double value for MachineRegister.
               block.m_originalWords.emplace_back( word );       // Add CNC_Word to CNC_Block.
            }
         }

         emit_parseBlock( block ); // Now feed parsed command line to interpreter/statemachine.
      }

      emit_parserEnded();
      return true;
   }




};


} // end namespace cnc
} // end namespace de


/*
void addListener( IParserListener* listener )
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

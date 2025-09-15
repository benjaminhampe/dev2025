#pragma once
#include <DarkImage.h>
//#include <de_irrTypes.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace de {
namespace cnc {

struct TextUtil
{
   static std::string
   loadText( std::string uri )
   {
      std::string content;
      std::ifstream file;
      file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
      try
      {
         file.open(uri);
         std::ostringstream s;
         s << file.rdbuf();
         file.close();
         content = s.str();
      }
      catch (std::ifstream::failure e)
      {
         std::cout << "ERROR: loadString() :: " << uri << std::endl;
      }
      return content;
   }

   static std::string
   replaceText( std::string const & txt,
                std::string const & from,
                std::string const & to,
                size_t* nReplacements = nullptr )
   {
      size_t nReplaces = 0;
      std::string s = txt;

      if ( s.empty() )
      {
         return s;
      }

      if ( to.empty() )
      {
         size_t pos = s.find( from );
         if ( pos == std::string::npos )
         {
         }
         else
         {
            while( pos != std::string::npos )
            {
               nReplaces++;
               s.erase( pos, pos + from.size() );
               pos = s.find( from, pos );
            }
         }
      }
      else
      {
         size_t pos = s.find( from );
         if ( pos == std::string::npos )
         {
         }
         else
         {
            while( pos != std::string::npos )
            {
               nReplaces++;
               s.replace( pos, from.size(), to ); // there is something to replace
               pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
               pos = s.find( from, pos ); // advance to next token, if any
            }
         }
      }
      if ( nReplacements ) *nReplacements = size_t(nReplaces);
      return s;
   }

   static std::vector< std::string >
   splitText( std::string const & txt, char searchChar )
   {
      std::vector< std::string > lines;

      std::string::size_type pos1 = 0;
      std::string::size_type pos2 = txt.find( searchChar, pos1 );

      while ( pos2 != std::string::npos )
      {
         std::string line = txt.substr( pos1, pos2-pos1 );
         if ( !line.empty() )
         {
            lines.emplace_back( std::move( line ) );
         }

         pos1 = pos2+1;
         pos2 = txt.find( searchChar, pos1 );
      }

      std::string line = txt.substr( pos1 );
      if ( !line.empty() )
      {
         lines.emplace_back( std::move( line ) );
      }

      return lines;
   }
};

struct HtmlUtil
{
   static std::string
   rgb( u32 color )
   {
      int r = color & 0x000000ff;
      int g = ( color >> 8 ) & 0x000000ff;
      int b = ( color >> 16 ) & 0x000000ff;
      std::ostringstream o;
      o << "rgb(" << r << "," << g << "," << b << ")";
      return o.str();
   }
};

struct CNC_Util
{
   static bool
   isWord( char word )
   {
      if (word >= 'A' && word <= 'Z') return true;
      if (word >= 'a' && word <= 'z') return true;
      if (word == '%') return true;
      return false;
   }

   static bool
   isIntegerWord( char word )
   {
      if (!isWord( word )) return false;
      if (word == 'G' || word == 'N' || word == 'M' || word == 'T') return true;
      return false;
   }

   static std::vector< std::string >
   splitCommandPart( std::string const & txt )
   {
      std::vector< std::string > commands;

      size_t pos1 = 0;
      size_t pos2 = 0;

      for ( size_t i = 0; i < txt.size(); ++i )
      {
         pos2 = i;

         char ch = txt[ i ];
         if ( ch >= 'a' && ch <= 'z' )
         {
            ch += char('A') - char('a'); // toUpper
         }

         if ( ch >= 'A' && ch <= 'Z' )
         {
            if ( pos2 > pos1 )
            {
               std::string command = txt.substr( pos1, pos2-pos1 );
               commands.emplace_back( std::move( command ) );
            }

            pos1 = pos2;
         }

      }

      if ( pos2 > pos1 )
      {
         std::string command = txt.substr( pos1 );
         commands.emplace_back( std::move( command ) );
      }

      return commands;
   }
};


} // end namespace cnc
} // end namespace de


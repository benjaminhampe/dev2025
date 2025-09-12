/*
 *  filesystem.cpp
 *
 *  Copyright (c) 2019 Nick Dowell
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is part of amsynth.
 *
 *  amsynth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  amsynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with amsynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "filesystem.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>


// STL C++17
// #include <filesystem>
// namespace fs = std::filesystem;

// GCC 7.3.0 needs extra link with -lstdc++fs
#include <filesystem> // I use GCC 7.3.0 and link with -lstdc++fs
//#include <de/String.hpp>

//#include <time.h>

namespace fs = std::filesystem;


Filesystem& Filesystem::get()
{
    static Filesystem singleton;
    return singleton;
}

Filesystem::Filesystem()
{
#ifdef PKGDATADIR
    const char *env_home = getenv("HOME");
    if (!env_home) {
        return;
    }

    std::string home(env_home);

    // https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

    const char *env_xdg_config_home = getenv("XDG_CONFIG_HOME");
    std::string xdg_config_home = env_xdg_config_home ? std::string(env_xdg_config_home) : home + "/.config";
    std::string amsynth_config_dir = xdg_config_home + "/amsynth";
    config      = amsynth_config_dir + "/config";
    controllers = amsynth_config_dir + "/controllers";

    const char *env_xdg_data_home = getenv("XDG_DATA_HOME");
    std::string xdg_data_home = env_xdg_data_home ? std::string(env_xdg_data_home) : home + "/.local/share";
    std::string amsynth_data_dir = xdg_data_home + "/amsynth";
    user_banks = amsynth_data_dir + "/banks";
    default_bank = user_banks + "/default";

    create_dir(amsynth_config_dir);

    if (!exists(controllers)) {
        move(home + "/.amSynthControllersrc", controllers);
    }

    if (!exists(config) &&
        !move(home + "/.amSynthrc", config) &&
        !copy(PKGDATADIR "/rc", config)) {
        std::cerr << "Error: could not create " << controllers << std::endl;
    }

    if (!exists(amsynth_data_dir) &&
        !move(home + "/.amsynth", amsynth_data_dir) &&
        !(create_dir(amsynth_data_dir) && create_dir(user_banks))) {
        std::cerr << "Error: could not create " << amsynth_data_dir << std::endl;
    }

    if (!exists(default_bank) &&
        !move(home + "/.amSynth.presets", default_bank) &&
        !copy(PKGDATADIR "/banks/amsynth_factory.bank", default_bank)) {
        std::cerr << "Error: could not create " << default_bank << std::endl;
    }
#endif
   std::string amsynth_data_dir = "media/AMSynth";
   user_banks = amsynth_data_dir + "/banks";
   default_bank = user_banks + "/amsynth_factory.bank";

   if (!exists(default_bank))
   {
      std::cerr << "Error: default_bank not exist: " << default_bank << std::endl;
   }
}

bool Filesystem::copy(const std::string &from, const std::string &to)
{
    std::ifstream input(from.c_str(), std::ios::in | std::ios::binary);
    if (!input.is_open()) {
        return false;
    }
    std::ofstream output(to.c_str(), std::ios::out | std::ios::binary);
    output << input.rdbuf();
    return true;
}

bool Filesystem::create_dir(std::string const & uri)
{
   if ( uri.empty() ) return false;

   fs::path p( uri );

   fs::file_status s = fs::status( p );

   if ( fs::exists( s ) && fs::is_directory( s ) )
   {
      // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
      return true;
   }

   std::error_code e;
   fs::create_directories( p, e );
   if ( e )
   {
      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
   }
   return true;
}

bool Filesystem::exists(std::string const & uri)
{
   if ( uri.empty() ) { return false; }
   fs::file_status s = fs::status( fs::path(uri) );
   if ( !fs::exists( s ) ) { return false; }
   return true;

   // struct stat sb;
   // return stat(path.c_str(), &sb) == 0;
}

bool 
Filesystem::move(const std::string &from, const std::string &to)
{
    return rename(from.c_str(), to.c_str()) == 0;
}

bool
Filesystem::endsWith( std::string const & str, std::string const & query )
{
   if ( str.empty() ) return false;
   if ( query.empty() ) return false;
   if ( query.size() == str.size() )
   {
      return query == str;
   }
   // abcde(5), cde(3), pos(2=c)
   else if ( query.size() < str.size() )
   {
      bool equal = true;
      for ( size_t i = 0; i < query.size(); ++i )
      {
         char const a = str[ str.size() - 1 - i ];
         char const b = query[ query.size() - 1 - i ];
         if ( a != b )
         {
            equal = false;
            break;
         }
      }

      return equal;
      // return query == str.substr( str.size() - query.size(), query.size() );
   }
   else
   {
      return false;
   }
}


uint32_t
Filesystem::replaceString( std::string & s, std::string const & from, std::string const & to )
{
   uint32_t replaceActionsDone = 0;

   if ( s.empty() )
   {
      return replaceActionsDone;
   }

   if ( to.empty() )
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.erase( pos, pos + from.size() );
            replaceActionsDone++;
            pos = s.find( from, pos );
         }
      }
   }
   else
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            replaceActionsDone++;   // advance action counter
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
   }
   return replaceActionsDone;
}


void
Filesystem::makeWinPath( std::string & uri )
{
   replaceString( uri, "/", "\\" );

   if ( endsWith( uri, "\\.") )
   {
      uri = uri.substr(0,uri.size()-2);
      //DE_DEBUG("endsWithSlashDot uri = ", uri)
   }
   if ( endsWith( uri, "\\") )
   {
      uri = uri.substr(0,uri.size()-1);
      //DE_DEBUG("endsWithSlash uri = ", uri)
   }
}

bool
Filesystem::existFile( std::string const & uri )
{
   if ( uri.empty() ) { return false; }
   fs::file_status s = fs::status( uri );
   if ( !fs::exists( s ) ) { return false; }
   if ( !fs::is_regular_file( s ) ) { return false; }
   return true;
}

bool
Filesystem::existDirectory( std::string const & fileName )
{
   if ( fileName.empty() ) return false;
   fs::file_status s = fs::status( fileName );
   bool ok = fs::exists( s ) && fs::is_directory( s );
   return ok;
}


std::string
Filesystem::makeAbsolute( std::string uri, std::string baseDir )
{
   std::string s;

   try
   {
      fs::path p;

      //DE_DEBUG("fs::path(uri) = ",file.string() )

      if ( baseDir.empty() )
      {
         //printf("baseDir is empty\n"); fflush( stdout );
         //printf("uri = %s\n", uri.c_str()); fflush( stdout );
//#if defined(_WIN32) || defined(WIN32)
//      makeWinPath( uri );
//#endif
         fs::path file( uri );
         //printf("file = %s\n", file.string().c_str()); fflush( stdout );
         //p = fs::absolute( file );
         p = fs::canonical( file );
         //printf("p = %s\n", p.string().c_str()); fflush( stdout );
         s = p.string();
         //printf("s = %s\n", s.c_str()); fflush( stdout );
      }
      else
      {
         fs::path file( uri );
#if defined(_WIN32) || defined(WIN32)
         makeWinPath( baseDir );
#endif
         // fs::path dir( baseDir );
         //DE_DEBUG("fs::path(dir) = ",dir.string() )
         // p = fs::absolute( file, dir );
         p = fs::absolute( file );
      }
      //fs::path cwd = fs::current_path();
      //DE_DEBUG("fs::current_path(",cwd.string(),")" )

      //DE_DEBUG("fs::path.absolute(",p.string(),")" )

      p = fs::canonical( p );

      //DE_DEBUG("fs::path.absolute.canonical(",p.string(),")" )

      s = p.string();

      //DE_DEBUG("fs::path.absolute.canonical.string(",s,")" )

      //s = StringUtil::makePosix( s );

      //DE_DEBUG("fs::path.absolute.canonical.string.posix(",s,")" )
   }
   catch ( std::exception & e )
   {
      //DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
      s = uri;
   }

   //DE_DEBUG("makeAbsolute(",uri,") = ",s)
   return s;
}

void
Filesystem::entryList( std::string baseDir, bool recursive, bool withFiles, bool withDirs,
               std::function< void( std::string const & ) > const & userFunc )
{
//   baseDir = makeAbsolute( baseDir );
//   if ( !existDirectory( baseDir ) )
//   {
//      return;
//   }

   if ( recursive )
   {
      fs::recursive_directory_iterator it( baseDir );
      while ( it != fs::recursive_directory_iterator() )
      {
         fs::path p = it->path();

         //printf("recursive :: p = %s\n", p.string().c_str()); fflush ( stdout );

         std::string absUri = makeAbsolute( p.string() );
         replaceString( absUri, "\\", "/" ); // make posix path

         if ( withDirs && fs::is_directory( p ) )
         {
            userFunc( absUri );
         }

         if ( withFiles && fs::is_regular_file( p ) )
         {
            userFunc( absUri );
         }

         std::error_code ec;
         it.increment( ec );
         if ( ec )
         {
            // DE_ERROR("Recursive find : ",absUri," :: ",ec.message() )
            break;
         }
      }
   }
   else
   {
      fs::directory_iterator it( baseDir );
      while ( it != fs::directory_iterator() )
      {
         fs::path p = it->path();

         //printf("non-recursive :: p = %s\n", p.string().c_str()); fflush ( stdout );

         std::string absUri = makeAbsolute( p.string() );

         //printf("non-recursive :: a = %s\n", absUri.c_str()); fflush ( stdout );

         replaceString( absUri, "\\", "/" ); // make posix path

         if ( withDirs && fs::is_directory( p ) )
         {
            userFunc( absUri );
         }

         if ( withFiles && fs::is_regular_file( p ) )
         {
            userFunc( absUri );
         }

         std::error_code ec;
         it.increment( ec );
         if ( ec )
         {
            // DE_ERROR("Iterative find : ",absUri," :: ",ec.message() )
            break;
         }
      }
   }
}


// static
bool
Filesystem::isAbsolute( std::string const & uri )
{
   return fs::path( uri ).is_absolute();
}

// static
std::string
Filesystem::fileBase( std::string const & uri )
{
   return fs::path( uri ).stem().string();
}

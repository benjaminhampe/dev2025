/*
 *  filesystem.h
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

#pragma once
#include <cstdint>
#include <sstream>
#include <functional>
#include <cmath>

struct Filesystem
{
   static Filesystem& get();

   std::string config;
   std::string controllers;
   std::string default_bank;
   std::string user_banks;

   Filesystem();

   bool copy(const std::string &from, const std::string &to);

   bool create_dir(const std::string &path);

   bool exists(const std::string &path);

   bool move(const std::string &from, const std::string &to);

   //New: by Benjamin Hampe

   static bool endsWith( std::string const & str, std::string const & query );

   static uint32_t replaceString( std::string & s, std::string const & from, std::string const & to );

   static void makeWinPath( std::string & uri );

   static bool existFile( std::string const & uri );

   static bool existDirectory( std::string const & fileName );

   static std::string makeAbsolute( std::string uri, std::string baseDir = "" );

   static bool isAbsolute( std::string const & uri );

   static std::string fileBase( std::string const & uri );

   static void entryList( std::string baseDir, bool recursive, bool withFiles, bool withDirs,
               std::function< void( std::string const & ) > const & userFunc );

   static float
   parseFloat( std::string const & text )
   {
      float value = std::nanf("");

      // atof() and friends are affected by currently configured locale,
      // which can change the decimal point character.
      static std::locale locale = std::locale("C");

      std::istringstream i(text);
      i.imbue(locale);
      i >> value;

      return value;
   }

   static std::string
   stringifyFloat( float const value )
   {
      std::ostringstream o;
      o << value;
      return o.str();
   }

};

/*
 *  editor_pane.h
 *
 *  Copyright (c) 2001-2012 Nick Dowell
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
#include "de_types.hpp"
#include <QWidget>

struct BigImage
{
   QString uri;
   QString name;
   QImage img;
   int icoWidth;
   int icoHeight;
   int icoCount;
};

// Wrap image loading for easier debugging...
inline void
loadImage( BigImage & big, int icoWidth, int icoHeight, int icoCount, QString name, QString uri )
{
   big.uri = uri;
   big.name = name;
   big.icoWidth = icoWidth;
   big.icoHeight = icoHeight;
   big.icoCount = icoCount;      // also refered to as frames, slices, depth, etc...

   if ( big.img.load( uri ) )
   {
      printf("Loaded image %s\n", uri.toStdString().c_str() );
   }
   else
   {
      printf("Cant load image %s\n", uri.toStdString().c_str() );
   }

   fflush(stdout);
}


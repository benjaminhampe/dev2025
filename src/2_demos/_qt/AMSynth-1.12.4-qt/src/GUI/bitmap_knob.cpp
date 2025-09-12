/*
 *  bitmap_knob.cpp
 *
 *  Copyright (c) 2001-2019 Nick Dowell
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

#include "bitmap_knob.h"

#include "../controls.h"

#include <math.h>
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
/*


static const gchar *bitmap_knob_key = "bitmap_knob";

////////////////////////////////////////////////////////////////////////////////

static gboolean bitmap_knob_expose			( GtkWidget *wigdet, GdkEventExpose *event );
static gboolean bitmap_knob_button_press	( GtkWidget *wigdet, GdkEventButton *event );
static gboolean bitmap_knob_button_release	( GtkWidget *wigdet, GdkEventButton *event );
static gboolean bitmap_knob_motion_notify	( GtkWidget *wigdet, GdkEventMotion *event );
static gboolean bitmap_knob_scroll		( GtkWidget *wigdet, GdkEventScroll *event );

static void		bitmap_knob_set_adjustment				( GtkWidget *widget, GtkAdjustment *adjustment );
static void		bitmap_knob_adjustment_changed			( GtkAdjustment *adjustment, gpointer data );
static void		bitmap_knob_adjustment_value_changed	( GtkAdjustment *adjustment, gpointer data );

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

static int tooltip_update (bitmap_knob *self)
{
   float value = (float) gtk_adjustment_get_value (self->adjustment);
   char display[32] = "";
   int result = parameter_get_display ((int) self->parameter_index, value, display, sizeof(display));
   gtk_label_set_text (GTK_LABEL (self->tooltip_label), display);
   return result;
}

static void tooltip_show (bitmap_knob *self)
{
   gtk_widget_show (self->tooltip_window);

   gint widget_x = 0, widget_y = 0;
   gdk_window_get_origin (gtk_widget_get_window (self->drawing_area), &widget_x, &widget_y);

   gint tooltip_height = 0;
   gdk_window_get_geometry (gtk_widget_get_window (self->tooltip_window), NULL, NULL, NULL, &tooltip_height, NULL);

   gint width = 0;
   gint height = 0;
   gtk_widget_get_size_request (self->drawing_area, &width, &height);

   gint tooltip_x = widget_x + width + 4;
   gint tooltop_y = widget_y + (height - tooltip_height) / 2;

   gtk_window_move (GTK_WINDOW (self->tooltip_window), tooltip_x, tooltop_y);

   return;
}


////////////////////////////////////////////////////////////////////////////////

*/

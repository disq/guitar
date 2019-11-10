/* GtkEmbedFileSelection - An embedded widget for selecting files
 * Modification to GtkFilesel
 * by Kemal 'disq' Hadimli <disq@iname.com>
 */

/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include "../common.h"

#ifdef EMBEDFILESEL

#ifndef __GTK_EMBEDFILESEL_H__
#define __GTK_EMBEDFILESEL_H__


#include <gdk/gdk.h>
#include <gtk/gtkwindow.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_TYPE_EMBED_FILE_SELECTION            (gtk_embed_file_selection_get_type ())
#define GTK_EMBED_FILE_SELECTION(obj)            (GTK_CHECK_CAST ((obj), GTK_TYPE_EMBED_FILE_SELECTION, GtkEmbedFileSelection))
#define GTK_EMBED_FILE_SELECTION_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_EMBED_FILE_SELECTION, GtkEmbedFileSelectionClass))
#define GTK_IS_EMBED_FILE_SELECTION(obj)         (GTK_CHECK_TYPE ((obj), GTK_TYPE_EMBED_FILE_SELECTION))
#define GTK_IS_EMBED_FILE_SELECTION_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_EMBED_FILE_SELECTION))


typedef struct _GtkEmbedFileSelection       GtkEmbedFileSelection;
typedef struct _GtkEmbedFileSelectionClass  GtkEmbedFileSelectionClass;

struct _GtkEmbedFileSelection
{
  GtkVBox vbox;

  GtkWidget *mixed_list;
  GtkWidget *selection_entry;
  GtkWidget *selection_text;
  GtkWidget *history_pulldown;
  GtkWidget *history_menu;
  GList     *history_list;
  GtkWidget *fileop_dialog;
  GtkWidget *fileop_entry;
  gchar     *fileop_file;
  gpointer   cmpl_state;
  
  GtkWidget *button_area;

  GtkWidget *fileop_c_dir;
  GtkWidget *fileop_del_file;
  GtkWidget *fileop_ren_file;
};

struct _GtkEmbedFileSelectionClass
{
  GtkVBoxClass parent_class;
};


GtkType    gtk_embed_file_selection_get_type            (void);
GtkWidget* gtk_embed_file_selection_new                 ();
void       gtk_embed_file_selection_set_filename        (GtkEmbedFileSelection *filesel,
						   const gchar      *filename);
gchar*     gtk_embed_file_selection_get_filename        (GtkEmbedFileSelection *filesel);
void	   gtk_embed_file_selection_complete		  (GtkEmbedFileSelection *filesel,
						   const gchar	    *pattern);
void       gtk_embed_file_selection_show_fileop_buttons (GtkEmbedFileSelection *filesel);
void       gtk_embed_file_selection_hide_fileop_buttons (GtkEmbedFileSelection *filesel);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_EMBEDFILESEL_H__ */

#endif /* EMBEDFILESEL */

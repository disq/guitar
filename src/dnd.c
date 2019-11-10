#include "common.h"
#include "main.h"
#include "gladesrc.h"
#include "dnd.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef _DND_


void widget_drag_data_recv(GtkWidget *w, GdkDragContext *context, int x, int y,
GtkSelectionData *seldata, guint info, guint32 time,gpointer data)
{
int ret=1;

GList *fnames, *fnp;
guint count;
dndFunctionType func;

func=(dndFunctionType)data;

  fnames=gnome_uri_list_extract_filenames((char *)seldata->data);
  count=g_list_length(fnames);

  if (count>0)
	for (fnp = fnames; fnp; fnp = fnp->next, count--)
	  {
	  if (func==dnd_OPEN) ret=files_open_real((char *)(fnp->data));
	  if (func==dnd_ADD) ret=add_file_real((char *)(fnp->data));
	  }

  gnome_uri_list_free_strings(fnames);


/* gtk_drag_finish(context,TRUE,FALSE,time); */
if (ret==0)  gtk_drag_finish(context,TRUE,FALSE,time);
		else gtk_drag_finish(context,FALSE,FALSE,time);
}



void set_dnd_stuff(GtkWidget *w, dndFunctionType func)
{
GtkTargetEntry dragtypes[] =
	  {
	  { "text/uri-list",0,0 }
	  };

  gtk_drag_dest_set(w, GTK_DEST_DEFAULT_ALL, dragtypes,
					sizeof(dragtypes) / sizeof(dragtypes[0]),
					GDK_ACTION_COPY);

  gtk_signal_connect(GTK_OBJECT(w), "drag_data_received", GTK_SIGNAL_FUNC(widget_drag_data_recv), (gpointer)func);
}



#ifndef GNOME

/*
 *
 * shamelessly copied from gnotepad+
 * http://members.xoom.com/ackahn/gnp/
 *
 */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

/*** the next three routines are taken straight from gnome-libs so that the
     gtk-only version can receive drag and drops as well ***/
/**
 * gnome_uri_list_free_strings:
 * @list: A GList returned by gnome_uri_list_extract_uris() or gnome_uri_list_extract_filenames()
 *
 * Releases all of the resources allocated by @list.
 */
void          
gnome_uri_list_free_strings      (GList *list)
{
	g_list_foreach (list, (GFunc)g_free, NULL);
	g_list_free (list);
}


/**
 * gnome_uri_list_extract_uris:
 * @uri_list: an uri-list in the standard format.
 *
 * Returns a GList containing strings allocated with g_malloc
 * that have been splitted from @uri-list.
 */
GList*        
gnome_uri_list_extract_uris (const gchar* uri_list)
{
	const gchar *p, *q;
	gchar *retval;
	GList *result = NULL;
	
	g_return_val_if_fail (uri_list != NULL, NULL);

	p = uri_list;
	
	/* We don't actually try to validate the URI according to RFC
	 * 2396, or even check for allowed characters - we just ignore
	 * comments and trim whitespace off the ends.  We also
	 * allow LF delimination as well as the specified CRLF.
	 */
	while (p) {
		if (*p != '#') {
			while (isspace((int)(*p)))
				p++;
			
			q = p;
			while (*q && (*q != '\n') && (*q != '\r'))
				q++;
		  
			if (q > p) {
			        q--;
				while (q > p && isspace((int)(*q)))
					q--;
				
				retval = (char *)g_malloc (q - p + 2);
				strncpy (retval, p, q - p + 1);
				retval[q - p + 1] = '\0';
				
				result = g_list_prepend (result, retval);
			}
		}
		p = strchr (p, '\n');
		if (p)
			p++;
	}
	
	return g_list_reverse (result);
}


/**
 * gnome_uri_list_extract_filenames:
 * @uri_list: an uri-list in the standard format
 *
 * Returns a GList containing strings allocated with g_malloc
 * that contain the filenames in the uri-list.
 *
 * Note that unlike gnome_uri_list_extract_uris() function, this
 * will discard any non-file uri from the result value.
 */
GList*        
gnome_uri_list_extract_filenames (const gchar* uri_list)
{
	GList *tmp_list, *node, *result;
	
	g_return_val_if_fail (uri_list != NULL, NULL);
	
	result = gnome_uri_list_extract_uris (uri_list);

	tmp_list = result;
	while (tmp_list) {
		gchar *s = (char *)tmp_list->data;
		
		node = tmp_list;
		tmp_list = tmp_list->next;

		if (!strncmp (s, "file:", 5)) {
			node->data = g_strdup (s+5);
		} else {
			result = g_list_remove_link(result, node);
			g_list_free_1 (node);
		}
		g_free (s);
	}
	return result;
}
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */


#endif


#endif

#include "common.h"

#ifndef _DND_H_
#ifdef _DND_

typedef enum
{
  dnd_OPEN,
  dnd_ADD
} dndFunctionType;

void widget_drag_data_recv(GtkWidget *w, GdkDragContext *context, int x, int y,
GtkSelectionData *seldata, guint info, guint time,gpointer data);

void set_dnd_stuff(GtkWidget *w, dndFunctionType func);

#ifndef GNOME
void gnome_uri_list_free_strings      (GList *list);
GList* gnome_uri_list_extract_uris (const gchar* uri_list);
GList* gnome_uri_list_extract_filenames (const gchar* uri_list);
#endif

#endif
#endif

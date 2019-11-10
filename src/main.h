#ifndef _MAIN_H_

#include "common.h"

typedef enum
{
  OPEN,
  CREATE,
  ADD,
  DELETE,
  VIEW,
  EXTRACT
} archivefunctionType;

typedef struct tarstruct
  {
  char ourfile[512];
  char uncomp;
  char special;
  char param_comp[256];
  GtkWidget *clist;
  GList *files;
  } tarstruct, *tarstruct_p;

GtkWidget *win;
unsigned int tarcount;
tarstruct *tartemp;
char lastdir_open[4096];
char lastdir_extract[4096];


#ifdef GNOME
  gint die (GnomeClient *client, gpointer client_data);
  gint save_state ();
#endif
int main (int argc, char *argv[]);


#endif
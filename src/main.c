#include "common.h"
#include <stdio.h>

#include "main.h"
#include "gladesrc.h"

#ifdef GNOME
gint die (GnomeClient *client, gpointer client_data)
{
  gtk_exit(0);
  return(FALSE);
}

gint save_state ()
{
  gint x=0, y=0, w=0, h=0;

  gdk_window_get_geometry (win->window, &x, &y, &w, &h, NULL);

  gnome_config_push_prefix ("/guiTAR/");
  
  gnome_config_set_int ("Geometry/x", x);
  gnome_config_set_int ("Geometry/y", y);
  gnome_config_set_int ("Geometry/w", w);
  gnome_config_set_int ("Geometry/h", h);

  gnome_config_set_string ("Options/LastOpenDir", lastdir_open);
  gnome_config_set_string ("Options/LastExtractDir", lastdir_extract);

  gnome_config_pop_prefix ();
  gnome_config_sync();

  return TRUE;
}
#endif



int main (int argc, char *argv[]) {

#ifdef GNOME
int x=0,y=0,w=0,h=0;
GnomeClient *client;

  gnome_init ("guiTAR", VERSION, argc, argv);

  bindtextdomain("guiTAR", GNOMELOCALEDIR);
  textdomain("guiTAR");

  client=gnome_master_client();
  
  gtk_signal_connect (GTK_OBJECT (client), "save_yourself", GTK_SIGNAL_FUNC (save_state), NULL);
  gtk_signal_connect (GTK_OBJECT (client), "die", GTK_SIGNAL_FUNC (die), NULL);

  gnome_config_push_prefix ("/guiTAR/");
	  
  x=gnome_config_get_int("Geometry/x");
  y=gnome_config_get_int("Geometry/y");
  w=gnome_config_get_int("Geometry/w");
  h=gnome_config_get_int("Geometry/h");

{
gchar *d;

  lastdir_open[0]=0;
  lastdir_extract[0]=0;
  
  d=gnome_config_get_string("Options/LastOpenDir");
  if (d!=NULL) sprintf(lastdir_open,"%s",d);
  g_free(d);

  d=gnome_config_get_string("Options/LastExtractDir");
  if (d!=NULL) sprintf(lastdir_extract,"%s",d);
  g_free(d);
}
	  
  gnome_config_pop_prefix ();

#else
  gtk_set_locale();
  gtk_init(&argc, &argv);
  lastdir_open[0]=0;
  lastdir_extract[0]=0;
#endif

  tarcount=0;
  aboutboxopen=0;

  create_win();
#ifdef GNOME
  if ((w!=0) && (h!=0)) gtk_widget_set_usize(win,w,h);
  if ((x!=0) && (y!=0)) gtk_widget_set_uposition(win,x,y);
#endif
/*  gtk_widget_show_now(win); */
  gtk_widget_show(win);

  if (argc>1) files_open_real(argv[1]);

  gtk_main();
  return(0);
}

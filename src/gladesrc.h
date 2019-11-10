#ifndef _GLADESRC_H_

#include "common.h"
#include <stdio.h>

typedef struct newarchwin_struct
  {
  GtkWidget *win;
  GtkWidget *entry;
  GtkWidget *but_browse;
  GtkWidget *optmenu;
  GtkWidget *optmenu_menu;
  GtkWidget *item[7];
  GtkWidget *frame;
  GtkWidget *radio[3];
  GtkWidget *but_ok;
  GtkWidget *but_cancel;
  int archive_type;
  } newarchwin_struct;

newarchwin_struct newarchwin;
GtkWidget *tempwidget;
GtkWidget *notebook;
GtkWidget *statusbar;
GtkWidget *helplabel;
GtkWidget *embedfilesel;

GtkWidget *toolbar_add;
GtkWidget *toolbar_extract;
GtkWidget *toolbar_view;
GtkWidget *toolbar_delete;
GtkWidget *toolbar_close;

guint statusbar_id;
unsigned int statusbar_total;
GtkWidget *aboutwin;
char aboutboxopen;
#ifdef _PROGRESS_
  GtkWidget *pbar;
#endif

#ifdef GNOME
  GnomeMDI *mdi;
#endif

void pbar_init();
void pbar_update();
void pbar_deinit();
void wait_child_finish();
signed int findstr(char s[], char find[]);
void readl(FILE *f, char line[]);
int compare_last(char s[], char find[]);
void put_dirname(char *fn, char *direc);
#ifdef MYFILESEL
  void export_myfilesel2file(char *fn, GtkWidget *fs);
#endif
void write1line(char *fn, char *text);

#ifndef GTK_HAVE_FEATURES_1_1_2
void gtk_window_set_modal (GtkWindow *window, gboolean modal);
#endif

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void destroy (GtkWidget *widget, gpointer data);
#ifndef GNOME
#ifndef _MENUS_
void destroy_main();
#endif
#endif
GtkWidget* new_pixmap (gchar *filename[], GdkWindow *window, GdkColor  *background);
void some_toolbar_items_sensitive(gboolean b);
void PopStatusbar();
void PushStatusbar(char *desc);

#ifdef _CLISTSORT_
void clist_click_column (GtkCList *aclist, gint column, gpointer data);
#endif

void delete_files();
void view_file_real(char fn[], tarstruct *tar);
int add_file_real(char tmp[]);
void view_file();
void add_file(GtkWidget *w, GtkWidget *fs);
void close_file();
int files_open_real(char fn[]);
void files_open(GtkWidget *w, GtkFileSelection *fs);
void open_tar_dlg();
void add_file_dlg();
void new_arch_browse(GtkWidget *w, gpointer data);
void new_arch_browse_ok(GtkWidget *w, GtkFileSelection *fs);
void new_arch_select_type(GtkWidget *w, gpointer data);
void new_arch_ok(gpointer data);
int new_arch_real(GtkWidget *w, GtkWidget *fs);
void tar_extract(GtkWidget *w, GtkFileSelection *fs);
void extract_files();
void select_all();
void deselect_all();
void invert_selection();
int write_selection_to_file(tarstruct *tar, char fn[]);
tarstruct *create_new_child(char name[]);
GtkWidget *create_child_content();
void create_win ();
void create_aboutwin();
GtkWidget* create_viewwin(char *fn, char *titl);
void popup_menu(GtkWidget *widget,GdkEventButton *event,gpointer data);
void new_arch_dlg();
#endif

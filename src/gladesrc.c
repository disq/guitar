#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "programs.h"
#include "gladesrc.h"
#include "menu.h"
#include "dnd.h"
#include "gtkmyfilesel/gtkembedfilesel.h"
#include "gtkmyfilesel/gtkmyfilesel.h"
#include "icon.xpm"

#ifndef GNOME
  #include "logo.xpm"
#endif

#ifdef _PROGRESS_
void pbar_init()
{
gtk_progress_set_activity_mode(GTK_PROGRESS(pbar),TRUE);
gtk_progress_set_value(GTK_PROGRESS(pbar),0.0);
gtk_grab_add(pbar);
while (gtk_events_pending()) gtk_main_iteration();
}

void pbar_update()
{
float a;
a=gtk_progress_get_value(GTK_PROGRESS(pbar));
a+=10; if (a>16000) a=0;
gtk_progress_set_value(GTK_PROGRESS(pbar),a);
while (gtk_events_pending()) gtk_main_iteration();
}

void pbar_deinit()
{
gtk_progress_set_activity_mode(GTK_PROGRESS(pbar),FALSE);
gtk_progress_set_value(GTK_PROGRESS(pbar),0.0);
gtk_grab_remove(pbar);
}
#else
void pbar_init()
{
}
void pbar_update()
{
}
void pbar_deinit()
{
}
#endif

void wait_child_finish()
{
wait(NULL);

/*
 **** THIS CODE DOES NOT WORK! ****

int status=0;

pbar_init();
waitpid(0,&status,WNOHANG);

while (WIFEXITED(status)==0);
  {
  pbar_update();
  waitpid(0,&status,WNOHANG);
  }

pbar_deinit();
*/
}


/* search find[] in s[] and return starting point */
signed int findstr(char s[], char find[])
{
int l,l2,i,j;

  l=strlen(s);
  l2=strlen(find);
  j=0;
  for(i=0;((i<l) && (j<l2));i++)
	if ((s[i] | 32)==(find[j] | 32)) j++; else j=0;
  if (j<l2) j=0;
  if (j==0) return(-1);
  return(i-j);
}


/* read a line */
void readl(FILE *f, char line[])
{
int ch,p=0;

ch=fgetc(f);
if ((ch=='\n') || (feof(f))) { line[0]=0; return; }
while ((ch!='\n') && (!feof(f)))
{
  line[p]=ch;
  p++;
  ch=fgetc(f);
}
line[p]=0;
}


int compare_last(char s[], char find[])
{
int l,i;

  l=strlen(s);
  i=strlen(find);

if (i>l) return(-2);

return(strcmp(&s[l-i],find));
}


void put_dirname(char *fn, char *direc)
{
gchar *d;

  d=g_dirname(fn);
  sprintf(direc,"%s/",d);
  g_free(d);
}


#ifdef MYFILESEL
void export_myfilesel2file(char *fn, GtkWidget *fs)
{
gint rows;
gint i;
gchar *t;
FILE *f;

  f=fopen(fn,"w");

  rows=GTK_CLIST(GTK_MY_FILE_SELECTION(fs)->selected_list)->rows;
  for(i=0;i<rows;i++)
	{
	gtk_clist_get_text(GTK_CLIST(GTK_MY_FILE_SELECTION(fs)->selected_list),i,0,&t);
	fprintf(f,"%s\n",t);
	}

  fclose(f);	
}
#endif


void write1line(char *fn, char *text)
{
FILE *f;

  f=fopen(fn,"w");
  fprintf(f,"%s\n",text);
  fclose(f);	
}


#ifndef GTK_HAVE_FEATURES_1_1_2
#ifndef _SET_MODAL_DEFINED_
#define _SET_MODAL_DEFINED_
/* From gtk+ 1.1.2's gtkwindow.c -- modified a bit */
void gtk_window_set_modal (GtkWindow *window, gboolean modal)
{
  g_return_if_fail (window != NULL);
  g_return_if_fail (GTK_IS_WINDOW (window));

  /* If the widget was showed already, adjust it's grab state */
  if (GTK_WIDGET_VISIBLE(GTK_WIDGET(window)))
    {
      if (!modal)
	gtk_grab_remove (GTK_WIDGET(window));
      else if (modal)
	gtk_grab_add (GTK_WIDGET(window));
    }
}
#endif
#endif

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
 if (widget==aboutwin) aboutboxopen=0;
#ifdef GNOME
 if (widget==win) save_state();
#endif
 return (FALSE);
}

void destroy (GtkWidget *widget, gpointer data)
{
if (widget==aboutwin) aboutboxopen=0;
if (widget==win) gtk_main_quit();
}

#ifndef GNOME
#ifndef _MENUS_
void destroy_main()
{
destroy(win, NULL);
}
#endif
#endif


GtkWidget* new_pixmap (gchar *filename[], GdkWindow *window, GdkColor  *background)
{
  GtkWidget *wpixmap;
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  pixmap = gdk_pixmap_create_from_xpm_d (window, &mask, background,filename);
  wpixmap = gtk_pixmap_new (pixmap, mask);
  gdk_pixmap_unref (pixmap);
  return wpixmap;
}

void some_toolbar_items_sensitive(gboolean b)
{
#ifndef GNOME
  gtk_widget_set_sensitive(toolbar_add,b);
  gtk_widget_set_sensitive(toolbar_extract,b);
  gtk_widget_set_sensitive(toolbar_view,b);
  gtk_widget_set_sensitive(toolbar_delete,b);
  gtk_widget_set_sensitive(toolbar_close,b);
#endif
}


void PopStatusbar()
{
  gtk_statusbar_pop(GTK_STATUSBAR(statusbar),statusbar_id);
  statusbar_total--;
}

void PushStatusbar(char *desc)
{
  while (statusbar_total>0) PopStatusbar();
  gtk_statusbar_push(GTK_STATUSBAR(statusbar),statusbar_id,desc);
  statusbar_total++;
}


#ifdef _CLISTSORT_
void clist_click_column (GtkCList *aclist, gint column, gpointer data)
{
  if (column == aclist->sort_column)
    {
      if (aclist->sort_type == GTK_SORT_ASCENDING)
	aclist->sort_type = GTK_SORT_DESCENDING;
      else
	aclist->sort_type = GTK_SORT_ASCENDING;
    }
  else
    gtk_clist_set_sort_column (aclist, column);

  gtk_clist_sort(aclist);
}
#endif


void delete_files()
{
tarstruct *tar;
int ret=2;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please open a file first"));
	return;
	}

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));

  if (GTK_CLIST(tar->clist)->selection==NULL)
	{
    PushStatusbar(N_(" Please select a file first"));
	return;
	}

  PushStatusbar(N_(" Deleting files..."));
  while (gtk_events_pending()) gtk_main_iteration();

  if (tar->uncomp==' ') ret=_tar(DELETE, tar, tar->ourfile, "", NULL, TRUE);
  if (tar->uncomp=='z') ret=_targz(DELETE, tar, tar->ourfile, "", NULL);
  if (tar->uncomp=='Z') ret=_tarz(DELETE, tar, tar->ourfile, "", NULL);
  if (tar->uncomp=='I') ret=_tarbz2(DELETE, tar, tar->ourfile, "", NULL);
  if (tar->special==3)  ret=_zip(DELETE, tar, tar->ourfile, "", NULL);
  if (tar->special==4) ret=_lha(DELETE, tar, tar->ourfile, "", NULL);
  if (tar->special==5) ret=_arj(DELETE, tar, tar->ourfile, "", NULL);
  if (tar->special==6) ret=_rar(DELETE, tar, tar->ourfile, "", NULL);

  if (ret==0) PushStatusbar(N_(" Ok"));
  if (ret==1) PushStatusbar(N_(" Error deleting file"));
  if (ret==2) PushStatusbar(N_(" Possibly error occured calling archiver"));
  if (ret==3) PushStatusbar(N_(" Operation not supported by archiver"));
}


int add_file_real(char tmp[])
{
  tarstruct *tar;
  int ret=2;
  char param_comp_org[256];
  
  if (tarcount==0) return(-2);

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));

  PushStatusbar(N_(" Adding file to archive..."));
  while (gtk_events_pending()) gtk_main_iteration();

  strcpy(param_comp_org,tar->param_comp);

  if (tar->uncomp==' ') ret=_tar(ADD, tar, tar->ourfile, "", (gpointer)tmp, TRUE);
  if (tar->uncomp=='z') ret=_targz(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);
  if (tar->uncomp=='Z') ret=_tarz(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);
  if (tar->uncomp=='I') ret=_tarbz2(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);
  if (tar->special==3)  ret=_zip(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);
  if (tar->special==4)  ret=_lha(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);
  if (tar->special==5)  ret=_arj(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);
  if (tar->special==6)  ret=_rar(ADD, tar, tar->ourfile, tar->param_comp, (gpointer)tmp);

  if (ret==0) PushStatusbar(N_(" Ok"));
  if (ret==1) PushStatusbar(N_(" Error adding file"));
  if (ret==2) PushStatusbar(N_(" Possibly error occured calling archiver"));
  if (ret==3) PushStatusbar(N_(" Operation not supported by archiver"));

  strcpy(tar->param_comp,param_comp_org);

  return(ret);
}


void add_file(GtkWidget *w, GtkWidget *fs)
{
  char *tmp1;

#ifndef MYFILESEL
  char fn[4096];
  sprintf(fn,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
  if ((fn[0]=='/') && (fn[1]=='/')) memmove(fn,&fn[1],strlen(fn));

  if (access(fn,F_OK)!=0) /* if file does not exist */
	{
	PushStatusbar(N_(" File does not exist"));
	return;
	}
  tmp1=tempnam(NULL,"guitar");
  write1line(tmp1,fn);
#else
  if (GTK_CLIST(GTK_MY_FILE_SELECTION(fs)->selected_list)->selection==NULL)
	{
	PushStatusbar(N_(" You must add at least one file to the list"));
    return;
	}
  tmp1=tempnam(NULL,"guitar");
  export_myfilesel2file(tmp1, fs);
#endif
  gtk_object_destroy(GTK_OBJECT(fs));
  add_file_real(tmp1);

  remove(tmp1); free(tmp1);
}


void view_file_real(char fn[], tarstruct *tar)
{
int ret=2;

  PushStatusbar(N_(" Viewing file..."));
  while (gtk_events_pending()) gtk_main_iteration();

  if (tar->uncomp==' ') ret=_tar(VIEW, tar, tar->ourfile, "", (gpointer)fn, TRUE);
  if (tar->uncomp=='z') ret=_targz(VIEW, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->uncomp=='Z') ret=_tarz(VIEW, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->uncomp=='I') ret=_tarbz2(VIEW, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==3)  ret=_zip(VIEW, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==4)  ret=_lha(VIEW, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==5)  ret=_arj(VIEW, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==6)  ret=_rar(VIEW, tar, tar->ourfile, "", (gpointer)fn);

  if (ret==0) PushStatusbar(N_(" Ok"));
  if (ret==1) PushStatusbar(N_(" Error viewing file"));
  if (ret==2) PushStatusbar(N_(" Possibly error occured calling archiver"));
  if (ret==3) PushStatusbar(N_(" Operation not supported by archiver"));
}


void view_file()
{
  GList *list;
  gchar *t,*fl;
  gint row;
  char a;
  char b[256];
  tarstruct *tar;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please open a file first"));
	return;
	}

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));

  
  if (GTK_CLIST(tar->clist)->selection==NULL)
	{
    PushStatusbar(N_(" Please select a file first"));
	return;
	}

while (GTK_CLIST(tar->clist)->selection!=NULL)
  {
  a=0;
  list = GTK_CLIST(tar->clist)->selection;
  while ((list) && (a==0))
	{
	row=(gint)list->data;
    gtk_clist_get_text(GTK_CLIST(tar->clist),row,0,&fl);
    gtk_clist_unselect_row (GTK_CLIST(tar->clist),row,0);
    list = GTK_CLIST(tar->clist)->selection;

	if ((fl[0]!='d') && (fl[0]!='l'))
	  {
  	  gtk_clist_get_text(GTK_CLIST(tar->clist),row,5,&t);
	  a=1;
	  }
	}

  if (a==0)
	{
    PushStatusbar(N_(" Please select a non-directory/non-link to view"));
	return;
	}

  sprintf(b,"%s",t);
  view_file_real(b,tar);
  }
}


void close_file()
{
tarstruct *tar;
gint curpage;
GList *l;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please open a file first"));
	return;
	}

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));
  curpage=_NOTEBOOK_GET_CURRENT_PAGE_(GTK_NOTEBOOK(notebook));

  l=tar->files;

  while(l)
	{
    g_free(l->data);
	l=l->next;
    }
  g_list_free(tar->files);

  free(tar);
  gtk_notebook_remove_page(GTK_NOTEBOOK(notebook),curpage);
  tarcount--;
  
  if (tarcount==0)
	{
    some_toolbar_items_sensitive(FALSE);
    gtk_widget_hide(notebook);
  	gtk_widget_show(helplabel);
    }

}




int files_open_real(char fn[])
{
  unsigned int l,i;
  char uncomp;
  char special;
  tarstruct *tar;
  int ret=0;

  if ((fn[0]=='/') && (fn[1]=='/')) memmove(fn,&fn[1],strlen(fn));

  put_dirname(fn,lastdir_open);

  if (access(fn,F_OK)!=0) /* if file does not exist */
	{
	PushStatusbar(N_(" File does not exist"));
    return(-1);
	}

  PushStatusbar(N_(" Opening file..."));

  while (gtk_events_pending()) gtk_main_iteration();

  uncomp=' ';
  l=strlen(fn);
  i=0; special=0;
  if ((fn[l-1]=='z') && (fn[l-2]=='g') && (fn[l-3]=='.')) { uncomp='z'; i=3; }
  if ((fn[l-1]=='Z') && (fn[l-2]=='.')) { uncomp='Z'; i=2; }
  if ((fn[l-1]=='z') && (fn[l-2]=='.')) { uncomp='Z'; i=2; }
  if ((fn[l-1]=='2') && (fn[l-2]=='z') && (fn[l-3]=='b') && (fn[l-4]=='.')) { uncomp='I'; i=4; }
  if ((fn[l-1]=='z') && (fn[l-2]=='g') && (fn[l-3]=='t') && (fn[l-4]=='.')) { uncomp='z'; special=1; i=0; }
  if ((fn[l-1]=='p') && (fn[l-2]=='l') && (fn[l-3]=='s') && (fn[l-4]=='.')) { uncomp='I'; special=2; i=0; }
  if ((fn[l-1]=='p') && (fn[l-2]=='i') && (fn[l-3]=='z') && (fn[l-4]=='.')) { uncomp=0; i=0; special=3; }
  if ((fn[l-1]=='a') && (fn[l-2]=='h') && (fn[l-3]=='l') && (fn[l-4]=='.')) { uncomp=0; i=0; special=4; }
  if ((fn[l-1]=='h') && (fn[l-2]=='z') && (fn[l-3]=='l') && (fn[l-4]=='.')) { uncomp=0; i=0; special=4; }
  if ((fn[l-1]=='j') && (fn[l-2]=='r') && (fn[l-3]=='a') && (fn[l-4]=='.')) { uncomp=0; i=0; special=5; }
  if ((fn[l-1]=='r') && (fn[l-2]=='a') && (fn[l-3]=='r') && (fn[l-4]=='.')) { uncomp=0; i=0; special=6; }

  if ((special==0) && ((fn[l-i-1]!='r') || (fn[l-i-2]!='a') || (fn[l-i-3]!='t') || (fn[l-i-4]!='.')))
	{
	PushStatusbar(N_(" Invalid filename"));
    return(-2);
	}

  tar=create_new_child(fn);

  strcpy(tar->ourfile,fn);
  tar->uncomp=uncomp;
  tar->special=special;

  if (uncomp==' ') ret=_tar(OPEN, tar, tar->ourfile, "", NULL, TRUE);
  if (uncomp=='z') ret=_targz(OPEN, tar, tar->ourfile, "", NULL);
  if (uncomp=='Z') ret=_tarz(OPEN, tar, tar->ourfile, "", NULL);
  if (uncomp=='I') ret=_tarbz2(OPEN, tar, tar->ourfile, "", NULL);
  if (special==3) ret=_zip(OPEN, tar, tar->ourfile, "", NULL);
  if (special==4) ret=_lha(OPEN, tar, tar->ourfile, "", NULL);
  if (special==5) ret=_arj(OPEN, tar, tar->ourfile, "", NULL);
  if (special==6) ret=_rar(OPEN, tar, tar->ourfile, "", NULL);

  if (ret==0) tartemp=tar;

  return(ret);
}


void files_open(GtkWidget *w, GtkFileSelection *fs)
{
  char fn[512];

  sprintf(fn,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
  gtk_object_destroy(GTK_OBJECT(fs));
  files_open_real(fn);  
}


void open_tar_dlg()
{
GtkWidget *files;

  files = gtk_file_selection_new (N_("Open archive file"));
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)files_open,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_FILE_SELECTION(files)->window),TRUE);
  gtk_file_selection_set_filename(GTK_FILE_SELECTION(files), lastdir_open);
  gtk_widget_show(files);
}

void add_file_dlg()
{
  GtkWidget *files;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please create/open a file first"));
	return;
	}

#ifndef MYFILESEL
  files = gtk_file_selection_new (N_("Add file to archive"));
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)add_file,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_FILE_SELECTION(files)->window),TRUE);
#else
  files = gtk_my_file_selection_new (N_("Add file to archive"));
  gtk_signal_connect(GTK_OBJECT(GTK_MY_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)add_file,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_MY_FILE_SELECTION (files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_MY_FILE_SELECTION(files)->window),TRUE);
#endif
  gtk_widget_show(files);
}

void new_arch_browse(GtkWidget *w, gpointer data)
{
  GtkWidget *files;

  files = gtk_file_selection_new (N_("Create new archive"));
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)new_arch_browse_ok,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_FILE_SELECTION(files)->window),TRUE);
  gtk_widget_show(files);
}

void new_arch_browse_ok(GtkWidget *w, GtkFileSelection *fs)
{
  gtk_entry_set_text(GTK_ENTRY(newarchwin.entry), gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
  gtk_object_destroy(GTK_OBJECT(fs));
}

void new_arch_select_type(GtkWidget *w, gpointer data)
{
gboolean b=TRUE;

newarchwin.archive_type=(int)data;

if ((newarchwin.archive_type==0) || (newarchwin.archive_type==4)) b=FALSE;
gtk_widget_set_sensitive(newarchwin.frame,b);
}

void new_arch_ok(gpointer data)
{
  GtkWidget *files;
  char fn[4096];

  sprintf(fn,"%s",gtk_entry_get_text(GTK_ENTRY(newarchwin.entry)));

  if (strcmp(fn,"")==0)
	{
	PushStatusbar(N_(" Please enter a filename"));
    return;
	}

  if ((fn[0]=='/') && (fn[1]=='/')) memmove(fn,&fn[1],strlen(fn));
  if (fn[strlen(fn)-1]=='/') fn[strlen(fn)-1]=0;

  if (newarchwin.archive_type==0)
	{
	if (compare_last(fn,".tar")!=0) strcat(fn,".tar");
	}
  else if (newarchwin.archive_type==1)
	{
	if ((compare_last(fn,".tgz")!=0) && (compare_last(fn,".tar.gz")!=0)) strcat(fn,".tar.gz");
	}
  else if (newarchwin.archive_type==2)
	{
	if (compare_last(fn,".tar.bz2")!=0) strcat(fn,".tar.bz2");
	}
  else if (newarchwin.archive_type==3)
	{
	if (compare_last(fn,".zip")!=0) strcat(fn,".zip");
	}
  else if (newarchwin.archive_type==4)
	{
	if (compare_last(fn,".lzh")!=0) strcat(fn,".lzh");
	}
  else if (newarchwin.archive_type==5)
	{
	if (compare_last(fn,".arj")!=0) strcat(fn,".arj");
	}
  else if (newarchwin.archive_type==6)
	{
	if (compare_last(fn,".rar")!=0) strcat(fn,".rar");
	}

  gtk_entry_set_text(GTK_ENTRY(newarchwin.entry),fn);

  if (access(fn,F_OK)==0) /* if file exists */
	{
	PushStatusbar(N_(" File exists! Click on 'open' to open file"));
    return;
	}

#ifndef MYFILESEL
  files = gtk_file_selection_new (N_("Add file to archive"));
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)new_arch_real,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_FILE_SELECTION(files)->window),TRUE);
#else
  files = gtk_my_file_selection_new (N_("Add file to archive"));
  gtk_signal_connect(GTK_OBJECT(GTK_MY_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)new_arch_real,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_MY_FILE_SELECTION (files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_MY_FILE_SELECTION(files)->window),TRUE);
#endif
  gtk_widget_show(files);
}

int new_arch_real(GtkWidget *w, GtkWidget *fs)
{
#ifndef MYFILESEL
  char addf[4096];
#endif
char fn[4096];
int comp=1;
int i;
char *tmp1;
tarstruct *tar;
int ret=2;
char param_comp_org[256];

  sprintf(fn,"%s",gtk_entry_get_text(GTK_ENTRY(newarchwin.entry)));
  if ((fn[0]=='/') && (fn[1]=='/')) memmove(fn,&fn[1],strlen(fn));

  if (access(fn,F_OK)==0) /* if file exists */
	{
	PushStatusbar(N_(" File exists! Click on 'open' to open file"));
    return(-2);
	}
  if (strcmp(fn,"")==0)
	{
	PushStatusbar(N_(" Please enter a filename"));
    return(-3);
	}


#ifndef MYFILESEL
  sprintf(addf,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
  if ((addf[0]=='/') && (addf[1]=='/')) memmove(addf,&addf[1],strlen(addf));
  if (addf[strlen(addf)-1]=='/') addf[strlen(addf)-1]=0;
  if (access(addf,F_OK)!=0) /* if file does not exist */
	{
	PushStatusbar(N_(" File does not exist"));
    return(-1);
	}
  tmp1=tempnam(NULL,"guitar");
  write1line(tmp1,addf);
#else
  if (GTK_CLIST(GTK_MY_FILE_SELECTION(fs)->selected_list)->selection==NULL)
	{
	PushStatusbar(N_(" You must add at least one file to the list"));
    return(-2);
	}
  tmp1=tempnam(NULL,"guitar");
  export_myfilesel2file(tmp1, fs);
#endif
  gtk_object_destroy(GTK_OBJECT(fs));


  for(i=0;i<3;i++) if ((int)(GTK_TOGGLE_BUTTON(newarchwin.radio[i])->active)==1) comp=i;
  gtk_widget_destroy(newarchwin.win);

  PushStatusbar(N_(" Creating file..."));

  while (gtk_events_pending()) gtk_main_iteration();

  tar=create_new_child(fn);

  sprintf(tar->ourfile,"%s",fn);
  tar->uncomp=' ';
  tar->special=0;
  strcpy(tar->param_comp,"");

  if (newarchwin.archive_type==1)
	{
	tar->uncomp='z'; 
	if (comp==0) strcpy(tar->param_comp,"-9");
	if (comp==1) strcpy(tar->param_comp,"");
	if (comp==2) strcpy(tar->param_comp,"-1");
	}
  if (newarchwin.archive_type==2)
	{
	tar->uncomp='I'; 
	if (comp==0) strcpy(tar->param_comp,"-9 --repetitive-best");
	if (comp==1) strcpy(tar->param_comp,"");
	if (comp==2) strcpy(tar->param_comp,"-1 --repetitive-fast");
	}
  if (newarchwin.archive_type==3)
	{
	tar->uncomp=0;
	tar->special=3;
	if (comp==0) strcpy(tar->param_comp,"-9");
	if (comp==1) strcpy(tar->param_comp,"");
	if (comp==2) strcpy(tar->param_comp,"-0");
	}
  if (newarchwin.archive_type==4)
	{
	tar->uncomp=0;
	tar->special=4;
	strcpy(tar->param_comp,"");
	}
  if (newarchwin.archive_type==5)
	{
	tar->uncomp=0;
	tar->special=5;
	strcpy(tar->param_comp,"");
	}
  if (newarchwin.archive_type==6)
	{
	tar->uncomp=0;
	tar->special=6;
	if (comp==0) strcpy(tar->param_comp,"-m5");
	if (comp==1) strcpy(tar->param_comp,"-m3");
	if (comp==2) strcpy(tar->param_comp,"-m0");
	}

  strcpy(param_comp_org,tar->param_comp);

  if (tar->uncomp==' ') ret=_tar(CREATE, tar, tar->ourfile, "", (gpointer)tmp1, TRUE);
  if (tar->uncomp=='z') ret=_targz(CREATE, tar, tar->ourfile, tar->param_comp, (gpointer)tmp1);
  if (tar->uncomp=='I') ret=_tarbz2(CREATE, tar, tar->ourfile, tar->param_comp, (gpointer)tmp1);
  if (tar->special==3) ret=_zip(CREATE, tar, tar->ourfile, tar->param_comp, (gpointer)tmp1);
  if (tar->special==4) ret=_lha(CREATE, tar, tar->ourfile, tar->param_comp, (gpointer)tmp1);
  if (tar->special==5) ret=_arj(CREATE, tar, tar->ourfile, tar->param_comp, (gpointer)tmp1);
  if (tar->special==6) ret=_rar(CREATE, tar, tar->ourfile, tar->param_comp, (gpointer)tmp1);

  remove(tmp1); free(tmp1);

  if (ret==1) PushStatusbar(N_(" Error creating file"));
  if (ret==2) PushStatusbar(N_(" Possibly error occured calling archiver"));
  if (ret==3) PushStatusbar(N_(" Operation not supported by archiver"));
  if (ret!=0) return(ret);

  if (tar->uncomp==' ') ret=_tar(OPEN, tar, tar->ourfile, "", NULL, TRUE);
  if (tar->uncomp=='z') ret=_targz(OPEN, tar, tar->ourfile, "", NULL);
  if (tar->uncomp=='I') ret=_tarbz2(OPEN, tar, tar->ourfile, "", NULL);
  if (tar->special==3) ret=_zip(OPEN, tar, tar->ourfile, "", NULL);
  if (tar->special==4) ret=_lha(OPEN, tar, tar->ourfile, "", NULL);
  if (tar->special==5) ret=_arj(OPEN, tar, tar->ourfile, "", NULL);
  if (tar->special==6) ret=_rar(OPEN, tar, tar->ourfile, "", NULL);

  strcpy(tar->param_comp,param_comp_org);

  return(ret);
}


void tar_extract(GtkWidget *w, GtkFileSelection *fs)
{
  char fn[1024];
  tarstruct *tar;
  int ret=2;

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));

  sprintf(fn,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
  gtk_object_destroy(GTK_OBJECT(fs));
  PushStatusbar(N_(" Extracting files..."));
  while (gtk_events_pending()) gtk_main_iteration();

  if ((fn[0]=='/') && (fn[1]=='/')) memmove(fn,&fn[1],strlen(fn));

  put_dirname(fn,lastdir_extract);

  if (tar->uncomp==' ') ret=_tar(EXTRACT, tar, tar->ourfile, "", (gpointer)fn, TRUE);
  if (tar->uncomp=='z') ret=_targz(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->uncomp=='Z') ret=_tarz(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->uncomp=='I') ret=_tarbz2(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==3)  ret=_zip(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==4)  ret=_lha(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==5)  ret=_arj(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);
  if (tar->special==6)  ret=_rar(EXTRACT, tar, tar->ourfile, "", (gpointer)fn);

  if (ret==0) PushStatusbar(N_(" Ok"));
  if (ret==1) PushStatusbar(N_(" Error extracting file"));
  if (ret==2) PushStatusbar(N_(" Possibly error occured calling archiver"));
  if (ret==3) PushStatusbar(N_(" Operation not supported by archiver"));

}

void extract_files()
{
GtkWidget *files;
tarstruct *tar;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please open a file first"));
	return;
	}

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));

  if ((GTK_CLIST(tar->clist)->selection!=NULL) && (tar->special==5))
	{
    PushStatusbar(N_(" Extraction of a seperate file not supported by archiver, unselect all and extract all"));
	return;
	}

  files = gtk_file_selection_new (N_("Extract files"));

  gtk_widget_hide(GTK_FILE_SELECTION(files)->file_list);
#ifdef _NO_CLIST_POLICY_
  gtk_widget_hide(GTK_FILE_SELECTION(files)->file_list->parent);
#endif

  gtk_widget_hide(GTK_FILE_SELECTION(files)->fileop_del_file);
  gtk_widget_hide(GTK_FILE_SELECTION(files)->fileop_ren_file);
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(files)->ok_button),"clicked",(GtkSignalFunc)tar_extract,files);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (files)->cancel_button),"clicked",(GtkSignalFunc)gtk_widget_destroy,GTK_OBJECT(files));
  gtk_window_set_modal(GTK_WINDOW(&GTK_FILE_SELECTION(files)->window),TRUE);
  gtk_file_selection_set_filename(GTK_FILE_SELECTION(files), lastdir_extract);
  gtk_widget_show(files);
}


int write_selection_to_file(tarstruct *tar, char fn[])
{
FILE *f;
GList *list;
gint row;
char *t;
gint count;
char *tmp1, *tmp2;
char buf[4096];
int ret;

  tmp1=tempnam(NULL,"guitar");
  tmp2=tempnam(NULL,"guitar");

  count=0;
  f=fopen(tmp1,"w");
  list = GTK_CLIST(tar->clist)->selection;
  while (list)
	{
	row=(gint)list->data;
    gtk_clist_get_text(GTK_CLIST(tar->clist),row,5,&t);
    fprintf(f,"%s\n",t);
	count++;
	list = list->next;
	}
  fclose(f);

  sprintf(buf,"sort -r %s > %s 2> %s",tmp1,fn,tmp2);
  system(buf);

  ret=check4errors(tmp2,N_("Error(s) reverse-sorting filelist"));

  remove(tmp1); free(tmp1);
  remove(tmp2); free(tmp2);

  if (ret!=0) return(ret * -1);

  return(count);
}


void select_all()
{
#ifdef _CLISTSELECTALL_
  tarstruct *tar;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please open a file first"));
	return;
	}


  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));
  gtk_clist_select_all(GTK_CLIST(tar->clist));
#endif
}

void deselect_all()
{
#ifdef _CLISTSELECTALL_
  tarstruct *tar;

  if (tarcount==0)
	{
    PushStatusbar(N_(" Please open a file first"));
	return;
	}

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));
  gtk_clist_unselect_all(GTK_CLIST(tar->clist));
#endif
}

void invert_selection()
{
#ifdef _CLISTSELECTALL_
  tarstruct *tar;
  GList *l=NULL;
  GList *s=NULL;

  if (tarcount==0)
    {
	PushStatusbar(N_(" Please open a file first"));
    return;
	}

  tar=gtk_object_get_user_data(GTK_OBJECT(GTK_NOTEBOOK(notebook)->cur_page->child));

  gtk_clist_freeze(GTK_CLIST(tar->clist));

  l=GTK_CLIST(tar->clist)->selection;
  while(l)
	{
	s=g_list_append(s,l->data);
	l=l->next;
	}

  gtk_clist_select_all(GTK_CLIST(tar->clist));

  l=s;
  while(l)
	{
	gtk_clist_unselect_row(GTK_CLIST(tar->clist),(gint)l->data,0);
	l=l->next;
	}

  gtk_clist_thaw(GTK_CLIST(tar->clist));
#endif
}


tarstruct *create_new_child(char name[])
{
GtkWidget *label;
GtkWidget *content;
GtkWidget *clist;
tarstruct *tar;

tar=(tarstruct *)malloc(sizeof(tarstruct));

content=create_child_content();

clist=tempwidget;

if (tarcount==0)
  {
  gtk_widget_hide(helplabel);
  gtk_widget_show(notebook);
  }

label = gtk_label_new(name);
gtk_widget_show(label);

gtk_notebook_append_page(GTK_NOTEBOOK(notebook),content,label);
/* gtk_notebook_set_page(GTK_NOTEBOOK(notebook),gtk_notebook_page_num(GTK_NOTEBOOK(notebook),content)); */

tar->clist=clist;
strcpy(tar->param_comp,"");
gtk_object_set_user_data(GTK_OBJECT(content),(gpointer)tar);
gtk_object_set_user_data(GTK_OBJECT(clist),(gpointer)tar);

tar->files=NULL;

if (tarcount==0) some_toolbar_items_sensitive(TRUE);
tarcount++;

return(tar);
}



GtkWidget *create_child_content()
{
  GtkWidget *clist;
  GtkWidget *child;
  GtkWidget *label;
#ifdef _NO_CLIST_POLICY_
  GtkWidget *scrolledwindow1;
#endif


#ifdef _NO_CLIST_POLICY_
  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL); /* scrolledwindow for clist */
  gtk_widget_show (scrolledwindow1);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  child=scrolledwindow1;
#endif

  clist=gtk_clist_new(7);

  gtk_widget_show (clist);

#ifdef _DND_
  set_dnd_stuff(clist,dnd_ADD);
#endif

#ifdef _NO_CLIST_POLICY_
gtk_container_add(GTK_CONTAINER(scrolledwindow1),clist);
#else
  gtk_clist_set_policy(GTK_CLIST(clist), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  child=clist;
#endif

#ifdef _MENUS_
  gtk_signal_connect(GTK_OBJECT(clist),"button_press_event",GTK_SIGNAL_FUNC(popup_menu),NULL);
#endif

  label = gtk_label_new(N_("stats"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 0, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

  label = gtk_label_new(N_("owner/group"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 1, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

  label = gtk_label_new(N_("size"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 2, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0);

  label = gtk_label_new(N_("date"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 3, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

  label = gtk_label_new(N_("time"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 4, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

  label = gtk_label_new(N_("file"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 5, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

  label = gtk_label_new(N_("symlink"));
  gtk_widget_show(label);
  gtk_clist_set_column_widget(GTK_CLIST(clist), 6, label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

  gtk_clist_set_column_width (GTK_CLIST (clist), 0, 70);
  gtk_clist_set_column_width (GTK_CLIST (clist), 1, 80);
  gtk_clist_set_column_width (GTK_CLIST (clist), 2, 50);
  gtk_clist_set_column_width (GTK_CLIST (clist), 3, 80);
  gtk_clist_set_column_width (GTK_CLIST (clist), 4, 40);
  gtk_clist_set_column_width (GTK_CLIST (clist), 5, 200);
  gtk_clist_set_column_width (GTK_CLIST (clist), 6, 20);

  gtk_clist_set_selection_mode (GTK_CLIST (clist), GTK_SELECTION_MULTIPLE);

  gtk_clist_column_titles_show (GTK_CLIST (clist));

  gtk_clist_set_column_justification (GTK_CLIST(clist),2,GTK_JUSTIFY_RIGHT);

#ifdef _CLIST_RESIZE_FUNCS_
  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 0, TRUE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 0, FALSE);

  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 1, FALSE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 1, TRUE);

  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 2, FALSE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 2, TRUE);

  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 3, FALSE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 3, TRUE);

  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 4, FALSE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 4, TRUE);

  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 5, FALSE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 5, TRUE);

  gtk_clist_set_column_auto_resize (GTK_CLIST (clist), 6, FALSE);
  gtk_clist_set_column_resizeable (GTK_CLIST (clist), 6, TRUE);
#endif

#ifdef _CLISTSORT_
    gtk_signal_connect(GTK_OBJECT(clist),"click_column",(GtkSignalFunc)clist_click_column,NULL);
#endif


tempwidget=clist;
return(child);

}






void create_win()
{
  GtkWidget *vbox1;
#ifdef EMBEDFILESEL
  GtkWidget *hbox1;
#endif
#ifndef GNOME
  GtkWidget *handlebox1;
  GtkWidget *toolbar1;
#endif

  GdkPixmap *iconpixmap;
  GdkBitmap *iconmask;

#ifndef GNOME
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#else
  win = gnome_app_new ("guiTAR", "guiTAR");
#endif

  gtk_widget_realize (win);
  gtk_container_border_width (GTK_CONTAINER (win), 2);
  gtk_window_set_title (GTK_WINDOW (win), "guiTAR");
  gtk_window_set_policy (GTK_WINDOW (win), TRUE, TRUE, TRUE);
  gtk_window_position (GTK_WINDOW(win), GTK_WIN_POS_CENTER);
  gtk_widget_set_usize(win, 625, 300);
  gtk_signal_connect (GTK_OBJECT(win),"delete_event",GTK_SIGNAL_FUNC(delete_event),NULL);
  gtk_signal_connect (GTK_OBJECT(win),"destroy",GTK_SIGNAL_FUNC(destroy),NULL);

#ifdef _DND_
  set_dnd_stuff(win,dnd_OPEN);
#endif

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);

#ifndef GNOME
  gtk_container_add (GTK_CONTAINER (win), vbox1);

#ifdef _MENUS_
  handlebox1 = gtk_handle_box_new();
  gtk_widget_show(handlebox1);
  gtk_box_pack_start(GTK_BOX(vbox1), handlebox1, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(handlebox1), create_menu());
#endif

  handlebox1 = gtk_handle_box_new ();
  gtk_widget_show (handlebox1);
  gtk_box_pack_start (GTK_BOX (vbox1), handlebox1, FALSE, FALSE, 0);
#else
  gnome_app_set_contents (GNOME_APP(win), vbox1);
  create_menu();
#endif

#ifdef GNOME
{
GnomeUIInfo toolbardata[] = {
  GNOMEUIINFO_ITEM_NONE(N_("New"),N_("New file"),(GtkSignalFunc)new_arch_dlg),
  GNOMEUIINFO_ITEM_NONE(N_("Open"),N_("Open file"),(GtkSignalFunc)open_tar_dlg),
  GNOMEUIINFO_ITEM_NONE(N_("Close"),N_("Close file"),(GtkSignalFunc)close_file),
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_ITEM_NONE(N_("Add"),N_("Add file"),(GtkSignalFunc)add_file_dlg),
  GNOMEUIINFO_ITEM_NONE(N_("View"),N_("View files"),(GtkSignalFunc)view_file),
  GNOMEUIINFO_ITEM_NONE(N_("Extract"),N_("Extract files"),(GtkSignalFunc)extract_files),
  GNOMEUIINFO_ITEM_NONE(N_("Delete"),N_("Delete files"),(GtkSignalFunc)delete_files),
/*
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_ITEM_NONE(N_("Select all"),N_("Select all files"),(GtkSignalFunc)select_all),
  GNOMEUIINFO_ITEM_NONE(N_("Deselect all"),N_("Deselect all files"),(GtkSignalFunc)deselect_all),
*/
/*
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_ITEM_NONE(N_("About"),N_("About guiTAR"),(GtkSignalFunc)create_aboutwin),
  GNOMEUIINFO_ITEM_NONE(N_("Quit"),N_("Quit guiTAR"),(GtkSignalFunc)destroy),
*/
  GNOMEUIINFO_END
  };
gnome_app_create_toolbar(GNOME_APP(win), toolbardata);

}
														  
#else
  toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);

  gtk_widget_show (toolbar1);
  gtk_container_add (GTK_CONTAINER (handlebox1), toolbar1);

  gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("New"), N_("Create new archive"), "",
			       NULL,
			       (GtkSignalFunc)new_arch_dlg, NULL);

  gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("Open"), N_("Open file"), "",
			       NULL,
			       (GtkSignalFunc)open_tar_dlg, NULL);

  toolbar_close=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("Close"), N_("Close file"), "",
			       NULL,
			       (GtkSignalFunc)close_file, NULL);

  gtk_toolbar_append_space(GTK_TOOLBAR(toolbar1));

  toolbar_add=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("Add"), N_("Add file"), "",
			       NULL,
			       (GtkSignalFunc)add_file_dlg, NULL);

  toolbar_view=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("View"), N_("View file"), "",
			       NULL,
			       (GtkSignalFunc)view_file, NULL);

  toolbar_extract=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("Extract"), N_("Extract files"), "",
			       NULL,
			       (GtkSignalFunc)extract_files, NULL);

  toolbar_delete=gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("Delete"), N_("Delete files"), "",
			       NULL,
			       (GtkSignalFunc)delete_files, NULL);

  some_toolbar_items_sensitive(FALSE);


#ifndef GNOME
#ifndef _MENUS_

  gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("About"), N_("About guiTAR"), "",
			       NULL,
			       (GtkSignalFunc)create_aboutwin, NULL);
  gtk_toolbar_append_item(GTK_TOOLBAR(toolbar1),N_("Quit"), N_("Quit guiTAR"), "",
			       NULL,
			       (GtkSignalFunc)destroy_main, NULL);
#endif
#endif

#endif

  notebook=gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook),TRUE);
  gtk_widget_hide(notebook);

  helplabel = gtk_label_new(N_("click on 'Open' to open a file"));
  gtk_widget_show(helplabel);
#ifdef EMBEDFILESEL
  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);
  gtk_widget_show (hbox1);

  gtk_box_pack_start(GTK_BOX (hbox1), notebook, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX (hbox1), helplabel, TRUE, TRUE, 0);

  embedfilesel=gtk_embed_file_selection_new();
  gtk_embed_file_selection_hide_fileop_buttons(GTK_EMBED_FILE_SELECTION(embedfilesel));

  gtk_box_pack_start(GTK_BOX (hbox1), embedfilesel, FALSE, TRUE, 0);
  gtk_widget_show(embedfilesel);
#else
  gtk_box_pack_start(GTK_BOX (vbox1), notebook, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX (vbox1), helplabel, TRUE, TRUE, 0);
#endif


  statusbar=gtk_statusbar_new();
  gtk_widget_set_usize(statusbar,-1,20);
  gtk_widget_show(statusbar);

  statusbar_id=gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),"statusbar");
  statusbar_total=0;

#ifdef _PROGRESS_
  pbar=gtk_progress_bar_new();
/*
    gtk_progress_bar_set_activity_step(GTK_PROGRESS_BAR(pbar), 5);
    gtk_progress_bar_set_activity_blocks(GTK_PROGRESS_BAR(pbar), 5);
*/
  gtk_progress_set_activity_mode (GTK_PROGRESS (pbar), FALSE);
  gtk_progress_set_value (GTK_PROGRESS (pbar), 0.0);
  gtk_widget_set_usize (pbar, 100, 20);
  gtk_widget_show(pbar);
  gtk_box_pack_start(GTK_BOX(statusbar), pbar, FALSE, TRUE, 0);
#endif

#ifdef GNOME
  gnome_app_set_statusbar(GNOME_APP(win), statusbar);
#else
  gtk_box_pack_start(GTK_BOX(vbox1), statusbar, FALSE, TRUE, 0);
#endif

#ifdef _MENUS_
  create_popup_menu();
#endif


  iconpixmap = gdk_pixmap_create_from_xpm_d(win->window, &iconmask,&win->style->bg[GTK_STATE_NORMAL],icon_xpm);
  gdk_window_set_icon(win->window,NULL,iconpixmap,iconmask);
  gdk_window_set_icon_name(win->window,"guiTAR");

}


void create_aboutwin ()
{
if (aboutboxopen!=0) return;

{
#ifdef GNOME
const gchar *authors[] =
{"Kemal 'disq' Hadimli <disq@iname.com>", NULL};

aboutwin = gnome_about_new ("guiTAR", VERSION,
                            "Copyright (C) 1998, 1999",
                                  authors,
                                  N_("Generic archive tool for GNOME.\n\nhttp://disq.bir.net.tr/guitar"),"guiTAR.xpm");
#endif
#ifndef GNOME
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *dialog_action_area1;
  GtkWidget *but_close;

  GtkWidget *pixmap1;
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  GtkStyle *style;
  char abouttext[1024];

  aboutwin = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (aboutwin), "aboutwin", aboutwin);
  GTK_WINDOW (aboutwin)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_title (GTK_WINDOW (aboutwin), "About guiTAR (tm)");
  gtk_window_set_policy (GTK_WINDOW (aboutwin), FALSE, FALSE, TRUE);
  gtk_window_position (GTK_WINDOW (aboutwin), GTK_WIN_POS_CENTER);
  gtk_widget_realize(aboutwin);

  dialog_vbox1 = GTK_DIALOG (aboutwin)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX(dialog_vbox1), hbox, TRUE, TRUE, 0);

  style=gtk_widget_get_style(aboutwin);
  pixmap=gdk_pixmap_create_from_xpm_d(aboutwin->window,&mask,
							  &style->bg[GTK_STATE_NORMAL],(gchar **)logo_xpm);
  pixmap1=gtk_pixmap_new(pixmap,mask);
  gdk_pixmap_unref (pixmap);
  gtk_widget_show(pixmap1);
  gtk_box_pack_start(GTK_BOX(hbox),pixmap1,TRUE,TRUE,0);

strcpy(abouttext,"");
sprintf(abouttext,
"\n\
\n    guiTAR \
\n    version %s \
\n    by Kemal 'disq' Hadimli \
\n    <disq@bir.net.tr> \
\n \
%s \
\n \
\n Copyright (c) 1998-1999",
VERSION,
N_("\n    Generic archive tool in GTK+.\n\n    http://disq.bir.net.tr/guitar"));

  label = gtk_label_new(abouttext);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

  dialog_action_area1 = GTK_DIALOG (aboutwin)->action_area;
  gtk_widget_show (dialog_action_area1);

  but_close = gtk_button_new_with_label (N_("Close"));
  gtk_widget_show (but_close);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), but_close, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (but_close), 5);
  gtk_signal_connect_object(GTK_OBJECT(but_close),"clicked",GTK_SIGNAL_FUNC(gtk_widget_destroy),(gpointer)aboutwin);
#endif

  gtk_signal_connect(GTK_OBJECT(aboutwin),"delete_event",GTK_SIGNAL_FUNC(delete_event),NULL);
  gtk_signal_connect(GTK_OBJECT(aboutwin),"destroy",GTK_SIGNAL_FUNC(destroy),NULL);
  gtk_widget_show(aboutwin);
  aboutboxopen++;
}
}


GtkWidget* create_viewwin(char *fn, char *titl)
{
  GtkWidget *viewwin;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *text;
  GtkWidget *scr;
  GtkWidget *but;
  FILE *f;
  char c[2];

  viewwin = gtk_window_new (GTK_WINDOW_DIALOG);

/*
#ifndef GNOME
  gtk_container_add (GTK_CONTAINER (win), vbox1);
#else
  gnome_app_set_contents (GNOME_APP(win), vbox1);
#endif
*/

  gtk_widget_set_usize (viewwin, 550, 220);
  gtk_container_border_width (GTK_CONTAINER (viewwin), 2);
  gtk_window_position (GTK_WINDOW (viewwin), GTK_WIN_POS_CENTER);
  gtk_window_set_policy (GTK_WINDOW (viewwin), TRUE, TRUE, FALSE);

  gtk_window_set_title (GTK_WINDOW (viewwin), titl);

  gtk_signal_connect (GTK_OBJECT(viewwin),"delete_event",GTK_SIGNAL_FUNC(delete_event),NULL);
  gtk_signal_connect (GTK_OBJECT(viewwin),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroy),(gpointer)GTK_WIDGET(viewwin));

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (viewwin), vbox);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

  text = gtk_text_new (NULL, NULL);
  gtk_widget_show (text);
  gtk_box_pack_start (GTK_BOX (hbox), text, TRUE, TRUE, 0);

  scr = gtk_vscrollbar_new(GTK_TEXT(text)->vadj);
  gtk_widget_show (scr);
  gtk_box_pack_start (GTK_BOX (hbox), scr, FALSE, TRUE, 0);

  gtk_widget_realize(text);
  gtk_text_freeze(GTK_TEXT(text));

  f=fopen(fn,"r");
  if (f!=NULL)
	{
	c[1]=0;
    c[0]=fgetc(f);
	while (!feof(f))  
	  {
  	  gtk_text_insert(GTK_TEXT(text),NULL,NULL,NULL,c,-1);
	  c[0]=fgetc(f);
  	  }
	fclose(f);
	PopStatusbar();
	} else
	{
	PushStatusbar(N_(" Error opening temporary file"));
	}

  gtk_text_thaw(GTK_TEXT(text));

  but = gtk_button_new_with_label (N_("Close"));
  gtk_widget_show (but);
  gtk_box_pack_start (GTK_BOX(vbox), but, FALSE, TRUE, 0);
  gtk_signal_connect_object(GTK_OBJECT(but),"clicked",GTK_SIGNAL_FUNC(gtk_widget_destroy),(gpointer)viewwin);

  gtk_widget_show(viewwin);
  return viewwin;
}


																											

void popup_menu(GtkWidget *widget,GdkEventButton *event,gpointer data)
{
#ifdef _MENUS_
  gint x, y;
  
  if (event->button==3)
	{
	gdk_window_get_pointer(NULL, &x, &y, NULL);
	gtk_item_factory_popup(it_popup,x,y,event->button,event->time);
	}
#endif
}



void new_arch_dlg()
{
  GtkWidget *vbox2;
  GtkWidget *table2;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *vbox3;
  GSList *gurup_group = NULL;
  GtkWidget *hbox1;

  newarchwin.win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_border_width (GTK_CONTAINER (newarchwin.win), 4);
  gtk_window_set_title (GTK_WINDOW (newarchwin.win), "Create new archive");
  gtk_window_set_policy (GTK_WINDOW (newarchwin.win), TRUE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT(newarchwin.win),"delete_event",GTK_SIGNAL_FUNC(delete_event),NULL);
  gtk_signal_connect (GTK_OBJECT(newarchwin.win),"destroy",GTK_SIGNAL_FUNC(destroy),NULL);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (newarchwin.win), vbox2);

  table2 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table2);
  gtk_box_pack_start (GTK_BOX (vbox2), table2, FALSE, TRUE, 0);

  label1 = gtk_label_new ("Filename:");
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table2), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_FILL, 0, 0);
  gtk_widget_set_usize (label1, -1, 16);
  gtk_misc_set_alignment (GTK_MISC (label1), 1, 0.5);
  gtk_misc_set_padding (GTK_MISC (label1), 4, 0);

  newarchwin.entry = gtk_entry_new ();
  gtk_widget_show (newarchwin.entry);
  gtk_table_attach (GTK_TABLE (table2), newarchwin.entry, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_FILL, 0, 0);
  gtk_widget_set_usize (newarchwin.entry, 200, 22);

  newarchwin.but_browse = gtk_button_new_with_label ("...");
  gtk_widget_show (newarchwin.but_browse);
  gtk_table_attach (GTK_TABLE (table2), newarchwin.but_browse, 2, 3, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 0, 0);
  gtk_widget_set_usize (newarchwin.but_browse, 22, 22);
  gtk_signal_connect_object(GTK_OBJECT(newarchwin.but_browse),"clicked",GTK_SIGNAL_FUNC(new_arch_browse),NULL);

  label2 = gtk_label_new ("Archive type:");
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table2), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 1, 0.5);
  gtk_misc_set_padding (GTK_MISC (label2), 4, 0);

  newarchwin.optmenu = gtk_option_menu_new ();
  gtk_widget_show (newarchwin.optmenu);
  gtk_table_attach (GTK_TABLE (table2), newarchwin.optmenu, 1, 3, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_FILL, 0, 0);
  gtk_widget_set_usize (newarchwin.optmenu, 119, 25);

  newarchwin.optmenu_menu = gtk_menu_new ();

  newarchwin.archive_type=0;
  
  newarchwin.item[0] = gtk_menu_item_new_with_label (".tar");
  gtk_widget_show (newarchwin.item[0]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[0]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[0]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)0);

  newarchwin.item[1] = gtk_menu_item_new_with_label (".tar.gz (or .tgz)");
  gtk_widget_show (newarchwin.item[1]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[1]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[1]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)1);

  newarchwin.item[2] = gtk_menu_item_new_with_label (".tar.bz2");
  gtk_widget_show (newarchwin.item[2]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[2]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[2]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)2);

  newarchwin.item[3] = gtk_menu_item_new_with_label (".zip");
  gtk_widget_show (newarchwin.item[3]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[3]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[3]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)3);

  newarchwin.item[4] = gtk_menu_item_new_with_label (".lzh");
  gtk_widget_show (newarchwin.item[4]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[4]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[4]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)4);

  newarchwin.item[5] = gtk_menu_item_new_with_label (".arj");
  gtk_widget_show (newarchwin.item[5]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[5]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[5]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)5);
  gtk_widget_set_sensitive(newarchwin.item[5],FALSE);

  newarchwin.item[6] = gtk_menu_item_new_with_label (".rar");
  gtk_widget_show (newarchwin.item[6]);
  gtk_menu_append (GTK_MENU (newarchwin.optmenu_menu), newarchwin.item[6]);
  gtk_signal_connect(GTK_OBJECT(newarchwin.item[6]),"activate",GTK_SIGNAL_FUNC(new_arch_select_type),(gpointer)6);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (newarchwin.optmenu), newarchwin.optmenu_menu);

  newarchwin.frame = gtk_frame_new ("Compression level");
  gtk_widget_show (newarchwin.frame);
  gtk_table_attach (GTK_TABLE (table2), newarchwin.frame, 0, 3, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_container_border_width (GTK_CONTAINER (newarchwin.frame), 13);
  gtk_frame_set_label_align (GTK_FRAME (newarchwin.frame), 0.02, 0.5);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (newarchwin.frame), vbox3);

  newarchwin.radio[0] = gtk_radio_button_new_with_label (gurup_group, "Best compression, slow");
  gurup_group = gtk_radio_button_group (GTK_RADIO_BUTTON (newarchwin.radio[0]));
  gtk_widget_show (newarchwin.radio[0]);
  gtk_box_pack_start (GTK_BOX (vbox3), newarchwin.radio[0], TRUE, TRUE, 0);
  gtk_widget_set_usize (newarchwin.radio[0], -1, 22);

  newarchwin.radio[1] = gtk_radio_button_new_with_label (gurup_group, "Normal");
  gurup_group = gtk_radio_button_group (GTK_RADIO_BUTTON (newarchwin.radio[1]));
  gtk_widget_show (newarchwin.radio[1]);
  gtk_box_pack_start (GTK_BOX (vbox3), newarchwin.radio[1], TRUE, TRUE, 0);
  gtk_widget_set_usize (newarchwin.radio[1], -1, 22);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (newarchwin.radio[1]), TRUE);

  newarchwin.radio[2] = gtk_radio_button_new_with_label (gurup_group, "Worst compression, fast");
  gurup_group = gtk_radio_button_group (GTK_RADIO_BUTTON (newarchwin.radio[2]));
  gtk_widget_show (newarchwin.radio[2]);
  gtk_box_pack_start (GTK_BOX (vbox3), newarchwin.radio[2], TRUE, TRUE, 0);
  gtk_widget_set_usize (newarchwin.radio[2], -1, 22);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (hbox1), 2);

  newarchwin.but_ok = gtk_button_new_with_label ("Create");
  gtk_widget_show (newarchwin.but_ok);
  gtk_box_pack_start (GTK_BOX (hbox1), newarchwin.but_ok, TRUE, TRUE, 0);
  gtk_widget_set_usize (newarchwin.but_ok, -1, 25);
  gtk_signal_connect_object(GTK_OBJECT(newarchwin.but_ok),"clicked",GTK_SIGNAL_FUNC(new_arch_ok),NULL);

  newarchwin.but_cancel = gtk_button_new_with_label ("Cancel");
  gtk_widget_show (newarchwin.but_cancel);
  gtk_box_pack_start (GTK_BOX (hbox1), newarchwin.but_cancel, TRUE, TRUE, 0);
  gtk_widget_set_usize (newarchwin.but_cancel, -1, 25);
  gtk_signal_connect_object(GTK_OBJECT(newarchwin.but_cancel),"clicked",GTK_SIGNAL_FUNC(gtk_widget_destroy),(gpointer)newarchwin.win);

  gtk_option_menu_set_history(GTK_OPTION_MENU(newarchwin.optmenu),1);
  new_arch_select_type(NULL, (gpointer)1);

  gtk_window_set_modal(GTK_WINDOW(newarchwin.win),TRUE);
  gtk_widget_show(newarchwin.win);
}

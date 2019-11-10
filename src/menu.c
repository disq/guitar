
#include "common.h"

#ifdef _MENUS_

#include "main.h"
#include "menu.h"
#include "gladesrc.h"

#ifndef GNOME
static  GtkItemFactoryEntry main_menu_items[] =
	{
  { "/_File",                   NULL,         NULL,      0, "<Branch>" },
  { "/File/tearoff0",                   NULL,         NULL,      0, "<Tearoff>" },
  { "/File/_New",      "<control>N", (GtkSignalFunc)new_arch_dlg,      0 },
  { "/File/_Open",      "<control>O", (GtkSignalFunc)open_tar_dlg,      0 },
  { "/File/_Close",     "<control>W", (GtkSignalFunc)close_file,      0 },
  { "/File/sep0",              NULL,         NULL,      0, "<Separator>" },
  { "/File/_Quit",     "<control>Q", (GtkSignalFunc)main_menu_quit,      0 },
  { "/_Archive",                   NULL,         NULL,      0, "<Branch>" },
  { "/Archive/tearoff1",                   NULL,         NULL,      0, "<Tearoff>" },
  { "/Archive/_Add",      "<control>P", (GtkSignalFunc)add_file_dlg,      0 },
  { "/Archive/_View",      "<control>V", (GtkSignalFunc)view_file,      0 },
  { "/Archive/_Extract",   "<control>T", (GtkSignalFunc)extract_files,      0 },
  { "/Archive/_Delete",   "<control>R", (GtkSignalFunc)delete_files,      0 },
  { "/Archive/sep1",              NULL,         NULL,      0, "<Separator>" },
  { "/Archive/_Select all",         "<control>A", (GtkSignalFunc)select_all,   0 },
  { "/Archive/_Deselect all",       "<control>D", (GtkSignalFunc)deselect_all, 0 },
  { "/Archive/_Invert selection",   "<control>I", (GtkSignalFunc)invert_selection, 0 },
  { "/_Help",                   NULL,         NULL,      0, "<Branch>" },
  { "/Help/tearoff2",                   NULL,         NULL,      0, "<Tearoff>" },
  { "/Help/_About",      NULL, (GtkSignalFunc)create_aboutwin,      0 },
	};
#else

static GnomeUIInfo file_menu[]= {
 { 
    GNOME_APP_UI_ITEM,
    N_("_New"), N_("Create new archive"),
    new_arch_dlg, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
    'N', GDK_CONTROL_MASK, NULL
  },
 { 
    GNOME_APP_UI_ITEM,
    N_("_Open"), N_("Open file"),
    open_tar_dlg, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_OPEN,
    'O', GDK_CONTROL_MASK, NULL
  },
 { 
    GNOME_APP_UI_ITEM,
    N_("_Close"), N_("Close file"),
    close_file, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CLOSE,
    'W', GDK_CONTROL_MASK, NULL
  },
 GNOMEUIINFO_SEPARATOR,
 { 
    GNOME_APP_UI_ITEM,
    N_("Q_uit"), N_("Quit program"),
    main_menu_quit, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_QUIT,
    'Q', GDK_CONTROL_MASK, NULL
  },
  GNOMEUIINFO_END
};


static GnomeUIInfo archive_menu[]=
{
  {
    GNOME_APP_UI_ITEM,
    N_("_Add"), N_("Add file"),
    add_file_dlg, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'P', GDK_CONTROL_MASK, NULL
  }, 
  {
    GNOME_APP_UI_ITEM,
    N_("_View"), N_("View file"),
    view_file, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'V', GDK_CONTROL_MASK, NULL
  }, 
  {
    GNOME_APP_UI_ITEM,
    N_("_Extract"), N_("Extract files"),
    extract_files, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'T', GDK_CONTROL_MASK, NULL
  }, 
  {
    GNOME_APP_UI_ITEM,
    N_("_Delete"), N_("Delete files"),
    delete_files, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'R', GDK_CONTROL_MASK, NULL
  }, 
 GNOMEUIINFO_SEPARATOR,
  {
    GNOME_APP_UI_ITEM,
    N_("_Select all"), N_("Select all files"),
    select_all, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'A', GDK_CONTROL_MASK, NULL
  }, 
  {
    GNOME_APP_UI_ITEM,
    N_("_Deselect all"), N_("Deselect all files"),
    deselect_all, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'D', GDK_CONTROL_MASK, NULL
  }, 
  {
    GNOME_APP_UI_ITEM,
    N_("_Invert selection"), N_("Invert the selection"),
    invert_selection, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    'I', GDK_CONTROL_MASK, NULL
  }, 
  GNOMEUIINFO_END
};

static GnomeUIInfo help_menu[]=
{
  {
    GNOME_APP_UI_ITEM,
    N_("_About"), N_("About guiTAR"),
    create_aboutwin, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
    0, (GdkModifierType)0, NULL
  }, 
  GNOMEUIINFO_END
};

static GnomeUIInfo main_menu[]= 
{
  GNOMEUIINFO_SUBTREE(N_("_File"), file_menu),
  GNOMEUIINFO_SUBTREE(N_("_Archive"), archive_menu),
  GNOMEUIINFO_SUBTREE(N_("_Help"), help_menu),
  GNOMEUIINFO_END
};
#endif

static  GtkItemFactoryEntry popup_items[] =
	{
  { "/_Add",      NULL, (GtkSignalFunc)add_file_dlg,      0 },
  { "/_View",     NULL, (GtkSignalFunc)view_file,      0 },
  { "/_Extract",  NULL, (GtkSignalFunc)extract_files,      0 },
  { "/_Delete",   NULL, (GtkSignalFunc)delete_files,      0 },
  { "/sep0",              NULL,         NULL,      0, "<Separator>" },
  { "/_Select all",   NULL, (GtkSignalFunc)select_all,   0 },
  { "/_Deselect all", NULL, (GtkSignalFunc)deselect_all, 0 },
  { "/_Invert selection",   NULL, (GtkSignalFunc)invert_selection, 0 },
  { "/sep1",              NULL,         NULL,      0, "<Separator>" },
  { "/_New",      NULL, (GtkSignalFunc)new_arch_dlg,      0 },
  { "/_Open",     NULL, (GtkSignalFunc)open_tar_dlg,      0 },
  { "/_Close",    NULL, (GtkSignalFunc)close_file,      0 }
	};


void main_menu_quit (gpointer callback_data,guint callback_action,GtkWidget *widget)
{
delete_event(win, NULL, NULL);
destroy(win, NULL);
}


GtkWidget *create_menu()
{
#ifndef GNOME
  GtkAccelGroup *accel_group;
  GtkWidget *w=NULL;
  int nmenu_items;

  accel_group = gtk_accel_group_new ();

    item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
	nmenu_items = sizeof (main_menu_items) / sizeof (main_menu_items[0]);
	gtk_item_factory_create_items (item_factory, nmenu_items, main_menu_items, NULL);
    gtk_accel_group_attach (accel_group, GTK_OBJECT(win));
    w=gtk_item_factory_get_widget(item_factory, "<main>");

  gtk_widget_show(w);

#ifndef _CLISTSELECTALL_
  gtk_widget_set_sensitive(gtk_item_factory_get_widget(item_factory, "<main>/Archive/Select all"),FALSE);
  gtk_widget_set_sensitive(gtk_item_factory_get_widget(item_factory, "<main>/Archive/Deselect all"),FALSE);
#endif

  return(w);

#else
  gnome_app_create_menus (GNOME_APP(win), main_menu);
  return(NULL);

#endif
}


void create_popup_menu()
{
  GtkAccelGroup *accel_group;
  int nmenu_items;

  accel_group = gtk_accel_group_new ();

  it_popup = gtk_item_factory_new (GTK_TYPE_MENU, "<main>", accel_group);
  nmenu_items = sizeof (popup_items) / sizeof (popup_items[0]);
  gtk_item_factory_create_items (it_popup, nmenu_items, popup_items, NULL);
  gtk_accel_group_attach (accel_group, GTK_OBJECT(win));

#ifndef _CLISTSELECTALL_
  gtk_widget_set_sensitive(gtk_item_factory_get_widget(it_popup, "<main>/Select all"),FALSE);
  gtk_widget_set_sensitive(gtk_item_factory_get_widget(it_popup, "<main>/Deselect all"),FALSE);
#endif

}

#endif

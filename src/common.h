
#ifdef HAVE_CONFIG_H
  #include "../config.h"
#endif

#ifdef MYFILESEL
  #define FNMATCH
#endif

#ifdef EMBEDFILESEL
  #ifndef FNMATCH
	#define FNMATCH
  #endif
#endif

#include <gtk/gtk.h>

#ifdef GNOME
  #include <gnome.h>
#else
  #define N_(String) String
#endif

#ifdef GTK_HAVE_FEATURES_1_1_0
  #define _PROGRESS_
#endif

#ifdef GTK_HAVE_FEATURES_1_1_2
  #define _CLISTSORT_
  #define _CLISTSELECTALL_
#endif

#ifdef GTK_HAVE_FEATURES_1_1_4
  #define _CLIST_RESIZE_FUNCS_
  #define _NO_CLIST_POLICY_
  #define _DND_
#endif

#ifdef GTK_HAVE_FEATURES_1_1_6
  #define _CLIST_SET_COLUMN_VISIBILITY_
#endif

#ifdef GTK_HAVE_FEATURES_1_1_6
  #define _NOTEBOOK_GET_CURRENT_PAGE_ gtk_notebook_get_current_page
#else
  #define _NOTEBOOK_GET_CURRENT_PAGE_ gtk_notebook_current_page
#endif

#ifdef __GTK_ITEM_FACTORY_H__
  #define _MENUS_
#endif


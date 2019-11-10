#ifndef _MENU_H_
#define _MENU_H_

#include "common.h"

#ifdef _MENUS_

GtkItemFactory *item_factory;
GtkItemFactory *it_popup;

void main_menu_quit (gpointer callback_data,guint callback_action,GtkWidget *widget);
GtkWidget *create_menu();
void create_popup_menu();

#endif
#endif

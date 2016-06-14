
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "core/bit-macros.h"
#include "core/debug.h"

#include "lcd_menu.h"



/** This is used when an invalid menu handle is required in
 *  a \ref MENU_ITEM() definition, i.e. to indicate that a
 *  menu has no linked parent, child, next or previous entry.
 */
menu_item_t PROGMEM NULL_MENU = {0};


void (*next_func)(void) = NULL;
void (*enter_func)(void) = NULL;
void (*display_func)(void) = NULL;

/** \internal
 *  Pointer to the generic menu text display function
 *  callback, to display the configured text of a menu item
 *  if no menu-specific display function has been set
 *  in the select menu item.
 */
void (*lcdm_write_function)(const char* text) = NULL;


/** \internal
 *  Pointer to the currently selected menu item.
 */
menu_item_t* current_menu_item = &NULL_MENU;


menu_item_t* lcdm_get_current_menu(void)
{
  return current_menu_item;
}


menu_item_t* lcdm_get_parent_menu(void)
{
  return MENU_PARENT;
}

menu_item_t* lcdm_get_child_menu(void)
{
  return MENU_CHILD;
}


void lcdm_navigate(menu_item_t* const new_menu)
{
  if ((new_menu == &NULL_MENU) || (new_menu == NULL))
    return;

  current_menu_item = new_menu;

  //if (lcdm_write_function) lcdm_write_function(current_menu_item->text);

  //void (* fkt)(void)
  //void (* display_func)(void) = MENU_ITEM_READ_POINTER(&current_menu_item->display_callback);
  
  next_func = MENU_ITEM_READ_POINTER(&current_menu_item->next_callback);
  enter_func = MENU_ITEM_READ_POINTER(&current_menu_item->enter_callback);
  display_func = MENU_ITEM_READ_POINTER(&current_menu_item->display_callback);

  /**
  void (*select_callback)(void) = MENU_ITEM_READ_POINTER(&current_menu_item->select_callback);

  if (select_callback)
    select_callback();
  **/
}

void lcdm_set_default_write_callback(void (*write_func)(const char* text))
{
  lcdm_write_function = write_func;
  //lcdm_navigate(current_menu_item);
}

void lcdm_enter_current_item(void)
{
  if ((current_menu_item == &NULL_MENU) || (current_menu_item == NULL))
    return;

  void (*enter_callback)(void) = MENU_ITEM_READ_POINTER(&current_menu_item->enter_callback);

  if (enter_callback)
    enter_callback();
}






const char* lcdm_get_parent_menu_text(menu_item_t* const cur_menu)
{
  
  menu_item_t* parent_menu = MENU_PARENT;

  return parent_menu->text;
}

const char* lcdm_get_child_menu_text(menu_item_t* const cur_menu)
{
  
  menu_item_t* parent_menu = MENU_CHILD;

  return parent_menu->text;
}


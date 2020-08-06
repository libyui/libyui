/*
  Copyright (C) 2020 SUSE LLC
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCMenuBar.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <algorithm>    // std::max()
#include <iterator>

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCMenuBar.h"
#include "YNCursesUI.h"
#include "NCPopupMenu.h"


// Margin to the left of the first toplevel item
#define LEFT_MARGIN     0

// Spacing between toplevel items
#define SPACING         2


struct NCMenuBar::Menu {
    wpos position;
    YMenuItem* item;
};


NCMenuBar::NCMenuBar(YWidget* parent) :
    YMenuBar(parent), NCWidget( parent ), _selected_menu(nullptr)
{
    defsze = wsze(1, 10);
}


NCMenuBar::~NCMenuBar()
{
    clear();
}


void
NCMenuBar::clear()
{
    for (Menu* menu : _menus)
	delete menu;

    _menus.clear();

    _selected_menu = nullptr;
    defsze = wsze(1, 10);
}


void
NCMenuBar::rebuildMenuTree()
{
    clear();

    int width = LEFT_MARGIN;

    for (YItemIterator it = itemsBegin(); it != itemsEnd(); ++it) {
	YMenuItem* item = dynamic_cast<YMenuItem*>(*it);
	YUI_CHECK_PTR(item);

	if (!item->isMenu())
	    YUI_THROW(YUIException("NCMenuBar: Only menus allowed on toplevel."));

	Menu* menu = new Menu();
	menu->item = item;
	menu->position = wpos(0, width);

	_menus.push_back(menu);
	item->setUiItem(menu);

	NClabel label(NCstring(menu->item->label()));
	label.stripHotkey();

	width += label.width() + SPACING;
    }

    defsze = wsze(1, width);
}


void
NCMenuBar::wRedraw()
{
    if ( !win )
	return;

    if (!_selected_menu)
	select_next_menu();

    for ( auto menu : _menus )
    {
	const NCstyle::StWidget& style = menu_style(menu);

	win->bkgdset(style.plain);

	NClabel label(NCstring(menu->item->label()));
	label.stripHotkey();

	label.drawAt(*win, style,  menu->position, wsze(-1, label.width() + SPACING), NC::LEFT);
    }

    if (defsze.W < win->width()) {
	const NCstyle::StWidget& bkg_style(widgetStyle(true));

	win->move(0, defsze.W);
	win->bkgdset( bkg_style.plain );
	win->clrtoeol();
    }
}


NCursesEvent
NCMenuBar::postMenu()
{
    wpos at(ScreenPos() + wpos(1, _selected_menu->position.C));

    NCPopupMenu* dialog = new NCPopupMenu(
	at,
	_selected_menu->item->childrenBegin(),
	_selected_menu->item->childrenEnd()
    );

    YUI_CHECK_NEW(dialog);

    int selection = dialog->post();

    if (selection < 0) {
	YDialog::deleteTopmostDialog();
	return NCursesEvent::none;
    }

    NCursesEvent event = NCursesEvent::menu;
    event.selection = findMenuItem(selection);
    YDialog::deleteTopmostDialog();

    return event;
}


void
NCMenuBar::setItemEnabled( YMenuItem * item, bool enabled )
{
    YMenuWidget::setItemEnabled( item, enabled );
    rebuildMenuTree();
    wRedraw();
}


void
NCMenuBar::activateItem( YMenuItem * item )
{
    if ( item->isMenu() || item->isSeparator() )
	return;

    NCursesEvent event = NCursesEvent::menu;

    event.widget = this;
    event.selection = item;

    YNCursesUI::ui()->sendEvent( event );
}


NCursesEvent
NCMenuBar::wHandleInput(wint_t key)
{
    NCursesEvent event = NCursesEvent::none;

    switch (key) {
	case KEY_LEFT:
	    select_previous_menu();
	    wRedraw();
	    break;
	case KEY_RIGHT:
	    select_next_menu();
	    wRedraw();
	    break;
	case KEY_DOWN:
	    event = postMenu();
	    break;
	default:
	    event = NCWidget::wHandleInput(key);
	    break;
    }

    return event;
}


int
NCMenuBar::preferredWidth()
{
    return defsze.W;
}


int
NCMenuBar::preferredHeight()
{
    return 1;
}


void
NCMenuBar::setSize(int newWidth, int newHeight)
{
    wRelocate(wpos(0), wsze(newHeight, newWidth));
}


void
NCMenuBar::setEnabled(bool enabled)
{
    NCWidget::setEnabled(enabled);
    YMenuBar::setEnabled(enabled);
}


bool
NCMenuBar::setKeyboardFocus()
{
    if ( !grabFocus() )
	return YWidget::setKeyboardFocus();

    return true;
}


bool
NCMenuBar::HasHotkey(int key)
{
    // TODO
    return false;
}


NCursesEvent
NCMenuBar::wHandleHotkey( wint_t key )
{
    // TODO
    NCursesEvent event = NCursesEvent::none;

    return event;
}


void
NCMenuBar::select_next_menu()
{
    std::vector<Menu*>::iterator begin = _menus.begin();

    if (_selected_menu)
    {
	auto current = find(_menus.begin(), _menus.end(), _selected_menu);

	if (current != _menus.end())
	    begin = std::next(current, 1);
    }

    auto next_menu = std::find_if(begin, _menus.end(), [](const Menu* menu) {
	return menu->item->isEnabled();
    });

    if (next_menu != _menus.end())
	_selected_menu = *next_menu;
}


void
NCMenuBar::select_previous_menu()
{
    if (!_selected_menu)
	return;

    auto current = find(_menus.begin(), _menus.end(), _selected_menu);

    std::reverse_iterator<std::vector<Menu *>::iterator> rbegin(current);

    auto previous_menu = std::find_if(rbegin, _menus.rend(), [](const Menu* menu) {
	return menu->item->isEnabled();
    });

    if (previous_menu != _menus.rend())
	_selected_menu = *previous_menu;
}


const NCstyle::StWidget& NCMenuBar::menu_style(const Menu * menu) const
{
    if (!menu->item->isEnabled())
	return wStyle().getWidget( NC::WSdisabled );

    bool non_active = (menu != _selected_menu);
    return widgetStyle(non_active);
}

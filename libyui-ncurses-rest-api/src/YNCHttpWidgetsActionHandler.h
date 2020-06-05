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

#ifndef YNCHttpWidgetsActionHandler_h
#define YNCHttpWidgetsActionHandler_h

#include <iostream>
#include <functional>
#include <yui/YEvent.h>

#include "YHttpWidgetsActionHandler.h"
#include "YNCHttpUI.h"
#include "YMenuItem.h"

#include "NCWidget.h"

class YNCHttpWidgetsActionHandler : public YHttpWidgetsActionHandler
{

public:

    YNCHttpWidgetsActionHandler() {}
    virtual ~YNCHttpWidgetsActionHandler() {}

protected:

    virtual void activate_widget( YCheckBoxFrame * widget );
    virtual void activate_widget( YComboBox * widget );
    virtual void activate_widget( YDateField * widget );
    virtual void activate_widget( YSelectionBox * widget );
    virtual void activate_widget( YTimeField * widget );

    virtual void activate_widget ( YMultiSelectionBox * widget, YItem * item );

private:

    template<typename T>
    void activate_nc_widget( T * widget ) {
        if( widget->notify() && dynamic_cast<NCWidget*>( widget ) )
        {
            NCursesEvent event = NCursesEvent::Activated;
            event.widget = dynamic_cast<NCWidget*>( widget );
            YNCHttpUI::ui()->sendEvent( event );
        }
    }

    template<typename T, typename I>
    void activate_nc_widget( T * widget , I * item) {
        if( widget->notify() && dynamic_cast<NCWidget*>( widget ) )
        {
            NCursesEvent event( NCursesEvent::menu );
            event.selection = (YMenuItem *) item;
            event.widget = dynamic_cast<NCWidget*>( widget );
            YNCHttpUI::ui()->sendEvent( event );
        }
    }

};

#endif // YNCHttpWidgetsActionHandler_h

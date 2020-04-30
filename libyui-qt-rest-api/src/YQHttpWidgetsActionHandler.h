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

#ifndef YQHttpWidgetsActionHandler_h
#define YQHttpWidgetsActionHandler_h

#include <iostream>
#include <functional>
#include <microhttpd.h>
#include <yui/YEvent.h>

#include "YHttpWidgetsActionHandler.h"
#include "YQHttpUI.h"

class YQHttpWidgetsActionHandler : public YHttpWidgetsActionHandler
{
public:

    YQHttpWidgetsActionHandler() {}
    virtual ~YQHttpWidgetsActionHandler() {}


protected:

    virtual void activate_widget( YDateField * widget );
    virtual void activate_widget( YComboBox * widget );
    virtual void activate_widget( YInputField * widget );
    virtual void activate_widget( YSelectionBox * widget );
    virtual void activate_widget( YTimeField * widget );

private:
    template<typename T>
    void activate_qt_widget( T * widget ) {
        if( widget->notify() )
            YQHttpUI::ui()->sendEvent( new YWidgetEvent( widget, YEvent::ValueChanged ) );
    }
};

#endif // YQHttpWidgetsActionHandler_h

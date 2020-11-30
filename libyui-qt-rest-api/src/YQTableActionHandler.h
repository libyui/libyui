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

#ifndef YQTableActionHandler_h
#define YQTableActionHandler_h

#include <iostream>
#include <functional>
#include <microhttpd.h>
#include <sstream>

#include "YQHttpUI.h"
#include "YTableActionHandler.h"

#include <boost/algorithm/string.hpp>

class YQTableActionHandler : public YTableActionHandler
{
protected:
    virtual void activate_widget( YTable * widget, YItem * item );
};

#endif //YQTableActionHandler_h

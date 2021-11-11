/*
  Copyright (C) 2021 SUSE LLC

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

#ifndef YMultiSelectionBoxActionHandler_h
#define YMultiSelectionBoxActionHandler_h

#include <functional>

#include <yui/YMultiSelectionBox.h>

#include "YWidgetActionHandler.h"


class YMultiSelectionBoxActionHandler : public YWidgetActionHandler
{
public:

    typedef enum Status {
      ON,
      OFF,
      TOGGLE
    } CheckBoxStatus;

    YMultiSelectionBoxActionHandler() {};
    virtual ~YMultiSelectionBoxActionHandler() {};

public:
    std::function<void (YMultiSelectionBox*)> check_handler( YMultiSelectionBox * widget,
                                               const std::string &value,
                                               CheckBoxStatus status );

    std::function<void (YMultiSelectionBox*)> select_handler( YMultiSelectionBox * widget,
                                               const std::string &value);

private:

    CheckBoxStatus new_state;

};

#endif // YMultiSelectionBoxActionHandler_h

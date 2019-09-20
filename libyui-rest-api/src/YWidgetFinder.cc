/*
  Copyright (C) 2017 SUSE LLC

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

// boost::erase_all
#include <boost/algorithm/string.hpp>
#include <functional>

#include "YDialog.h"
#include "YWidget.h"
#include "YWidgetID.h"

#include "YWidgetFinder.h"

// internal helper methods
static bool filter_by_label_rec(YWidget *w, const std::string &label);
static bool filter_by_id_rec(YWidget *w, const std::string &id);
static bool filter_by_type_rec(YWidget *w, const std::string &type);
static void find_widgets(YWidget *w, WidgetArray &array, std::function<bool (YWidget*)> filter_func);

// WidgetArray YWidgetFinder::find(const std::string &label, const std::string &id, const std::string &type)
WidgetArray YWidgetFinder::find( const char* label, const char* id, const char* type )
{
    WidgetArray ret;
    find_widgets(YDialog::topmostDialog(), ret, [& label, & id, & type] (YWidget *w) {
        return (!label || filter_by_label_rec(w, label)) &&
               (!id || filter_by_id_rec(w, id)) &&
               (!type || filter_by_type_rec(w, type));
    } );
    return ret;
}

WidgetArray YWidgetFinder::by_label(const std::string &label)
{
    WidgetArray ret;
    find_widgets(YDialog::topmostDialog(), ret, [& label] (YWidget *w) {
        return filter_by_label_rec(w, label);
    } );
    return ret;
}

WidgetArray YWidgetFinder::by_id(const std::string &id)
{
    WidgetArray ret;
    find_widgets(YDialog::topmostDialog(), ret, [& id] (YWidget *w) {
        return filter_by_id_rec(w, id);
    } );
    return ret;
}

WidgetArray YWidgetFinder::by_type(const std::string &type)
{
    WidgetArray ret;
    find_widgets(YDialog::topmostDialog(), ret, [& type] (YWidget *w) {
        return filter_by_type_rec(w, type);
    } );
    return ret;
}

WidgetArray YWidgetFinder::all()
{
    WidgetArray ret;
    find_widgets(YDialog::topmostDialog(), ret, [] (YWidget *w) {
        return true;
    } );
    return ret;
}

void find_widgets(YWidget *w, WidgetArray &array, std::function<bool (YWidget*)> filter_func) {
    if (!w) return;

    if(filter_func(w)) array.push_back(w);

    for(YWidget *child: *w)
    {
        find_widgets(child, array, filter_func);
    };
}

static bool filter_by_label_rec(YWidget *w, const std::string &label)
{
    // check the widget label if it is defined
    if (w->propertySet().contains("Label"))
    {
        std::string widget_label = w->getProperty("Label").stringVal();
        boost::erase_all(widget_label, "&");

        if (widget_label == label) return true;
    }
    return false;
}

static bool filter_by_id_rec(YWidget *w, const std::string &id)
{
    if (w->hasId() && w->id()->toString() == id)
        return true;

    return false;
}


static bool filter_by_type_rec(YWidget *w, const std::string &type)
{
    auto propSet = w->propertySet();

    if (propSet.contains("WidgetClass") && w->getProperty("WidgetClass").stringVal() == type)
        return true;

    return false;
}

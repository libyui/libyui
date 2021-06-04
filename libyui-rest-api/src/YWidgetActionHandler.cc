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

#include <codecvt>
#include <boost/range/algorithm/remove_if.hpp>

#include "YWidgetActionHandler.h"

static
std::string normalize_label_bidi(const std::string& label)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    //static const std::wstring bidi_controls(L"\u202a\u202b\u202c\u202d\u202e\u2066\u2067\u2068\u2069");
    static wchar_t bidi_controls[] { L'\u202a', L'\u202b', L'\u202c', L'\u202d', L'\u202e', L'\u2066', L'\u2067', L'\u2068', L'\u2069' };

    std::wstring wlabel(conv.from_bytes(label));
    wlabel.erase(boost::remove_if(wlabel, boost::is_any_of(bidi_controls)), wlabel.end());
    std::string cleaned(conv.to_bytes(wlabel));

    return cleaned;
}

static
std::string normalize_label_shortcut(const std::string& label)
{
    return boost::erase_all_copy(label, ShortcutChar);
}

std::string YWidgetActionHandler::normalize_label(const std::string & label)
{
    return normalize_label_shortcut(normalize_label_bidi(label));
}
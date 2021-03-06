/*
  Copyright (C) 2000-2012 Novell, Inc
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

   File:       NCstyle.xterm.h

   Author:     Generated by class NCstyle

/-*/

#ifndef NCstyle_xterm_h
#define NCstyle_xterm_h

#include "NCstyle.h"

inline void NCstyleInit_xterm( std::vector<NCstyle::Style> & styleSet )
{
    //=================================================================
    // init global attributes
    //=================================================================
    NCattrset * attrset( &styleSet[NCstyle::DefaultStyle].getAttrGlobal() );
    attrset->setAttr( NCstyle::AppTitle, 14080 );
    attrset->setAttr( NCstyle::AppText, 8192 );
    //=================================================================
    // init local attributes
    //=================================================================
    // DefaultStyle
    attrset = &styleSet[NCstyle::DefaultStyle].getAttrLocal();
    attrset->setAttr( NCstyle::DialogBorder, 8192 );
    attrset->setAttr( NCstyle::DialogTitle, 8192 );
    attrset->setAttr( NCstyle::DialogActiveBorder, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveTitle, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::DialogText, 8192 );
    attrset->setAttr( NCstyle::DialogHeadline, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogDisabled, 8448 );
    attrset->setAttr( NCstyle::DialogPlain, 8192 );
    attrset->setAttr( NCstyle::DialogLabel, 8192 );
    attrset->setAttr( NCstyle::DialogData, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogScrl, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActivePlain, 14080 );
    attrset->setAttr( NCstyle::DialogActiveLabel, 14080 );
    attrset->setAttr( NCstyle::DialogActiveData, 14080 );
    attrset->setAttr( NCstyle::DialogActiveHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveScrl, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFramePlain, 8192 );
    attrset->setAttr( NCstyle::DialogFrameLabel, 8192 );
    attrset->setAttr( NCstyle::DialogFrameData, 8192 );
    attrset->setAttr( NCstyle::DialogFrameHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFrameScrl, 8192 );
    attrset->setAttr( NCstyle::DialogActiveFramePlain, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameLabel, 14080 );
    attrset->setAttr( NCstyle::DialogActiveFrameData, 8192 );
    attrset->setAttr( NCstyle::DialogActiveFrameHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameScrl, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListTitle, 8192 );
    attrset->setAttr( NCstyle::ListPlain, 8192 );
    attrset->setAttr( NCstyle::ListLabel, 8192 );
    attrset->setAttr( NCstyle::ListData, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelPlain, 8192 );
    attrset->setAttr( NCstyle::ListSelLabel, 8192 );
    attrset->setAttr( NCstyle::ListSelData, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveTitle, 8192 );
    attrset->setAttr( NCstyle::ListActivePlain, 8192 );
    attrset->setAttr( NCstyle::ListActiveLabel, 8192 );
    attrset->setAttr( NCstyle::ListActiveData, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelPlain, 14080 );
    attrset->setAttr( NCstyle::ListActiveSelLabel, 14080 );
    attrset->setAttr( NCstyle::ListActiveSelData, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextPlain, 16128 );
    attrset->setAttr( NCstyle::RichTextTitle, 15872 );
    attrset->setAttr( NCstyle::RichTextLink, 15104 );
    attrset->setAttr( NCstyle::RichTextArmedlink, 15104 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextActiveArmedlink, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextVisitedLink, 59 );
    attrset->setAttr( NCstyle::RichTextB, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextI, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextBI, 14336 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextBT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextIT, 15872 );
    attrset->setAttr( NCstyle::RichTextBIT, 14848 );
    attrset->setAttr( NCstyle::ProgbarCh, 0x1720 );
    attrset->setAttr( NCstyle::ProgbarBgch, 16160 );
    attrset->setAttr( NCstyle::TextCursor, 7936 | A_BLINK );
    // InfoStyle
    attrset = &styleSet[NCstyle::InfoStyle].getAttrLocal();
    attrset->setAttr( NCstyle::DialogBorder, 5888 );
    attrset->setAttr( NCstyle::DialogTitle, 5888 );
    attrset->setAttr( NCstyle::DialogActiveBorder, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveTitle, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogText, 5888 );
    attrset->setAttr( NCstyle::DialogHeadline, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogDisabled, 5376 | A_BOLD );
    attrset->setAttr( NCstyle::DialogPlain, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogLabel, 5888 );
    attrset->setAttr( NCstyle::DialogData, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogHint, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogScrl, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActivePlain, 0 );
    attrset->setAttr( NCstyle::DialogActiveLabel, 0 );
    attrset->setAttr( NCstyle::DialogActiveData, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveHint, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveScrl, 16128 );
    attrset->setAttr( NCstyle::DialogFramePlain, 5888 );
    attrset->setAttr( NCstyle::DialogFrameLabel, 5888 );
    attrset->setAttr( NCstyle::DialogFrameData, 5888 );
    attrset->setAttr( NCstyle::DialogFrameHint, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFrameScrl, 4096 );
    attrset->setAttr( NCstyle::DialogActiveFramePlain, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameLabel, 0 );
    attrset->setAttr( NCstyle::DialogActiveFrameData, 5888 );
    attrset->setAttr( NCstyle::DialogActiveFrameHint, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameScrl, 5376 | A_BOLD );
    attrset->setAttr( NCstyle::ListTitle, 5888 );
    attrset->setAttr( NCstyle::ListPlain, 5888 );
    attrset->setAttr( NCstyle::ListLabel, 5888 );
    attrset->setAttr( NCstyle::ListData, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::ListHint, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelPlain, 5888 );
    attrset->setAttr( NCstyle::ListSelLabel, 5888 );
    attrset->setAttr( NCstyle::ListSelData, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelHint, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveTitle, 5888 );
    attrset->setAttr( NCstyle::ListActivePlain, 5888 );
    attrset->setAttr( NCstyle::ListActiveLabel, 5888 );
    attrset->setAttr( NCstyle::ListActiveData, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveHint, 4096 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelPlain, 0 );
    attrset->setAttr( NCstyle::ListActiveSelLabel, 0 );
    attrset->setAttr( NCstyle::ListActiveSelData, 0 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelHint, 0 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextPlain, 16128 );
    attrset->setAttr( NCstyle::RichTextTitle, 15360 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextLink, 15104 );
    attrset->setAttr( NCstyle::RichTextArmedlink, 15104 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextActiveArmedlink, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextVisitedLink, 59 );
    attrset->setAttr( NCstyle::RichTextB, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextI, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextT, 14592 );
    attrset->setAttr( NCstyle::RichTextBI, 14336 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextBT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextIT, 14592 );
    attrset->setAttr( NCstyle::RichTextBIT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::ProgbarCh, 0x1720 );
    attrset->setAttr( NCstyle::ProgbarBgch, 16160 );
    attrset->setAttr( NCstyle::TextCursor, 7936 | A_BLINK );
    // WarnStyle
    attrset = &styleSet[NCstyle::WarnStyle].getAttrLocal();
    attrset->setAttr( NCstyle::DialogBorder, 3840 );
    attrset->setAttr( NCstyle::DialogTitle, 3840 );
    attrset->setAttr( NCstyle::DialogActiveBorder, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveTitle, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogText, 3840 );
    attrset->setAttr( NCstyle::DialogHeadline, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogDisabled, 3584 | A_BOLD );
    attrset->setAttr( NCstyle::DialogPlain, 3840 );
    attrset->setAttr( NCstyle::DialogLabel, 3840 );
    attrset->setAttr( NCstyle::DialogData, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogHint, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogScrl, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActivePlain, 1536 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveLabel, 1536 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveData, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveHint, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveScrl, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFramePlain, 3840 );
    attrset->setAttr( NCstyle::DialogFrameLabel, 3840 );
    attrset->setAttr( NCstyle::DialogFrameData, 3840 );
    attrset->setAttr( NCstyle::DialogFrameHint, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFrameScrl, 3584 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFramePlain, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameLabel, 1536 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameData, 3840 );
    attrset->setAttr( NCstyle::DialogActiveFrameHint, 0 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameScrl, 3584 | A_BOLD );
    attrset->setAttr( NCstyle::ListTitle, 3840 );
    attrset->setAttr( NCstyle::ListPlain, 3840 );
    attrset->setAttr( NCstyle::ListLabel, 3840 );
    attrset->setAttr( NCstyle::ListData, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::ListHint, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelPlain, 3840 );
    attrset->setAttr( NCstyle::ListSelLabel, 3840 );
    attrset->setAttr( NCstyle::ListSelData, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelHint, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveTitle, 3840 );
    attrset->setAttr( NCstyle::ListActivePlain, 3840 );
    attrset->setAttr( NCstyle::ListActiveLabel, 3840 );
    attrset->setAttr( NCstyle::ListActiveData, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveHint, 2048 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelPlain, 1536 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelLabel, 1536 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelData, 0 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelHint, 0 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextPlain, 16128 );
    attrset->setAttr( NCstyle::RichTextTitle, 15360 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextLink, 15104 );
    attrset->setAttr( NCstyle::RichTextArmedlink, 15104 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextActiveArmedlink, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextVisitedLink, 59 );
    attrset->setAttr( NCstyle::RichTextB, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextI, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextT, 14592 );
    attrset->setAttr( NCstyle::RichTextBI, 14336 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextBT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextIT, 14592 );
    attrset->setAttr( NCstyle::RichTextBIT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::ProgbarCh, 0x1720 );
    attrset->setAttr( NCstyle::ProgbarBgch, 16160 );
    attrset->setAttr( NCstyle::TextCursor, 7936 | A_BLINK );
    // PopupStyle
    attrset = &styleSet[NCstyle::PopupStyle].getAttrLocal();
    attrset->setAttr( NCstyle::DialogBorder, 14080 );
    attrset->setAttr( NCstyle::DialogTitle, 14080 );
    attrset->setAttr( NCstyle::DialogActiveBorder, 12288 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveTitle, 12288 | A_BOLD );
    attrset->setAttr( NCstyle::DialogText, 14080 );
    attrset->setAttr( NCstyle::DialogHeadline, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogDisabled, 14080 | A_BOLD );
    attrset->setAttr( NCstyle::DialogPlain, 14080 );
    attrset->setAttr( NCstyle::DialogLabel, 14080 );
    attrset->setAttr( NCstyle::DialogData, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogScrl, 1024 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActivePlain, 8192 );
    attrset->setAttr( NCstyle::DialogActiveLabel, 8192 );
    attrset->setAttr( NCstyle::DialogActiveData, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveScrl, 1024 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFramePlain, 14080 );
    attrset->setAttr( NCstyle::DialogFrameLabel, 14080 );
    attrset->setAttr( NCstyle::DialogFrameData, 13056 );
    attrset->setAttr( NCstyle::DialogFrameHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::DialogFrameScrl, 14080 );
    attrset->setAttr( NCstyle::DialogActiveFramePlain, 12288 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameLabel, 8192 );
    attrset->setAttr( NCstyle::DialogActiveFrameData, 14080 );
    attrset->setAttr( NCstyle::DialogActiveFrameHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::DialogActiveFrameScrl, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListTitle, 14080 );
    attrset->setAttr( NCstyle::ListPlain, 14080 );
    attrset->setAttr( NCstyle::ListLabel, 14080 );
    attrset->setAttr( NCstyle::ListData, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelPlain, 14080 );
    attrset->setAttr( NCstyle::ListSelLabel, 14080 );
    attrset->setAttr( NCstyle::ListSelData, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListSelHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveTitle, 14080 );
    attrset->setAttr( NCstyle::ListActivePlain, 14080 );
    attrset->setAttr( NCstyle::ListActiveLabel, 14080 );
    attrset->setAttr( NCstyle::ListActiveData, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveHint, 13312 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelPlain, 8192 );
    attrset->setAttr( NCstyle::ListActiveSelLabel, 8192 );
    attrset->setAttr( NCstyle::ListActiveSelData, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::ListActiveSelHint, 9216 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextPlain, 16128 );
    attrset->setAttr( NCstyle::RichTextTitle, 15360 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextLink, 15104 );
    attrset->setAttr( NCstyle::RichTextArmedlink, 15104 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextActiveArmedlink, 8192 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextVisitedLink, 59 );
    attrset->setAttr( NCstyle::RichTextB, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextI, 16128 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextT, 14592 );
    attrset->setAttr( NCstyle::RichTextBI, 14336 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextBT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::RichTextIT, 14592 );
    attrset->setAttr( NCstyle::RichTextBIT, 14592 | A_BOLD );
    attrset->setAttr( NCstyle::ProgbarCh, 0x1720 );
    attrset->setAttr( NCstyle::ProgbarBgch, 16160 );
    attrset->setAttr( NCstyle::TextCursor, 7936 | A_BLINK );
}

#endif // NCstyle_xterm_h

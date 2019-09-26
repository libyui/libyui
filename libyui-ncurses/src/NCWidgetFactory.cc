/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2019 SUSE LLC
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

  File:		NCWidgetFactory.cc

  Authors:	Stefan Hundhammer <sh@suse.de>
		Gabriele Mohr <gs@suse.de>

/-*/

#include "NCWidgetFactory.h"
#include <yui/YUIException.h>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "YNCursesUI.h"

#include <string>


NCWidgetFactory::NCWidgetFactory()
    : YWidgetFactory()
{
    // NOP
}


NCWidgetFactory::~NCWidgetFactory()
{
    // NOP
}




//
// Dialogs
//

NCDialog *
NCWidgetFactory::createDialog( YDialogType dialogType, YDialogColorMode colorMode )
{
    yuiDebug() << "Flush input buffer - new dialog" << std::endl;
    ::flushinp();

    NCDialog * dialog = new NCDialog( dialogType, colorMode );
    YUI_CHECK_NEW( dialog );

    return dialog;
}


//
// Common Leaf Widgets
//

NCPushButton *
NCWidgetFactory::createPushButton( YWidget * parent, const std::string & label )
{
    NCPushButton * pushButton = new NCPushButton( parent, label );
    YUI_CHECK_NEW( pushButton );

    return pushButton;
}



NCLabel *
NCWidgetFactory::createLabel( YWidget *		parent,
			      const std::string &	text,
			      bool		isHeading,
			      bool		isOutputField )
{
    NCLabel * label = new NCLabel( parent, text, isHeading, isOutputField );
    YUI_CHECK_NEW( label );

    return label;
}


NCInputField *
NCWidgetFactory::createInputField( YWidget * parent, const std::string & label, bool passwordMode )
{
    NCInputField * inputField = new NCInputField( parent, label, passwordMode );
    YUI_CHECK_NEW( inputField );

    return inputField;
}


NCCheckBox *
NCWidgetFactory::createCheckBox( YWidget * parent, const std::string & label, bool isChecked )
{
    NCCheckBox * checkBox = new NCCheckBox( parent, label, isChecked );
    YUI_CHECK_NEW( checkBox );

    return checkBox;
}


NCRadioButton *
NCWidgetFactory::createRadioButton( YWidget * parent, const std::string & label, bool checked )
{
    NCRadioButton * radioButton = new NCRadioButton( parent, label, checked );
    YUI_CHECK_NEW( radioButton );

    // Register radio button with its button group.
    // This has to be done after all constructors are done so virtual functions
    // can be used.

    if ( radioButton->buttonGroup() )
	radioButton->buttonGroup()->addRadioButton( radioButton );

    return radioButton;
}


NCComboBox *
NCWidgetFactory::createComboBox( YWidget * parent, const std::string & label, bool editable	)
{
    NCComboBox * comboBox = new NCComboBox( parent, label, editable );
    YUI_CHECK_NEW( comboBox );

    return comboBox;
}


NCSelectionBox *
NCWidgetFactory::createSelectionBox( YWidget * parent, const std::string & label )
{
    NCSelectionBox * selectionBox = new NCSelectionBox( parent, label );
    YUI_CHECK_NEW( selectionBox );

    return selectionBox;
}


NCTree *
NCWidgetFactory::createTree( YWidget * parent, const std::string & label, bool multiselection, bool recursiveselection )
{
    NCTree * tree = new NCTree( parent, label, multiselection, recursiveselection );
    YUI_CHECK_NEW( tree );

    return tree;
}


NCTable *
NCWidgetFactory::createTable( YWidget * parent, YTableHeader * tableHeader, bool multiSelection )
{
    NCTable *table = new NCTable( parent, tableHeader, multiSelection );
    YUI_CHECK_NEW( table );

    return table;
}


NCProgressBar *
NCWidgetFactory::createProgressBar( YWidget * parent, const std::string & label, int maxValue )
{
    NCProgressBar * progressBar = new NCProgressBar( parent, label, maxValue );
    YUI_CHECK_NEW( progressBar );

    return progressBar;
}


NCBusyIndicator *
NCWidgetFactory::createBusyIndicator( YWidget * parent, const std::string & label, int timeout)
{
   NCBusyIndicator * busyIndicator = new NCBusyIndicator( parent, label, timeout );
   YUI_CHECK_NEW( busyIndicator );

   return busyIndicator;
}


NCRichText *
NCWidgetFactory::createRichText( YWidget * parent, const std::string & text, bool plainTextMode )
{
    NCRichText * richText = new NCRichText( parent, text, plainTextMode );
    YUI_CHECK_NEW( richText );

    return richText;
}


//
// Less Common Leaf Widgets
//

NCIntField *
NCWidgetFactory::createIntField( YWidget * parent, const std::string & label, int minVal, int maxVal, int initialVal )
{
    NCIntField * intField = new NCIntField( parent, label, minVal, maxVal, initialVal );
    YUI_CHECK_NEW( intField );

    return intField;
}


NCMenuButton *
NCWidgetFactory::createMenuButton( YWidget * parent, const std::string & label )
{
    NCMenuButton * menuButton = new NCMenuButton( parent, label );
    YUI_CHECK_NEW( menuButton );

    return menuButton;
}


NCMultiLineEdit *
NCWidgetFactory::createMultiLineEdit( YWidget * parent, const std::string & label )
{
    NCMultiLineEdit * multiLineEdit = new NCMultiLineEdit( parent, label );
    YUI_CHECK_NEW( multiLineEdit );

    return multiLineEdit;
}


NCLogView *
NCWidgetFactory::createLogView( YWidget * parent, const std::string & label, int visibleLines, int storedLines )
{
    NCLogView * logView = new NCLogView( parent, label, visibleLines, storedLines );
    YUI_CHECK_NEW( logView );

    return logView;
}


NCMultiSelectionBox *
NCWidgetFactory::createMultiSelectionBox( YWidget * parent, const std::string & label )
{
    NCMultiSelectionBox * multiSelectionBox = new NCMultiSelectionBox( parent, label );
    YUI_CHECK_NEW( multiSelectionBox );

    return multiSelectionBox;
}


NCItemSelector *
NCWidgetFactory::createItemSelector( YWidget * parent, bool enforceSingleSelection )
{
    NCItemSelector * itemSelector = new NCItemSelector( parent, enforceSingleSelection );
    YUI_CHECK_NEW( itemSelector );

    return itemSelector;
}


//
// Layout Helpers
//

NCSpacing *
NCWidgetFactory::createSpacing( YWidget * parent, YUIDimension dim, bool stretchable, YLayoutSize_t size )
{
    NCSpacing * spacing = new NCSpacing( parent, dim, stretchable, size );
    YUI_CHECK_NEW( spacing );

    return spacing;
}


NCLayoutBox *
NCWidgetFactory::createLayoutBox( YWidget * parent, YUIDimension dim )
{
    NCLayoutBox * layoutBox = new NCLayoutBox( parent, dim );
    YUI_CHECK_NEW( layoutBox );

    return layoutBox;
}


NCButtonBox *
NCWidgetFactory::createButtonBox( YWidget * parent )
{
    NCButtonBox * buttonBox = new NCButtonBox( parent );
    YUI_CHECK_NEW( buttonBox );

    return buttonBox;
}


NCEmpty *
NCWidgetFactory::createEmpty( YWidget * parent )
{
    NCEmpty * empty = new NCEmpty( parent );
    YUI_CHECK_NEW( empty );

    return empty;
}


NCAlignment *
NCWidgetFactory::createAlignment( YWidget *	 parent,
				  YAlignmentType horAlignment,
				  YAlignmentType vertAlignment )
{
    NCAlignment * alignment = new NCAlignment( parent, horAlignment, vertAlignment );
    YUI_CHECK_NEW( alignment );

    return alignment;
}


NCSquash *
NCWidgetFactory::createSquash( YWidget * parent, bool horSquash, bool vertSquash )
{
    NCSquash * squash = new NCSquash( parent, horSquash, vertSquash );
    YUI_CHECK_NEW( squash );

    return squash;
}


NCFrame *
NCWidgetFactory::createFrame( YWidget * parent, const std::string & label )
{
    NCFrame * frame = new NCFrame( parent, label );
    YUI_CHECK_NEW( frame );

    return frame;
}


NCCheckBoxFrame *
NCWidgetFactory::createCheckBoxFrame( YWidget *	parent, const std::string & label, bool checked )
{
    NCCheckBoxFrame * checkBoxFrame = new NCCheckBoxFrame( parent, label, checked );
    YUI_CHECK_NEW( checkBoxFrame );

    return checkBoxFrame;
}


NCRadioButtonGroup *
NCWidgetFactory::createRadioButtonGroup( YWidget * parent )
{
    NCRadioButtonGroup * radioButtonGroup = new NCRadioButtonGroup( parent );
    YUI_CHECK_NEW( radioButtonGroup );

    return radioButtonGroup;
}


NCReplacePoint *
NCWidgetFactory::createReplacePoint( YWidget * parent )
{
    NCReplacePoint * replacePoint = new NCReplacePoint( parent );
    YUI_CHECK_NEW( replacePoint );

    return replacePoint;
}


NCImage *
NCWidgetFactory::createImage( YWidget * parent, const std::string & imageFileName, bool animated )
{
    NCImage * image = new NCImage( parent, imageFileName, animated );
    YUI_CHECK_NEW( image );

    return image;
}


YPackageSelector *
NCWidgetFactory::createPackageSelector( YWidget * parent, long modeFlags )
{
    NCPackageSelectorPluginStub * plugin = YNCursesUI::ui()->packageSelectorPlugin();

    if ( plugin )
	return plugin->createPackageSelector( parent, modeFlags );
    else
	return 0;
}


// Creates special widgets used for the package selection dialog.
// This is special to the NCurses UI;  there is no  a corresponding widget
// in the Qt UI.
YWidget *
NCWidgetFactory::createPkgSpecial( YWidget * parent, const std::string & subwidget )
{
    YWidget * w = 0;

    NCPackageSelectorPluginStub * plugin = YNCursesUI::ui()->packageSelectorPlugin();

    if ( plugin )
    {
	w = plugin->createPkgSpecial( parent, subwidget );
    }

    return w;
}



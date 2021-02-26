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

  File:         NCWidgetFactory.h

  Author:       Gabriele Mohr <gs@suse.de>

/-*/

#ifndef NCWidgetFactory_h
#define NCWidgetFactory_h


#include <yui/YWidgetFactory.h>

#include "NCAlignment.h"
#include "NCBusyIndicator.h"
#include "NCButtonBox.h"
#include "NCCheckBox.h"
#include "NCCheckBoxFrame.h"
#include "NCComboBox.h"
#include "NCDialog.h"
#include "NCEmpty.h"
#include "NCFrame.h"
#include "NCImage.h"
#include "NCInputField.h"
#include "NCIntField.h"
#include "NCLabel.h"
#include "NCLogView.h"
#include "NCMenuButton.h"
#include "NCMultiLineEdit.h"
#include "NCMultiSelectionBox.h"
#include "NCPackageSelectorPluginStub.h"
#include "NCProgressBar.h"
#include "NCPushButton.h"
#include "NCRadioButton.h"
#include "NCRadioButtonGroup.h"
#include "NCReplacePoint.h"
#include "NCRichText.h"
#include "NCSelectionBox.h"
#include "NCSpacing.h"
#include "NCSquash.h"
#include "NCTable.h"
#include "NCTree.h"
#include "NCLayoutBox.h"


/**
 * Concrete widget factory for mandatory widgets.
 **/
class NCWidgetFactory: public YWidgetFactory
{
public:
    // Note: Using covariant return types for all createSomeWidget() methods
    // (returning NCSomeWidget where the base class declares virtual methods that
    // return YSomeWidget)


    //
    // Dialogs
    //

    virtual NCDialog *          createDialog            ( YDialogType dialogType, YDialogColorMode colorMode = YDialogNormalColor );

    //
    // Layout Boxes
    //

    virtual NCLayoutBox *       createLayoutBox         ( YWidget * parent, YUIDimension dim );
    virtual NCButtonBox * 	createButtonBox		( YWidget * parent );


    //
    // Common Leaf Widgets
    //

    virtual NCPushButton *      createPushButton        ( YWidget * parent, const std::string & label );
    virtual NCLabel *           createLabel             ( YWidget * parent, const std::string & text,  bool isHeading    = false, bool isOutputField = false );
    virtual NCInputField *      createInputField        ( YWidget * parent, const std::string & label, bool passwordMode = false );
    virtual NCCheckBox *        createCheckBox          ( YWidget * parent, const std::string & label, bool isChecked    = false );
    virtual NCRadioButton *     createRadioButton       ( YWidget * parent, const std::string & label, bool isChecked    = false );
    virtual NCComboBox *        createComboBox          ( YWidget * parent, const std::string & label, bool editable     = false );
    virtual NCSelectionBox *    createSelectionBox      ( YWidget * parent, const std::string & label );
    virtual NCTree *            createTree              ( YWidget * parent, const std::string & label, bool multiselection = false, bool recursiveselection = false );
    virtual NCTable *           createTable             ( YWidget * parent, YTableHeader * tableHeader, bool multiSelection = false );
    virtual NCProgressBar *     createProgressBar       ( YWidget * parent, const std::string & label, int maxValue = 100 );
    virtual NCRichText *        createRichText          ( YWidget * parent, const std::string & text = std::string(), bool plainTextMode = false );


    //
    // Less Common Leaf Widgets
    //

    virtual NCIntField *        createIntField          ( YWidget * parent, const std::string & label, int minVal, int maxVal, int initialVal );
    virtual NCMenuButton *      createMenuButton        ( YWidget * parent, const std::string & label );
    virtual NCMultiLineEdit *   createMultiLineEdit     ( YWidget * parent, const std::string & label );
    virtual NCImage *           createImage             ( YWidget * parent, const std::string & imagePath, bool animated = false );
    virtual NCLogView *         createLogView           ( YWidget * parent, const std::string & label, int visibleLines, int storedLines = 0 );
    virtual NCMultiSelectionBox*createMultiSelectionBox ( YWidget * parent, const std::string & label );
    virtual YPackageSelector *  createPackageSelector   ( YWidget * parent, long ModeFlags = 0 );
    virtual NCBusyIndicator *   createBusyIndicator     ( YWidget * parent, const std::string & label, int timeout = 1000 );

    // NCurses only
    virtual YWidget *           createPkgSpecial        ( YWidget * parent,  const std::string & subwidgetName );


    //
    // Layout Helpers
    //

    virtual NCSpacing *         createSpacing           ( YWidget * parent, YUIDimension dim, bool stretchable = false, YLayoutSize_t size = 0.0 );
    virtual NCEmpty *           createEmpty             ( YWidget * parent );
    virtual NCAlignment *       createAlignment         ( YWidget * parent, YAlignmentType horAlignment, YAlignmentType vertAlignment );
    virtual NCSquash *          createSquash            ( YWidget * parent, bool horSquash, bool vertSquash );


    //
    // Visual Grouping
    //

    virtual NCFrame *           createFrame             ( YWidget * parent, const std::string & label );
    virtual NCCheckBoxFrame *   createCheckBoxFrame     ( YWidget * parent, const std::string & label, bool checked );


    //
    // Logical Grouping
    //

    virtual NCRadioButtonGroup *createRadioButtonGroup  ( YWidget * parent );
    virtual NCReplacePoint *    createReplacePoint      ( YWidget * parent );


protected:

    friend class YNCursesUI;

    /**
     * Constructor.
     *
     * Use YUI::widgetFactory() to get the singleton for this class.
     **/
    NCWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~NCWidgetFactory();

}; // class NCWidgetFactory


#endif // NCWidgetFactory_h

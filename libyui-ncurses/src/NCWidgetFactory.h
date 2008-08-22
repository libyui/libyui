/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:         NCWidgetFactory.h

  Author:       Gabriele Mohr <gs@suse.de>

/-*/

#ifndef NCWidgetFactory_h
#define NCWidgetFactory_h


#include "YWidgetFactory.h"

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

using std::string;



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

    virtual NCPushButton *      createPushButton        ( YWidget * parent, const string & label );
    virtual NCLabel *           createLabel             ( YWidget * parent, const string & text,  bool isHeading    = false, bool isOutputField = false );
    virtual NCInputField *      createInputField        ( YWidget * parent, const string & label, bool passwordMode = false );
    virtual NCCheckBox *        createCheckBox          ( YWidget * parent, const string & label, bool isChecked    = false );
    virtual NCRadioButton *     createRadioButton       ( YWidget * parent, const string & label, bool isChecked    = false );
    virtual NCComboBox *        createComboBox          ( YWidget * parent, const string & label, bool editable     = false );
    virtual NCSelectionBox *    createSelectionBox      ( YWidget * parent, const string & label );
    virtual NCTree *            createTree              ( YWidget * parent, const string & label );
    virtual NCTable *           createTable             ( YWidget * parent, YTableHeader * tableHeader, bool multiSelection = false );
    virtual NCProgressBar *     createProgressBar       ( YWidget * parent, const string & label, int maxValue = 100 );
    virtual NCRichText *        createRichText          ( YWidget * parent, const string & text = string(), bool plainTextMode = false );

    
    //
    // Less Common Leaf Widgets
    //
    
    virtual NCIntField *        createIntField          ( YWidget * parent, const string & label, int minVal, int maxVal, int initialVal );
    virtual NCMenuButton *      createMenuButton        ( YWidget * parent, const string & label );
    virtual NCMultiLineEdit *   createMultiLineEdit     ( YWidget * parent, const string & label );
    virtual NCImage *           createImage             ( YWidget * parent, const string & imagePath, bool animated = false );
    virtual NCLogView *         createLogView           ( YWidget * parent, const string & label, int visibleLines, int storedLines = 0 );
    virtual NCMultiSelectionBox*createMultiSelectionBox ( YWidget * parent, const string & label );
    virtual YPackageSelector *  createPackageSelector   ( YWidget * parent, long ModeFlags = 0 );
    virtual NCBusyIndicator *   createBusyIndicator     ( YWidget * parent, const string & label, int timeout = 1000 );

    // NCurses only
    virtual YWidget *           createPkgSpecial        ( YWidget * parent,  const string & subwidgetName );

    
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

    virtual NCFrame *           createFrame             ( YWidget * parent, const string & label );
    virtual NCCheckBoxFrame *   createCheckBoxFrame     ( YWidget * parent, const string & label, bool checked );

    
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

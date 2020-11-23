%module yui

#if defined(SWIGPERL5)
%{
   #undef NORMAL
   #undef readdir
   #undef Fflush
   #undef Mkdir
   #undef strerror
%}
#endif

%{
/* Includes the header in the wrapper code */
#include <string>
#include <sstream>
#include <stdexcept>

#if defined(SWIGRUBY)

extern "C" {
/* Init_ for the %module, defined by Swig */
SWIGEXPORT void Init_yui(void);

/* Init_ for the .so lib, called by Ruby */
SWIGEXPORT void Init__yui(void) {
  Init_yui();
  }
}
#endif

#define YUILogComponent "bindings"
#include "yui/YUILog.h"

#include "yui/YUI.h"
#include "yui/YWidgetFactory.h"
#include "yui/YExternalWidgets.h"
#include "yui/YExternalWidgetFactory.h"
#include "yui/YOptionalWidgetFactory.h"
#include "yui/YDialog.h"
#include "yui/YLayoutBox.h"
#include "yui/YEvent.h"

#include "yui/YSelectionBox.h"
#include "yui/YLabel.h"
#include "yui/YPushButton.h"
#include "yui/YAlignment.h"
#include "yui/YApplication.h"
#include "yui/YBarGraph.h"
#include "yui/YBuiltinCaller.h"
#include "yui/YBusyIndicator.h"
#include "yui/YCheckBox.h"
#include "yui/YCheckBoxFrame.h"
#include "yui/YComboBox.h"
#include "yui/YCommandLine.h"
#include "yui/YDateField.h"
#include "yui/YDownloadProgress.h"
#include "yui/YDumbTab.h"
#include "yui/YEmpty.h"
#include "yui/YFrame.h"
#include "yui/YImage.h"
#include "yui/YInputField.h"
#include "yui/YItem.h"
#include "yui/YItemSelector.h"
#include "yui/YLogView.h"
#include "yui/YMacro.h"
#include "yui/YMacroPlayer.h"
#include "yui/YMacroRecorder.h"
#include "yui/YMenuBar.h"
#include "yui/YMenuButton.h"
#include "yui/YMenuItem.h"
#include "yui/YMenuWidget.h"
#include "yui/YMultiLineEdit.h"
#include "yui/YMultiProgressMeter.h"
#include "yui/YMultiSelectionBox.h"
#include "yui/YPackageSelector.h"
#include "yui/YPackageSelectorPlugin.h"
#include "yui/YProgressBar.h"
#include "yui/YPartitionSplitter.h"
#include "yui/YRadioButtonGroup.h"
#include "yui/YRadioButton.h"
#include "yui/YReplacePoint.h"
#include "yui/YRichText.h"
#include "yui/YRpmGroupsTree.h"
#include "yui/YShortcut.h"
#include "yui/YShortcutManager.h"
#include "yui/YSimpleInputField.h"
#include "yui/YSlider.h"
#include "yui/YSpacing.h"
#include "yui/YStringTree.h"
#include "yui/YSquash.h"
#include "yui/YTable.h"
#include "yui/YTableItem.h"
#include "yui/YTableHeader.h"
#include "yui/YTimezoneSelector.h"
#include "yui/YTimeField.h"
#include "yui/YTransText.h"
#include "yui/YTree.h"
#include "yui/YTreeItem.h"
#include "yui/YWidgetID.h"
#include "yui/YWizard.h"

#include "yui/YUILoader.h"

#if defined(WITH_MGA)
#include "yui/mga/YMGA_CBTable.h"
#include "yui/mga/YMGAMsgBox.h"
#include "yui/mga/YMGAAboutDialog.h"
#include "yui/mga/YMGAWidgetExtensionFactory.h"
#endif

using namespace std;

extern void *start_ui_thread( void *ui_int );

#if 0
template < typename T >
class intrusive_ptr {
  public:
  T *operator->();
};
#endif

%}

%define DEFINE_PTR_TYPE(name)
%enddef

#if !defined(SWIGCSHARP)
%rename("+") "operator+";
%rename("<<") "operator<<";
%rename("!=") "operator!=";
%rename("!") "operator!";
%rename("==") "operator==";

%include "exception.i"
%include "std_string.i"
%include "std_list.i"
%include "std_vector.i"

#endif

#if defined(SWIGPERL5) || defined(SWIGCSHARP)
/* %include "std/std_set.i" # doesn't compile ?! */
#else
%include "std_set.i"
#endif

%include "stl.i"

class Exception;
%include yui/YUI.h

%define YUILogComponent "bindings"
%enddef

#if defined(SWIGPYTHON)
%ignore None; /* is a reserved word in Python */
#endif

/*
 * See https://bugzilla.novell.com/show_bug.cgi?id=427372#c16
 *
 * YMenuButton.h:    virtual void addItem( YItem * item_disown );
 * YSelectionWidget.h:    virtual void addItem( YItem * item_disown );
 * YSimpleEventHandler.h:    void sendEvent( YEvent * event_disown );
 * YTableItem.h:    void addCell( YTableCell * cell_disown );
 * YWidget.h:     void setId( YWidgetID * newId_disown );
 * YWidgetFactory.h:    virtual YTable *createTable( YWidget *parent, YTableHeader * header_disown, bool multiSelection = false  ):
     virtual YReplacePoint *createReplacePoint( YWidget *parent_disown )     = 0;

 *
 */

%apply SWIGTYPE *DISOWN { YItem *item_disown };
%apply SWIGTYPE *DISOWN { YEvent *event_disown };
%apply SWIGTYPE *DISOWN { YTableCell *cell_disown };
%apply SWIGTYPE *DISOWN { YWidgetID *newId_disown };
%apply SWIGTYPE *DISOWN { YTableHeader *header_disown };
%apply SWIGTYPE *DISOWN { YTableHeader *header_disown };
%apply SWIGTYPE *DISOWN { YWidget *parent_disown };

%include yui/YUILog.h
%include yui/YUIPlugin.h

%include yui/YTypes.h
%include yui/YWidget.h
%include yui/YSingleChildContainerWidget.h
%include yui/YSelectionWidget.h
%include yui/YSimpleInputField.h
%include yui/YItem.h
%include yui/YTreeItem.h
%include yui/YStringTree.h

%include yui/YWidgetFactory.h

%include yui/YDialog.h

%include yui/YAlignment.h
%include yui/YApplication.h
%include yui/YBarGraph.h
%include yui/YBothDim.h
%include yui/YBuiltinCaller.h
%include yui/YBusyIndicator.h
%include yui/YCheckBoxFrame.h
%include yui/YCheckBox.h
%include yui/YChildrenManager.h
%include yui/YColor.h
%include yui/YComboBox.h
%include yui/YCommandLine.h
%include yui/YDateField.h
%include yui/YDownloadProgress.h
%include yui/YDumbTab.h
%include yui/YEmpty.h
%include yui/YEvent.h
%include yui/YFrame.h
%include yui/YImage.h
%include yui/YInputField.h
%include yui/YIntField.h
%include yui/YItemSelector.h
%include yui/YLabel.h
%include yui/YLayoutBox.h
%include yui/YLogView.h
%include yui/YMacro.h
%include yui/YMacroPlayer.h
%include yui/YMacroRecorder.h
%include yui/YMenuBar.h
%include yui/YMenuButton.h
%include yui/YMenuItem.h
%include yui/YMenuWidget.h
%include yui/YMultiLineEdit.h
%include yui/YMultiProgressMeter.h
%include yui/YMultiSelectionBox.h
%include yui/YWizard.h
%include yui/YOptionalWidgetFactory.h
%include yui/YPackageSelector.h
%include yui/YPackageSelectorPlugin.h
%include yui/YPartitionSplitter.h
%include yui/YProgressBar.h
%include yui/YProperty.h
%include yui/YPushButton.h
%include yui/YRadioButtonGroup.h
%include yui/YRadioButton.h
%include yui/YReplacePoint.h
%include yui/YRichText.h
%include yui/YRpmGroupsTree.h
%include yui/YSelectionBox.h
%include yui/YSettings.h
%include yui/YShortcut.h
%include yui/YShortcutManager.h
%include yui/YSimpleEventHandler.h
%include yui/YSlider.h
%include yui/YSpacing.h
%include yui/YSquash.h
%include yui/YTable.h
%include yui/YTableHeader.h
%include yui/YTableItem.h
%include yui/YTimeField.h
%include yui/YTimezoneSelector.h
%include yui/YTransText.h
%include yui/YTree.h
%include yui/YUIException.h
%include yui/YUI.h
%include yui/YUILoader.h
%include yui/YUILog.h
%include yui/YUISymbols.h
%include yui/YWidgetID.h
%include yui/YExternalWidgetFactory.h
%include yui/YExternalWidgets.h

#if defined(WITH_MGA)
%include yui/mga/YMGA_CBTable.h
%include yui/mga/YMGAAboutDialog.h
%include yui/mga/YMGAMsgBox.h
%include yui/mga/YMGAWidgetExtensionFactory.h
#endif

#if defined(SWIGRUBY)
%extend YEvent {
  VALUE mywidget() { return INT2FIX( $self->widget() ); }
}
#endif

%extend YItem {
#if defined(SWIGPERL5)
  int __eq__( YItem *i )
  { return ($self == i); }
  int __ne__( YItem *i )
  { return ($self != i); }
#endif
#if defined(SWIGPYTHON)
  int __eq__( YItem *i )
  { return ($self == i); }
  int __ne__( YItem *i )
  { return ($self != i); }
#endif
#if defined(SWIGRUBY)
  %rename( "==" ) equals( YItem *i );
  %typemap(out) int equals
    "$result = ($1 != 0) ? Qtrue : Qfalse;";	
#endif
  int equals( YItem *i ) { return ($self == i); }
}

namespace std {
    %template(YItemCollection) vector<YItem *>;
}

using namespace std;
typedef std::vector<YItem *> YItemCollection;

%extend YWidget {
#if defined(SWIGPERL5)
  int __eq__( YWidget *w )
  { return ($self == w); }
  int __ne__( YWidget *w )
  { return ($self != w); }
#endif
#if defined(SWIGPYTHON)
  int __eq__( YWidget *w )
  { return ($self == w); }
  int __ne__( YWidget *w )
  { return ($self != w); }
#endif
#if defined(SWIGRUBY)
  %rename( "==" ) equals( YWidget *w );
  %typemap(out) int equals
    "$result = ($1 != 0) ? Qtrue : Qfalse;";	
#endif
  int equals( YWidget *w ) { return ($self == w); }
}

%inline %{

/* C++-style cast */
YWidgetEvent*  toYWidgetEvent(YEvent *event) {
  return dynamic_cast<YWidgetEvent*>(event);
}

YKeyEvent*     toYKeyEvent(YEvent *event) {
  return dynamic_cast<YKeyEvent*>(event);
}

YMenuEvent*    toYMenuEvent(YEvent *event) {
  return dynamic_cast<YMenuEvent*>(event);
}

YCancelEvent*  toYCancelEvent(YEvent *event) {
  return dynamic_cast<YCancelEvent*>(event);
}

YDebugEvent*   toYDebugEvent(YEvent *event) {
  return dynamic_cast<YDebugEvent*>(event);
}
YTimeoutEvent* toYTimeoutEvent(YEvent *event) {
  return dynamic_cast<YTimeoutEvent*>(event);
}

YTreeItem* toYTreeItem(YItem *item) {
  return dynamic_cast<YTreeItem*>(item);
}

YTableItem* toYTableItem(YItem *item) {
  return dynamic_cast<YTableItem*>(item);
}

YItem* toYItem(YItemIterator iter) {
  return *iter;
}

YTableCell* toYTableCell(YTableCellIterator iter) {
  return *iter;
}

/* C++ Iterators*/
YItemIterator incrYItemIterator( YItemIterator currentIterator) {
  return ++currentIterator;
}

YItemIterator beginYItemCollection( YItemCollection *coll) {
  return coll->begin();
}

YItemIterator endYItemCollection( YItemCollection *coll) {
  return coll->end();
}

YTableCellIterator incrYTableCellIterator(YTableCellIterator currentIterator) {
  return ++currentIterator;
}
  

%}


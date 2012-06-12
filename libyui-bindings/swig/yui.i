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
#include "yui/YLogView.h"
#include "yui/YMacro.h"
#include "yui/YMacroPlayer.h"
#include "yui/YMacroRecorder.h"
#include "yui/YMenuButton.h"
#include "yui/YMenuItem.h"
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

%rename("+") "operator+";
%rename("<<") "operator<<";
%rename("!=") "operator!=";
%rename("!") "operator!";
%rename("==") "operator==";

%include "exception.i"
%include "std_string.i"
%include "std_list.i"

#if defined(SWIGPERL5)
/* %include "std/std_set.i" # doesn't compile ?! */
#else
%include "std_set.i"
#endif

%include "stl.i"

class Exception;
%include YUI.h

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

%include YUILog.h
%include YUIPlugin.h

%include YTypes.h
%include YWidget.h
%include YSingleChildContainerWidget.h
%include YSelectionWidget.h
%include YSimpleInputField.h
%include YItem.h
%include YTreeItem.h
%include YStringTree.h

%include YWidgetFactory.h

%include YDialog.h

%include YAlignment.h
%include YApplication.h
%include YBarGraph.h
%include YBothDim.h
%include YBuiltinCaller.h
%include YBusyIndicator.h
%include YCheckBoxFrame.h
%include YCheckBox.h
%include YChildrenManager.h
%include YColor.h
%include YComboBox.h
%include YCommandLine.h
%include YDateField.h
%include YDownloadProgress.h
%include YDumbTab.h
%include YEmpty.h
%include YEvent.h
%include YFrame.h
%include YImage.h
%include YInputField.h
%include YIntField.h
%include YLabel.h
%include YLayoutBox.h
%include YLogView.h
%include YMacro.h
%include YMacroPlayer.h
%include YMacroRecorder.h
%include YMenuButton.h
%include YMenuItem.h
%include YMultiLineEdit.h
%include YMultiProgressMeter.h
%include YMultiSelectionBox.h
%include YOptionalWidgetFactory.h
%include YPackageSelector.h
%include YPackageSelectorPlugin.h
%include YPartitionSplitter.h
%include YProgressBar.h
%include YProperty.h
%include YPushButton.h
%include YRadioButtonGroup.h
%include YRadioButton.h
%include YReplacePoint.h
%include YRichText.h
%include YRpmGroupsTree.h
%include YSelectionBox.h
%include YShortcut.h
%include YShortcutManager.h
%include YSimpleEventHandler.h
%include YSlider.h
%include YSpacing.h
%include YSquash.h
%include YTable.h
%include YTableHeader.h
%include YTableItem.h
%include YTimeField.h
%include YTimezoneSelector.h
%include YTransText.h
%include YTree.h
%include YUIException.h
%include YUI.h
%include YUILoader.h
%include YUILog.h
%include YUISymbols.h
%include YWidgetID.h
%include YWizard.h

#if defined(SWIGRUBY)
%extend YEvent {
  VALUE mywidget() { return INT2FIX( $self->widget() ); }
}
#endif

%extend YWidget {
#if defined(SWIGPERL5)
  int __eq__( YWidget *w )
  { return ($self == w); }
  int __ne__( YWidget *w )
  { return ($self != w); }
#endif
#if defined(SWIGPYTHON)
  int __cmp__( YWidget *w )
  { return ($self - w); }
#endif
#if defined(SWIGRUBY)
  %rename( "==" ) equals( YWidget *w );
  %typemap(out) int equals
    "$result = ($1 != 0) ? Qtrue : Qfalse;";	
#endif
  int equals( YWidget *w ) { return ($self == w); }
}

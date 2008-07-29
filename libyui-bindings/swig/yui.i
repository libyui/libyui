%module yui

#ifdef SWIGPERL5
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

#define YUILogComponent "bindings"
#include "YaST2/yui/YUILog.h"

#include "YaST2/yui/YUI.h"
#include "YaST2/yui/YWidgetFactory.h"
#include "YaST2/yui/YDialog.h"
#include "YaST2/yui/YLayoutBox.h"
#include "YaST2/yui/YEvent.h"

#include "YaST2/yui/YSelectionBox.h"
#include "YaST2/yui/YLabel.h"
#include "YaST2/yui/YPushButton.h"
#include "YaST2/yui/YAlignment.h"
#include "YaST2/yui/YApplication.h"
#include "YaST2/yui/YBarGraph.h"
#include "YaST2/yui/YBuiltinCaller.h"
#include "YaST2/yui/YBusyIndicator.h"
#include "YaST2/yui/YCheckBox.h"
#include "YaST2/yui/YCheckBoxFrame.h"
#include "YaST2/yui/YComboBox.h"
#include "YaST2/yui/YCommandLine.h"
#include "YaST2/yui/YDateField.h"
#include "YaST2/yui/YDownloadProgress.h"
#include "YaST2/yui/YDumbTab.h"
#include "YaST2/yui/YEmpty.h"
#include "YaST2/yui/YFrame.h"
#include "YaST2/yui/YImage.h"
#include "YaST2/yui/YInputField.h"
#include "YaST2/yui/YLogView.h"
#include "YaST2/yui/YMacro.h"
#include "YaST2/yui/YMacroPlayer.h"
#include "YaST2/yui/YMacroRecorder.h"
#include "YaST2/yui/YMenuButton.h"
#include "YaST2/yui/YMenuItem.h"
#include "YaST2/yui/YMultiLineEdit.h"
#include "YaST2/yui/YMultiProgressMeter.h"
#include "YaST2/yui/YMultiSelectionBox.h"
#include "YaST2/yui/YOptionalWidgetFactory.h"
#include "YaST2/yui/YPackageSelector.h"
#include "YaST2/yui/YPackageSelectorPlugin.h"
#include "YaST2/yui/YProgressBar.h"
#include "YaST2/yui/YPartitionSplitter.h"
#include "YaST2/yui/YRadioButtonGroup.h"
#include "YaST2/yui/YRadioButton.h"
#include "YaST2/yui/YReplacePoint.h"
#include "YaST2/yui/YRichText.h"
#include "YaST2/yui/YRpmGroupsTree.h"
#include "YaST2/yui/YShortcut.h"
#include "YaST2/yui/YShortcutManager.h"
#include "YaST2/yui/YSimpleInputField.h"
#include "YaST2/yui/YSlider.h"
#include "YaST2/yui/YSpacing.h"
#include "YaST2/yui/YStringTree.h"
#include "YaST2/yui/YSquash.h"
#include "YaST2/yui/YTable.h"
#include "YaST2/yui/YTableItem.h"
#include "YaST2/yui/YTableHeader.h"
#include "YaST2/yui/YTimezoneSelector.h"
#include "YaST2/yui/YTimeField.h"
#include "YaST2/yui/YTransText.h"
#include "YaST2/yui/YTree.h"
#include "YaST2/yui/YTreeItem.h"
#include "YaST2/yui/YWidgetID.h"
#include "YaST2/yui/YWizard.h"

#include "YaST2/yui/YUILoader.h"

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

#ifdef SWIGPERL5
/* %include "std/std_set.i" # doesn't compile ?! */
#else
%include "std_set.i"
#endif

%include "stl.i"

class Exception;
%include YUI.h

%define YUILogComponent "bindings"
%enddef

#ifdef SWIGPYTHON
%ignore None; /* is a reserved word in Python */
#endif

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

%extend YEvent {
#ifdef SWIGRUBY
  VALUE mywidget() { return INT2FIX( $self->widget() ); }
#endif
}

%extend YWidget {
#ifdef SWIGPERL
  int __eq__( YWidget *w )
  { return ($self == w); }
  int __ne__( YWidget *w )
  { return ($self != w); }
#endif
#ifdef SWIGPYTHON
  int __cmp__( YWidget *w )
  { return ($self - w); }
#endif
#ifdef SWIGRUBY
  %rename( "==" ) equals( YWidget *w );
  %typemap(out) int equals
    "$result = ($1 != 0) ? Qtrue : Qfalse;";	
#endif
  int equals( YWidget *w ) { return ($self == w); }
}

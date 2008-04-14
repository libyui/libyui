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

using namespace std;

void *
start_ui_thread( void *ui_int )
{
  return NULL;
}

%}

%define DEFINE_PTR_TYPE(name)
%enddef

%rename("+") "operator+";
%rename("<<") "operator<<";
%rename("!=") "operator!=";
%rename("!") "operator!";
%rename("==") "operator==";


template < typename T >
class intrusive_ptr {
  public:
  T *operator->();
};


%include "std_string.i"
%include "std_list.i"
%include "std_set.i"
%include "stl.i"

#ifdef SWIGPYTHON
%include "python/callbacks.i"
#endif

class Exception;
%include YUI.h
%ignore YUI::start_ui_thread;


%define YUILogComponent "bindings"
%enddef
%include YUILog.h

%include YTypes.h
%include YWidgetFactory.h
%include YWidget.h
%include YSingleChildContainerWidget.h
%include YDialog.h
%include YLayoutBox.h
%include YEvent.h
%include YSelectionWidget.h
%include YSelectionBox.h
%include YLabel.h
%include YPushButton.h
%include YAlignment.h
%include YItem.h

%extend YEvent {
  VALUE mywidget() { return INT2FIX( $self->widget() ); }
}

%extend YWidget {
#ifdef SWIGRUBY
  %rename( "==" ) equals( YWidget *w );
  %typemap(out) int equals
    "$result = ($1 != 0) ? Qtrue : Qfalse;";	
#endif
  int equals( YWidget *w ) { return ($self == w ); }
}
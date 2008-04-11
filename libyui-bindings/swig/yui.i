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
#include <sstream>
#include <stdexcept>
#include "YaST2/yui/YUI.h"
#include "YaST2/yui/YWidgetFactory.h"
#include "YaST2/yui/YDialog.h"
#include "YaST2/yui/YLayoutBox.h"
#include "YaST2/yui/YEvent.h"

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


# %include "YApplication.i"
# %include "YWidget.i"
# %include "YWidgetFactory.i"
# %include "YOptionalWidgetFactory.i"
# %include "YEvent.i"
# %include "YBuiltinCaller.i"
# %include "YDialog.i"
# %include "YMacroPlayer.i"
# %include "YMacroRecorder.i"

#ifdef SWIGPYTHON
%include "python/callbacks.i"
#endif

class Exception;
%include YUI.h
%ignore YUI::start_ui_thread;

%include YWidgetFactory.h
%include YWidget.h
%include YSingleChildContainerWidget.h
%include YDialog.h
%include YLayoutBox.h
%include YEvent.h

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQRadioButtonGroup.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQRadioButtonGroup_h
#define YQRadioButtonGroup_h

#include <qwidget.h>
#include "YRadioButtonGroup.h"

class QButtonGroup;

class YQRadioButtonGroup : public QWidget, public YRadioButtonGroup
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQRadioButtonGroup( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQRadioButtonGroup();

    /**
     * Add a RadioButton.
     *
     * Reimplemented from YRadioButtonGroup.
     **/
    virtual void addRadioButton( YRadioButton * button );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );


private slots:

    /**
     * Triggered when one of the radio buttons changed state.
     * This implements the radio box behaviour, i.e. unselects all but the last
     * selected radio button.
     **/
    void radioButtonClicked( bool newState );


private:

    /**
     * Used to prevent infinite recursion due to signal race conditions.
     **/
    bool _recursive;
};


#endif // YQRadioButtonGroup_h

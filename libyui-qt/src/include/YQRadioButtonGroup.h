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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQRadioButtonGroup_h
#define YQRadioButtonGroup_h

#include <qwidget.h>
#include "YRadioButtonGroup.h"

class YUIQt;
class QButtonGroup;

class YQRadioButtonGroup : public QWidget, public YRadioButtonGroup
{
    Q_OBJECT

public:
    /**
     * Creates a new YQRadioButtonGroup.
     */
    YQRadioButtonGroup(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt);

    /**
     * Cleans up
     */
    ~YQRadioButtonGroup();

    /**
     * Inherited from YRadioButtonGroup
     * Adds a radio button to the button group.
     * Calls YRadioButtonGroup::addRadioButton()
     * as well.
     */
    void addRadioButton(YRadioButton *button);

    /**
     * Inherited from YRadioButtonGroup
     * Removes a radio button from the button group.
     * Calls YRadioButtonGroup::removeRadioButton()
     * as well.
     */
    void removeRadioButton(YRadioButton *button);

    /**
     * Resize this widget
     */
    void setSize(long newwidth, long newheight);


private slots:
    /**
     * Triggered when one of the radio buttons changed state.
     * This implements the radio box behaviour, i.e. unselects all but the last
     * selected radio button.
     */
    void radioButtonClicked( bool newState );


private:

    /**
     * Prevent infinite recursion due to signal race conditions
     */
    int recursionCounter;
};


#endif // YQRadioButtonGroup_h

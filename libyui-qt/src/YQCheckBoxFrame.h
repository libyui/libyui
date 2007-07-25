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

  File:	      YQCheckBoxFrame.h

  Author:     Stefan Hundhammer <sh@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQCheckBoxFrame_h
#define YQCheckBoxFrame_h

#include <qgroupbox.h>

#include "YCheckBoxFrame.h"

class QWidget;
class QCheckBox;

class YQCheckBoxFrame : public QGroupBox, public YCheckBoxFrame
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQCheckBoxFrame( QWidget * 		parent,
		     const YWidgetOpt & opt,
		     const YCPString &	label,
		     bool 		initiallyChecked );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling( bool enabled );

    /**
     * Set the size.
     */
    void setSize ( long newWidth, long newHeight );

    /**
     * Change the CheckBoxFrame label.
     */
    void setLabel( const YCPString & newLabel );
    
    /**
     * Sets the checked-state of the checkbox
     */
    void setValue( bool checked );

    /**
     * Returns whether or not the checkbox is checked.
     */
    bool getValue();

    /**
     * Geometry management: Returns the widget's preferred size.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize ( YUIDimension dim );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    
private slots:

    /**
     * Triggered when the on/off status is changed
     */
    void stateChanged ( bool newState );


protected:

    /**
     * Callback function that reports to the ui specific
     * widget that a child has been added. The default implementation
     * does nothing.
     */
    void childAdded( YWidget * child );

    /**
     * Prevent standard QGroupBox behaviour to always enable or disable all
     * children according to its check box's status (can't be inverted or left
     * to the application).
     *
     * This is a nasty hack, but it prevents oversights from the Qt designers.
     * Their view on how that check box should affect the frame content appears
     * to be somewhat bit limiting.
     **/
    void preventQGroupBoxAutoEnablement();

    /**
     * Reimplemented from QGroupBox to prevent QGroupBox from disabling
     * children according to the check box status as the children are inserted.
     **/
    virtual void childEvent( QChildEvent * );


    //
    // Data members
    //

    QCheckBox * _checkBox;
};


#endif // YQCheckBoxFrame_h

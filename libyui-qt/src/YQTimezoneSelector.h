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

  File:		YQTimezoneSelector.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQTimezoneSelector_h
#define YQTimezoneSelector_h

#include <QFrame>
#include "YTimezoneSelector.h"

class YQTimezoneSelectorPrivate;

class YQTimezoneSelector : public QFrame, public YTimezoneSelector
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQTimezoneSelector( YWidget *	parent,
                        const string & pixmap,
			const map<string,string> & timezones );

    /**
     * Destructor.
     **/
    virtual ~YQTimezoneSelector();

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    virtual void paintEvent( QPaintEvent *event );

    virtual void mousePressEvent ( QMouseEvent * event );

    virtual bool event(QEvent *event);

    /**
     * subclasses have to implement this to return value
     */
    virtual std::string currentZone() const;

    /**
     * subclasses have to implement this to set value
     */
    virtual void setCurrentZone( const std::string &zone, bool zoom );

protected:
    YQTimezoneSelectorPrivate *d;

protected slots:
    void slotBlink();

};


#endif // YQTimeField_h

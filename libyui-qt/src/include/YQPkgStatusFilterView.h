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

  File:	      YQPkgStatusFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgStatusFilterView_h
#define YQPkgStatusFilterView_h

#include <qvbox.h>
#include <qregexp.h>
#include <zypp/Package.h>


class QComboBox;
class QCheckBox;
class QPushButton;


/**
 * @short Filter view for packages that made problems during update
 **/
class YQPkgStatusFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgStatusFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgStatusFilterView();

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * Check if pkg matches the filter criteria.
     **/
    bool check( zypp::Package::constPtr pkg );


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *	  filterStart()
     *	  filterMatch() for each pkg that matches the filter
     *	  filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( zypp::ui::Selectable::Ptr	selectable,
		      zypp::Package::constPtr	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected:

    /**
     * Add a check box
     **/
    QCheckBox * addStatusCheckBox( QWidget * 		parent,
				   const QString & 	label,
				   const QPixmap &	icon,
				   bool 		initiallyChecked );

    // Data members

    QCheckBox *		_showAutoDel;
    QCheckBox *		_showAutoInstall;
    QCheckBox *		_showAutoUpdate;
    QCheckBox *		_showDel;
    QCheckBox *		_showInstall;
    QCheckBox *		_showKeepInstalled;
    QCheckBox *		_showNoInst;
    QCheckBox *		_showTaboo;
    QCheckBox *		_showProtected;
    QCheckBox *		_showUpdate;

    QPushButton *	_refreshButton;
};



#endif // ifndef YQPkgStatusFilterView_h

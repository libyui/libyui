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

  File:	      YQPkgSearchFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgSearchFilterView_h
#define YQPkgSearchFilterView_h

#include <qvbox.h>
#include <qregexp.h>
#include <y2pm/PMPackage.h>


class QComboBox;
class QCheckBox;
class QRadioButton;


/**
 * @short Filter view for packages that made problems during update
 **/
class YQPkgSearchFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSearchFilterView( QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSearchFilterView();


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
    void filterMatch( PMPackagePtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();

protected:

    /**
     * Check if pkg matches the search criteria.
     **/
    bool check( PMPackagePtr pkg, const QRegExp & regexp );

    /**
     * Check if a single pkg attribute matches the search criteria.
     **/
    bool check( const std::string & attribute, const QRegExp & regexp );

    /**
     * Check multi-line attribute
     **/
    bool check( const std::list<std::string> & strList, const QRegExp & regexp );

    /**
     * Check PkgRelList attribute (PMSolvable::provides(), PMSolvable::requires(), ...)
     **/
    bool check( const PMSolvable::PkgRelList_type & relList, const QRegExp & regexp );
    
    /**
     * Add vertical stretchable space.
     **/
    void addVStretch( QWidget * parent );

    /**
     * Add horizontal stretchable space.
     **/
    void addHStretch( QWidget * parent );

    
    // Data members

    QComboBox *		_searchText;

    QCheckBox *		_searchInName;
    QCheckBox *		_searchInSummary;
    QCheckBox *		_searchInDescription;
    QCheckBox *		_searchInRequires;
    QCheckBox *		_searchInProvides;

    QCheckBox *		_caseSensitive;

    QRadioButton *	_contains;
    QRadioButton *	_beginsWith;
    QRadioButton *	_exactMatch;
    QRadioButton *	_useWildcards;
    QRadioButton *	_useRegexp;
};



#endif // ifndef YQPkgSearchFilterView_h

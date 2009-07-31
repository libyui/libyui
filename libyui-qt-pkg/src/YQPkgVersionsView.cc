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

  File:	      YQPkgVersionsView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"

#include <YQZypp.h>
#include <zypp/Repository.h>
#include "YUILog.h"
#include <QTabWidget>
#include <QRegExp>
#include <QHeaderView>

#include "YQPkgVersionsView.h"
#include "YQPkgRepoList.h"
#include "YQIconPool.h"
#include "YQi18n.h"
#include "utf8.h"



YQPkgVersionsView::YQPkgVersionsView( QWidget * parent, bool userCanSwitch )
    : QScrollArea( parent )
    , _content(0)
    , _layout(0)
{
    _selectable		= 0;
    _parentTab		= dynamic_cast<QTabWidget *> (parent);
    _userCanSwitch 	= userCanSwitch;

    _buttons = new QButtonGroup(this);

    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged(QWidget *) ),
		 this,   SLOT  ( reload        (QWidget *) ) );
    }
}


YQPkgVersionsView::~YQPkgVersionsView()
{
    // NOP
}


void
YQPkgVersionsView::reload( QWidget * newCurrent )
{
    if ( newCurrent == this )
	showDetailsIfVisible( _selectable );
}


void
YQPkgVersionsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentWidget() == this )  // Is this page the topmost?
	    showDetails( selectable );
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( selectable );
    }
}


void
YQPkgVersionsView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	// Delete all installed items
	qDeleteAll( _installed );
	_installed.clear();

	_content = new QWidget( this );
	setWidget( _content );
        _content->show();
        return;
    }

    // old widget is autodestroyed by setWidget later
    _content = new QWidget( this );
    _layout = new QVBoxLayout( _content );
    _content->setLayout( _layout );

    // also paint the scrollarea background
    setPalette( QPalette( Qt::white ) );
    setAutoFillBackground( true );

    _content->setPalette( QPalette( Qt::white ) );
    _content->setAutoFillBackground( true );
   
    QLabel * pkgNameLabel = new QLabel( this );

    if ( ! selectable->theObj() )
        return;

    _layout->addWidget( pkgNameLabel );

    QFont font = pkgNameLabel->font();
    font.setBold( true );

    QFontMetrics fm( font) ;
    font.setPixelSize( (int) ( fm.height() * 1.1 ) );
    
    pkgNameLabel->setFont( font );
    pkgNameLabel->setText( selectable->theObj()->name().c_str() );

    // New scope
    {    
        QListIterator<QAbstractButton*> it( _buttons->buttons() );
	
        while ( it.hasNext() )
        {
            delete it.next();
        }
    }

    // Delete all installed items
    qDeleteAll( _installed );
    _installed.clear();   

    
    //
    // Fill installed objects
    //
    {
        zypp::ui::Selectable::installed_iterator it = selectable->installedBegin();

        while ( it != selectable->installedEnd() )
        {
            QString text = _( "%1-%2 from vendor %3 (installed)" )
		.arg( (*it)->edition().asString().c_str() )
		.arg( (*it)->arch().asString().c_str() )
		.arg( (*it)->vendor().c_str() ) ;

            QWidget * installedVersion = new QWidget( this );
	    QHBoxLayout * instLayout = new QHBoxLayout( installedVersion );
	    instLayout->setContentsMargins( 0, 0, 0, 0 );

	    QLabel * icon = new QLabel( installedVersion );
	    icon->setPixmap( YQIconPool::pkgSatisfied() );    
	    instLayout->addWidget( icon );
	    
	    QLabel * textLabel = new QLabel( text, installedVersion );
	    instLayout->addWidget( textLabel );
	    instLayout->addStretch();
                
            _installed.push_back( installedVersion );
            _layout->addWidget( installedVersion );
    
            ++it;
        }
    }

    
    //
    // Fill available objects
    //
    
    {
        zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

        while ( it != selectable->availableEnd() )
        {
            QRadioButton *radioButton = new YQPkgVersion( this, selectable, *it, _userCanSwitch );
            connect( radioButton, SIGNAL( clicked( bool ) ), SLOT( checkForChangedCandidate() ) );

            _buttons->addButton( radioButton );
            _layout->addWidget( radioButton );
            
            
            if ( selectable->hasCandidateObj() &&
                 selectable->candidateObj()->edition() == (*it)->edition() &&
                 selectable->candidateObj()->arch()    == (*it)->arch() )
            {
                radioButton->setChecked(true);
            }
            
            ++it;
        }
    }
    
    _layout->addStretch();
    setWidget( _content );
    _content->show();
}


void
YQPkgVersionsView::checkForChangedCandidate()
{   
    QListIterator<QAbstractButton*> it( _buttons->buttons() );
    
    while ( it.hasNext() )
    {
        YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (it.next());
        
        if ( versionItem && versionItem->isChecked() )
        {
            ZyppObj newCandidate = versionItem->zyppObj();
            
            if ( _selectable && newCandidate != _selectable->candidateObj() )
            {
                yuiMilestone() << "Candidate changed" << endl;
                
                // Change status of selectable
                
                ZyppStatus status = _selectable->status();
                
                if ( !_selectable->installedEmpty() &&
                     _selectable->installedObj()->arch()    == newCandidate->arch() &&
                     _selectable->installedObj()->edition() == newCandidate->edition() )
                {
                    // Switch back to the original instance -
                    // the version that was previously installed
		    status = S_KeepInstalled;
                }
                else
                {
                    switch ( status )
                    {
                    case S_KeepInstalled:
                    case S_Protected:
                    case S_AutoDel:
                    case S_AutoUpdate:
                    case S_Del:
                    case S_Update:
                        
                        status = S_Update;
                        break;
                        
                    case S_NoInst:
                    case S_Taboo:
                    case S_Install:
                    case S_AutoInstall:
                        status = S_Install;
                        break;
                    }
                }
                
                _selectable->setStatus( status );
                
                
                // Set candidate
                
                _selectable->setCandidate( newCandidate );
                emit candidateChanged( newCandidate );
                return;
            }
        }
        
    }
}


QSize
YQPkgVersionsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}






YQPkgVersion::YQPkgVersion( QWidget *	parent,
			    ZyppSel	selectable,
			    ZyppObj 	zyppObj,
			    bool	enabled )
    : QRadioButton( parent )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    // Translators: %1 is a package version, %2 the package architecture,
    // %3 describes the repository where it comes from,
    // %4 is the repository's priority 
    // %5 is the vendor of the package
    // Examples:
    //     2.5.23-i568 from Packman with priority 100 and vendor openSUSE
    //     3.17.4-i386 from openSUSE-11.1 update repository with priority 20 and vendor openSUSE
    //     ^^^^^^ ^^^^      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^               ^^            ^^^^^^^^
    //        %1   %2                %3                                   %4                %5
    setText( _( "%1-%2 from %3 with priority %4 and vendor %5" )
	     .arg( zyppObj->edition().asString().c_str() )
	     .arg( zyppObj->arch().asString().c_str() )
	     .arg( zyppObj->repository().info().name().c_str() )
	     .arg( zyppObj->repository().info().priority() )
	     .arg( zyppObj->vendor().c_str() ) );
}


YQPkgVersion::~YQPkgVersion()
{
    // NOP
}


QString
YQPkgVersion::toolTip(int)
{
    QString tip;

    if ( zyppObj() == selectable()->installedObj() )
	tip = _( "This version is installed in your system." );

    return tip;
}



#include "YQPkgVersionsView.moc"

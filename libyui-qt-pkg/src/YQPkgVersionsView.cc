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

class InstalledItemLabel : public QWidget
{
public:
    InstalledItemLabel( QWidget *parent, const QString &text )
        : QWidget(parent)
    {
        QHBoxLayout *layout = new QHBoxLayout(this);
        
        _picture = new QLabel(this);
        _text = new QLabel(this);
        _text->setText(text);
        _picture->setPixmap(YQIconPool::pkgSatisfied());    
        layout->addWidget(_picture);
        layout->addWidget(_text);
        layout->addStretch();
        
    }
    

private:
    QLabel *_picture;
    QLabel *_text;
    
    };
    
    


YQPkgVersionsView::YQPkgVersionsView( QWidget * parent, bool userCanSwitch )
    : QWidget( parent )
    , _layout(0)
    , _label(0)
{
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);

    _selectable		= 0;
    _parentTab		= dynamic_cast<QTabWidget *> (parent);
    _userCanSwitch 	= userCanSwitch;

    _buttons = new QButtonGroup(parent);

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
        return;

    delete _label;
    delete _layout;
    
    _layout = new QVBoxLayout(this);
    _label = new QLabel(this);

    if ( !selectable->theObj() )
        return;

    _layout->addWidget(_label);

    QFont font = _label->font();
    font.setBold(true);

    QFontMetrics fm(font);
    font.setPixelSize( fm.height() * 1.1 );
    
    _label->setFont(font);
    
    _label->setText(selectable->theObj()->name().c_str());

    // new scope
    {    
        QListIterator<QAbstractButton*> it(_buttons->buttons());
        while (it.hasNext())
        {
            delete it.next();
        }
    }

    // delete all installed items
    qDeleteAll(_installed);
    _installed.clear();    
    
    // Fill installed objects
    {
        
        zypp::ui::Selectable::installed_iterator it = selectable->installedBegin();

        while ( it != selectable->installedEnd() )
        {
            QString text = _( "%1-%2 (installed)" ).arg( (*it)->edition().asString().c_str() ).arg(  (*it)->arch().asString().c_str() );

            QWidget *b = new InstalledItemLabel(this, text);
                
            _installed.push_back(b);
            _layout->addWidget(b);
    
            ++it;
        }
    }

    // Fill available objects
    {
        
        zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

        while ( it != selectable->availableEnd() )
        {
            QRadioButton *b = new YQPkgVersion( this, selectable, *it, _userCanSwitch );
            connect( b, SIGNAL(clicked(bool)), SLOT(checkForChangedCandidate()) );

            _buttons->addButton(b);
            _layout->addWidget(b);
            
            
            if ( selectable->hasCandidateObj() &&
                 selectable->candidateObj()->edition() == (*it)->edition() &&
                 selectable->candidateObj()->arch() == (*it)->arch() )
            {
                b->setChecked(true);
            }
            
            ++it;
        }
    }
    
    _layout->addStretch();
}


void
YQPkgVersionsView::checkForChangedCandidate()
{   
    QListIterator<QAbstractButton*> it(_buttons->buttons());
    while (it.hasNext())
    {
        YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (it.next());
        
        if ( versionItem && versionItem->isChecked() )
        {
            ZyppObj newCandidate = versionItem->zyppObj();
            
            if ( newCandidate != _selectable->candidateObj() )
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


YQPkgVersion::YQPkgVersion( YQPkgVersionsView *	pkgVersionList,
			    ZyppSel		selectable,
			    ZyppObj 		zyppObj,
			    bool		enabled )
    : QRadioButton( pkgVersionList )
    , _pkgVersionList( pkgVersionList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    setText( _( "%1-%2 from %3" ).arg( zyppObj->edition().asString().c_str() ).arg(  zyppObj->arch().asString().c_str() ).arg(  zyppObj->repository().name().c_str() ) );
    
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

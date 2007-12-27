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

  File:	      YQPkgConflictList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QMessageBox>
#include <QRadioButton>
#include <QList>
#include <QDebug>
#include <QVBoxLayout>

#include <errno.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <zypp/ZYppFactory.h>
#include "YQPkgConflictList.h"
#include "YQPkgConflictDialog.h"
#include "YQIconPool.h"

#include "YQApplication.h"
#include "YQUI.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;


#define LIST_SPLIT_THRESHOLD	8

#define RED			QColor( 0xC0, 0, 0 )
#define BRIGHT_RED		QColor( 0xFF, 0, 0 )
#define BLUE			QColor( 0, 0, 0xC0 )
#define LIGHT_BLUE		QColor( 0xE0, 0xE0, 0xF8 )
#define LIGHT_GREY		QColor( 0xE0, 0xE0, 0xE0 )
#define MAGENTA			Qt::magenta
#define DEEP_ORANGE		QColor( 0xFF, 0x80, 0x20 )
#define LIGHT_ORANGE		QColor( 0xFF, 0xC0, 0x50 )


YQPkgConflictList::YQPkgConflictList( QWidget * parent )
    : QScrollArea( parent ), _layout( 0 )
{
    setWidget( new QFrame( this ) );
    _layout = new QVBoxLayout;
    widget()->setLayout( _layout );
    clear();

    QLayoutItem *b = new QSpacerItem(0, 0, QSizePolicy::Minimum,
                                     QSizePolicy::Expanding);


    //setHeaderLabel( _( "Dependency Conflict" ) );
    //setRootIsDecorated( true );
    //setSortByInsertionSequence( true );
}


YQPkgConflictList::~YQPkgConflictList()
{
    // NOP
}

void
YQPkgConflictList::clear()
{
    YQPkgConflict * conflict;
    foreach( conflict, _conflicts )
    {
        _layout->removeWidget( conflict );
        delete conflict;
    }
    _conflicts.clear();
    // kill the stretch item too
    delete _layout->takeAt( 0 );
}

void
YQPkgConflictList::fill( zypp::ResolverProblemList problemList )
{
    clear();
    string text;

    // for some weired reason, the layout's minSize is still 18x18 even after 3000 pixels
    // inserted, so we have to do the math on our own
    QSize minSize = QSize( _layout->margin() * 2, _layout->margin() * 2 );

    zypp::ResolverProblemList::iterator it = problemList.begin();

    while ( it != problemList.end() )
    {
	YQPkgConflict *conflict = new YQPkgConflict( widget(), *it );
	Q_CHECK_PTR( conflict );

        minSize = minSize.expandedTo( conflict->minimumSizeHint() );
        minSize.rheight() += conflict->minimumSizeHint().height() + _layout->spacing();
        _layout->addWidget( conflict );
        _conflicts.push_back( conflict );
	++it;
    }
    _layout->addStretch( 1 );

    widget()->resize( minSize );
    setWidgetResizable( false );
}


void
YQPkgConflictList::applyResolutions()
{
    zypp::ProblemSolutionList userChoices;

    YQPkgConflict *conflict;
    foreach( conflict, _conflicts )
    {
        zypp::ProblemSolution_Ptr userChoice = conflict->userSelectedResolution();

        if ( userChoice )
            userChoices.push_back( userChoice );
    }

    zypp::getZYpp()->resolver()->applySolutions( userChoices );
    emit updatePackages();
}


void
YQPkgConflictList::askSaveToFile() const
{
    QString filename = YQApplication::askForSaveFileName( "conflicts.txt",	// startsWith
							  "*.txt",		// filter
							  _( "Save Conflicts List" ) );
    if ( ! filename.isEmpty() )
	saveToFile( filename, true );
}


void
YQPkgConflictList::saveToFile( const QString filename, bool interactive ) const
{
#if 0
    // Open file
    QFile file(filename);

    if ( ! file.open(QIODevice::WriteOnly) )
    {
	y2error( "Can't open file %s", qPrintable(filename) );

	if ( interactive )
	{
	    // Post error popup.

	    QMessageBox::warning( 0,						// parent
				  _( "Error" ),					// caption
				  _( "Cannot open file %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
	return;
    }


    // Write header

    QString header = "#### YaST2 conflicts list - generated ";
    header += QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" );
    header += " ####\n\n";

   file.write(header.toUtf8());


    // Recursively write all items
    int count=0;
    const QTreeWidgetItem * item;

    while ( (item = topLevelItem(count)) )
    {
	saveItemToFile( file, item );
	count++;
    }


    // Write footer

    file.write("\n#### YaST2 conflicts list END ###\n" );


    // Clean up

    if ( file.isOpen() )
	file.close();

#endif
}


void
YQPkgConflictList::saveItemToFile( QFile 			&file,
				   const YQPkgConflict * 	item ) const
{
#if FIXME
    if ( ! item || ! file.isOpen() )
	return;

    // Write indentation
    for ( int level = 0; level < item->depth(); level++ )
	file.write( "    " );

    // Write item

    const QTreeWidgetItem * checkListItem = dynamic_cast<const QTreeWidgetItem *> (item);

    if ( checkListItem )
    {
	switch ( checkListItem->type() )
	{
            QString buffer;
	    case Q3CheckListItem::CheckBox:
		buffer.sprintf( "[%c] ", checkListItem->( checkState(0) == Qt::Checked ) ? 'x' : ' ' );
		break;
	    case Q3CheckListItem::RadioButton:
		sbuffer.sprintf( "(%c) ", checkListItem->( checkState(0) == Qt::Checked ) ? 'x' : ' ' );
		break;
	    default:
		break;
	}
    file.write(buffer.toUtf8());
    }

    buffer.sprintf("%s\n", qPrintable(item->text(0)) );
    file.write(buffer.toUtf8());

    if ( item->isExpanded() )
    {
	// Recursively write children

	const QTreeWidgetItem * child = item->firstChild();

	while ( child )
	{
	    saveItemToFile( file, child );
	    child = child->nextSibling();
	}
    }
#endif
}

YQPkgConflict::YQPkgConflict( QWidget *		parent,
			      zypp::ResolverProblem_Ptr		problem	)
    : QFrame( parent )
    , _problem( problem )
    , _resolutionsHeader( 0 )
{
    _layout = new QVBoxLayout( this );
    _layout->setSpacing( 0 );
    _layout->setMargin( 0 );
    formatHeading();
    QLabel *label = new QLabel( fromUTF8 ( _problem->details() ), this );
    _layout->addWidget( label );
    //YQPkgConflictList::dumpList( this,  );

    addSolutions();
    setMinimumSize( _layout->minimumSize() );
}


void
YQPkgConflict::formatHeading()
{
    QFrame *frame = new QFrame( this );
    QHBoxLayout *hbox = new QHBoxLayout(frame);

    QLabel *pix = new QLabel( this );
    pix->setPixmap( YQIconPool::normalPkgConflict() );

    hbox->addWidget( pix );

    QString text = fromUTF8( problem()->description() );
    QLabel *heading = new QLabel( text, this );
    heading->setStyleSheet( "font-size: +2; color: red; font: bold;" );
    hbox->addWidget( heading );
    hbox->addStretch( 1 );

    frame->setStyleSheet( "background-color: lightgray;" );
    _layout->addWidget(frame);
}


void
YQPkgConflict::addSolutions()
{
    // Heading for the choices
    // how to resolve this conflict
    _resolutionsHeader = new QLabel( _( "Conflict Resolution:" ), this );
    _layout->addWidget( _resolutionsHeader );
    Q_CHECK_PTR( _resolutionsHeader );

    zypp::ProblemSolutionList solutions = problem()->solutions();
    zypp::ProblemSolutionList::iterator it = solutions.begin();

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addSpacing( 20 );

    QVBoxLayout *vbox = new QVBoxLayout();
    hbox->addLayout( vbox );
    _layout->addLayout( hbox );

    while ( it != solutions.end() )
    {
        QRadioButton * s = new QRadioButton( fromUTF8( ( *it )->description() ), this );
        Q_CHECK_PTR( s );
        _solutions[ s ] = *it;
        vbox->addWidget( s );

	++it;
    }
}

zypp::ProblemSolution_Ptr
YQPkgConflict::userSelectedResolution()
{
    QMap<QRadioButton*, zypp::ProblemSolution_Ptr>::iterator it;

    for ( it = _solutions.begin(); it != _solutions.end(); ++it )
    {
        QRadioButton *button = it.key();
        if ( !button->isChecked() )
            continue;
        zypp::ProblemSolution_Ptr solution = it.value();

        y2milestone( "User selected resolution \"%s\" for problem \"%s\"",
                     solution->description().c_str(),
                     solution->problem()->description().c_str() );
        return solution;
    }

    return zypp::ProblemSolution_Ptr();		// Null pointer
}

#include "YQPkgConflictList.moc"

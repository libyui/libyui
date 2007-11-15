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


#include <qpainter.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qmessagebox.h>

#include <errno.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <zypp/ZYppFactory.h>
#include "YQPkgConflictList.h"
#include "YQPkgConflictDialog.h"
#include "YQIconPool.h"

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
    : QY2ListView( parent )
{
    addColumn( _( "Dependency Conflict" ) );
    setRootIsDecorated( true );
    setSortByInsertionSequence( true );
}


YQPkgConflictList::~YQPkgConflictList()
{
    // NOP
}


void
YQPkgConflictList::fill( zypp::ResolverProblemList problemList )
{
    clear();
    string text;


    zypp::ResolverProblemList::iterator it = problemList.begin();

    while ( it != problemList.end() )
    {
	YQPkgConflict * conflict = new YQPkgConflict( this, *it );
	CHECK_PTR( conflict );

	++it;
    }
}


void
YQPkgConflictList::applyResolutions()
{
    zypp::ProblemSolutionList userChoices;

    QListViewItem * child = firstChild();

    while ( child )
    {
	YQPkgConflict * conflict = dynamic_cast<YQPkgConflict *> (child);

	if ( conflict )
	{
	    zypp::ProblemSolution_Ptr userChoice = conflict->userSelectedResolution();

	    if ( userChoice )
		userChoices.push_back( userChoice );
	}

	child = child->nextSibling();
    }

    zypp::getZYpp()->resolver()->applySolutions( userChoices );

    emit updatePackages();
}


void
YQPkgConflictList::askSaveToFile() const
{
    QString filename = YQUI::ui()->askForSaveFileName( "conflicts.txt",	// startsWith
						       "*.txt",		// filter
						       _( "Save Conflicts List" ) );
    if ( ! filename.isEmpty() )
	saveToFile( filename, true );
}


void
YQPkgConflictList::saveToFile( const QString filename, bool interactive ) const
{
    // Open file

    FILE * file = fopen( (const char *) filename, "w" );

    if ( ! file )
    {
	y2error( "Can't open file %s", (const char *) filename );

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

    fputs( (const char *) header, file );


    // Recursively write all items

    const QListViewItem * item = firstChild();

    while ( item )
    {
	saveItemToFile( file, item );
	item = item->nextSibling();
    }


    // Write footer

    fprintf( file, "\n#### YaST2 conflicts list END ###\n" );


    // Clean up

    if ( file )
	fclose( file );
}


void
YQPkgConflictList::saveItemToFile( FILE * 			file,
				   const QListViewItem * 	item ) const
{
    if ( ! item || ! file )
	return;

    // Write indentation

    for ( int level = 0; level < item->depth(); level++ )
	fprintf( file, "    " );


    // Write item

    const QCheckListItem * checkListItem = dynamic_cast<const QCheckListItem *> (item);

    if ( checkListItem )
    {
	switch ( checkListItem->type() )
	{
	    case QCheckListItem::CheckBox:
		fprintf( file, "[%c] ", checkListItem->isOn() ? 'x' : ' ' );
		break;
	    case QCheckListItem::RadioButton:
		fprintf( file, "(%c) ", checkListItem->isOn() ? 'x' : ' ' );
		break;
	    default:
		break;
	}
    }

    fprintf( file, "%s\n", (const char *) item->text(0) );


    if ( item->isOpen() )
    {
	// Recursively write children

	const QListViewItem * child = item->firstChild();

	while ( child )
	{
	    saveItemToFile( file, child );
	    child = child->nextSibling();
	}
    }
}


void
YQPkgConflictList::dumpList( QListViewItem * 	parent,
			     const QString &	longText,
			     const QString & 	header,
			     int		splitThreshold )
{
    if ( ! parent )
    {
	y2error( "Null parent" );
	return;
    }

    if ( longText.isEmpty() )
	return;

    if ( ! header.isEmpty() )
    {
	parent = new QY2ListViewItem( parent, header );
	CHECK_PTR( parent );
	parent->setOpen( true );
    }

    QStringList lines = QStringList::split( '\n', longText,
					    true );		// allowEmptyEntries
    QValueList<QString>::const_iterator it = lines.begin();

    bool doSplit	= splitThreshold > 1 && lines.size() > (unsigned) splitThreshold + 3;
    bool didSplit	= false;
    int  count		= 0;


    while ( it != lines.end() )
    {
	if ( doSplit && ! didSplit && ++count > splitThreshold )
	{
	    // Split list

	    int more = lines.size() - count + 1;
	    QString text = ( _( "%1 more..." ) ).arg( more );
	    QY2ListViewItem * sublist = new QY2ListViewItem( parent, text );
	    didSplit = true;

	    if ( sublist )
	    {
		sublist->setBackgroundColor( LIGHT_ORANGE );
		parent = sublist;
	    }
	}

	new QY2ListViewItem( parent, *it );
	++it;
    }
}







YQPkgConflict::YQPkgConflict( YQPkgConflictList *		parentList,
			      zypp::ResolverProblem_Ptr		problem	)
    : QY2ListViewItem( parentList )
    , _problem( problem )
    , _resolutionsHeader( 0 )
{
    setBackgroundColor( LIGHT_BLUE );
    setOpen( true );

    formatHeading();
    YQPkgConflictList::dumpList( this, fromUTF8( _problem->details() ) );

    addSolutions();
}


void
YQPkgConflict::formatHeading()
{
    QString text;
    QPixmap icon = YQIconPool::normalPkgConflict();
    setTextColor( BRIGHT_RED );

    setText( 0, fromUTF8( problem()->description() ) );
    setPixmap( 0, icon );
}


void
YQPkgConflict::addSolutions()
{
    _resolutionsHeader = new QY2CheckListItem( this,
					       // Heading for the choices
					       // how to resolve this conflict
					       _( "Conflict Resolution:" ),
					       QCheckListItem::Controller );
    CHECK_PTR( _resolutionsHeader );
    _resolutionsHeader->setOpen( true );
    _resolutionsHeader->setBackgroundColor( LIGHT_GREY );

    zypp::ProblemSolutionList solutions = problem()->solutions();
    zypp::ProblemSolutionList::iterator it = solutions.begin();

    while ( it != solutions.end() )
    {
	new YQPkgConflictResolution( _resolutionsHeader, *it );
	++it;
    }
}


void
YQPkgConflict::paintCell( QPainter *		painter,
			  const QColorGroup &	colorGroup,
			  int			column,
			  int			width,
			  int			alignment )
{
    painter->setFont( YQUI::ui()->headingFont() );
    QY2ListViewItem::paintCell( painter, colorGroup, column, width, alignment );
}


zypp::ProblemSolution_Ptr
YQPkgConflict::userSelectedResolution()
{
    QListViewItem * item = _resolutionsHeader->firstChild();

    while ( item )
    {
	YQPkgConflictResolution * res = dynamic_cast<YQPkgConflictResolution *> (item);

	if ( res && res->isOn() )
	{
	    zypp::ProblemSolution_Ptr solution = res->solution();

	    y2milestone( "User selected resolution \"%s\" for problem \"%s\"",
			 solution->description().c_str(),
			 solution->problem()->description().c_str() );
	    return solution;
	}

	item = item->nextSibling();
    }

    return zypp::ProblemSolution_Ptr();		// Null pointer
}






YQPkgConflictResolution::YQPkgConflictResolution( QY2CheckListItem * 		parent,
						  zypp::ProblemSolution_Ptr	solution )
    : QY2CheckListItem( parent,
			fromUTF8( solution->description() ),
			QCheckListItem::RadioButton )
    , _solution( solution )
{
    YQPkgConflictList::dumpList( this, fromUTF8( solution->details() ) );
}



#include "YQPkgConflictList.moc"

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
#include <QList>

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
    : QY2ListView( parent )
{

    setHeaderLabel( _( "Dependency Conflict" ) );
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
	Q_CHECK_PTR( conflict );

	++it;
    }
}


void
YQPkgConflictList::applyResolutions()
{
    zypp::ProblemSolutionList userChoices;

    int count=0;
    QTreeWidgetItem * child;

    while ( (child = topLevelItem(count)) )
    {
	YQPkgConflict * conflict = dynamic_cast<YQPkgConflict *> (child);

	if ( conflict )
	{
	    zypp::ProblemSolution_Ptr userChoice = conflict->userSelectedResolution();

	    if ( userChoice )
		userChoices.push_back( userChoice );
	}

	count++;
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
}


void
YQPkgConflictList::saveItemToFile( QFile 			&file,
				   const QTreeWidgetItem * 	item ) const
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


void
YQPkgConflictList::dumpList( QTreeWidgetItem * 	parent,
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

#if FIXME
    if ( ! header.isEmpty() )
    {
	parent = new QY2ListViewItem( parent, header );
	Q_CHECK_PTR( parent );
	parent->setExpanded( true );
    }

    QStringList lines = QStringList::split( '\n', longText,
					    true );		// allowEmptyEntries
    QList<QString>::const_iterator it = lines.begin();

    bool doSplit	= splitThreshold > 1 && lines.size() > splitThreshold + 3;
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
#endif
}







YQPkgConflict::YQPkgConflict( YQPkgConflictList *		parentList,
			      zypp::ResolverProblem_Ptr		problem	)
    : QY2ListViewItem( parentList )
    , _problem( problem )
    , _resolutionsHeader( 0 )
{
    setBackgroundColor( LIGHT_BLUE );
#if FIXME
    setExpanded( true );
#endif

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

#if FIXME
    setText( 0, Qt::DisplayRole, fromUTF8( problem()->description() ) );
#endif
    setData( 0, Qt::DecorationRole, icon );
}


void
YQPkgConflict::addSolutions()
{
    _resolutionsHeader = new QY2CheckListItem( this,
					       // Heading for the choices
					       // how to resolve this conflict
					       _( "Conflict Resolution:" ) );
    Q_CHECK_PTR( _resolutionsHeader );
#if FIXME
    _resolutionsHeader->setExpanded( true );
#endif
    _resolutionsHeader->setBackgroundColor( LIGHT_GREY );

    zypp::ProblemSolutionList solutions = problem()->solutions();
    zypp::ProblemSolutionList::iterator it = solutions.begin();

    while ( it != solutions.end() )
    {
	YQPkgConflictResolution * solution = new YQPkgConflictResolution( _resolutionsHeader, *it );
	Q_CHECK_PTR( solution );
	//FIXME solution->setExpanded(true);

	++it;
    }
}


// void
// YQPkgConflict::paintCell( QPainter *		painter,
// 			  const QColorGroup &	colorGroup,
// 			  int			column,
// 			  int			width,
// 			  int			alignment )
// {
//     painter->setFont( YQUI::yqApp()->headingFont() );
// #if FIXME
//     QY2ListViewItem::paintCell( painter, colorGroup, column, width, alignment );
// #endif
// }


zypp::ProblemSolution_Ptr
YQPkgConflict::userSelectedResolution()
{
    int count = 0;
    QTreeWidgetItem * item;

#if FIXME
    while ( item = _resolutionsHeader->topLevelItem(count) )
    {
	YQPkgConflictResolution * res = dynamic_cast<YQPkgConflictResolution *> (item);

	if ( res && res->( checkState(0) == Qt::Checked ) )
	{
	    zypp::ProblemSolution_Ptr solution = res->solution();

	    y2milestone( "User selected resolution \"%s\" for problem \"%s\"",
			 solution->description().c_str(),
			 solution->problem()->description().c_str() );
	    return solution;
	}

	count++;
    }

#endif
    return zypp::ProblemSolution_Ptr();		// Null pointer
}






YQPkgConflictResolution::YQPkgConflictResolution( QY2CheckListItem * 		parent,
						  zypp::ProblemSolution_Ptr	solution )
    : QY2CheckListItem( parent,
			fromUTF8( solution->description() ) )
			/*, Q3CheckListItem::RadioButton) */
{
    _solution = solution;
    YQPkgConflictList::dumpList( this, fromUTF8( solution->details() ) );
}



#include "YQPkgConflictList.moc"

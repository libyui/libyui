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

  Textdomain "qt-pkg"

/-*/

#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QMessageBox>
#include <QRadioButton>
#include <QList>
#include <QToolTip>
#include <QDebug>
#include <QVBoxLayout>

#include <errno.h>

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

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

    widget()->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );

    //setHeaderLabel( _( "Dependency Conflict" ) );
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

    zypp::ResolverProblemList::iterator it = problemList.begin();

    while ( it != problemList.end() )
    {
	YQPkgConflict *conflict = new YQPkgConflict( widget(), *it );
	Q_CHECK_PTR( conflict );

        connect( conflict, SIGNAL( expanded() ),
                 SLOT( relayout() ) );
        _layout->addWidget( conflict );
        _conflicts.push_back( conflict );
	++it;
    }
    _layout->addStretch( 1 );
    relayout();
}

void YQPkgConflictList::relayout()
{
     // for some weired reason, the layout's minSize is still 18x18 even after 3000 pixels
    // inserted, so we have to do the math on our own
    QSize minSize = QSize( _layout->margin() * 2, _layout->margin() * 2 );

    YQPkgConflict * conflict;
    foreach( conflict, _conflicts )
    {
        minSize = minSize.expandedTo( conflict->minimumSizeHint() );
        minSize.rheight() += conflict->minimumSizeHint().height() + _layout->spacing();
    }

    widget()->resize( minSize );
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
    // Open file
    QFile file(filename);

    if ( ! file.open(QIODevice::WriteOnly) )
    {
	yuiError() << "Can't open file " << filename << endl;

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

    YQPkgConflict * conflict;
    foreach( conflict, _conflicts )
    {
	conflict->saveToFile( file );
    }


    // Write footer

    file.write("\n#### YaST2 conflicts list END ###\n" );


    // Clean up

    if ( file.isOpen() )
	file.close();

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

    setProperty( "class", "conflict" );
    addSolutions();
    setMinimumSize( _layout->minimumSize() );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
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
    heading->setProperty( "class", "conflict-heading" );
    heading->setStyleSheet( "font-size: +2; color: red; font: bold;" );
    hbox->addWidget( heading );
    hbox->addStretch( 1 );

    frame->setProperty( "class", "conflict-frame" );
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

        QString details = fromUTF8( ( *it )->details() );
        if ( !details.isEmpty() )
        {
            QStringList lines = details.split( "\n" );
            if ( lines.count() > 7 )
            {
                details = "<qt>";
                for ( int i = 0; i < 4; i++ )
                    details += lines[i] + "<br>\n";
                details += _( "<a href='/'>%1 more...</a>" ).arg( lines.count() - 4 );
            }
            QLabel * d = new QLabel( details, this );
            connect( d, SIGNAL( linkActivated ( const QString & ) ),
                     SLOT( detailsExpanded() ) );
            connect( d, SIGNAL( linkHovered ( const QString & ) ),
                     SLOT( detailsTooltip() ) );

            QHBoxLayout *hbox = new QHBoxLayout();
            hbox->addSpacing( 15 );
            hbox->addWidget( d );
            vbox->addLayout( hbox );
            _details[ d ] = *it;
        }
	++it;
    }
}

void
YQPkgConflict::detailsExpanded()
{
    QLabel *obj = qobject_cast<QLabel*>( sender() );
    if ( !obj || !_details.contains( obj ) )
        return;

    QSize _size = size();
    int oldHeight = obj->height();
    obj->setText( fromUTF8( _details[obj]->details() ) );

    resize( _size.width(), _size.height() + ( obj->minimumSizeHint().height() - oldHeight ) );
    emit expanded();
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

        yuiMilestone() << "User selected resolution \""<< solution->description()
		       << "\" for problem \"" << solution->problem()->description()
		       <<"\"" << endl;
        return solution;
    }

    return zypp::ProblemSolution_Ptr();		// Null pointer
}

void
YQPkgConflict::saveToFile( QFile &file ) const
{
    if ( ! file.isOpen() )
	return;

    // Write item

    QMap<QRadioButton*, zypp::ProblemSolution_Ptr>::const_iterator it;

    file.write( problem()->description().c_str() );
    file.write( "\n" );
    file.write( problem()->details().c_str() );
    file.write( "\n" );

    QString buffer;

    for ( it = _solutions.begin(); it != _solutions.end(); ++it )
    {
        QRadioButton *button = it.key();
        zypp::ProblemSolution_Ptr solution = it.value();
        buffer.sprintf( "    [%c] %s\n", button->isChecked() ? 'x' : ' ', qPrintable( fromUTF8( solution->description() ) ) );
        buffer += fromUTF8( solution->details() );
        file.write(buffer.toUtf8());
    }
    file.write( "\n\n" );
}

#include "YQPkgConflictList.moc"

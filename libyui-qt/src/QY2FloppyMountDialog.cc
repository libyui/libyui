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

  File:	      QY2FloppyMountDialog.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

  This is a pure Qt widget - it can be used independently of YaST2.


/-*/


#include "QY2FloppyMountDialog.h"
#include "YQi18n.h"

#ifndef NO_YAST2_ENVIRONMENT
#   define y2log_component "qt-pkg"
#   include <ycp/y2log.h>
#endif


#include <stdlib.h>

#include <qapplication.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


QY2FloppyMountDialog::QY2FloppyMountDialog( QWidget * 		parent,
					    const QString & 	startWith,
					    const QString & 	filter,
					    const QString & 	headline,
					    const QString & 	floppyDevice,
					    const QString & 	floppyMountPoint,
					    bool 		startWithFloppy  )
    : QDialog( parent )
    , _startWith( startWith )
    , _filter( filter )
    , _floppyDevice( floppyDevice )
    , _floppyMountPoint( floppyMountPoint )
    , _floppyMounted( false )
    , _wantExistingFile( false )
{
    QHBox *		hbox;
    QLabel *		label;
    QPushButton *	button;

    if ( _floppyDevice.isEmpty() )	_floppyDevice	  = "/dev/fd0";
    if ( _floppyMountPoint.isEmpty() )	_floppyMountPoint = "/media/floppy";



    // Dialog title
    setCaption( _( "Select File" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog (can't simply insert a QVBox)

    QVBoxLayout * layout = new QVBoxLayout( this,	// parent
					    8,		// margin
					    SPACING );	// spacing
    CHECK_PTR( layout );


    // Headline

    label = new QLabel( "<b>" + headline + "</b>", this );
    CHECK_PTR( label );
    layout->addWidget( label );


    // Radio box for device (hard disk / floppy)

    QVButtonGroup * bgroup = new QVButtonGroup( _( "Device" ), this );
    CHECK_PTR( bgroup );
    bgroup->setRadioButtonExclusive( true );
    layout->addWidget( bgroup );

    _useHardDisk = new QRadioButton( _( "&Hard Disk" ), bgroup );
    CHECK_PTR( _useHardDisk );
    _useHardDisk->setChecked( ! startWithFloppy );

    _useFloppy = new QRadioButton( _( "&Floppy" ), bgroup );
    CHECK_PTR( _useFloppy );
    _useFloppy->setChecked( startWithFloppy );


    // Box for floppy parameters

    hbox = new QHBox( bgroup );
    CHECK_PTR( hbox );
    addHSpacing( hbox, 25 );

    _floppyParams = new QFrame( hbox );
    CHECK_PTR( _floppyParams );
    _floppyParams->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    QGridLayout * grid = new QGridLayout( _floppyParams,	// parent
					  2,			// rows
					  2,			// cols
					  MARGIN,		// margin
					  SPACING );		// spacing
    CHECK_PTR( grid );
    grid->setColStretch( 0, 0 );	// label column - dont' stretch
    grid->setColStretch( 1, 1 );	// input field (combo box) column may stretch as desired


    // Input field + caption for "Floppy Device"

    label = new QLabel( _( "Floppy &Device:" ), _floppyParams );
    CHECK_PTR( label );
    grid->addWidget( label, 0, 0 );

    _floppyDeviceField = new QComboBox( _floppyParams );
    CHECK_PTR( _floppyDeviceField );
    grid->addWidget( _floppyDeviceField, 0, 1 );
    _floppyDeviceField->setEditable( true );
    _floppyDeviceField->setAutoCompletion( true );
    _floppyDeviceField->insertItem( _floppyDevice );
    label->setBuddy( _floppyDeviceField );



    // Input field + caption for "Mount Point"

    label = new QLabel( _( "&Mount Point:" ), _floppyParams );
    CHECK_PTR( label );
    grid->addWidget( label, 1, 0 );

    _floppyMountPointField = new QComboBox( _floppyParams );
    CHECK_PTR( _floppyMountPointField );
    grid->addWidget( _floppyMountPointField, 1, 1 );
    _floppyMountPointField->setEditable( true );
    _floppyMountPointField->setAutoCompletion( true );
    _floppyMountPointField->insertItem( _floppyMountPoint );
    label->setBuddy( _floppyMountPointField );

    addHStretch( hbox );

    enableWidgets();
    connect( _useFloppy, SIGNAL( stateChanged( int ) ),
	     this, 	 SLOT  ( enableWidgets()     ) );


    // HBox for path field, its caption and the "Browse..." button

    hbox = new QHBox( this );
    CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addWidget( hbox );


    // Input field for path

    label = new QLabel( _( "&Path:" ), hbox );
    CHECK_PTR( label );

    _pathField = new QLineEdit( startWith, hbox );
    CHECK_PTR( _pathField );
    _pathField->setMinimumSize( 300, _pathField->sizeHint().height() );
    label->setBuddy( _pathField );


    // "Browse..." button (open regular file selection box)

    addHSpacing( hbox );
    button = new QPushButton( _( "Bro&wse..." ), hbox );
    CHECK_PTR( button );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( browse()  ) );


    // Button box

    hbox = new QHBox( this );
    CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addWidget( hbox );

    addHStretch( hbox );


    // "OK" button

    button = new QPushButton( _( "&OK" ), hbox );
    CHECK_PTR( button );
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    addHStretch( hbox );


    // "Cancel" button

    button = new QPushButton( _( "&Cancel" ), hbox );
    CHECK_PTR( button );

    connect( button,	SIGNAL( clicked() ),
	     this,     	SLOT  ( reject()  ) );

    addHStretch( hbox );
}



QY2FloppyMountDialog::~QY2FloppyMountDialog()
{
    unmount();
}



QString
QY2FloppyMountDialog::askForSaveFileName()
{
    return askForFile( false );
}


QString
QY2FloppyMountDialog::askForExistingFile()
{
    return askForFile( true );
}


QString
QY2FloppyMountDialog::askForFile( bool wantExistingFile )
{
    _wantExistingFile = wantExistingFile;
    exec();

    if ( result() == QDialog::Rejected )
    {
	unmount();
	return "";
    }

    mount();
    QString filename = _pathField->text();

    if ( _useFloppy->isChecked() )
    {
	QString mountPoint = _floppyMountPointField->currentText();

	if ( ! mountPoint.endsWith( "/" ) )
	     mountPoint.append( "/" );

	if ( ! filename.startsWith( mountPoint ) )
	    filename.prepend( mountPoint );
    }

    return filename;
}


void
QY2FloppyMountDialog::browse()
{
    if ( ! mount() )
	return;

    QString filename;
    QString startWith = _useFloppy->isChecked() ?
	_floppyMountPointField->currentText() :
	_startWith;

    if ( _wantExistingFile )
    {
	filename = QFileDialog::getOpenFileName( startWith,
						 _filter,
						 this,			// parent
						 _( "Select File" ) );	// caption

    }
    else
    {
	filename = QFileDialog::getSaveFileName( startWith,
						 _filter,
						 this,			// parent
						 _( "Select File" ) );	// caption
    }

    unmount();

    if ( ! filename.isEmpty() )
	_pathField->setText( filename );
}


void
QY2FloppyMountDialog::enableWidgets()
{
    _floppyParams->setEnabled( _useFloppy->isChecked() );
}


bool
QY2FloppyMountDialog::mount()
{
    if ( ! _useFloppy->isChecked() )
	return true;

    if ( _floppyMounted )
	unmount();

    QString device	= _floppyDeviceField->currentText();
    QString mountPoint	= _floppyMountPointField->currentText();

    if ( device.isEmpty() )
    {
	error( _( "Error: No floppy device specified" ) );
	return false;
    }

    if ( mountPoint.isEmpty() )
    {
	error( _( "Error: No floppy mount point specified" ) );
	return false;
    }

    QString command = "/bin/mount " + device + " " + mountPoint;
    y2milestone( "Mounting floppy: %s", (const char *) command );

    while ( true )
    {
	qApp->setOverrideCursor( waitCursor );
	int result = system( (const char *) command );
	qApp->restoreOverrideCursor();

	if ( result != 0 )
	{
	    y2warning( "\"%s\" returned %d", (const char *) command, result );

	    int button = QMessageBox::critical( this, "",
						_( "Cannot mount floppy" ),
						_( "&Retry" ),
						_( "&Cancel" ) );

	    if ( button == 1 )	// Cancel
		return false;
	}
	else
	{
	    _floppyMounted = true;
	    return true;	// success
	}
    }

    /*NOTREACHED*/
    return false;
}


void
QY2FloppyMountDialog::unmount( bool verbose )
{
    if ( _floppyMounted )
    {
	QString command = "/bin/umount " + _floppyDeviceField->currentText();
	y2milestone( "Unmounting floppy: %s", (const char *) command );
	int result = system( (const char *) command );

	if ( result == 0 )
	{
	    if ( verbose )
		QMessageBox::information( this, "", _( "Floppy unmounted successfully -\n"
						       "safe to remove floppy from drive.") );
	}
	else
	{
	    y2warning( "\"%s\" returned %d", (const char *) command, result );
	    error( _("Cannot unmount floppy") );
	}

	_floppyMounted = false;
    }
}


void
QY2FloppyMountDialog::error( const QString & msg )
{
    QMessageBox::critical( this, "", msg, QMessageBox::Ok, QMessageBox::NoButton );
}


void QY2FloppyMountDialog::addVStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) ); // hor/vert
}


void QY2FloppyMountDialog::addHStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum) ); // hor/vert
}


void QY2FloppyMountDialog::addVSpacing( QWidget * parent, int height )
{
    QWidget * spacer = new QWidget( parent );
    CHECK_PTR( spacer );
    spacer->setFixedHeight( height );
}


void QY2FloppyMountDialog::addHSpacing( QWidget * parent, int width )
{
    QWidget * spacer = new QWidget( parent );
    CHECK_PTR( spacer );
    spacer->setFixedWidth( width );
}



#include "QY2FloppyMountDialog.moc.cc"

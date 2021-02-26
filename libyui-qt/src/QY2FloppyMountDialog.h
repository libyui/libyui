/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt widgets		       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2FloppyMountDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/



#ifndef QY2FloppyMountDialog_h
#define QY2FloppyMountDialog_h

#include <qdialog.h>


class QRadioButton;
class QComboBox;
class QLineEdit;
class QFrame;


/**
 * Extended file selection box that allows floppy access ( mount/unmount floppy )
 * as well as simply opening a standard file selection box.
 **/
class QY2FloppyMountDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor: Creates an extended file selection box that allows floppy
     * access.
     * 
     * If the user selects a file on floppy, this dialog takes care of mounting
     * and unmounting (the floppy remains mounted as long as the dialog exists).
     *
     * Parameters:
     *
     * 'parent' the parent widget.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g. "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * 'floppyDevice' ( default: "/dev/fd0" ) is the device name for the floppy.
     *
     * 'floppyMountPoint' ( default: "/media/floppy" ) is the mount point for the floppy.
     *
     * 'startWithFloppy' indicates if the default should be the floppy or the hard disk.
     **/
    QY2FloppyMountDialog( QWidget * 		parent,
			  const QString & 	startWith,
			  const QString & 	filter,
			  const QString & 	headline,
			  const QString & 	floppyDevice	 = QString::null,
			  const QString & 	floppyMountPoint = QString::null,
			  bool 			startWithFloppy	 = false );

    /**
     * Destructor.
     **/
    virtual ~QY2FloppyMountDialog();

    /**
     * Execute the dialog and ask for a file to save data to.
     * Automatically asks for confirmation if the user selects an existing file.
     *
     * Returns the selected file name
     * or an empty string if the user canceled the operation.
     **/
    QString askForSaveFileName();

    /**
     * Execute the dialog and ask for an existing file to load data from.
     *
     * Returns the selected file name
     * or an empty string if the user canceled the operation.
     **/
    QString askForExistingFile();


public slots:

    /**
     * Mount floppy if floppy is selected. Does nothing if hard disk is
     * selected. Handles failed mount attempts with its own "Retry" / "Cancel"
     * popup. 
     *
     * Returns 'true' on success, 'false' on error if the user chose not to retry.
     **/
    bool mount();

    /**
     * Unmount floppy if it is mounted. Does nothing if it is not mounted.
     * Pops up a message box in verbose mode if unmounting was successful.
     **/
    void unmount( bool verbose = false );

    
protected slots:

    /**
     * Open a ( normal ) file selection box upon pressing the "Browse..." button.
     * Selects an existing or a new file depending on _wantExistingFile.
     **/
    void browse();

    /**
     * Enable / disable widgets depending on current settings.
     **/
    void enableWidgets();

    
protected:

    /**
     * Execute the dialog and ask for a file to save data to.
     **/
    QString askForFile( bool wantExistingFile );
    
    /**
     * Show an error message
     **/
    void error( const QString & msg );
    

    // Data members

    QRadioButton *	_useHardDisk;
    QRadioButton *	_useFloppy;
    QFrame *		_floppyParams;
    QComboBox *		_floppyDeviceField;
    QComboBox *		_floppyMountPointField;
    QLineEdit *		_pathField;

    QString		_startWith;
    QString		_filter;
    QString		_floppyDevice;
    QString		_floppyMountPoint;
    bool		_floppyMounted;
    bool		_wantExistingFile;
};




#endif // ifndef QY2FloppyMountDialog_h

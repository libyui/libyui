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

  File:	      YQPkgTextDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgTextDialog_h
#define YQPkgTextDialog_h

#include <qdialog.h>
#include <zypp/ResObject.h>

class QPushButton;
class QTextBrowser;

using std::list;
using std::string;


/**
 * @short Dialog that shows a scrolled (HTML) text.
 **/
class YQPkgTextDialog: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * Creates a dialog with a text browser and two buttons.
     * This does not pop up a dialog yet.
     * Use 'exec()' (inherited from QDialog).
     * Or, better yet, use the static confirmText() method.
     * 
     **/
    YQPkgTextDialog( const QString & 	text,
		     QWidget * 		parent,
		     const QString & 	acceptButtonLabel,
		     const QString & 	rejectButtonLabel );
    /**
     * Constructor.
     *
     * Creates a dialog with a text browser and an "OK" button.
     * This does not pop up a dialog yet.
     * Use 'exec()' (inherited from QDialog).
     * Or, better yet, use the static showText() method.
     **/
    YQPkgTextDialog( const QString & text, QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgTextDialog();

    /**
     * Reimplemented from QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

    /**
     * Show a text and wait until the user confirmed with 'OK'.
     **/
    static void showText( QWidget * parent, const QString & text );
    static void showText( QWidget * parent, const string & text );
    static void showText( QWidget * parent, const list<string> & text );

    /**
     * Show a text with a headline identifying a zypp::ResObject (name+summary).
     **/
    static void showText( QWidget * parent,
			  zypp::ResObject::constPtr zyppObj,
			  const list<string> & text );
    static void showText( QWidget * parent,
			  zypp::ResObject::constPtr zyppObj,
			  const string & text );

    /**
     * Let the user confirm a text. Returns "true" if the user clicked the
     * accept button, false if he clicked the reject button.
     **/ 
    static bool confirmText( QWidget * 	parent,
			     const QString & 	text,
			     const QString & 	acceptButtonLabel,
			     const QString & 	rejectButtonLabel );

    /**
     * Let the use confirm a text with buttons "Accept" and "Cancel".
     * Returns "true" if the user clicked "Accept", "false" on "Cancel".
     **/
    static bool confirmText( QWidget * parent, const QString & text );

    /**
     * Let the use confirm a text with a headline identifying a zypp::ResObject
     * (name+summary) with buttons "Accept" and "Cancel".
     * Returns "true" if the user clicked "Accept", "false" on "Cancel".
     **/
    static bool confirmText( QWidget * parent,
			     zypp::ResObject::constPtr zyppObj,
			     const list<string> & text );
    
    static bool confirmText( QWidget * parent,
			     zypp::ResObject::constPtr zyppObj,
			     const string & text );
    
    /**
     * Convert a string list into its HTML paragraphs ( empty lines delimit
     * paragraphs ).
     **/
    static QString htmlParagraphs( const list<string> & text );

    /**
     * Returns a uniform heading in HTML format.
     **/
    static QString htmlHeading( const QString & text );

    /**
     * Returns a uniform heading in HTML format for the specified package:
     * Package name and summary
     **/
    static QString htmlHeading( zypp::ResObject::constPtr zyppObj );

    /**
     * Escapes characters special to HTML in a ( plain text ) string, such as:
     * '<'   ->   '&lt;'
     * '>'   ->   '&gt;'
     * '&'   ->   '&amp;'
     *
     * Returns the escaped string.
     **/
    static QString htmlEscape( const QString & plainText );


public slots:

    /**
     * Set the text contents.
     **/
    void setText( const QString & text );
    void setText( const string & text );
    void setText( const list<string> & text );

    /**
     * Show a text with a headline identifying a zypp::ResObject ( name+summary ).
     **/
    void setText( zypp::ResObject::constPtr zyppObj,
		  const list<string> & text );

protected:

    /**
     * Create the dialog. Called from all constructors.
     **/
    void buildDialog( const QString & 	text,
		      QWidget * 	parent,
		      const QString & 	acceptButtonLabel,
		      const QString & 	rejectButtonLabel = "" );
    /**
     * Grab [Return] press events and close dialog.
     * Inherited from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * ev );


    // Data members

    QPushButton *	_acceptButton;
    QPushButton * 	_rejectButton;
    QTextBrowser *	_textBrowser;
};



#endif // ifndef YQPkgTextDialog_h

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		QY2HelpDialog.cc

  Author:	Stephan Kulow <coolo@suse.de>

  Textdomain	"qt"

/-*/

#include <QY2HelpDialog.h>
#include "ui_QHelpDialog.h"
#include <QDebug>
#include <QTextObject>
#include "YQi18n.h"

#include "icons/viewmag.xpm"


QY2HelpDialog::QY2HelpDialog( const QString& helpText, QWidget *parent )
    : QDialog( parent )
{
    _ui = new Ui_QHelpDialog();
    _ui->setupUi( this );
    _ui->textBrowser->setText( helpText );

    _ui->label->setPixmap ( QPixmap( viewmag ) );
    connect( _ui->lineEdit, SIGNAL( textEdited( QString ) ),
             SLOT( searchStringChanged( QString ) ) );

    _ui->lineEdit->setFocus( Qt::OtherFocusReason );
    _ui->pushButton->setAutoDefault(false);

    // Window title for help wizard window
    setWindowTitle( _( "Help" ) );

    // Close button for wizard help window
    _ui->pushButton->setText( _( "&Close" ) );
}

void QY2HelpDialog::setHelpText( const QString& helpText )
{
    _ui->textBrowser->setText( helpText );
    _ui->lineEdit->setText( QString() );
    _ui->lineEdit->setFocus( Qt::OtherFocusReason );
}

QY2HelpDialog::~QY2HelpDialog()
{
    delete _ui;
}

void QY2HelpDialog::searchStringChanged( QString text )
{
    QTextCharFormat fmt;
    fmt.setBackground( Qt::yellow );
    QTextDocument *d = _ui->textBrowser->document();

    for ( QList<QTextCursor>::iterator it = _marks.begin(); it != _marks.end(); ++it )
        ( *it ).setCharFormat( QTextCharFormat() );
    _marks.clear();

    QTextCursor c( d );

    while ( true )
    {
        c = d->find( text, c );
        if ( c.isNull() )
            break;
        c.setCharFormat( fmt );
        c.select( QTextCursor::WordUnderCursor );
        _marks.push_back( c );
    }
}

#include "QY2HelpDialog.moc"

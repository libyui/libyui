#include <QY2HelpDialog.h>
#include "ui_QHelpDialog.h"
#include <QDebug>
#include <QTextObject>

#include "pkg/icons/viewmag.xpm"

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

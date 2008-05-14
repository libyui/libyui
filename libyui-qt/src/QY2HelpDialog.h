#ifndef _QY2HelpDialog_h
#define _QY2HelpDialog_h

#include <QDialog>

class QTextCursor;
class Ui_QHelpDialog;

class QY2HelpDialog : public QDialog
{
    Q_OBJECT

public:
    QY2HelpDialog( const QString &helpText, QWidget *parent );
    ~QY2HelpDialog();
    void setHelpText( const QString &helpText );

public slots:
    void searchStringChanged( QString );

private:
    Ui_QHelpDialog *_ui;
    QList<QTextCursor> _marks;
};

#endif


#ifndef _QY2StyleEditor_h
#define _QY2StyleEditor_h

#include <QDialog>
#include "ui_QStyleEditor.h"


/**
 * @short Stylesheet Editor Dialog
 **/

class QY2StyleEditor : public QDialog
{
    Q_OBJECT

public:

    /**
    * Constructor.
    **/
    QY2StyleEditor( QWidget *parent );

    /**
    * Destructor.
    **/
    ~QY2StyleEditor();

private slots:

    /**
    * sets the text written in the text browser as style sheet
    **/
    void slotApplyStyle();

    /**
    * loads a style sheet form a file
    **/
    void slotLoadFile();

    /**
    * triggers style sheet auto apply if user has selected 
    **/
    void slotTextChanged();

private:
     Ui_QStyleEditor ui;

};

#endif


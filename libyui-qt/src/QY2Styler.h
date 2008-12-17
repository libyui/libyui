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

  File:		QY2Styler.h

  Author:	Stefan Kulow <coolo@suse.de>

/-*/


#ifndef QY2Styler_h
#define QY2Styler_h

#include <QObject>
#include <QHash>
#include <QString>
#include <QImage>
#include <QMap>


class QY2Styler : public QObject
{
    Q_OBJECT

protected:

    /**
     * Constructor. Use the static styler() function instead to return the
     * singleton for this class. 
     **/
    QY2Styler( QObject * parent );
    
public:

    static QY2Styler * styler();

    void loadStyleSheet( const QString &file );         // reads style sheet form a file
    void setStyleSheet( const QString & text );      
    QString themeDir() const;
    void registerWidget( QWidget *widget );
    void unregisterWidget( QWidget *widget );
    void registerChildWidget( QWidget *parent, QWidget *widget );
    QString textStyle() const { return _textStyle; }

    bool updateRendering( QWidget *wid );

protected:
    void renderParent( QWidget *wid );
    QImage getScaled( const QString name, const QSize & size );

    /**
     * Search and replace some self-defined macros in the style sheet.
     * Among other things, expands the file name inside url( filename.png ) in
     * the style sheet with the full path.
     **/
    void processUrls( QString & text );
    
    /*
     * Reimplemented from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

    
private:
    
    struct BackgrInfo
    {
	QString filename;
	QImage pix;
        QImage scaled;
        QSize lastscale;
	bool full;
    };

    QHash<QString,BackgrInfo> _backgrounds;
    QMap<QWidget*, QList< QWidget* > > _children;
    QString _style;
    QString _textStyle;
};


#endif // QY2Styler_h

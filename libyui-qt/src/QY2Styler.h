#ifndef QY2STYLER_H
#define QY2STYLER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QImage>
#include <QMap>

class QY2Styler : public QObject
{
    Q_OBJECT

public:
    QY2Styler( QObject *parent );

    void setStyleSheet( const QString &file );
    QString themeDir() const;
    void registerWidget( QWidget *widget );
    void unregisterWidget( QWidget *widget );
    void registerChildWidget( QWidget *parent, QWidget *widget );
    QString textStyle() const { return _textStyle; }

    static QY2Styler *self() {
        return _self;
    }

    bool updateRendering( QWidget *wid );

protected:
    void renderParent( QWidget *wid );
    QImage getScaled( const QString name, const QSize & size );

    void processUrls(QString &text);
    /*
     * Reimplemented from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

private:
    struct BackgrInfo {
	QString filename;
	QImage pix;
        QImage scaled;
        QSize lastscale;
	bool full;
    };

    QHash<QString,BackgrInfo> _backgrounds;
    QMap<QWidget*, QList< QWidget* > > _children;

    static QY2Styler *_self;

    QString _style;
    QString _textStyle;
};

#endif

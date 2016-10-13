/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

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

#define HIGH_CONTRAST_STYLE_SHEET "highcontrast.qss"
#define DEFAULT_STYLE_SHEET "style.qss"

class QY2Styler : public QObject
{
    Q_OBJECT

protected:

    /**
     * Constructor. Use the static styler() function instead to return the
     * singleton for this class.
     **/
    QY2Styler( QObject * parent,
               const QString & defaultStyleSheet = "",
               const QString & alternateStyleSheet = "" );

public:

    static QY2Styler * styler();

    /**
     * Determines if an style sheet exists.
     *
     * \param file Filename. It should live in the themeDir() directory.
     * \return true if the file was found; false otherwise.
     */
    bool styleSheetExists( const QString & file );

    /**
     * Loads and apply a style sheet from a file.
     *
     * \param file Filename. It should live in the themeDir() directory.
     * \return true if the file was found (and applied); false otherwise.
     */
    bool loadStyleSheet( const QString &file );

    /**
     * Applies a style sheet from a string.
     *
     * \param text Style sheet content.
     */
    void setStyleSheet( const QString & text );

    /**
     * Loads the default stylesheet.
     *
     * The default stylesheet is determined by the environment variable Y2STYLE.
     * If this variable is not set, the DEFAULT_STYLE_SHEET style sheet will be used.
     *
     * \return true if the stylesheet was loaded; false otherwise.
     */
    bool loadDefaultStyleSheet();

    /**
     * Loads the alternate stylesheet
     *
     * The alternate stylesheet is determined by the environment variable Y2COLORMODE.
     * If this variable is not set, the HIGH_COLOR_STYLE_SHEET style sheet will be used.
     *
     * \return true if the stylesheet was loaded; false otherwise.
     */
    bool loadAlternateStyleSheet();

    /**
     * Returns the path to the style sheets directory.
     */
    QString themeDir() const;

    /**
     * Registers a widget and applies the style sheet
     *
     * \param widget Widget to register.
     */
    void registerWidget( QWidget *widget );

    /**
     * Unregisters a widget.
     *
     * \param widget Widget to unregister.
     */
    void unregisterWidget( QWidget *widget );


    /**
     * Registers a child widget.
     *
     * \param parent Parent widget.
     * \param widget Widget to register.
     */
    void registerChildWidget( QWidget *parent, QWidget *widget );

    QString textStyle() const { return _textStyle; }

    /**
     * Set style sheet for the default theme
     *
     * If the style sheet does not exists, it won't be changed.
     *
     * \param styleSheet Style sheet file name
     */
    void setDefaultStyleSheet(const QString & styleSheet);

    /**
     * Set style sheet for the alternate theme
     *
     * If the style sheet does not exists, it won't be changed.
     *
     * \param styleSheet Style sheet file name
     */
    void setAlternateStyleSheet(const QString & styleSheet);

    /**
     * Toggle between default/alternate style sheets.
     */
    void toggleAlternateStyleSheet();

    /**
     * Determines if the alternate style is being used.
     */
    bool usingAlternateStyleSheet() { return _usingAlternateStyleSheet; }

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

    QString _currentStyleSheet;
    QString _defaultStyleSheet = DEFAULT_STYLE_SHEET;
    QString _alternateStyleSheet = HIGH_CONTRAST_STYLE_SHEET;
    bool _usingAlternateStyleSheet = false;

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
    // remember all registered widgets to allow styling not only for
    // the explicitly requested children widgets (stored in _children)
    QList< QWidget* > _registered_widgets;
    QString _style;
    QString _textStyle;
};


#endif // QY2Styler_h

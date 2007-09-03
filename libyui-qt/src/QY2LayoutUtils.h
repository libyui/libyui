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

  File:	      QY2LayoutUtils.h

  Author:     Stefan Hundhammer <sh@suse.de>

  These are pure Qt functions - they can be used independently of YaST2.

/-*/


#ifndef QY2LayoutUtils_h
#define QY2LayoutUtils_h

class QWidget;


/**
 * Add vertical stretchable space.
 **/
QWidget * addVStretch( QWidget * parent );

/**
 * Add horizontal stretchable space.
 **/
QWidget * addHStretch( QWidget * parent );

/**
 * Add a fixed height vertical space
 **/
QWidget * addVSpacing( QWidget * parent, int height = 8 );

/**
 * Add a fixed height horizontal space
 **/
QWidget * addHSpacing( QWidget * parent, int width = 8 );

/**
 * Limit a QSize to what is available on the same screen as 'widget'.
 **/
QSize limitToScreenSize( const QWidget * widget, const QSize & desiredSize );
QSize limitToScreenSize( const QWidget * widget, int width, int height );

#endif // ifndef QY2LayoutUtils_h

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

// -*- c++ -*-

#ifndef QY2LayoutUtils_h
#define QY2LayoutUtils_h

class QWidget;


/**
 * Add vertical stretchable space.
 **/
void addVStretch( QWidget * parent );

/**
 * Add horizontal stretchable space.
 **/
void addHStretch( QWidget * parent );

/**
 * Add a fixed height vertical space
 **/
void addVSpacing( QWidget * parent, int height = 8 );

/**
 * Add a fixed height horizontal space
 **/
void addHSpacing( QWidget * parent, int width = 8 );


#endif // ifndef QY2LayoutUtils_h

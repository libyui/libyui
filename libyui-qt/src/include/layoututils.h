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

  File:	      layoututils.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef layoututils_h
#define layoututils_h

class QWidget;

#define YQWIDGET_BORDER 3

/**
 * Layout one widget with a standard border around it
 */
void oneWithBorder(QWidget *parent, QWidget *widget);

/**
 * Layout two widget vertically with a standard border
 */
void oneUponTheOther(QWidget *parent, QWidget *upper, QWidget *lower);

#endif // layoututils_h

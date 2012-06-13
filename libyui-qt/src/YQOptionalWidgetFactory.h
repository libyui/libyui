/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                        ***  Qt4 plugin  ***                                         ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

  File:		YQOptionalWidgetFactory.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQOptionalWidgetFactory_h
#define YQOptionalWidgetFactory_h


#include "YOptionalWidgetFactory.h"

#include "YQBarGraph.h"
#include "YQDateField.h"
#include "YQDownloadProgress.h"
#include "YQDumbTab.h"
#include "YQMultiProgressMeter.h"
#include "YQPartitionSplitter.h"
#include "YQSlider.h"
#include "YQTimeField.h"

class YQWizard;

using std::string;
using std::vector;



/**
 * Widget factory for optional ("special") widgets.
 *
 * Remember to always check with the corresponding "has..()" method if the
 * current UI actually provides the requested widget. Otherwise the
 * "create...()" method will throw an exception.
 **/
class YQOptionalWidgetFactory: public YOptionalWidgetFactory
{
public:

    //
    // Optional Widgets
    //


    virtual bool			hasWizard();
    virtual YWizard *			createWizard		( YWidget *   		parent,
								  const std::string & 	backButtonLabel,
								  const std::string & 	abortButtonLabel,
								  const std::string & 	nextButtonLabel,
								  YWizardMode 	wizardMode = YWizardMode_Standard );

    virtual bool			hasDumbTab();
    virtual YQDumbTab *			createDumbTab		( YWidget * parent );

    virtual bool			hasSlider();
    virtual YQSlider *			createSlider		( YWidget *		parent,
								  const std::string	&	label,
								  int 			minVal,
								  int 			maxVal,
								  int 			initialVal );

    virtual bool			hasDateField();
    virtual YQDateField *		createDateField		( YWidget * parent, const std::string & label );

    virtual bool			hasTimeField();
    virtual YQTimeField *		createTimeField		( YWidget * parent, const std::string & label );

    virtual bool			hasBarGraph();
    virtual YQBarGraph *		createBarGraph		( YWidget * parent );

    virtual bool			hasPatternSelector();
    virtual YWidget *		        createPatternSelector	( YWidget * parent, long modeFlags = 0 );

    virtual bool			hasSimplePatchSelector();
    virtual YWidget *	                createSimplePatchSelector( YWidget * parent, long modeFlags = 0 );

    virtual bool			hasMultiProgressMeter();
    virtual YQMultiProgressMeter *	createMultiProgressMeter( YWidget * parent, YUIDimension dim, const vector<float> & maxValues );

    virtual bool			hasPartitionSplitter();
    virtual YQPartitionSplitter *	createPartitionSplitter	( YWidget * 		parent,
								  int 			usedSize,
								  int 			totalFreeSize,
								  int 			newPartSize,
								  int 			minNewPartSize,
								  int 			minFreeSize,
								  const std::string &	usedLabel,
								  const std::string &	freeLabel,
								  const std::string &	newPartLabel,
								  const std::string &	freeFieldLabel,
								  const std::string &	newPartFieldLabel );

    virtual bool			hasDownloadProgress();
    virtual YQDownloadProgress *	createDownloadProgress	( YWidget * 		parent,
								  const std::string & 	label,
								  const std::string & 	filename,
								  YFileSize_t		expectedFileSize );

    virtual bool                        hasTimezoneSelector();
    virtual YTimezoneSelector *         createTimezoneSelector( YWidget * parent,
								const std::string & pixmap,
								const map<string,string> & timezones );

    virtual bool			hasGraph();
    virtual YGraph *			createGraph( YWidget * parent, const std::string & filename,
						     const std::string & layoutAlgorithm );
    virtual YGraph *			createGraph( YWidget * parent, graph_t * graph );

    virtual bool			hasContextMenu();

protected:

    friend class YQUI;

    /**
     * Constructor.
     *
     * Use YUI::optionalWidgetFactory() to get the singleton for this class.
     **/
    YQOptionalWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~YQOptionalWidgetFactory();

}; // class YQOptionalWidgetFactory



#endif // YQOptionalWidgetFactory_h

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
								  const string & 	backButtonLabel,
								  const string & 	abortButtonLabel,
								  const string & 	nextButtonLabel,
								  YWizardMode 	wizardMode = YWizardMode_Standard );

    virtual bool			hasDumbTab();
    virtual YQDumbTab *			createDumbTab		( YWidget * parent );

    virtual bool			hasSlider();
    virtual YQSlider *			createSlider		( YWidget *		parent,
								  const string	&	label,
								  int 			minVal,
								  int 			maxVal,
								  int 			initialVal );

    virtual bool			hasDateField();
    virtual YQDateField *		createDateField		( YWidget * parent, const string & label );

    virtual bool			hasTimeField();
    virtual YQTimeField *		createTimeField		( YWidget * parent, const string & label );

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
								  const string &	usedLabel,
								  const string &	freeLabel,
								  const string &	newPartLabel,
								  const string &	freeFieldLabel,
								  const string &	newPartFieldLabel );

    virtual bool			hasDownloadProgress();
    virtual YQDownloadProgress *	createDownloadProgress	( YWidget * 		parent,
								  const string & 	label,
								  const string & 	filename,
								  YFileSize_t		expectedFileSize );

    virtual bool                        hasTimezoneSelector();
    virtual YTimezoneSelector *         createTimezoneSelector( YWidget * parent,
								const string & pixmap,
								const map<string,string> & timezones );

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

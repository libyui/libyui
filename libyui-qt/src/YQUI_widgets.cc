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

  File:	      	YUIQt_widgets.cc

  Author:    	Stefan Hundhammer <sh@suse.de>
  Maintainer: 	Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQUI.h"

#include "YQAlignment.h"
#include "YQBarGraph.h"
#include "YQCheckBox.h"
#include "YQCheckBoxFrame.h"
#include "YQColoredLabel.h"
#include "YQComboBox.h"
#include "YQDate.h"
#include "YQDialog.h"
#include "YQDownloadProgress.h"
#include "YQDumbTab.h"
#include "YQEmpty.h"
#include "YQFrame.h"
#include "YQImage.h"
#include "YQIntField.h"
#include "YQLabel.h"
#include "YQLogView.h"
#include "YQMenuButton.h"
#include "YQMultiLineEdit.h"
#include "YQMultiProgressMeter.h"
#include "YQMultiSelectionBox.h"
#include "YQPackageSelector.h"
#include "YQPartitionSplitter.h"
#include "YQPatternSelector.h"
#include "YQProgressBar.h"
#include "YQPushButton.h"
#include "YQRadioButton.h"
#include "YQRadioButtonGroup.h"
#include "YQReplacePoint.h"
#include "YQRichText.h"
#include "YQSelectionBox.h"
#include "YQSlider.h"
#include "YQSpacing.h"
#include "YQSplit.h"
#include "YQSquash.h"
#include "YQTable.h"
#include "YQTextEntry.h"
#include "YQTime.h"
#include "YQTree.h"
#include "YQWizard.h"



YContainerWidget * YQUI::createReplacePoint( YWidget *		parent,
					      YWidgetOpt & 	opt )
{
    return new YQReplacePoint( ( (QWidget *)parent->widgetRep() ), opt );
}

YWidget * YQUI::createEmpty		( YWidget *		parent,
					  YWidgetOpt & 		opt )
{
    return new YQEmpty( (QWidget *) parent->widgetRep(), opt );
}

YWidget * YQUI::createSpacing		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  float 		size,
					  bool 			horizontal,
					  bool 			vertical )
{
    return new YQSpacing( (QWidget *) parent->widgetRep(), opt, size, horizontal, vertical);
}

YContainerWidget * YQUI::createFrame	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQFrame ( ( (QWidget *) parent->widgetRep() ), opt, label );
}

YContainerWidget * YQUI::createCheckBoxFrame	( YWidget *		parent,
						  YWidgetOpt & 		opt,
						  const YCPString & 	label,
						  bool			initialValue )
{
    return new YQCheckBoxFrame ( ( (QWidget *) parent->widgetRep() ), opt, label, initialValue );
}

YContainerWidget * YQUI::createSplit	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YUIDimension 		dimension )
{
    return new YQSplit( ( (QWidget *)parent->widgetRep() ), opt, dimension);
}

YContainerWidget * YQUI::createAlignment( YWidget *		parent,
					   YWidgetOpt & 	opt,
					   YAlignmentType 	halign,
					   YAlignmentType 	valign )
{
    return new YQAlignment( ( (QWidget *)parent->widgetRep() ), opt, halign, valign);
}

YContainerWidget * YQUI::createSquash	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  bool 			hsquash,
					  bool 			vsquash )
{
    return new YQSquash( ( (QWidget *)parent->widgetRep() ), opt, hsquash, vsquash);
}

YWidget * YQUI::createLabel		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	text )
{
    return new YQLabel( (QWidget *) ( parent->widgetRep() ), opt, text);
}


bool YQUI::hasDate()
{
    return true;
}

YWidget * YQUI::createDate		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	date )
{
    return new YQDate( (QWidget *) ( parent->widgetRep() ), opt, label, date);
}


bool YQUI::hasTime()
{
    return true;
}


YWidget * YQUI::createTime		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	time )
{
    return new YQTime( (QWidget *) ( parent->widgetRep() ), opt, label, time);
}


YWidget * YQUI::createLogView		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString &	label,
					  int 			visibleLines,
					  int 			maxLines )
{
    return new YQLogView( (QWidget *) ( parent->widgetRep() ), opt, label, visibleLines, maxLines);
}

YWidget * YQUI::createRichText		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	text )
{
    return new YQRichText( (QWidget *) ( parent->widgetRep() ), opt, text);
}

YWidget * YQUI::createPackageSelector	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	floppyDevice )
{
    _auto_activate_dialogs = false;
    YWidget * w = 0;
    
    try
    {
	w = new YQPackageSelector( (QWidget *) ( parent->widgetRep() ), opt );
    }
    catch (const std::exception & e)
    {
	y2error( "Caught std::exception: %s", e.what() );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    catch (...)
    {
	y2error( "Caught unspecified exception." );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    return w;
}

YWidget * YQUI::createPkgSpecial	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	subwidget )
{
    y2error( "The Qt UI does not support PkgSpecial subwidgets!" );
    return 0;
}

YWidget * YQUI::createPushButton	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    YQDialog * dialog = dynamic_cast<YQDialog *> ( parent->yDialog() );
    return new YQPushButton( (QWidget *) ( parent->widgetRep() ), dialog, opt, label);
}

YWidget * YQUI::createMenuButton	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQMenuButton( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YQUI::createCheckBox		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  bool 			checked )
{
    return new YQCheckBox( (QWidget *) ( parent->widgetRep() ), opt, label, checked);
}

YWidget * YQUI::createRadioButton	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YRadioButtonGroup *	rbg,
					  const YCPString & 	label,
					  bool 			checked)
{
    return new YQRadioButton( (QWidget *) ( parent->widgetRep() ), opt, rbg, label, checked);
}

YContainerWidget * YQUI::createRadioButtonGroup( YWidget *	parent,
						  YWidgetOpt & 	opt )
{
    return new YQRadioButtonGroup( (QWidget *) ( parent->widgetRep() ), opt );
}

YWidget * YQUI::createTextEntry	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	text )
{
    return new YQTextEntry( (QWidget *) ( parent->widgetRep() ), opt, label, text);
}

YWidget * YQUI::createMultiLineEdit	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	initialText )
{
    return new YQMultiLineEdit( (QWidget *) ( parent->widgetRep() ), opt, label, initialText);
}

YWidget * YQUI::createSelectionBox	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQSelectionBox( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YQUI::createMultiSelectionBox( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString &	label )
{
    return new YQMultiSelectionBox( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YQUI::createComboBox		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQComboBox( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YQUI::createTree		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQTree( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YQUI::createTable		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  vector<string> 	header	)
{
    return new YQTable( (QWidget *) ( parent->widgetRep() ), opt, header);
}

YWidget * YQUI::createProgressBar	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPInteger & 	maxProgress,
					  const YCPInteger & 	progress )
{
    return new YQProgressBar( (QWidget *) ( parent->widgetRep() ), opt, label, maxProgress, progress );
}


YWidget * YQUI::createImage		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YCPByteblock 		imagedata,
					  YCPString 		default_text )
{
    return new YQImage( (QWidget *) ( parent->widgetRep() ), opt, imagedata );
}

YWidget * YQUI::createImage		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YCPString 		file_name,
					  YCPString 		default_text )
{
    return new YQImage( (QWidget *) ( parent->widgetRep() ), opt, file_name );
}


YWidget * YQUI::createIntField		( YWidget *		parent,
					  YWidgetOpt &		opt,
					  const YCPString &	label,
					  int 			minValue,
					  int 			maxValue,
					  int 			initialValue )
{
    return new YQIntField( (QWidget *) ( parent->widgetRep() ),
			   opt,
			   label,
			   minValue,
			   maxValue,
			   initialValue );
}



//========= Optional widgets ========================================================


bool YQUI::hasBarGraph()
{
    return true;
}

YWidget * YQUI::createBarGraph( YWidget * parent, YWidgetOpt & opt)
{
    return new YQBarGraph( (QWidget *) ( parent->widgetRep() ), opt );
}


bool YQUI::hasColoredLabel()
{
    return QColor::numBitPlanes() >= 15;
}

YWidget * YQUI::createColoredLabel	( YWidget *	parent,
					  YWidgetOpt & 	opt,
					  YCPString 	label,
					  YColor 	foreground,
					  YColor 	background,
					  int 		margin )
{
    if ( QColor::numBitPlanes() < 15 )
    {
	y2error( "Display depth is %d, need at least 15 for ColoredLabel",
		 QColor::numBitPlanes() );

	return 0;
    }


    return new YQColoredLabel( (QWidget *) ( parent->widgetRep() ), opt,
			       label, foreground, background, margin );
}


bool YQUI::hasDownloadProgress()
{
    return true;
}

YWidget * YQUI::createDownloadProgress	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	filename,
					  int 			expectedSize)
{
    return new YQDownloadProgress( (QWidget *) ( parent->widgetRep() ),
				   opt,
				   label,
				   filename,
				   expectedSize );
}


bool YQUI::hasDumbTab()
{
    return true;
}

YWidget * YQUI::createDumbTab		( YWidget *		parent,
					  YWidgetOpt & 		opt )
{
    return new YQDumbTab( (QWidget *) ( parent->widgetRep() ), opt );
}


bool YQUI::hasMultiProgressMeter()
{
    return true;
}


YWidget * YQUI::createMultiProgressMeter( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  bool 			horizontal,
					  const YCPList & 	maxValues )
{
    return new YQMultiProgressMeter( (QWidget *) ( parent->widgetRep() ),
				     opt,
				     horizontal,
				     maxValues );
}


bool YQUI::hasSlider()
{
    return true;
}


YWidget * YQUI::createSlider( YWidget *		parent,
			       YWidgetOpt &		opt,
			       const YCPString &	label,
			       int 			minValue,
			       int 			maxValue,
			       int 			initialValue )
{
    return new YQSlider( (QWidget *) ( parent->widgetRep() ),
			 opt,
			 label,
			 minValue,
			 maxValue,
			 initialValue );
}


bool YQUI::hasPartitionSplitter()
{
    return true;
}

YWidget * YQUI::createPartitionSplitter( YWidget *		parent,
					 YWidgetOpt &		opt,
					 int 			usedSize,
					 int 			totalFreeSize,
					 int 			newPartSize,
					 int 			minNewPartSize,
					 int 			minFreeSize,
					 const YCPString &	usedLabel,
					 const YCPString &	freeLabel,
					 const YCPString &	newPartLabel,
					 const YCPString &	freeFieldLabel,
					 const YCPString &	newPartFieldLabel )
{
    return new YQPartitionSplitter( (QWidget *) ( parent->widgetRep() ),
				    opt,
				    usedSize,
				    totalFreeSize,
				    newPartSize,
				    minNewPartSize,
				    minFreeSize,
				    usedLabel,
				    freeLabel,
				    newPartLabel,
				    freeFieldLabel,
				    newPartFieldLabel );
}



bool YQUI::hasPatternSelector()
{
    return true;
}


YWidget * YQUI::createPatternSelector( YWidget *		parent,
				       YWidgetOpt &		opt )
{
    YWidget * w = 0;
    
    try
    {
	w = new YQPatternSelector( (QWidget *) ( parent->widgetRep() ), opt );
    }
    catch (const std::exception & e)
    {
	y2error( "Caught std::exception: %s", e.what() );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    catch (...)
    {
	y2error( "Caught unspecified exception." );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    return w;
}



bool YQUI::hasWizard()
{
    return true;
}


YWidget * YQUI::createWizard( YWidget *parent, YWidgetOpt & opt,
			      const YCPValue & backButtonId,	const YCPString & backButtonLabel,
			      const YCPValue & abortButtonId,	const YCPString & abortButtonLabel,
			      const YCPValue & nextButtonId,	const YCPString & nextButtonLabel  )
{
    return new YQWizard( (QWidget *) ( parent->widgetRep() ), opt,
			 backButtonId,	backButtonLabel,
			 abortButtonId,	abortButtonLabel,
			 nextButtonId,	nextButtonLabel  );
}


// EOF

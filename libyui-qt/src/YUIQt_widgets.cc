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

  Textdomain 	"packages-qt"

/-*/

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YUIQt.h"

#include "YQAlignment.h"
#include "YQBarGraph.h"
#include "YQCheckBox.h"
#include "YQComboBox.h"
#include "YQColoredLabel.h"
#include "YQDialog.h"
#include "YQDownloadProgress.h"
#include "YQEmpty.h"
#include "YQFrame.h"
#include "YQImage.h"
#include "YQIntField.h"
#include "YQLabel.h"
#include "YQLogView.h"
#include "YQMenuButton.h"
#include "YQMultiLineEdit.h"
#include "YQMultiSelectionBox.h"
#include "YQPartitionSplitter.h"
#include "YQProgressBar.h"
#include "YQPackageSelector.h"
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
#include "YQTree.h"



YContainerWidget * YUIQt::createReplacePoint( YWidget *		parent,
					      YWidgetOpt & 	opt )
{
    return new YQReplacePoint( ( (QWidget *)parent->widgetRep() ), opt );
}

YWidget * YUIQt::createEmpty		( YWidget *		parent,
					  YWidgetOpt & 		opt )
{
    return new YQEmpty( (QWidget *) parent->widgetRep(), opt );
}

YWidget * YUIQt::createSpacing		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  float 		size,
					  bool 			horizontal,
					  bool 			vertical )
{
    return new YQSpacing( (QWidget *) parent->widgetRep(), opt, size, horizontal, vertical);
}

YContainerWidget * YUIQt::createFrame	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQFrame ( ( (QWidget *) parent->widgetRep() ), opt, label );
}

YContainerWidget * YUIQt::createSplit	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YUIDimension 		dimension )
{
    return new YQSplit( ( (QWidget *)parent->widgetRep() ), opt, dimension);
}

YContainerWidget * YUIQt::createAlignment( YWidget *		parent,
					   YWidgetOpt & 	opt,
					   YAlignmentType 	halign,
					   YAlignmentType 	valign )
{
    return new YQAlignment( ( (QWidget *)parent->widgetRep() ), opt, halign, valign);
}

YContainerWidget * YUIQt::createSquash	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  bool 			hsquash,
					  bool 			vsquash )
{
    return new YQSquash( ( (QWidget *)parent->widgetRep() ), opt, hsquash, vsquash);
}

YWidget * YUIQt::createLabel		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	text )
{
    return new YQLabel( (QWidget *) ( parent->widgetRep() ), opt, text);
}

YWidget * YUIQt::createLogView		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString &	label,
					  int 			visibleLines,
					  int 			maxLines )
{
    return new YQLogView( (QWidget *) ( parent->widgetRep() ), opt, label, visibleLines, maxLines);
}

YWidget * YUIQt::createRichText		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	text )
{
    return new YQRichText( (QWidget *) ( parent->widgetRep() ), opt, text);
}

YWidget * YUIQt::createPackageSelector	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	floppyDevice )
{
    _auto_activate_dialogs = false;
    return new YQPackageSelector( (QWidget *) ( parent->widgetRep() ), opt, floppyDevice );
}

YWidget * YUIQt::createPkgSpecial	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	subwidget )
{
    y2error( "The Qt UI does not support PkgSpecial subwidgets!" );
    return 0;
}

YWidget * YUIQt::createPushButton	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    YQDialog * dialog = dynamic_cast<YQDialog *> ( parent->yDialog() );
    return new YQPushButton( (QWidget *) ( parent->widgetRep() ), dialog, opt, label);
}

YWidget * YUIQt::createMenuButton	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQMenuButton( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YUIQt::createCheckBox		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  bool 			checked )
{
    return new YQCheckBox( (QWidget *) ( parent->widgetRep() ), opt, label, checked);
}

YWidget * YUIQt::createRadioButton	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YRadioButtonGroup *	rbg,
					  const YCPString & 	label,
					  bool 			checked)
{
    return new YQRadioButton( (QWidget *) ( parent->widgetRep() ), opt, rbg, label, checked);
}

YContainerWidget * YUIQt::createRadioButtonGroup( YWidget *	parent,
						  YWidgetOpt & 	opt )
{
    return new YQRadioButtonGroup( (QWidget *) ( parent->widgetRep() ), opt );
}

YWidget * YUIQt::createTextEntry	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	text )
{
    return new YQTextEntry( (QWidget *) ( parent->widgetRep() ), opt, label, text);
}

YWidget * YUIQt::createMultiLineEdit	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPString & 	initialText )
{
    return new YQMultiLineEdit( (QWidget *) ( parent->widgetRep() ), opt, label, initialText);
}

YWidget * YUIQt::createSelectionBox	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQSelectionBox( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YUIQt::createMultiSelectionBox( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString &	label )
{
    return new YQMultiSelectionBox( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YUIQt::createComboBox		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQComboBox( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YUIQt::createTree		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label )
{
    return new YQTree( (QWidget *) ( parent->widgetRep() ), opt, label);
}

YWidget * YUIQt::createTable		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  vector<string> 	header	)
{
    return new YQTable( (QWidget *) ( parent->widgetRep() ), opt, header);
}

YWidget * YUIQt::createProgressBar	( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  const YCPString & 	label,
					  const YCPInteger & 	maxProgress,
					  const YCPInteger & 	progress )
{
    return new YQProgressBar( (QWidget *) ( parent->widgetRep() ), opt, label, maxProgress, progress );
}

YWidget * YUIQt::createImage		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YCPByteblock 		imagedata,
					  YCPString 		default_text )
{
    return new YQImage( (QWidget *) ( parent->widgetRep() ), opt, imagedata );
} 

YWidget * YUIQt::createImage		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  YCPString 		file_name,
					  YCPString 		default_text )
{
    return new YQImage( (QWidget *) ( parent->widgetRep() ), opt, file_name );
}

YWidget * YUIQt::createImage		( YWidget *		parent,
					  YWidgetOpt & 		opt,
					  ImageType 		img,
					  YCPString 		default_text )
{
    return new YQImage( (QWidget *) ( parent->widgetRep() ), opt, img );
}

YWidget * YUIQt::createIntField		( YWidget *		parent,
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


bool YUIQt::hasBarGraph()
{
    return true;
}

YWidget * YUIQt::createBarGraph( YWidget * parent, YWidgetOpt & opt)
{
    return new YQBarGraph( (QWidget *) ( parent->widgetRep() ), opt );
}


bool YUIQt::hasColoredLabel()
{
    return QColor::numBitPlanes() >= 15;
}

YWidget * YUIQt::createColoredLabel	( YWidget *	parent,
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


bool YUIQt::hasDownloadProgress()
{
    return true;
}

YWidget * YUIQt::createDownloadProgress	( YWidget *		parent,
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


bool YUIQt::hasSlider()
{
    return true;
}


YWidget * YUIQt::createSlider( YWidget *		parent,
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


bool YUIQt::hasPartitionSplitter()
{
    return true;
}

YWidget * YUIQt::createPartitionSplitter( YWidget *		parent,
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


// EOF

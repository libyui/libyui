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

  File:	      YQPkgTechnicalDetailsView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>

#include "YQPkgTechnicalDetailsView.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgTechnicalDetailsView::YQPkgTechnicalDetailsView( QWidget * parent, bool youMode )
    : YQPkgGenericDetailsView( parent )
    , _youMode( youMode )
{
}


YQPkgTechnicalDetailsView::~YQPkgTechnicalDetailsView()
{
    // NOP
}


void
YQPkgTechnicalDetailsView::showDetails( PMObjectPtr pmObj )
{
    _pmObj = pmObj;

    if ( ! pmObj )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( pmObj );

    string name = pmObj->name();
    y2debug( "Showing technical details for PMObject %s", name.c_str() );

    PMPackagePtr candidate = pmObj->getCandidateObj();
    PMPackagePtr installed = pmObj->getInstalledObj();

#if 0
    html_text += complexTable( installed, installed );
#else
    if ( candidate && installed && candidate != installed )
    {
	html_text += complexTable( installed, candidate );
    }
    else
    {
	if ( candidate )
	    html_text += simpleTable( candidate );

	if ( installed )
	    html_text += simpleTable( installed );
    }
#endif

    setTextFormat( Qt::RichText );
    setText( html_text );
}


QString
YQPkgTechnicalDetailsView::authorsListCell( PMPackagePtr pkg ) const
{
    QString html = "<td align=top>";
    QString line;
    std::list<std::string> authors = pkg->authors();
    std::list<std::string>::const_iterator it = authors.begin();

    while ( it != authors.end() )
    {
	line = fromUTF8(*it);
	line = htmlEscape( line );
	html += line + "<br>";
	++it;
    }

    html += "</td>";

    return html;
}


QString
YQPkgTechnicalDetailsView::formatRpmGroup( PMPackagePtr pkg ) const
{
    std::string group = Y2PM::packageManager().translatedRpmGroup( pkg->group_ptr() );
    return fromUTF8( group );
}


QString
YQPkgTechnicalDetailsView::simpleTable( PMPackagePtr pkg )
{
    QString html = "<br>" +
	table(
	       row( hcell( _( "Version:"	) ) + cell( pkg->version() + "-" + pkg->release() ) ) +
	       row( hcell( _( "Build Time:"	) ) + cell( pkg->buildtime()			  ) ) +
	       ( pkg->isInstalledObj() ?
		 row( hcell( _( "Install Time:" ) ) + cell( pkg->installtime()			  ) )
		 : "" ) +
	       row( hcell( _( "Package Group:"	) ) + cell( formatRpmGroup( pkg )		  ) ) +
	       row( hcell( _( "License:"	) ) + cell( pkg->license()			  ) ) +
	       row( hcell( _( "Installed Size:" ) ) + cell( pkg->size().form()			  ) ) +
	       row( hcell( _youMode ? _( "Download Size:") : _( "Archive Size:" ) )
		                                    + cell( pkg->archivesize().form()	  	  ) ) +
	       row( hcell( _( "Distribution:"	) ) + cell( pkg->distribution()			  ) ) +
	       row( hcell( _( "Vendor:"		) ) + cell( pkg->vendor()			  ) ) +
	       row( hcell( _( "Packager:"	) ) + cell( pkg->packager()			  ) ) +
	       row( hcell( _( "Architecture:"	) ) + cell( pkg->arch()				  ) ) +
	       row( hcell( _( "OS:"		) ) + cell( pkg->os()				  ) ) +
	       row( hcell( _( "Build Host:"	) ) + cell( pkg->buildhost()			  ) ) +
	       row( hcell( _( "URL:"		) ) + cell( pkg->url()				  ) ) +
	       row( hcell( _( "Source RPM:"	) ) + cell( pkg->sourceloc()			  ) ) +
	       row( hcell( _( "Location:"	) ) + cell( pkg->location()			  ) ) +
	       row( hcell( _( "Media No.:"	) ) + cell( pkg->medianr()			  ) ) +
	       row( hcell( _( "Authors:"	) ) + authorsListCell( pkg			  ) )
	       );

    return html;
}


QString
YQPkgTechnicalDetailsView::complexTable( PMPackagePtr installed, PMPackagePtr candidate )
{
    PMPackagePtr p1 = candidate;
    PMPackagePtr p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    QString html = "<br>" +
	table(
	       row( hcell( QString("") )	    + hcell( "<b>" + p1_header + "</b>"	    ) + hcell( "<b>" + p2_header + "</b>"     ) ) +

	       row( hcell( _( "Version:"	) ) + cell( p1->version() + "-" + p1->release() ) + cell( p2->version() + "-" + p2->release() ) ) +
	       row( hcell( _( "Build Time:"	) ) + cell( p1->buildtime()			) + cell( p2->buildtime()		      ) ) +
	       row( hcell( _( "Install Time:"	) ) + cell( p1->installtime()			) + cell( p2->installtime()		      ) ) +
	       row( hcell( _( "Package Group:"	) ) + cell( formatRpmGroup( p1 )		) + cell( formatRpmGroup( p2 )		      ) ) +
	       row( hcell( _( "License:"	) ) + cell( p1->license()			) + cell( p2->license()			      ) ) +
	       row( hcell( _( "Installed Size:" ) ) + cell( p1->size().form()			) + cell( p2->size().form()		      ) ) +
	       row( hcell( _youMode ? _( "Download Size:"	) : _( "Archive Size:" ) )
						    + cell( p1->archivesize().form()		) + cell( p2->archivesize().form()	      ) ) +
	       row( hcell( _( "Distribution:"	) ) + cell( p1->distribution()			) + cell( p2->distribution()		      ) ) +
	       row( hcell( _( "Vendor:"		) ) + cell( p1->vendor()			) + cell( p2->vendor()			      ) ) +
	       row( hcell( _( "Packager:"	) ) + cell( p1->packager()			) + cell( p2->packager()		      ) ) +
	       row( hcell( _( "Architecture:"	) ) + cell( p1->arch()				) + cell( p2->arch()			      ) ) +
	       row( hcell( _( "OS:"		) ) + cell( p1->os()				) + cell( p2->os()			      ) ) +
	       row( hcell( _( "Build Host:"	) ) + cell( p1->buildhost()			) + cell( p2->buildhost()		      ) ) +
	       row( hcell( _( "URL:"		) ) + cell( p1->url()				) + cell( p2->url()			      ) ) +
	       row( hcell( _( "Source RPM:"	) ) + cell( p1->sourceloc()			) + cell( p2->sourceloc()		      ) ) +
	       row( hcell( _( "Location:"	) ) + cell( p1->location()			) + cell( p2->location()		      ) ) +
	       row( hcell( _( "Media No.:"	) ) + cell( p1->medianr()			) + cell( p2->medianr()			      ) ) +
	       row( hcell( _( "Authors:"	) ) + authorsListCell( p1			) + authorsListCell( p2			      ) )
	       );

    return html;
}



#include "YQPkgTechnicalDetailsView.moc.cc"

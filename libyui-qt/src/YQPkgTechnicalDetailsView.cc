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

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qdatetime.h>
#include "YQPkgTechnicalDetailsView.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgTechnicalDetailsView::YQPkgTechnicalDetailsView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgTechnicalDetailsView::~YQPkgTechnicalDetailsView()
{
    // NOP
}


void
YQPkgTechnicalDetailsView::showPkgDetails( PMPackagePtr pkg )
{
    _pkg = pkg;

    if ( ! pkg )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( pkg );

    string name = pkg->name();
    y2debug( "Showing technical details for package %s", name.c_str() );

    PMPackagePtr candidate = pkg->getCandidateObj();
    PMPackagePtr installed = pkg->getInstalledObj();

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
YQPkgTechnicalDetailsView::table( const QString & contents ) const
{
    return "<table border=1 bgcolor=#F0F0F0>" + contents + "</table>";
}


QString
YQPkgTechnicalDetailsView::row( const QString & contents ) const
{
    return "<tr>" + contents + "</tr>";
}


QString
YQPkgTechnicalDetailsView::cell( const QString & contents ) const
{
    return "<td align=top>" + contents + "</td>";
}


QString
YQPkgTechnicalDetailsView::cell( int contents ) const
{
    QString html;
    html.sprintf( "<td align=top>%d</td>", contents );

    return html;
}


QString
YQPkgTechnicalDetailsView::cell( const std::string & contents ) const
{
    return "<td align=top>" + QString::fromUtf8( contents.c_str() ) + "</td>";
}


QString
YQPkgTechnicalDetailsView::hcell( const QString & contents ) const
{
    return "<td align=top bgcolor=#D0D0D0>" + contents + "</td>";
}


QString
YQPkgTechnicalDetailsView::formatAuthorsList( PMPackagePtr pkg ) const
{
    QString html;
    std::list<std::string> authors = pkg->authors();
    std::list<std::string>::const_iterator it = authors.begin();

    while ( it != authors.end() )
    {
	html += fromUTF8(*it) + "<br>";
	++it;
    }

    return html;
}


QString
YQPkgTechnicalDetailsView::formatDate( Date weird_date ) const
{
    time_t seconds = weird_date;

    if ( seconds == 0 )	// Special case: Don't display "1.1.1970 0:00"
	return "";
    
    QDateTime date;
    date.setTime_t( seconds );

    return date.toString( Qt::LocalDate );
}


QString
YQPkgTechnicalDetailsView::simpleTable( PMPackagePtr pkg )
{
    pkg->startRetrieval();

    QString html = "<br>" +
	table(
	       row( hcell( _( "Version:"	) ) + cell( pkg->version()		   ) ) +
	       row( hcell( _( "Release:"	) ) + cell( pkg->release()		   ) ) +
	       row( hcell( _( "Build Time:"	) ) + cell( formatDate( pkg->buildtime() ) ) ) +
	       ( pkg->isInstalledObj() ?
		 row( hcell( _( "Install Time:" ) ) + cell( formatDate( pkg->installtime() ) ) )
		 : "" ) +
	       row( hcell( _( "Package Group:"	) ) + cell( pkg->group()		   ) ) +
	       row( hcell( _( "License:"	) ) + cell( pkg->license()		   ) ) +
	       row( hcell( _( "Installed Size:" ) ) + cell( pkg->size().form()		   ) ) +
	       row( hcell( _( "Archive Size:"	) ) + cell( pkg->archivesize().form()	   ) ) +
	       row( hcell( _( "Distribution:"	) ) + cell( pkg->distribution()		   ) ) +
	       row( hcell( _( "Vendor:"		) ) + cell( pkg->vendor()		   ) ) +
	       row( hcell( _( "Packager:"	) ) + cell( pkg->packager()		   ) ) +
	       row( hcell( _( "Architecture:"	) ) + cell( pkg->arch()			   ) ) +
	       row( hcell( _( "OS:"		) ) + cell( pkg->os()			   ) ) +
	       row( hcell( _( "Build Host:"	) ) + cell( pkg->buildhost()		   ) ) +
	       row( hcell( _( "URL:"		) ) + cell( pkg->url()			   ) ) +
	       row( hcell( _( "Source RPM:"	) ) + cell( pkg->sourcerpm()		   ) ) +
	       row( hcell( _( "Location:"	) ) + cell( pkg->location()		   ) ) +
	       row( hcell( _( "Media No.:"	) ) + cell( pkg->medianr()		   ) ) +
	       row( hcell( _( "Authors:"	) ) + cell( formatAuthorsList( pkg )	   ) )
	       );

    pkg->stopRetrieval();

    return html;
}


QString
YQPkgTechnicalDetailsView::complexTable( PMPackagePtr installed, PMPackagePtr candidate )
{
    PMPackagePtr p1 = candidate;
    PMPackagePtr p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    p1->startRetrieval();
    p2->startRetrieval();

    QString html = "<br>" +
	table(
	       row( hcell( QString("") )	    + hcell( "<b>" + p1_header + "</b>"	    ) + hcell( "<b>" + p2_header + "</b>"     ) ) +

	       row( hcell( _( "Version:"	) ) + cell( p1->version()		    ) + cell( p2->version()		      ) ) +
	       row( hcell( _( "Release:"	) ) + cell( p1->release()		    ) + cell( p2->release()		      ) ) +
	       row( hcell( _( "Build Time:"	) ) + cell( formatDate( p1->buildtime() )   ) + cell( formatDate( p2->buildtime() )   ) ) +
	       row( hcell( _( "Install Time:"	) ) + cell( formatDate( p1->installtime() ) ) + cell( formatDate( p2->installtime() ) ) ) +
	       row( hcell( _( "Package Group:"	) ) + cell( p1->group()			    ) + cell( p2->group()		      ) ) +
	       row( hcell( _( "License:"	) ) + cell( p1->license()		    ) + cell( p2->license()		      ) ) +
	       row( hcell( _( "Installed Size:" ) ) + cell( p1->size().form()		    ) + cell( p2->size().form()		      ) ) +
	       row( hcell( _( "Archive Size:"	) ) + cell( p1->archivesize().form()	    ) + cell( p2->archivesize().form()	      ) ) +
	       row( hcell( _( "Distribution:"	) ) + cell( p1->distribution()		    ) + cell( p2->distribution()	      ) ) +
	       row( hcell( _( "Vendor:"		) ) + cell( p1->vendor()		    ) + cell( p2->vendor()		      ) ) +
	       row( hcell( _( "Packager:"	) ) + cell( p1->packager()		    ) + cell( p2->packager()		      ) ) +
	       row( hcell( _( "Architecture:"	) ) + cell( p1->arch()			    ) + cell( p2->arch()		      ) ) +
	       row( hcell( _( "OS:"		) ) + cell( p1->os()			    ) + cell( p2->os()			      ) ) +
	       row( hcell( _( "Build Host:"	) ) + cell( p1->buildhost()		    ) + cell( p2->buildhost()		      ) ) +
	       row( hcell( _( "URL:"		) ) + cell( p1->url()			    ) + cell( p2->url()			      ) ) +
	       row( hcell( _( "Source RPM:"	) ) + cell( p1->sourcerpm()		    ) + cell( p2->sourcerpm()		      ) ) +
	       row( hcell( _( "Location:"	) ) + cell( p1->location()		    ) + cell( p2->location()		      ) ) +
	       row( hcell( _( "Media No.:"	) ) + cell( p1->medianr() 		    ) + cell( p2->medianr() 		      ) ) +
	       row( hcell( _( "Authors:"	) ) + cell( formatAuthorsList( p1 )	    ) + cell( formatAuthorsList( p2 )	      ) )
	       );

    p1->stopRetrieval();
    p2->stopRetrieval();

    return html;
}



#include "YQPkgTechnicalDetailsView.moc.cc"

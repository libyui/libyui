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

  File:	      YQPkgDescriptionView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qregexp.h>
#include "YQPkgDescriptionView.h"
#include "YQPkgDescriptionDialog.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;


YQPkgDescriptionView::YQPkgDescriptionView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
    setMimeSourceFactory( 0 );
}


YQPkgDescriptionView::~YQPkgDescriptionView()
{
    // NOP
}


void
YQPkgDescriptionView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( selectable );

    QString description = fromUTF8( selectable->theObj()->description() );

    if ( ! description.contains( "<!-- DT:Rich -->" ) )
	description = simpleHtmlParagraphs( description );
    else
	y2debug( "Description for %s is preformatted in DT:Rich", selectable->theObj()->name().c_str() );

    html_text += description;


    setTextFormat( Qt::RichText );
    setText( html_text );
    ensureVisible( 0, 0 );	// Otherwise hyperlinks will be centered
}



QString YQPkgDescriptionView::simpleHtmlParagraphs( QString text )
{
    bool foundAuthorsList = false;
    QString html_text = "<p>";

    QStringList lines = QStringList::split( '\n', text.stripWhiteSpace(),
					    true ); // allowEmptyEntries
    QValueList<QString>::const_iterator it = lines.begin();

    while ( it != lines.end() )
    {
	QString line = htmlEscape( *it ).stripWhiteSpace();

	if ( line.startsWith( "Authors:" ) )
	{
	    line = "<p><b>" + line + "</b><ul>";
	    foundAuthorsList = true;
	}

	if ( foundAuthorsList )
	{
	    if ( ! line.startsWith( "-----" ) && ! line.isEmpty() )
		html_text += "<li>" + line + "</li>";
	}
	else
	{
	    if ( line.isEmpty() )
		html_text += "</p><p>";
	    else
		html_text += " " + line;
	}

	++it;
    }

    if ( foundAuthorsList )
	html_text += "</ul>";
    
    html_text += "</p>";

    return html_text;
}


void
YQPkgDescriptionView::showLink( const QString & url )
{
    if ( url.startsWith( "pkg:" ) )
    {
	QString pkgName = url;
	pkgName.remove( QRegExp( "^pkg:/*" ) );	// Remove leading protocol and slashes
	pkgName.remove( QRegExp( "/*$" ) );	// Remove trailing slashes
	y2milestone( "Hyperlinking to package '%s'", (const char *) pkgName );
	YQPkgDescriptionDialog::showDescriptionDialog( pkgName );
    }
    else
    {
	y2error( "Protocol not supported - can't follow hyperlink '%s'",
		 (const char *) url );
    }
}


void
YQPkgDescriptionView::setSource( const QString & url )
{
    showLink( url );
}


#include "YQPkgDescriptionView.moc"

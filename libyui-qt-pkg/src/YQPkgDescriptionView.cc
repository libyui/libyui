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

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <QRegExp>
#include <QList>
#include "YQPkgDescriptionView.h"
#include "YQPkgDescriptionDialog.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;
using namespace zypp;

YQPkgDescriptionView::YQPkgDescriptionView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
    //FIXME setMimeSourceFactory( 0 );
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

    QString html_text = htmlStart();
    
    html_text += htmlHeading( selectable );

    QString description = fromUTF8( selectable->theObj()->description() );

    if ( ! description.contains( "<!-- DT:Rich -->" ) )
        description = simpleHtmlParagraphs( description );

    html_text += ( "<p>" + description + "</p>");

    // if the object is a patch, show the problem references too
    Patch::constPtr patch = asKind<Patch>(selectable->theObj());
    if ( patch )
    {
        html_text += "<p>";
        html_text += _("References:");
        html_text += "</p>";
        html_text +=  "<ul>";
        
        for ( Patch::ReferenceIterator rit = patch->referencesBegin();
              rit != patch->referencesEnd();
              ++rit )
        {
            html_text +=  QString().sprintf("<li>%s (%s) : %s</li>", rit.id().c_str(),  rit.type().c_str(), rit.title().c_str() );          
        }
        html_text += "</ul>";
    }
    

    html_text += htmlEnd();
    setHtml( html_text );
    //FIXME ensureVisible( 0, 0 );	// Otherwise hyperlinks will be centered
}



QString YQPkgDescriptionView::simpleHtmlParagraphs( QString text )
{
    bool foundAuthorsList = false;
    QString html_text = "<p>";

    QStringList lines = text.trimmed().split( '\n', QString::KeepEmptyParts );
    QStringList::const_iterator it = lines.begin();

    while ( it != lines.end() )
    {
	QString line = htmlEscape( *it ).trimmed();

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
YQPkgDescriptionView::showLink( const QUrl & url )
{
    if ( url.scheme() == "pkg" )
    {
	QString pkgName = url.authority();
	yuiMilestone() << "Hyperlinking to package \"" << pkgName << "\"" << endl;
	YQPkgDescriptionDialog::showDescriptionDialog( pkgName );
    }
    else
    {
	yuiError() << "Protocol not supported - can't follow hyperlink \""
		   << url.toString() << "\"" << endl;
    }
}


void
YQPkgDescriptionView::setSource( const QUrl & url )
{
    showLink( url );
}


#include "YQPkgDescriptionView.moc"

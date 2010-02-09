/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QSettings>
#include "zypp/VendorSupportOptions.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgDescriptionDialog.h"
#include "YQi18n.h"
#include "utf8.h"

#define DESKTOP_TRANSLATIONS    "desktop_translations"
#define DESKTOPFILEDIR		"\\/share\\/applications\\/.*\\.desktop$"	// RegExp
#define ICONSIZE		"32x32"
#define ICONPATH 		"/usr/share/icons/locolor/"ICONSIZE"/apps/"	\
				<< "/usr/share/icons/hicolor/"ICONSIZE"/apps/"	\
				<< "/usr/share/icons/oxygen/"ICONSIZE"/apps/"	\
				<< "/usr/share/icons/Tango/"ICONSIZE"/apps/"	\
				<< "/usr/share/icons/gnome/"ICONSIZE"/apps/"	\
				<< "/opt/kde3/share/icons/hicolor/"ICONSIZE"/apps/";



using std::list;
using std::string;
using namespace zypp;

YQPkgDescriptionView::YQPkgDescriptionView( QWidget * parent, bool showSupportability )
    : YQPkgGenericDetailsView( parent ) 
    , _showSupportability ( showSupportability )
{
    //FIXME setMimeSourceFactory( 0 );
    initLang();
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

    // if it is a package, show the support information
    Package::constPtr package = asKind<Package>(selectable->theObj());
    if ( _showSupportability && package )
    {
        html_text += "<p>";
        // Translators: %1 contains the support level like "Level 3", "unsupported" or "unknown"
        html_text += _("Supportability: %1").arg( fromUTF8(asUserString(package->vendorSupport()).c_str() ));
        html_text += "</p>";
    }

    // show application names and icons from desktop files if available
    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );
    if ( installed )
    {
        // ma@: It might be worth passing Package::FileList directly
        // instead of copying _all_ filenames into a list first.
        // Package::FileList is a query, so it does not eat much memory.
        zypp::Package::FileList f( installed->filelist() );
        std::list<std::string> tmp( f.begin(), f.end() );
	html_text += applicationIconList( tmp );
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


QString
YQPkgDescriptionView::applicationIconList( const list<string> & fileList ) const
{
    QString html;
    QMap<QString, QString> desktopEntries;

    QStringList desktopFiles = findDesktopFiles( fileList );

    if ( desktopFiles.size() == 0 )
        return QString();

    // headline for a list of application icons that belong to a selected package
    html += _("This package contains: ");
    html += "<table border='0'>";

    for ( int i = 0; i < desktopFiles.size(); ++i )
    {
        desktopEntries = readDesktopFile( desktopFiles[i] );

        html += "<tr><td valign='middle' align='center'>";
        html += QString( "<img src=\"" ) + findDesktopIcon ( desktopEntries["Icon"] ) + QString( "\">" );
        html += "</td><td valign='middle' align='left'>";
        html += "<b>" + desktopEntries["Name"] + "</b>";
        html += "</td></tr>";
    }

    html += "</table>";

    return "<p>" + html + "</p>";
}


QString
YQPkgDescriptionView::findDesktopIcon ( const QString& iconName ) const
{
    QStringList path;
    path << ICONPATH;

    for ( int i=0; i < path.size(); ++i )
    {
	QString fullName = path[i] + iconName + ".png";
	QFile file(fullName);
	if ( file.exists() )
	    return fullName;
    }

    return QString();
}


QMap<QString, QString>
YQPkgDescriptionView::readDesktopFile( const QString & fileName ) const
{
    QMap<QString, QString> desktopEntries;
    QString name, genericName;

    QSettings file( fileName, QSettings::IniFormat );
    file.beginGroup( "Desktop Entry" );
    desktopEntries["Icon"] = file.value( "Icon" ).toString();
    desktopEntries["Exec"] = file.value( "Exec" ).toString();

    // translate Name
    name = file.value( QString( "Name[%1]" ).arg( langWithCountry ) ).toString();

    if ( name.isEmpty() )
	name= file.value( QString( "Name[%1]" ).arg( lang ) ).toString() ;

    if ( name.isEmpty() )
    {
	QFileInfo fileInfo (fileName);
	QString msgid = QString( "Name(%1)" ).arg( fileInfo.fileName() );
	msgid += ": ";
	msgid += file.value( QString( "Name" )).toString();
	name = QString::fromUtf8( dgettext( DESKTOP_TRANSLATIONS, msgid.toAscii() ) );

	if ( name == msgid )
	    name = "";
    }
    if ( name.isEmpty() )
	name= file.value( QString( "Name" ) ).toString() ;
    desktopEntries["Name"] = name;

    file.endGroup();

    return desktopEntries;
}


QStringList
YQPkgDescriptionView::findDesktopFiles( const list<string> & fileList ) const
{
    QStringList desktopFiles;

    for ( list<string>::const_iterator it = fileList.begin();
	    it != fileList.end(); ++it )
    {
	QString line = fromUTF8( *it );

	if ( line.contains( QRegExp( DESKTOPFILEDIR ) ) )
	    desktopFiles << line;
    }

    return desktopFiles;
}


void YQPkgDescriptionView::initLang()
{
    const char *lang_cstr = getenv( "LANG" );

    if ( lang_cstr )
    {
	langWithCountry = lang_cstr;
	langWithCountry.replace( QRegExp( "[@\\.].*$" ), "" );  // remove .utf8 / @euro etc.

	lang = langWithCountry;
	lang.replace( QRegExp( "_.*$" ), "" );                  // remove _DE etc.
    }
}


#include "YQPkgDescriptionView.moc"

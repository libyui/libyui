/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/


/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgPackageDetails.cc

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgTable.h"
#include "NCPkgPackageDetails.h"
#include "NCPackageSelector.h"

#include <boost/algorithm/string.hpp>

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

NCPkgPackageDetails::NCPkgPackageDetails ( YWidget *parent, std::string initial_text, NCPackageSelector *pkger)
    : NCRichText (parent, initial_text)
    , pkg (pkger)
{

}

std::string NCPkgPackageDetails::createRelLine( const zypp::Capabilities & info )
{
    std::string text = "";
    zypp::Capabilities::const_iterator
	b = info.begin(),
	e = info.end(),
	it;
    unsigned int i, n = info.size();

    for ( it = b, i = 0; it != e; ++it, ++i )
    {
	text = text + (*it).asString();
	if ( i < n - 1 )
	{
	    text = text + ", ";
	}
    }

    return text;
}

std::string NCPkgPackageDetails::createText( std::list<std::string> info, bool oneline )
{
    std::list<std::string>::iterator it;
    std::string text = "";
    unsigned int i;

    for ( i = 0, it = info.begin(); it != info.end() && i < 1000; ++it, i++ )
    {
	text += (*it);
	if ( i < info.size()-1 )
	{
	    if ( oneline && i < 999 )
	    {
                if ( boost::ends_with( text, ":" ) )
                    text += " ";
                else
                    text += ", ";
	    }
	    else
	    {
		text += "<br>";
	    }
	}
	if ( i == 999 )
	{
	    text += "...";
	}
    }

    return text;
}

std::string NCPkgPackageDetails::commonHeader( ZyppObj pkgPtr )
{
   std::string text = "";

   if ( !pkgPtr )
       return text;

   text += "<h3>" + pkgPtr->name() + " - ";
   text += pkgPtr->summary() + "</h3>";

   return text;
}

void NCPkgPackageDetails::longDescription ( ZyppObj pkgPtr )
{
   std::string text = "";

   if ( !pkgPtr )
       return;

   // text += commonHeader( pkgPtr );
   text += pkgPtr->description();

   // show the description
   setValue( createHtmlText(text) );
}

void NCPkgPackageDetails::technicalData( ZyppObj pkgPtr, ZyppSel slbPtr )
{
    std::string instVersion = "";
    std::string version = "";
    std::string text = "";

    if ( !pkgPtr || !slbPtr )
        return;

    text += commonHeader( pkgPtr );

    if ( slbPtr->hasBothObjects() )
    {
        ZyppObj io = slbPtr->installedObj();
        instVersion = io->edition().version();
        instVersion += "-";
        instVersion += io->edition().release();
        ZyppObj co = slbPtr->candidateObj();
        version = co->edition().version();
        version += "-";
        version += co->edition().release();
    }
    else
    {
        version = pkgPtr->edition().version();
        version += "-";
        version += pkgPtr->edition().release();
    }

    text += NCPkgStrings::Version();
    text +=  version;
    if ( instVersion != "" )
    {
        text += "  ";
        text += NCPkgStrings::InstVersion();
        text += instVersion;
    }
    text +=  "<br>";

    // show the size
    text += NCPkgStrings::Size();
    text += pkgPtr->installSize().asString();
    text +=  "  ";

    ZyppPkg package = tryCastToZyppPkg( pkgPtr );
    ZyppPkg candidate = tryCastToZyppPkg( slbPtr->candidateObj() );
    ZyppPkg installed = tryCastToZyppPkg( slbPtr->installedObj() );

    if ( installed )
        package = installed;
    else if ( candidate )
        package = candidate;

    if ( package )
    {
        // add the media nr
        text += NCPkgStrings::MediaNo();
        char num[5];
        int medianr = package->mediaNr();
        sprintf( num, "%d", medianr );
        text += num;
        text += "<br>";

        // the license
        text += NCPkgStrings::License();
        text += package->license();
        text += "  ";
        text += "<br>";

	// name of the source package
	text += "<b>" + _( "Source Package: " ) + "</b>";
	text += package->sourcePkgName();
	text += "-";
	text += package->sourcePkgEdition().asString();
	text += "<br>";

        std::list<std::string> authors = package->authors(); // zypp::Package
        if ( !authors.empty() )
        {
            std::string author_text;
            text += NCPkgStrings::Authors();
            // authors, in one line
            author_text = createText( authors, true );
            // escape html
            boost::replace_all( author_text, "<", "&lt;" );
            boost::replace_all( author_text, ">", "&gt;" );
            text += author_text;
        }
    }

    setValue (text);

}

void NCPkgPackageDetails::fileList( ZyppSel slbPtr )
{
   std::string text = "";
   // the file list is available only for installed packages
   ZyppPkg package = tryCastToZyppPkg (slbPtr->installedObj());

   if ( package )
   {
       text += commonHeader( slbPtr->theObj() );
       text += NCPkgStrings::ListOfFiles();
       // get the file list from the package manager/show the list
       zypp::Package::FileList pkgfilelist( package->filelist() );
       std::list<std::string> fileList( pkgfilelist.begin(), pkgfilelist.end() );
       text += createText( fileList, false );
   }

   else
	text = _( "<i>This information is available for installed packages only.</i>" );

   setValue(text);
}

void NCPkgPackageDetails::dependencyList( ZyppObj pkgPtr, ZyppSel slbPtr )
{
    std::string text = commonHeader( pkgPtr );
    // show the relations, all of them except provides which is above
    zypp::Dep deptypes[] = {
	zypp::Dep::PROVIDES,
        zypp::Dep::PREREQUIRES,
        zypp::Dep::REQUIRES,
        zypp::Dep::CONFLICTS,
        zypp::Dep::OBSOLETES,
        zypp::Dep::RECOMMENDS,
        zypp::Dep::SUGGESTS,
        zypp::Dep::ENHANCES,
        zypp::Dep::SUPPLEMENTS,
    };
    for (size_t i = 0; i < sizeof (deptypes)/sizeof(deptypes[0]); ++i)
    {
        zypp::Dep deptype = deptypes[i];
        zypp::Capabilities relations = pkgPtr->dep (deptype);
        std::string relline = createRelLine (relations);
        if (!relline.empty())
        {
    	// FIXME: translate
    	text += "<b>" + deptype.asString() + ": </b>"
    	    + relline + "<br>";
        }
    }

    setValue (text);

}

std::string NCPkgPackageDetails::createHtmlText( std::string value )
{
    yuiDebug() << "Description: " << value << endl;

    // check RichText tag
    if ( value.find( std::string(DOCTYPETAG) ) != std::string::npos )
    {
	return value;	// input is rich text
    }
    // escape html
    boost::replace_all( value, "&", "&amp;" );
    boost::replace_all( value, "<", "&lt;" );
    boost::replace_all( value, ">", "&gt;" );

    NCstring input( value );
    NCtext descr( input );
    NCtext html_descr( NCstring("<p>") );
    std::string description = "";
    bool ul_begin = false;
    bool ul_found = false;
    std::list<NCstring>::const_iterator line;

    for ( line = descr.Text().begin(); line != descr.Text().end(); ++line )
    {
        NCstring curr_line( *line );

        if ( curr_line.Str().empty() )
        {
            if ( ul_found )     // empty line after list
            {
                html_descr.append( NCstring("</li></ul><p>") );
                ul_found = false;
                ul_begin = false;
            }
            else
            {
                html_descr.append( NCstring("</p><p>") );
            }
        }
        else if ( curr_line.Str().substr(0, 2) == "- "
                  || curr_line.Str().substr(0, 2) == "* ")         // list item found
        {
            ul_found = true;
            if ( !ul_begin )
            {
                html_descr.append( NCstring("</p><ul><li>") );
                ul_begin = true;
            }
            else
            {
                html_descr.append( NCstring("</li><li>") );
            }
            html_descr.append( NCstring(curr_line.Str().substr(2)) );
        }
        else if ( curr_line.Str().substr(0, 2) == "  " )      // white spaces at begin
        {
            // just append the line (is added to list item or to paragraph)
            html_descr.append( NCstring( curr_line.Str() ) );
        }
        else
        {
            if ( ul_found )     // first line after list
            {
                html_descr.append( NCstring("</li></ul><p>") );
                ul_found = false;
                ul_begin = false;
            }
            html_descr.append( NCstring(" " + curr_line.Str()) );
        }
    }

    if ( ul_found )
        html_descr.append( NCstring("</li></ul>") );
    else
        html_descr.append( NCstring("</p>") );

    // create description
    for ( line = html_descr.Text().begin(); line != html_descr.Text().end(); ++line )
    {
        NCstring curr_line( *line );
        description += curr_line.Str();
    }
    // reduce number of empty lines
    boost::replace_all( description, "</p><p></p>", "</p>" );

    return description;
}

bool NCPkgPackageDetails::patchDescription( ZyppObj objPtr, ZyppSel selectable )
{
    ZyppPatch patchPtr = tryCastToZyppPatch( objPtr );

    if ( !patchPtr || !selectable )
    {
	yuiError() << "Patch not valid" << endl;
	return false;
    }

    std::string descr;
    descr += "<p>";
    descr += NCPkgStrings::Patch();
    descr += selectable->name();
    descr += "&nbsp;";
    // the patch size is not available
    // descr += NCPkgStrings::Size();
    // descr += patchPtr->size().asString( 8 );
    descr += "<b>";
    descr += NCPkgStrings::PatchKind();
    descr += ": </b>";
    descr += patchPtr->category();
    descr += "&nbsp;";
    descr += NCPkgStrings::Version();
    descr += patchPtr->edition().asString();
    descr += "</p>";

    // get and format the patch description
    std::string value = patchPtr->description();
    std::string html_text = "";
    const std::string htmlIdent(DOCTYPETAG);

    if ( value.find( htmlIdent ) != std::string::npos )
    {
	html_text = value;	// HTML text
    }
    else
    {
        // escape html
        boost::replace_all( value, "&", "&amp;" );
        boost::replace_all( value, "<", "&lt;" );
        boost::replace_all( value, ">", "&gt;" );
	html_text = "<pre>" + value + "</pre>";	// add <pre> to preserve newlines and spaces
    }

    descr += html_text;

    descr +=  _( "References:<br>" );
    for ( zypp::Patch::ReferenceIterator rit = patchPtr->referencesBegin();
	  rit != patchPtr->referencesEnd();
	  ++rit )
    {
	descr += rit.id().c_str();
	descr += "&nbsp;";
	descr += "(";
	descr += rit.type().c_str();
	descr += "):";
	descr += "&nbsp;";
	descr += rit.title().c_str();
	descr += "<br>";
    }
    // show the description
    setValue( descr );

    return true;
}

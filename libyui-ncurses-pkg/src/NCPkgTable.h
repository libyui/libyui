/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| Copyright (C) 2020 SUSE LLC
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

   File:       NCPkgTable.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgTable_h
#define NCPkgTable_h

#include <iosfwd>

#include "NCPadWidget.h"
#include "NCTablePad.h"
#include "NCTable.h"
#include "NCTableItem.h"
#include "NCPkgStrings.h"

#include <map>
#include <string>
#include <utility>      // for STL std::pair

#include <zypp/ui/Selectable.h>

#include "NCPkgStatusStrategy.h"


class NCPackageSelector;

/**
 * This class is used for the first column of the package table
 * which contains the status information of the package (installed,
 * not installed, to be deleted and so on).
 **/
class NCPkgTableTag : public YTableCell
{
private:

    ZyppStatus status;
    ZyppObj dataPointer;
    // cannot get at it from dataPointer
    ZyppSel selPointer;

public:

    NCPkgTableTag( ZyppObj pkgPtr,
		   ZyppSel selPtr,
		   ZyppStatus stat = S_NoInst );

    ~NCPkgTableTag() {}

    void setStatus( ZyppStatus  stat ) { status = stat; }
    ZyppStatus getStatus() const { return status; }
    // returns the corresponding std::string value to given package status
    std::string statusToString( ZyppStatus stat ) const;

    ZyppObj getDataPointer() const { return dataPointer; }
    ZyppSel getSelPointer() const  { return selPointer; }
};


class NCPkgTableSort : public NCTableSortStrategyBase
{
public:

    NCPkgTableSort( const std::vector<std::string> & head )
	: _header( head )
	{}

    virtual void sort( YItemIterator itemsBegin,
                       YItemIterator itemsEnd ) override
    {
        if ( _header[ sortCol() ] == NCPkgStrings::PkgSize() )
        {
            std::sort( itemsBegin, itemsEnd, CompareSize() );
        }
        else if ( _header[ sortCol() ] == NCPkgStrings::PkgName() )
        {
            std::sort( itemsBegin, itemsEnd, CompareName( sortCol() ) );
        }
        else
        {
            std::sort( itemsBegin, itemsEnd, Compare( sortCol() ) );
        }

        if ( reverse() )
            std::reverse( itemsBegin, itemsEnd );
    }

private:

    std::vector<std::string> _header;


    /**
     * Return the content column no. 'col' for an item.
     **/
    static std::wstring cellContent( YItem * item, int col )
    {
        std::wstring empty;

        if ( ! item )
            return empty;

        YTableItem * tableItem = dynamic_cast<YTableItem *>( item );

        if ( ! tableItem )
            return empty;

        YTableCell * tableCell = tableItem->cell( col );

        if ( ! tableCell )
            return empty;

        return NCstring( tableCell->label() ).str();
    }


    class CompareSize
    {
    public:
	CompareSize()
	    {}

	bool operator() ( YItem * item1, YItem * item2 ) const
	{
            YTableItem * tableItem1 = dynamic_cast<YTableItem *>( item1 );
            YTableItem * tableItem2 = dynamic_cast<YTableItem *>( item2 );

            if ( ! tableItem1 ) return true;
            if ( ! tableItem2 ) return true;

            const NCPkgTableTag * tag1 = static_cast<const NCPkgTableTag *>( tableItem1->cell(0) );
            const NCPkgTableTag * tag2 = static_cast<const NCPkgTableTag *>( tableItem2->cell(0) );

            return tag1->getDataPointer()->installSize() <
                tag2->getDataPointer()->installSize();
        }
    };


    class CompareName
    {
    public:
	CompareName( int uiCol )
	    : _uiCol( uiCol )
	    {}

	bool operator() ( YItem * item1, YItem * item2 ) const
	{
            std::wstring w1 = cellContent( item1, _uiCol );
            std::wstring w2 = cellContent( item2, _uiCol );

            // It is safe to use collate unaware wscasecmp() here because package names
            // are 7 bit ASCII only. Better yet, we don't even want this to be sorted
            // by locale specific rules: "ch" for example would be sorted after "h" in
            // Czech which in the context of package names (which are English) would be
            // plain wrong.
            int result = wcscasecmp( w1.data(), w2.data() );

            return result < 0;
        }

    private:
	const int _uiCol;
    };


    class Compare
    {
    public:
	Compare( int uiCol )
	    : _uiCol( uiCol )
	    {}

	bool operator() ( YItem * item1, YItem * item2 ) const
	{
            std::wstring w1 = cellContent( item1, _uiCol );
            std::wstring w2 = cellContent( item2, _uiCol );

            int result = wcscoll ( w1.data(), w2.data() );

            return result < 0;
        }

    private:
	const int _uiCol;
    };
};


/**
 * The package table class. Provides methods to fill the table,
 * set the status info and so on.
 * Has a connection to the PackageSelector which is used to do
 * changes which affect other widgets.
 *
 **/
class NCPkgTable : public NCTable
{
public:

    enum NCPkgTableType
    {
	T_Packages,
	T_Availables,
	T_Patches,
	T_Update,
	T_PatchPkgs,
	T_Selections,
	T_Languages,
        T_MultiVersion,
	T_Unknown
    };

    enum NCPkgTableListAction
    {
	A_Install,
	A_Delete,
	A_Keep,
	A_UpdateNewer,
	A_Update,
	A_Unknown
    };

    enum NCPkgTableListType
    {
	L_Changes,
	L_Installed,
	L_Unknown
    };

    enum NCPkgTableInfoType
    {
	I_Descr,
	I_Technical,
	I_Versions,
	I_Files,
	I_Deps,
	I_PatchDescr,
	I_PatchPkgs
    };

private:

    NCPkgTable & operator=( const NCPkgTable & );
    NCPkgTable            ( const NCPkgTable & );

    NCPackageSelector * packager;		// connection to the PackageSelector,

    NCPkgStatusStrategy * statusStrategy; 	// particular methods to get the status

    NCPkgTableType tableType;	// the type (e.g. table of packages, patches)
    bool haveInstalledVersion;	// for T_Packages and T_Update

    // returns the first column of line with 'index' (the tag)
    NCPkgTableTag * getTag ( const int & index );

    NCPkgTableInfoType visibleInfo;

    std::vector<std::string> header;		// the table header


public:

    /**
     * Constructor
     */
    NCPkgTable( YWidget * parent, YTableHeader * tableHeader );

    virtual ~NCPkgTable();


    /**
     * This method is called to add a line to the package list.
     * @param status The package status (first column of the table)
     * @param elements A std::vector<std::string> containing the package data
     * @param objPtr The pointer to the packagemanager object
     * @param objPtr The pointer to the selectable object
     * @return void
     */
    virtual void addLine( ZyppStatus status,
			  const std::vector<std::string> & elements,
			  ZyppObj objPtr,
			  ZyppSel slbPtr );

    /**
     * Draws the package list (has to be called after the loop with addLine() calls)
     */
    void drawList() { sortItems( 1 ); return DrawPad(); }

    /**
     * Clears the package list
     */
    virtual void itemsCleared();

    /**
     * Returns the contents of a certain cell in table
     * @param index The table line
     * @param column The column
     * @eturn NClabel
     */
    NClabel getCellContents( int index, int colnum );

    /**
     * Handles the events concerning the package table (e.g. scroll the list,
     * change the package status, ...)
     * @param key The key which is pressed
     * @return NCursesEvent
     */
    virtual NCursesEvent wHandleInput( wint_t key );

    /**
     * Sets the member variable PackageSelector *packager
     * @param pkg The PackageSelector pointer
     * @return void
     */
    void setPackager( NCPackageSelector * pkg ) { packager = pkg; }

    /**
     * Informs the package manager about the status change of
     * the currently selected package and updates the states
     * of all packages in the list
     * @param newstat The new status
     * @param slbPtr The pointer to the object to change
     * @param objPtr is candidatePtr or what the user selected instead of it.
     * @return bool
     */
    bool changeStatus( ZyppStatus newstat,
		       const ZyppSel & slbPtr,
		       ZyppObj objPtr,
		       bool singleChange );

    bool changeObjStatus( int key );

    bool changeListObjStatus( NCPkgTableListAction key );

    bool cycleObjStatus();

    /**
     * Set the status information if status has changed
     * @return bool
     */
    bool updateTable();

    /**
     * Gets the currently displayed package status.
     * @param index The index in package table (the line)
     * @return ZyppStatus
     */
    ZyppStatus getStatus( int index );

#ifdef FIXME
    /**
     * Toggles the installation of the source package.
     * @param install
     * @return bool
     */
    bool SourceInstall( bool install );
#endif

    /**
     * Sets the type of the table and the status strategy (which means call particular methods
     * to set/get the status for different zypp::ResObjects (zypp::Patch, zypp::Package or available zypp::Package)
     * @param type	The type (see enum NCPkgTableType)
     * @param strategy  The certain strategy (available strategies see NCPkgStatusStrategy.h).
     * 			Has to be allocated with new - is deleted by NCPkgTable.
     * @return bool
     */
    bool setTableType( NCPkgTableType type, NCPkgStatusStrategy * strategy )
    {
	if ( !strategy )
	    return false;

	delete statusStrategy;
	statusStrategy = strategy;
	tableType = type;

	return true;
    }

    NCPkgTableType getTableType() { return tableType; }

    /**
     * Gets the data pointer of a certain package.
     * @param index The index in package table (the line)
     * @return ZyppObj
     */
    ZyppObj getDataPointer( int index );

    /**
     * Gets the selectable pointer of a certain package.
     * @param index The index in package table (the line)
     * @return ZyppSel
     */
    ZyppSel getSelPointer( int index );

    /**
     * Returns the number of lines in the table (the table size)
     * @return unsigned int
     */
    unsigned int getNumLines() { return myPad()->Lines(); }

    /**
     * Fills the header of the table
     * @return void
     */
    void fillHeader();

    /**
     * Creates a line in the package table.
     * @param pkgPtr The package pointer
     * @param slbPtr The selectable pointer
     * @return bool
     */
    bool createListEntry ( ZyppPkg pkgPtr, ZyppSel slbPtr );

    /**
     * Creates a line in the YOU patch table.
     * @param pkgPtr The YOU patch pointer
     * @return bool
     */
    bool createPatchEntry ( ZyppPatch pkgPtr,  ZyppSel slbPtr );

    /**
     * Creates a line in the table shwing an info text.
     * @param text The information
     * @return bool
     */
    bool createInfoEntry ( std::string text );

    /**
     * Show the corresponding information (e.g. the package description).
     * @return bool
     */
    bool showInformation();

    /**
     * Ask the user for confirmation of installing a retracted package.
     * This returns 'true' if the user confirmed, 'false' if not.
     **/
    bool confirmRetracted( ZyppObj pkg, ZyppSel sel );

    void setVisibleInfo( NCPkgTableInfoType info) { visibleInfo = info;  }

    NCPkgTableInfoType VisibleInfo() { return visibleInfo; }

    bool fillAvailableList ( ZyppSel slb );
    bool fillSummaryList ( NCPkgTableListType type );

    void updateInfo( ZyppObj pkgPtr, ZyppSel slbPtr, NCPkgTableInfoType mode );

};

///////////////////////////////////////////////////////////////////

#endif // NCPkgTable_h

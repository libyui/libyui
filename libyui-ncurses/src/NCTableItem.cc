/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2020 SUSE LLC
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCTableItem.cc

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTableItem.h"
#include "NCTable.h"
#include "stringutil.h"
#include "stdutil.h"

using stdutil::form;
using std::string;
using std::wstring;
using std::endl;


NCTableLine::NCTableLine( std::vector<NCTableCol*> & cells,
                          int                        index,
                          bool                       nested,
                          unsigned                   state )
    : _cells( cells )
    , _state( state )
    , _index( index )
    , _yitem( 0 )
    , _nested( nested )
    , _treeLevel( 0 )
    , _parent( 0 )
    , _nextSibling( 0 )
    , _firstChild( 0 )
    , _vstate( S_HIDDEN )
    , _prefix( 0 )
{
    initPrefixPlaceholder();
}


NCTableLine::NCTableLine( NCTableLine *              parentLine,
                          YItem *                    yitem,
                          std::vector<NCTableCol*> & cells,
                          int                        index,
                          bool                       nested,
                          unsigned                   state )
    : _cells( cells )
    , _state( state )
    , _index( index )
    , _yitem( yitem )
    , _nested( nested )
    , _treeLevel( 0 )
    , _parent( parentLine )
    , _nextSibling( 0 )
    , _firstChild( 0 )
    , _vstate( S_HIDDEN )
    , _prefix( 0 )
{
    setYItem( yitem );
    treeInit( parentLine, yitem );
    initPrefixPlaceholder(); // This needs to be done AFTER treeInit()!

    if ( ! cells.empty() && nested )
        cells[0]->setPrefix( _prefixPlaceholder );
}


NCTableLine::NCTableLine( unsigned colCount,
                          int      index,
                          bool     nested,
                          unsigned state )
    : _cells( colCount, (NCTableCol *) 0 )
    , _state( state )
    , _index( index )
    , _yitem( 0 )
    , _nested( nested )
    , _treeLevel( 0 )
    , _parent( 0 )
    , _nextSibling( 0 )
    , _firstChild( 0 )
    , _vstate( S_HIDDEN )
    , _prefix( 0 )
{
    initPrefixPlaceholder();
}


NCTableLine::NCTableLine( NCTableLine * parentLine,
                          YItem *       yitem,
                          unsigned      colCount,
                          int           index,
                          bool          nested,
                          unsigned      state )
    : _cells( colCount, (NCTableCol *) 0 )
    , _state( state )
    , _index( index )
    , _yitem( yitem )
    , _nested( nested )
    , _treeLevel( 0 )
    , _parent( parentLine )
    , _nextSibling( 0 )
    , _firstChild( 0 )
    , _vstate( S_HIDDEN )
    , _prefix( 0 )
{
    setYItem( yitem );
    treeInit( parentLine, yitem );
    initPrefixPlaceholder(); // This needs to be done AFTER treeInit()!
}


NCTableLine::~NCTableLine()
{
    ClearLine();
    delete [] _prefix;
}


void NCTableLine::treeInit( NCTableLine * parentLine,
                            YItem *       yitem      )
{
    _parent = parentLine;

    if ( _parent )
    {
        addToTree( _parent );
        _treeLevel = _parent->treeLevel() + 1;
        _nested    = true;

        if ( ! isOpen( parentLine->yitem() ) )
            SetState( S_HIDDEN );
    }
    else
    {
        _firstChild  = 0;
        _nextSibling = 0;
        _treeLevel   = 0;
    }
}


void NCTableLine::initPrefixPlaceholder()
{
    // Notice that this needs to be done AFTER treeInit() because prefixLen()
    // depends on treeLevel() which is only known after the parent is set.

    // Just reserve enough space with blanks. They will be overwritten later in
    // DrawAt() with real line graphics.

    _prefixPlaceholder = indentationStr();
}


string NCTableLine::indentationStr() const
{
    return _nested ? string( prefixLen(), ' ' ) : "";
}


void NCTableLine::addToTree( NCTableLine * parent )
{
    if ( parent )
    {
        if ( parent->firstChild() ) // The parent already has children
        {
            // Find the last child of the parent

            NCTableLine * lastChild = parent->firstChild();

            while ( lastChild->nextSibling() )
                lastChild = lastChild->nextSibling();

            lastChild->setNextSibling( this );
        }
        else // The parent does not have any children yet
        {
            parent->setFirstChild( this );
        }
    }
}


bool NCTableLine::isOpen( YItem * yitem ) const
{
    if ( ! yitem )
        return false;

    YTreeItem * treeItem = dynamic_cast<YTreeItem *>( yitem );

    if ( treeItem )
        return treeItem->isOpen();

    // No need to dynamic_cast to YTableItem as well:
    // YTableItem now (as of 8/2020) inherits YTreeItem.

    return false;
}


void NCTableLine::setOrigItem( YTableItem * yitem )
{
    setYItem( yitem );
}


void NCTableLine::setYItem( YItem * yitem )
{
    _yitem = yitem;

    if ( _yitem )
        _yitem->setData( this ) ;
}


void NCTableLine::assertCol( unsigned idx )
{
    if ( idx >= Cols() )
	SetCols( idx + 1 );
}


void NCTableLine::SetCols( unsigned idx )
{
    if ( idx == Cols() )
	return;

    if ( idx < Cols() )
    {
	for ( unsigned i = idx; i < Cols(); ++i )
	{
	    delete _cells[i];
	}
    }

    _cells.resize( idx, 0 );
}


void NCTableLine::stripHotkeys()
{
    for ( unsigned i = 0; i < Cols(); ++i )
    {
	if ( _cells[i] )
	    _cells[i]->stripHotkey();
    }
}


void NCTableLine::SetCols( std::vector<NCTableCol*> & newCells )
{
    SetCols( 0 );
    _cells = newCells;
}


void NCTableLine::AddCol( unsigned idx, NCTableCol * cell )
{
    assertCol( idx );
    delete _cells[idx];
    _cells[idx] = cell;
}


void NCTableLine::DelCol( unsigned idx )
{
    if ( idx < Cols() )
    {
	delete _cells[idx];
	_cells[idx] = 0;
    }
}


NCTableCol * NCTableLine::GetCol( unsigned idx )
{
    if ( idx < Cols() )
	return _cells[idx];

    return 0;
}


void NCTableLine::UpdateFormat( NCTableStyle & tableStyle )
{
    tableStyle.AssertMinCols( Cols() );

    for ( unsigned col = 0; col < Cols(); ++col )
    {
	if ( !_cells[ col ] )
	    continue;

	tableStyle.MinColWidth( col, _cells[ col ]->Size().W );
    }

    if ( _nested && ! _prefix )
        updatePrefix(); // Put together line graphics for the tree hierarchy
}


void NCTableLine::DrawAt( NCursesWindow & w,
                          const wrect     at,
			  NCTableStyle &  tableStyle,
			  bool            active ) const
{
    _vstate = S_HIDDEN;

    if ( isVisible() )
    {
	if ( isDisabled() )
	    _vstate = S_DISABLED;
	else
	    _vstate = active ? S_ACTIVE : S_NORMAL;
    }

    w.bkgdset( tableStyle.getBG( _vstate ) );

    for ( int i = 0; i < at.Sze.H; ++i )
    {
	w.move( at.Pos.L + i, at.Pos.C );
	w.clrtoeol();
    }

    DrawItems( w, at, tableStyle, active );
}


void NCTableLine::DrawItems( NCursesWindow & w,
                             const wrect     at,
			     NCTableStyle &  tableStyle,
			     bool            active ) const
{
    if ( !( at.Sze > wsze( 0 ) ) )
	return;

    wrect    lRect( at );
    unsigned destWidth;

    for ( unsigned col = 0; col < Cols(); ++col )
    {
	if ( col > 0 && tableStyle.ColSepWidth() )
	{
	    // draw centered
	    destWidth = tableStyle.ColSepWidth() / 2;

	    if ( destWidth < (unsigned) lRect.Sze.W )
	    {
		w.bkgdset( tableStyle.getBG( _vstate, NCTableCol::SEPARATOR ) );
		w.vline( lRect.Pos.L, lRect.Pos.C + destWidth,
			 lRect.Sze.H, tableStyle.ColSepChar() );
		// skip over
		destWidth = tableStyle.ColSepWidth();

		if ( (unsigned) lRect.Sze.W <= destWidth )
		    break;

		lRect.Pos.C += destWidth;
		lRect.Sze.W -= destWidth;
	    }
	}

	destWidth = tableStyle.ColWidth( col );

	wrect cRect( lRect );

	// Adjust drawing rectangle for the screen space we just used
	lRect.Pos.C += destWidth;
	lRect.Sze.W -= destWidth;

	if ( lRect.Sze.W < 0 )
	    cRect.Sze.W = destWidth + lRect.Sze.W;
	else
	    cRect.Sze.W = destWidth;

	if ( _cells[ col ] )
	{
            // Draw item
	    _cells[ col ]->DrawAt( w, cRect, tableStyle, _vstate, col );

            // Draw tree hierarchy line graphics over the prefix placeholder

            if ( col == 0 && _prefix )
                drawPrefix( w, cRect, tableStyle );
	}
    }
}


void NCTableLine::updatePrefix()
{
    if ( _prefix )
        delete[] _prefix;

    // Build from right to left: Start with the line for this (deepest) level

    _prefix = new chtype[ prefixLen() ];
    chtype * tagend = &_prefix[ prefixLen()-1 ];
    *tagend-- = ACS_HLINE;
    *tagend-- = firstChild() ? ACS_TTEE : ACS_HLINE;

    if ( _parent )
    {
        // Draw vertical connector for the siblings on this level

        *tagend-- = nextSibling() ? ACS_LTEE : ACS_LLCORNER;


        // From right to left, for each higher level, draw a vertical line
        // or a blank if this is the last branch on that level

        for ( NCTableLine * p = parent(); p; p = p->parent() )
        {
            *tagend-- = p->nextSibling() ? ACS_VLINE : ( ' '&A_CHARTEXT );
        }
    }
    else // This is a toplevel item
    {
        *tagend-- = ACS_HLINE; // One more horizontal line to the left
    }
}


void NCTableLine::drawPrefix( NCursesWindow & w,
                              const wrect     at,
                              NCTableStyle  & tableStyle ) const
{
    if ( ! _prefix )
        return;

    w.move( at.Pos.L, at.Pos.C );

    for ( int i = 0; i < prefixLen(); ++i )
        w.addch( _prefix[i] );


    // Draw the "+" indicator if this branch can be opened

    w.move( at.Pos.L, at.Pos.C + prefixLen() - 2 );

    if ( firstChild() && !isSpecial() )
    {
        w.bkgdset( tableStyle.highlightBG( _vstate,
                                           NCTableCol::HINT,
                                           NCTableCol::SEPARATOR ) );
    }

    if ( firstChild() && !firstChild()->isVisible() )
        w.addch( '+' );
    else
        w.addch( _prefix[ prefixLen() - 2 ] );
}


bool NCTableLine::handleInput( wint_t key )
{
    bool handled = false;

    switch ( key )
    {
        case '?':
            yuiDebug() << _yitem << ": index: " << index() << endl;
            break;

        case KEY_IC:    // "Insert" key ("Insert Character")
        case '+':
            openBranch();
            handled = true;
            break;

        case KEY_DC:    // "Delete" key ("Delete Character")
        case '-':
            closeBranch();
            handled = true;
            break;

        case KEY_RETURN:
            // Propagate up to the pad; see bsc#67350
            break;

        case KEY_SPACE: // Toggle open/closed state of this branch

            if ( _nested )
            {
                toggleOpenClosedState();
                handled = true;
            }
            // else
            //   cascade the event up to NCTable::wHandleInput()
            //   to let it toggle the selection status in multiSelection mode
            break;
    }

    return handled;
}


void NCTableLine::openBranch()
{
    if ( firstChild() && ! firstChild()->isVisible() )
    {
        // YTableItem inherits YTreeItem which inherits YItem,
        // so we need to cast the YItem to YTreeItem which has the _isOpen flag.
        YTreeItem * treeItem = dynamic_cast<YTreeItem *>( _yitem );

        if ( treeItem )
        {
            treeItem->setOpen( true );
            yuiDebug() << "Opening item " << treeItem->label() << endl;

            for ( NCTableLine * child = firstChild(); child; child = child->nextSibling() )
                child->ClearState( S_HIDDEN );
        }
    }
}


void NCTableLine::closeBranch()
{
    if ( firstChild() && firstChild()->isVisible() )
    {
        // YTableItem inherits YTreeItem which inherits YItem,
        // so we need to cast the YItem to YTreeItem which has the _isOpen flag.
        YTreeItem * treeItem = dynamic_cast<YTreeItem *>( _yitem );

        if ( treeItem )
        {
            treeItem->setOpen( false );
            yuiDebug() << "Closing item " << treeItem->label() << endl;

            for ( NCTableLine * child = firstChild(); child; child = child->nextSibling() )
                child->SetState( S_HIDDEN );
        }
    }
}


void NCTableLine::toggleOpenClosedState()
{
    if ( firstChild() )
    {
        if ( firstChild()->isVisible() )
            closeBranch();
        else
            openBranch();
    }
}


bool NCTableLine::isVisible() const
{
    return ! parent() || ( !isHidden() && parent()->isVisible() );
}



NCTableTag * NCTableLine::tagCell() const
{
    NCTableTag * ret = 0;

    if ( ! _cells.empty() )
        ret = dynamic_cast<NCTableTag *>( _cells[0] );

    return ret;
}


std::ostream & operator<<( std::ostream & str, const NCTableLine & obj )
{
    str << "Line: cols " << obj.Cols() << endl;

    for ( unsigned idx = 0; idx < obj.Cols(); ++idx )
    {
	str << "  " << idx << " ";
	const NCTableCol * cell = obj.GetCol( idx );

	if ( cell )
	    str << *cell;
	else
	    str << "NO_ITEM";

	str << endl;
    }

    return str;
}



//
//----------------------------------------------------------------------
//


NCTableCol::NCTableCol( const NCstring & label, STYLE style )
    : _label( label )
    , _style( style )
{
}


NCTableCol::~NCTableCol()
{
}


chtype NCTableCol::setBkgd( NCursesWindow &    w,
			    NCTableStyle &     tableStyle,
			    NCTableLine::STATE linestate,
			    STYLE              colstyle ) const
{
    chtype bkgdstyle = tableStyle.getBG( linestate, colstyle );

    if ( bkgdstyle != NCTableStyle::currentBG )
	w.bkgdset( bkgdstyle );
    else
	bkgdstyle = w.getbkgd();

    return bkgdstyle;
}


wrect NCTableCol::prefixAdjusted( const wrect origRect ) const
{
    wrect newRect = origRect;

    if ( _prefix.width() > 0 )
    {
        newRect.Pos.C += _prefix.width();
        newRect.Sze.W -= _prefix.width();
    }

    return newRect;
}


void NCTableCol::DrawAt( NCursesWindow &    w,
                         const wrect        at,
			 NCTableStyle &     tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned           colidx ) const
{
    chtype bg       = setBkgd( w, tableStyle, linestate, _style );
    chtype hbg      = tableStyle.hotBG( linestate, colidx );

    if ( hbg == NCTableStyle::currentBG )
	hbg = bg;

    if ( _prefix.width() > 0 )
        _prefix.drawAt( w, bg, hbg, at );

    _label.drawAt( w, bg, hbg,
                   prefixAdjusted( at ),
                   tableStyle.ColAdjust( colidx ) );
}


std::ostream & operator<<( std::ostream & str, const NCTableCol & obj )
{
    return str << obj._label;
}


//
//----------------------------------------------------------------------
//


void NCTableHead::DrawAt( NCursesWindow & w,
                          const wrect     at,
			  NCTableStyle &  tableStyle,
			  bool            active ) const
{
    _vstate = S_HEADLINE;
    w.bkgdset( tableStyle.getBG( _vstate ) );

    for ( int i = 0; i < at.Sze.H; ++i )
    {
	w.move( at.Pos.L + i, at.Pos.C );
	w.clrtoeol();
    }

    DrawItems( w, at, tableStyle, active );
}


//
//----------------------------------------------------------------------
//


NCTableStyle::NCTableStyle( const NCWidget & parentWidget )
    : _parentWidget( parentWidget )
    , _headline( 0 )
    , _colWidth( 0 )
    , _colAdjust( 0 )
    , _colSepWidth( 1 )
    , _colSepChar( ACS_VLINE )
    , _hotCol( (unsigned) - 1 )
{
}


bool NCTableStyle::SetStyleFrom( const std::vector<NCstring> & head )
{
    unsigned ncols = head.size();

    _headline.ClearLine();
    _headline.SetCols( ncols );

    _colWidth.clear();
    _colAdjust.clear();
    AssertMinCols( ncols );

    bool hasContent = false;

    for ( unsigned i = 0; i < head.size(); ++i )
    {
	const wstring & entry( head[i].str() );
	bool strip = false;

	if ( entry.length() )
	{
	    switch ( entry[0] )
	    {
		case 'R':
		    strip = true;
		    _colAdjust[i] = NC::RIGHT;
		    break;

		case 'C':
		    strip = true;
		    _colAdjust[i] = NC::CENTER;
		    break;

		case 'L':
		    strip = true;
		    _colAdjust[i] = NC::LEFT;
		    break;

		default:
		    yuiWarning() << "No style char [LRC] at beginning of '" << entry << "'" << std::endl;
		    break;
	    }
	}

	NCstring coltxt = strip ? entry.substr( 1 ) : entry;
	_headline.AddCol( i, new NCTableCol( coltxt ) );

	if ( ! hasContent && coltxt.str().length() )
	    hasContent = true;
    }

    return hasContent;
}


chtype NCTableStyle::highlightBG( const NCTableLine::STATE lstate,
				  const NCTableCol::STYLE  cstyle,
				  const NCTableCol::STYLE  dstyle ) const
{
    return getBG( lstate, cstyle );
}


chtype NCTableStyle::getBG( const NCTableLine::STATE lstate,
			    const NCTableCol::STYLE  cstyle ) const
{
    switch ( lstate )
    {
	case NCTableLine::S_NORMAL:

	    switch ( cstyle )
	    {
		case NCTableCol::PLAIN:         return listStyle().item.plain;
		case NCTableCol::DATA:          return listStyle().item.data;
		case NCTableCol::ACTIVEDATA:    return listStyle().item.plain;
		case NCTableCol::HINT:          return listStyle().item.hint;
		case NCTableCol::SEPARATOR:     return listStyle().item.plain;
		case NCTableCol::NONE:          return currentBG;
	    }
	    break;


	case NCTableLine::S_ACTIVE:

	    switch ( cstyle )
	    {
		case NCTableCol::PLAIN:         return listStyle().selected.plain;
		case NCTableCol::DATA:          return listStyle().selected.data;
		case NCTableCol::ACTIVEDATA:    return listStyle().selected.data;
		case NCTableCol::HINT:          return listStyle().selected.hint;
		case NCTableCol::SEPARATOR:     return listStyle().selected.plain;
		case NCTableCol::NONE:          return currentBG;
	    }
	    break;

	case NCTableLine::S_DISABLED:

	    switch ( cstyle )
	    {
		case NCTableCol::PLAIN:         return _parentWidget.wStyle().disabledList.item.plain;
		case NCTableCol::DATA:          return _parentWidget.wStyle().disabledList.item.data;
		case NCTableCol::ACTIVEDATA:    return _parentWidget.wStyle().disabledList.item.plain;
		case NCTableCol::HINT:          return _parentWidget.wStyle().disabledList.item.hint;
		case NCTableCol::SEPARATOR:     return listStyle().item.plain;
		case NCTableCol::NONE:          return currentBG;
	    }
	    break;


	case NCTableLine::S_HEADLINE:
	    return listStyle().title;
	    break;

	case NCTableLine::S_HIDDEN:
	    return currentBG;
	    break;
    }

    return currentBG;
}


std::ostream & operator<<( std::ostream & str, const NCTableStyle & obj )
{
    str << form( "cols %d, sep %d (%lx)\n",
                 obj.Cols(), obj.ColSepWidth(), (unsigned long)obj.ColSepChar() );

    for ( unsigned i = 0; i < obj.Cols(); ++i )
    {
	str << form( "%2d %d(%3d) ", i, obj.ColAdjust( i ), obj.ColWidth( i ) );

	if ( obj.Headline().GetCol( i ) )
	    str << obj.Headline().GetCol( i )->Label();

	str << std::endl;
    }

    return str;
}


/*
  Copyright (C) 2000-2012 Novell, Inc
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

   File:       tnode.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef tnode_h
#define tnode_h

#include <iosfwd>

/**
 * Tree node.
 *
 * Traversing the tree with \ref Next and \ref Prev is done
 * pre-order (self before children)
 * and depth-first (children before siblings)
 *
 * See also
 * - https://en.wikipedia.org/wiki/Depth-first_search
 * - https://en.wikipedia.org/wiki/Tree_traversal#Pre-order_(NLR)
 *
 * In practice all instances of this template use NCWidget * for n_value.
 */
template <class n_value> class tnode
{

    tnode & operator=( const tnode & );
    tnode( const tnode & );

protected:

    typedef tnode<n_value>   self;

    mutable n_value val;

private:

    self * parent;
    self * psibling;    // prev sibling
    self * nsibling;    // next sibling
    self * fchild;      // first child
    self * lchild;      // last  child

    // Disconnect from old parent, connect to new parent *p*.
    //
    // If *s* is specified (not nilptr), insert this
    // as its previous sibling (if *behind* is false)
    // or as its next sibling (if *behind* is true).
    //
    // If *s* is omitted (nilptr), become the first (behind==false) or last
    // (behind=true) child
    //
    // In case *s* is specified but is not in fact a child of *p* then it is
    // treated as omitted.
    //
    // @param p new parent
    // @param s reference sibling
    // @param behind true: insert after *s*; false: insert before *s*
    // @return true on success;
    //   false on failure (*p* is myself or a descendant of mine)
    bool DoReparentTo( self & p, self * s, bool behind )
    {

	if ( &p == this || p.IsDescendantOf( this ) )
	    return false;

	Disconnect();

	parent = &p;

	PreReparent();

	if ( !s || s->parent != parent ) // using default sibling
	    s = behind ? parent->lchild : parent->fchild;

	if ( !s )
	{
	    // no sibling, so we'll be the only child
	    parent->fchild = parent->lchild = this;
	}
	else
	{
	    if ( behind )
	    {
		psibling    = s;
		nsibling    = s->nsibling;
		s->nsibling = this;

		if ( nsibling )
		    nsibling->psibling = this;
		else
		    parent->lchild     = this;
	    }
	    else
	    {
		psibling    = s->psibling;
		nsibling    = s;
		s->psibling = this;

		if ( psibling )
		    psibling->nsibling = this;
		else
		    parent->fchild     = this;
	    }
	}

	PostReparent();

	return true;
    }

protected:

    virtual void PreDisconnect()  {}

    virtual void PostDisconnect() {}

    virtual void PreReparent()	  {}

    virtual void PostReparent()   {}

public:

    /// New node, added as the last child by default (which is natural).
    /// @param p parent
    tnode( n_value v, self * p = 0, bool behind = true )
	    : val( v )
	    , parent( 0 )
	    , psibling( 0 )
	    , nsibling( 0 )
	    , fchild( 0 )
	    , lchild( 0 )
    {
	if ( p )
	    DoReparentTo( *p, 0, behind );
    }

    /// New node, added as the last child by default (which is natural).
    /// @param p parent
    tnode( n_value v, self & p, bool behind = true )
	    : val( v )
	    , parent( 0 )
	    , psibling( 0 )
	    , nsibling( 0 )
	    , fchild( 0 )
	    , lchild( 0 )
    {
	DoReparentTo( p, 0, behind );
    }

    /// New node under *p*, just after *s* (or before *s* if behind==false)
    /// @param p parent
    /// @param s reference sibling
    tnode( n_value v, self & p, self & s, bool behind = true )
	    : val( v )
	    , parent( 0 )
	    , psibling( 0 )
	    , nsibling( 0 )
	    , fchild( 0 )
	    , lchild( 0 )
    {
	DoReparentTo( p, &s, behind );
    }


    virtual ~tnode()
    {
	while ( fchild )
	    fchild->Disconnect();

	Disconnect();
    }

    /// Disconnect from the parent and siblings, but keep children.
    void Disconnect()
    {
	if ( !parent )
	    return;

	PreDisconnect();

	if ( psibling )
	    psibling->nsibling = nsibling;
	else
	    parent->fchild     = nsibling;

	if ( nsibling )
	    nsibling->psibling = psibling;
	else
	    parent->lchild     = psibling;

	parent = psibling = nsibling = 0;

	PostDisconnect();
    }

    /// Disconnect from old parent, connect to new parent *p*.
    /// Become the last child (or the first, if behind==false).
    ///
    /// @param p new parent
    /// @return true on success;
    ///   false on failure (*p* is myself or a descendant of mine)
    bool ReparentTo( self & p, bool behind = true )
    {
	return DoReparentTo( p, 0, behind );
    }

    /// Disconnect from old parent, connect to new parent *p* and sibling *s*.
    ///
    /// Insert this as just after *s* (or just before, if behind==false).
    ///
    /// In case *s* is not in fact a child of *p* then we become *p*'s last
    /// (first) child.
    ///
    /// @param p new parent
    /// @param s reference sibling
    /// @param behind true: insert after *s*; false: insert before *s*
    /// @return true on success;
    ///   false on failure (*p* is myself or a descendant of mine)
    bool ReparentTo( self & p, self & s, bool behind = true )
    {
	return DoReparentTo( p, &s, behind );
    }


    n_value & Value()	   const { return val; }

    /// Alias for \ref Value
    n_value & operator()() const { return Value(); }

    self *	 Parent()	  { return parent; }

    const self * Parent()   const { return parent; }

    /// Previous sibling
    self *	 Psibling()	  { return psibling; }

    /// Previous sibling
    const self * Psibling() const { return psibling; }

    /// Next sibling
    self *	 Nsibling()	  { return nsibling; }

    /// Next sibling
    const self * Nsibling() const { return nsibling; }

    /// First child
    self *	 Fchild()	  { return fchild; }

    /// First child
    const self * Fchild()   const { return fchild; }

    /// Last child
    self *	 Lchild()	  { return lchild; }

    /// Last child
    const self * Lchild()   const { return lchild; }

    bool HasParent()   const { return parent; }

    bool HasSiblings() const { return psibling || nsibling; }

    bool HasChildren() const { return fchild; }

    bool IsParentOf( const self & c ) const { return c.parent == this; }

    bool IsSiblingOf( const self & s ) const { return parent && s.parent == parent; }

    bool IsChildOf( const self & p ) const { return parent == &p; }

    /// Depth: zero if no parent, otherwise 1 + parent's depth.
    unsigned Depth() const
    {
	self * l = const_cast<self *>( this );
	unsigned level = 0;

	while ( l->parent )
	{
	    l = l->parent;
	    ++level;
	}

	return level;
    }

    // tree walk

    bool IsDescendantOf( const self & n ) const
    {
	for ( const self * l = parent; l; l = l->parent )
	{
	    if ( l == &n )
		return true;
	}

	return false;
    }

    bool IsDescendantOf( const self * n ) const
    {
	return( n && IsDescendantOf( *n ) );
    }

    /// Root of the tree
    self & Top()
    {
	self * l = this;

	while ( l->parent )
	    l = l->parent;

	return *l;
    }

    /// Next node: depth first, pre-order.
    /// @param restart if true, the last node's Next is the first (\ref Top);
    ///   otherwise nilptr.
    self * Next( bool restart = false )
    {
	if ( fchild ) // down first
	    return fchild;

	self * l = this; // then next

	while ( !l->nsibling )
	{
	    if ( l->parent )
		l = l->parent;
	    else
		return restart ? l : 0; // at Top()
	}

	return l->nsibling;
    }

    self * Prev( bool restart = false )
    {
	if ( !psibling && parent )
	    return parent;

	if ( !psibling && !restart )
	    return 0;

	// have psibling or at Top() and restart:
	self * l = psibling ? psibling : this;

	while ( l->lchild )
	    l = l->lchild;

	return l;
    }

    /// Return \ref Next and assign it to *c*.
    self * Next( self *& c, bool restart = false )
    {
	return c = Next( restart );
    }

    /// Return \ref Prev and assign it to *c*.
    self * Prev( self *& c, bool restart = false )
    {
	return c = Prev( restart );
    }

    // const versions

    const self & Top() const
    {
	return const_cast<self *>( this )->Top();
    }

    const self * Next( bool restart = false ) const
    {
	return const_cast<self *>( this )->Next( restart );
    }

    const self * Prev( bool restart = false ) const
    {
	return const_cast<self *>( this )->Prev( restart );
    }

    const self * Next( const self *& c, bool restart = false ) const
    {
	return c = const_cast<self *>( this )->Next( restart );
    }

    const self * Prev( const self *& c, bool restart = false ) const
    {
	return c = const_cast<self *>( this )->Prev( restart );
    }

};

#endif // tnode_h

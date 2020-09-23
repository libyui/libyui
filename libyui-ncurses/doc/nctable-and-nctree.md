# libyui-ncurses: The Scary Widgets

Author: Stefan Hundhammer <shundhammer@suse.com>

Initial document 2020-09-22 23:30 - 1:30

_This started as a midnight brain dump to write down what was still fresh in my
head after a long, very deep dive into the NCurses UI._

_If it sounds somewhat incoherent in some places, you may be right._


# NCTable and NCTree

NCTable and NCTree are by far the most powerful widgets that libyui-ncurses has
to offer, thus also the most complex and scariest ones. Here is some
higher-level documentation about them.

NCTable is a multi-column table widget that supports multi-selection and (as of
9/2020) also _nested_ items, i.e. a tree-like structure. Each item is one line
of the table with multiple _cells_. Each cell can have its own label (text).

NCTable is a tree widget that also supports multi-selection, unlimited tree
depth, collapsing and expanding tree branches. Each item has one label (text).

Both widgets can scroll in both dimensions. See section _Pads_ below.


## UI Events: _notify_ and _immediate_

Both widgets can give instant feedback to the calling application if the
_notify_ option is set, and even more immediate when the _immediate_ option is
also set.

Normally in libyui, `UI.UserInput()` or `UI.WaitForEvent()` only return when
the user presses a PushButton or activates a MenuButton's item, not when
anything is going on in an input field or in a selection widget like a
SelectionBox, ComboBox, or a Table or Tree. Setting the _notify_ option changes
that: A widget with that option also maks `UI.UserInput()` return, and the
application can react immediately.

Notice that this is not always desirable: When a dialog is mostly an input form
waiting for the user to enter or select values, it makes perfect sense to wait
until the "OK" or "Next" button is pressed.

But when the status of other widgets depends on what the user selected in a
ComboBox or a Table, it makes sense to add the _notify_ option to that ComboBox
or Table so the dialog can handle that changed status.

In the Qt UI, there is not much difference between _notify_ and _immediate_.

But NCurses is special in that it does not just always return, even when a
widget has the _notify_ option set; since the user can only use the keyboard,
that would mean that each CursorDown press in a selection widget would already
make the application react because the selected item just changed. If that
causes major screen updates in other widgets, performance suffers; so typically
only when the user explicitly confirms a selection with the Return key (or
sometimes the Space key), UI.UserInput() returns.

The old language selection in the YaST installer was such an example: Moving
the selection up or down the SelectionBox caused translations for that language
to be loaded, all texts in that dialog retranslated, the complete dialog
completely redisplayed. That was slow; a really bad user experience.

So, this only happens with the _immediate_ option in addition to _notify_.


## Virtual Scrollable Windows: _Pads_

In NCurses, a _pad_ is a virtual window that can scroll in both dimension, and
that can provide (more or less) unlimited screen space. Thus, all scrollable
widgets in libyui-ncurses use a _pad_; they all inherit _NCPadWidget_ which
provides a method _myPad()_ that returns an NCPad. **NCPad is not a widget.**
It is a stand-alone class very closely related to the low-level NCurses
_window_.

Since most widgets use a specialized subclass of NCPad, they overwrite
_myPad()_ with a type cast that returns the kind of pad they are dealing with:

- NCTable::myPad() returns an NCTablePad
- NCTree::myPad() returns an NCTreePad

- NCTablePad inherits NCTablePadBase inherits NCPad
- NCTreePad inherits NCTablePadBase inherits NCPad

- NCTable inherits YTable and NCPadWidget and has an NCTablePad
- NCTree inherits YTree and NCPadWidget and has an NCTreePad


A _pad_ deals with lines:

- NCTablePad has NCTableLines
- NCTreePad has NCTreeLines which inherit NCTableLines

Lines have a flat structure, even when we are dealing with tree-structured
items.


## YSelectionWidgets

All selection widgets in libyui inherit YSelectionWidget:

- YTable inherits YSelectionWidget
- YTree inherits YSelectionWidget
- YMenuBar inherits YMenuWidget inherits YSelectionWidget
- YMenuButton inherits YMenuWidget inherits YSelectionWidget
- YComboBox  inherits YSelectionWidget
- YSelectionBox inherits YSelectionWidget
- YMultiSelectionBox  inherits YSelectionWidget
- YDumbTab inherits YSelectionWidget

They all deal with YItems of some kind.


## YItems

YItem on the libyui level is a simple class with basically just a _label_ text
and a _selected_ status. But since we always need a connection to some other
object, it can also store a transparent _data_ pointer. YItem never does
anything with that pointer, it only stores it.

A widget in a concrete UI can use that to store a counterpart to a different
type of item, for example a Qt QListWidgetItem. This serves as a connection
between the abstract libyui world and a concrete toolkit like Qt or NCurses.

On the downside, YItem can only store a void pointer, and you have to type-cast
that pointer (without any type checking for safety) to the correct pointer type
every time you use it.

On the side of the concrete Qt or NCurses widget it is advisable to store a
pointer to the corresponding YItem in each item to access the YItem efficiently.

YItem also provides an _index_ by which it can be uniquely identified.
Similarly to the _data_ pointer, YItem only stores it for use from the outside;
the application has to set the index and make sure it is really unique within
its context.

YItem has some subclasses:

- YTreeItem inherits YItem
- YTableItem inherits YTreeItem inherits YItem

YTreeItem adds tree structure, YTableItem adds multiple columns (cells).

Notice how (for historical reasons) the inheritance hierarchy is the other way
round as with NCurses item classes:

- NCTreeLine inherits NCTableLine
- YTableItem inherits YTreeItem inherits YItem

YSelectionWidget has an YItemCollection which is just a
std::vector<YItem*>. Use YSelectionWidget::itemsBegin() and
YSelectionWidget::itemsEnd() to iterate over them.

Notice that for tree-structured (nested) items, the YItemCollection itself only
contains the toplevel items. But each YItem provides iterators
YItem::childrenBegin() and YItem::childrenEnd() to dive deeper into the hierarchy.

On the YItem level, the children-related methods are only empty stubs that do
nothing; YTreeItem (and thus YTableItem) provides the real functionality and
holds a YItemCollection for its (direct) children.

All that means that you can safely operate recursively on tree-structured items
on the YItem level without using a dynamic cast.

YItems are **always** owned by the YSelectionWidget. Never attempt to delete
one! Always use methods of YSelectionWidget and its libyui subclasses.

Where toplevel YItems live in their YSelectionWidget's YItemCollection, a child
YItem lives in its parent's YItemCollection (childrenBegin(), childrenEnd()).

YItems are managed on the libyui level. They serve as the original to copy from
when creating counterparts on the UI toolkit (Qt, NCurses) level.

But don't just copy their data away and forget them; there is no need to
duplicate a YItem's label text in an NCurses item. As long as the NCurses item
is relevant, the YItem is always there to fetch data from; similar with
YTableItem cells.


## NCPad Lines: NCTableLine, NCTreeLine

NCPad-related items like NCTableLine and NCTreeLine each correspond to one
YTableItem or NCTreeItem, but they do not inherit any libyui item class.

They store a pointer to the YItem counterpart, and that pointer typically needs
a lot of dynamic casting to do anything useful. Thus there is an unfortunate
tendency in the NCurses classes to duplicate a lot of data that would actually
be available directly from the YItem: NCTableLine stores a vector of NCTableCol
pointers to hold each _cell_.

There are also pointers for the tree structure (_parent_, _nextSibling_,
_firstChild_) and a number of tree-related methods.

There are also some state variables for the item's attribute flags: _normal_,
_active_(the currently selected item), _disabled_, _hidden_, _headline_. Those
influence the visual appearance (background / foreground color attributes) or,
in the case of _hidden_, if the item is displayed at all; for example, if a
tree branch is collapsed, each of the collapsed items would have the _hidden_
attribute set.

Those attributes are handled on the NCPad level. It is usually enough to set
the correctly initially.


## NCTableCol

NCTableCol corresponds to one cell in an NCTableLine; it has a _label_ text and
a _style_ attribute. And it can draw itself.

Of course an NCTableLine can also draw itself, but what it does is only to
iterate over its cells (its NCTableCols) and let them draw themselves at the
correct screen positions.


## items vs. visibleItems

NCTreePad NCTablePad store a vector of their NCTreeLine / NCTableLine items in
_items_. That is where they live, and where they are owned. _Lines()_ returns
the number of items there, and you can safely iterate with a simple _for_ loop
with an _unsigned_ (!) loop counter over them one by one.

But that's not what you see on the screen; that contains everything, even
the content of collapsed tree branches.

_visibleItems_ on the other hand is a vector of those NCTreeLine / NCTableLine
items that are currently visible on screen, and _visibleLines()_ returns their
number. This loop iterates over all the visible lines, i.e. all that can be
made visible by scrolling the pad around:

```C++
    for ( unsigned i=0; i < visibleLines(); i++ )
    {
        doSomething( _visibleItems[i] );
    }
```

_visibleItems_ does not own any of the NCTableLines; it stores only pointers to
the NCTableLines owned by _items_.


## NCTableTags

Some pads need to display status information that is independent on an item's
label text; things like selection markers, for example:

```
    [ ]
```

or

```
    [x]
```

That is sometimes done (e.g. in NCTableLine) with a special subclass of
NCTableCol called NCTableTag: This is only yet another table cell that draws
itself, but not with a text that it gets from a YItem, but with a fixed text
like those selection markers depending on its internal status:

```C++
    if ( selected() )
        draw( "[x]" );
    else
        draw( "[ ]" );
```

Having such a special class even for this primitive purpose takes care of
reserving screen space in a table for drawing such status information.



# Key Event Handlers

For an NCTable, we have the NCTable widget, the NCTablePad, and
NCTableLines. But where are keyboard events handled? What happens when the user
presses the _Space_ bar or the cursor keys? Who handles the events?

The answer is: All of them, in the right sequence, and all the way up and down
the inheritance hierarchy.

The NCurses UI first identifies the widget with the keyboard focus and sends
the key event there; more exactly, it calls its _wHandleInput()_ method. This
is inherited from NCWidget but reimplemented in basically every widget
subclass.

The NCTable widget first hands the key event over to its NCTablePad; the pad
forwards it to it current NCTableLine item. If the item doesn't know what to do
with the key event, it signals that with a _false_ return code, and the pad
checks if it can do anything with it; if not, it goes back up to the widget.

The underlying idea is to handle each key event as locally as possible: The
item might know how to handle pressing the `+` key to open a tree branch. It
knows if it even has children so there is a branch that can be opened.

One level higher, the pad knows how to move the cursor (the currently selected
item) up or down or how to scroll the pad.

On the widget level, the widget can assemble an NCursesEvent to return to
UI.UserInput() if the _notify_ and/or _immediate_ flags are set.

The complete sequence for NCTable is:

- NCTable::wHandleInput()
- NCTablePad::handleInput()
- NCTablePadBase::handleInput()
- NCTableLine::handleInput()

For NCTree it goes through more inherited class layers:

- NCTree::wHandleInput()
- NCTreePad::handleInput()
- NCTablePadBase::handleInput()
- NCTreeLine::handleInput()
- NCTableLine::handleInput()

Many of those methods are only stubs that do nothing, only propagating the
event further to the next layer. But they are there in case they need to be
extended. The reasoning is that xit's not at all easy to figure out what method
to override in what class and how to make sure that it is actually called. So
the infrastructure for this is now already in place for the NCTable / NCTree
familiy, waiting to be used when necessary.


# NCurses-Pkg

When doing any change in libyui-ncurses or more specifically in NCTable /
NCTree, beware that the NCPackageSelector in libyui-ncurses-pkg makes heavy use
of those widgets.

For application code, libyui-ncurses is **not** a valid API to use;
applications are to use strictly the libyui API, either in C++, or via Ruby
with the UI interpreter (yast-ycp-ui-bindings), or with the SWIG bindings.

But NCurses-Pkg, being an extension of the NCurses UI to provide package
management functionality, is an exception: It uses widgets like NCTable and
NCTree directly, and often it uses them in very creative ways.

So make sure to always also build libyui-ncurses-pkg and _test_ it:

```Shell
    cd ~/src/libyui-ncurses/build
    make && sudo make install

    cd ~/src/libyui-ncurses-pkg/build
    make && sudo make install
    sudo yast sw_single
```

It is unfortunate that right now this can realistically only be done with the
real thing, the YaST software module, and only running as root.

**Make sure to use a VM that you can easily revert to a working snapshot** in
case anything goes horribly wrong.


# Misc

## NCTable multiSelection

NCTable has a widget option to enable multi-selection. In that case, a user can
select more than one item.

Each item gets a marker `[ ]` or `[x]` in its first column. This uses an
NCTableTag (see below) to add another cell before the real content cells are
added.



## NCTable nestedItems

NCTable displays line graphics for the tree when it detects that at least one
item has children; it does that fully automatically.

Most applications won't even notice because they don't add child items; the
widget behaves like a flat table.

But that automatic detection depends on the application behaving nicely: If it
adds all its items at once with `YSelectionWidget::addItems( YItemCollection )`,
everything works right out of the box.

But it might stop working when an application adds items one by one, and the
first few don't have child items; those would be added in normal _flat_ table
mode, and later ones (starting from the first item with a child item) would be
added in _nested_ mode, leaving space for tree indentation where line graphics
will later be drawn.

The fix is simple: Build a YItemCollection and add all items at once.

This is a known limitation of that nested items feature; but it saves the
application developers having to remember to set a special flag or widget
option like _nestedItems_.


## NCTable Sorting

With the advent of the nested items feature it is no longer simple to sort a
table: The tree structure of the items has to be taken into account, sorting
each tree branch on each level individually.

NCTableLines are flat lines by nature, living in a vector with no tree
structure; that makes sorting difficult.

So NCTable goes an unorthodox way: When sorting is requested, it simply deletes
all NCTableLines, sorts the YItems on the YItem level recursively, and
recreates the NCTableLines from the now sorted YItems.


### NCTable sortStrategy

NCTable has an auxiliary _sortStrategy_ object that can be exchanged for another
one. This _sortStrategy_ handles comparisons between YItems, by default on the
basis of one column.

The default _sortStrategy_ uses the _sortKey_ that each YTableCell supports, and
if none was set (which is the usual case), it uses the _label_ of that cell.

It tries to be smart, attempting to convert the content to a numeric value and
doing a numeric comparison when possible.


The NCurses-Pkg NCPkgTable widget has its own sort strategy that uses libzypp
calls for comparing packages against each other, e.g. by package size.


### Interactive Sorting

Ctrl-O (for "Order") in an NCTable opens a pop-up with the table headers to let
the user interactively choose a column to sort by. Sorting by the same column
again reverses the sort order, sorting by the same column once more reverses it
back to normal.

The _keepSorting_ flag / widget option diables this, leaving the item insertion
order intact.


## Testing

### Ruby UI Examples

At the time of this writing, there is no automated test suite for
libyui-ncurses.

But there are lots of example programs in the yast-ycp-ui-bindings package:

https://github.com/yast/yast-ycp-ui-bindings/tree/master/examples

The package name is deceiving; it no longer deals with the old YCP scripting
language. It's about Ruby now; but it deals with YCPValue containers to
transport data.

For NCTable, look at the `Table*.rb` examples; for NCTree, it's `Tree*.rb`.

In general, examples start simple and become increasingly more complex:
`Table1.rb` demonstrates the most basic use of a table widget, `Table2.rb`
becomes a little bit more complex etc.

For everything related to UI event handling, use the `Events.rb` example. It
also serves as a widget gallery for (almost?) all available widget types.

Call an example with

```Shell
    y2base ./Events.rb ncurses
```

or

```Shell
    y2base ./Events.rb qt
```

(make sure to have a symlink to the `/usr/lib/YaST2/bin/y2base` binary from a
convenient directory like `$HOME/bin` that you have in your `$PATH`).

The Ruby examples are logging to `$HOME/.y2log`.


### C++ Examples

There are also some C++ examples in libyui/examples:

https://github.com/libyui/libyui/tree/master/examples

Make sure to test them as well; some details tend to be subtly different from
using libyui from Ruby via the UI interpreter. The C++ examples tend to be
fewer, but more elaborate, showcasing more features.

Each example is a standalone binary. You can invoke it directly:

```Shell
    cd libyui/build/examples
    ./Table-nested-items
```

if `$DISPLAY` is set, this will start it with the Qt UI. To start it with
NCurses, make sure `$DISPLAY` is not set:

```Shell
    DISPLAY="" ./Table-nested-items
```

or

```Shell
    unset DISPLAY
    ./Table-nested-items
```

The C++ examples are logging to `/tmp/libyui-examples.log`.

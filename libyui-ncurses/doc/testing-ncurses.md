# libyui-ncurses: Testing

Author: Stefan Hundhammer <shundhammer@suse.com>

Initial document: 2020-09-24

# Automated Testing

At the time of this writing, there is no automated test suite for
libyui-ncurses.


# Manual Testing Basics


## Ruby UI Examples


There are lots of example programs in the yast-ycp-ui-bindings package:

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


## C++ Examples

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


## Quick Test: Events.rb

- Start the Events.rb example. It showcases more or less all widgets.

  - Do all the selection widgets have items? From left to right, there are
    - A tree
    - A SelectionBox
    - A MultiSelectionBox
    - A Table
    - A RichText

    Below that, there are
    - A ComboBox
    - A MultiLineEdit ("Address")
    - A RadioBox

    The "Address" multi-line input field starts up emtpy; this is normal and expected.
    All other widgets should have content.

  - The widgets should also react when you select something in them: You should
    see the description of an event in the white "Event" OutputField. After a
    few seconds, the event is overwritten by a TimeoutEvent; this is normal.

  - Watch the y2log while you are experimenting. Are there any errors or
    warnings from the UI?


## Deeper Test: MyWidget*.rb

Check out the examples/ directory in yast-ycp-ui-bindings. If you changed any
specific widget, invoke the examples one by one.

In an ideal world, we would have more of the elaborate examples like Events.rb
where you can do more experimenting. This is work in progress.

For more recent widgets like ItemSelector or MenuBar, there are such examples,
usually even in a C++ version and in a Ruby version. Make sure to invoke both;
for Ruby, there is the UI interpreter as an intermediate layer that subtly
changes things like adding YCPValue-based IDs to widgets and items.

The C++ API on the other hand gives an application developer more freedom, so
there are more cases to consider.


## Testing the Work Horse: NCTable(Pad) and Friends

Any change to NCTablePad and its base class NCTablePadBase can have effects on
a lot of widgets:

- NCTable (as expected)
- NCTree (which uses an NCTreePad which inherits NCTablePad)

- NCFileSelection
- NCItemSelector
- NCMultiSelectionBox
- NCSelectionBox

...and in the future possibly even more.

So make sure to test all of them after changes to NCTablePad or NCTablePadBase.

Also, the table classes from libyui-ncurses-pkg are using it heavily:

- NCPkgTable
- NCPkgLocaleTable
- NCPkgRepoTable

So if you change anything here, make sure to test all those classes.


### Testing Nested Table Items

Use those UI examples:

- Table-nested-items.rb
- Table-nested-multiSel.rb
- Table-flat-items.rb
- Table-flat-multiSel.rb

That uses all combination of nested vs. flat items with or without
multi-selection. Also, test the Tree with and without multi-selection,
e.g. with:

- Tree2.rb
- Tree-Checkbox1.rb



## Testing the PackageSelector


### UI Examples

There are some UI examples for the PackageSelector widget. They use libzypp,
but without root permissions, so some operations are not possible.


```Shell
    cd src/yast-ycp-ui-bindings/examples

    y2base ./PackageSelector.rb ncurses
    y2base ./PackageSelector-YOU.rb ncurses
```

or

```Shell
    cd src/yast-ycp-ui-bindings/examples

    y2base ./PackageSelector.rb qt
    y2base ./PackageSelector-YOU.rb qt
```


### The YaST Software Module

For a full-blown test of all PackageSelector features, you will need to start
the YaST software module with root permissions.

**Make sure to use a virtual machine that you can easily revert to a working
snapshot** in case anything goes horribly wrong.

NCurses:

```Shell
    sudo yast sw_single
```

or

```Shell
    sudo DISPLAY="" yast2 sw_single
```

Qt:

```Shell
    xhost +
    sudo yast2 sw_single
```

### What to Watch out For

- When you select a package in the packages list on the right, are the details
  for the correct package displayed in the "Details" panel below the list?

- When you change the status of a package, is the status set for the correct package?
  Does the status indicator change? Is it displayed in the correct table line?

- When you switch to "Installation Summary", does it show your change?


## Further Reading

- "libyui-ncurses: The Scary Widgets; NCTable and NCTree"
[nctable-and-nctree.md](nctable-and-nctree.md) in the same directory

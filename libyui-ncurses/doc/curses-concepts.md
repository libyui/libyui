# Curses Concepts

This is a *short* introduction, or refresher, to supplement the [reference
materials][ref] and [tutorials][tut].

[ref]: https://invisible-island.net/ncurses/man/index.html
[tut]: https://invisible-island.net/ncurses/ncurses-intro.html

The important concepts being defined are set in bold: **foo**.

## Terminal Concepts

A **terminal** screen is a rectangular grid of **cells**, each having a
**character** and some **attributes**, like foreground and background color,
or bold font.

The program controls the position and attributes of printable characters by
printing **control sequences** (which start with a nonprintable character).

For input, some (printing) keys send the program a single character, other
(control) keys send a sequence of characters.

The input may be buffered and sent to the program a line at a time (**cooked
mode**), or unbuffered with keys sent directly (**raw mode**).

The terminal may **echo** the typed keys: print them without the program doing
it. This is a mode that can be enabled and disabled.

Now back to:

## Curses Concepts

The output goes to the **screen**.

The **cursor** is the screen position where text will be output if coordinates
are not specified.  **Coordinates** begin at top left (0, 0), in the order
(**line**, **column**), in other words (y, x).

A **window** is a rectangular part of the *screen*. They may *not*
overlap. (For overlapping things, see *panel*.)

Curses optimizes for minimal changes to the terminal to accommodate slow
networking over modems. Therefore it needs a **`refresh`** call to actually
update the terminal. When using multiple windows, *`refresh`*-ing them all
would flicker. But a *`refresh`* is implemented as **`noutrefresh`** +
**`doupdate`**, so we should call the former for all windows and then the
latter for the *screen*.

A **pad** is a virtual window that may be bigger than the screen. Create it
with a size and no origin, its *`refresh`* call will display a section of it
at a chosen position of the screen.

A **panel** is a member of a global stack of overlapping windows.

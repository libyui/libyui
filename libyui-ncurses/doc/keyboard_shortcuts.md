# Libyui-ncurses Keyboard Shortcuts

The libyui-ncurses interface implements several keyboard shortcuts.

## Widget Shortcuts

Some widgets use special keyboard shortcuts. Make sure that the respective
widget is active (the cursor is placed there).

### Table

- `Ctrl+O` - Change the ordering. After pressing it a list with the column
  names is displayed. Select which column should be used for ordering, to
  change the direction (ascending/descending) select the same column again.

## Global Shortcuts

These shortcuts can be used anytime, it does not matter which widget is active
(or if any widget is active at all).

- `Shift+F1` - Display a hot key help.

- `Ctrl+L` - Redraw the current screen. If for whatever reason the screen
  content it not displayed properly you can force redrawing the whole screen.

- `Shift+F4` - Change the color style. This cycles over the built-in color styles.

- `Shift+F6` - Ask for widget ID. Enter the ID of the button which should be
  activated. This is intended for automated tests.

- `Ctrl+D Shift+D` - Dump the current dialog into the log. It dumps the screenshot
  and also the logical structure of the dialog (the widget tree).

- `Ctrl+D Shift+Y` - Start the widget inspector. Displays a special dialog which
  can navigate in the widget tree and display detailed widget properties. It can
  also change some widget properties and even delete or add new widgets.

- `Ctrl+D Shift+G` - Send a debug event. This should start the debugger in the
  running application.

- `Ctrl+D Shift+C` - Send a configuration event. This should display a special
  dialog for configuring the application.

- `Ctrl+D Shift+S` (only when `Y2NCDBG` environment variable is set) - Display
  a special style testing dialog.

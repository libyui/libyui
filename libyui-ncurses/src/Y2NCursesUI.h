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

   File:       Y2NCursesUI.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
// -*- c++ -*-

#ifndef Y2NCursesUI_h
#define Y2NCursesUI_h

#include <iosfwd>

#include "YUIInterpreter.h"
#include "NCurses.h"

class NCDialog;

ostream & operator<<( std::ostream & STREAM, const YUIInterpreter::EventType ev );

class Y2NCursesUI : public NCurses, public YUIInterpreter {

  protected:

    virtual bool want_colors();
    virtual void init_title();

  public:

    Y2NCursesUI( bool with_threads, Y2Component *callback );
    virtual ~Y2NCursesUI();

    /**
     * Idle around until fd_ycp is readable
    */
    virtual void idleLoop( int fd_ycp );

    /**
     * Event loop and return as soon as the user has pressed some widget that
     * passes control back to the module. Currently this are push buttons and
     * the window-close button.
    */
    virtual YWidget * userInput( YDialog * dialog, EventType * event );

    /**
     * like userInput, but return 0(ET_NONE) after timeout_millisec milliseconds of inactivity
    */
    virtual YWidget * timeoutUserInput( YDialog * dialog, EventType * event, unsigned timeout_millisec );

    /**
     * This virtual method is called, when the YCPUIInterpreter evaluates
     * the YCP command <tt>PollInput()</tt>. You should <i>not</i> go into
     * your event loop but just look whether there is pending user input.
    */
    virtual YWidget * pollInput( YDialog * dialog, EventType * event );

    /**
     * Inherited from YUIInterpreter. Creates a dialog.
    */
    virtual YDialog * createDialog( YWidgetOpt & opt );

    /**
     * Inherited from YUIInterpreter. Shows and activates a dialog
    */
    virtual void showDialog( YDialog * dialog );

    /**
     * Inherited from YUIInterpreter. Decativates and closes a dialog
    */
    virtual void closeDialog( YDialog * dialog );

    //
    // Widget creation functions - container widgets
    //

    /**
     * Creates a split
    */
    virtual YContainerWidget * createSplit( YWidget * parent, YWidgetOpt & opt,
					    YUIDimension dimension );

    /**
     * Creates a replace point.
    */
    virtual YContainerWidget * createReplacePoint( YWidget * parent, YWidgetOpt & opt );

    /**
     * Creates an alignment widget
    */
    virtual YContainerWidget * createAlignment( YWidget * parent, YWidgetOpt & opt,
						YAlignmentType halign,
						YAlignmentType valign);

    /**
     * Creates a squash widget
    */
    virtual YContainerWidget * createSquash( YWidget * parent, YWidgetOpt & opt,
					     bool hsquash,
					     bool vsquash );

    /**
     * Creates a radio button group.
    */
    virtual YContainerWidget * createRadioButtonGroup( YWidget * parent, YWidgetOpt & opt );

    /**
     * Creates a frame widget
    */
    virtual YContainerWidget * createFrame( YWidget * parent, YWidgetOpt & opt,
					    const YCPString & label );

    //
    // Widget creation functions - leaf widgets
    //

    /**
     * Creates an empty widget
    */
    virtual YWidget * createEmpty( YWidget * parent, YWidgetOpt & opt );

    /**
     * Creates a spacing widget
    */
    virtual YWidget * createSpacing( YWidget * parent, YWidgetOpt & opt,
				     float size,
				     bool horizontal,
				     bool vertical );

    /**
     * Creates a label.
     * @param text Initial text of the label
     * @param heading true if the label is a Heading()
     * @param output_field true if the label should look like an output field (3D look)
    */
    virtual YWidget * createLabel( YWidget * parent, YWidgetOpt & opt,
				   const YCPString & text );

    /**
     * Creates a rich text widget
     * @param text Initial text of the label
    */
    virtual YWidget * createRichText( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & text );

    /**
     * Creates a log view widget
     * @param label label above the log view
     * @param visibleLines default number of vislible lines
     * @param maxLines number of lines to store (use 0 for "all")
    */
    virtual YWidget * createLogView( YWidget * parent, YWidgetOpt & opt,
				     const YCPString & label,
				     int visibleLines,
				     int maxLines );

    /**
     * Creates a MultiLineEdit widget
     * @param label label above the edit field
     * @param text initial contents of the edit field
    */
    virtual YWidget * createMultiLineEdit( YWidget * parent, YWidgetOpt & opt,
					   const YCPString & label,
					   const YCPString & text);

    /**
     * Creates a push button.
     * @param label Label of the button
     * @param default_button true if the button should be the dialogs default button
    */
    virtual YWidget * createPushButton( YWidget * parent, YWidgetOpt & opt,
					const YCPString & label );

    /**
     * Creates a menu button.
     * @param label Label of the button
     */
    virtual YWidget * createMenuButton( YWidget * parent, YWidgetOpt & opt,
					const YCPString & label );

    /**
     * Creates a radio button and inserts it into a radio button group
     * @param label Label of the radio button
     * @param rbg the radio button group the new button will belong to
    */
    virtual YWidget * createRadioButton( YWidget * parent, YWidgetOpt & opt,
					 YRadioButtonGroup * rbg,
					 const YCPString & label,
					 bool checked );

    /**
     * Creates a check box
     * @param label Label of the checkbox
     * @param true if it is checked
    */
    virtual YWidget * createCheckBox( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & label,
				      bool checked );

    /**
     * Creates a text entry or password entry field.
    */
    virtual YWidget * createTextEntry( YWidget * parent, YWidgetOpt & opt,
				       const YCPString & label,
				       const YCPString & text );

    /**
     * Creates a selection box
    */
    virtual YWidget * createSelectionBox( YWidget * parent, YWidgetOpt & opt,
					  const YCPString & label );

    /**
     * Creates a multi selection box
     */
    virtual YWidget * createMultiSelectionBox( YWidget *parent, YWidgetOpt & opt,
					       const YCPString & label );

    /**
     * Creates a combo box
    */
    virtual YWidget * createComboBox( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & label );

    /**
     * Creates a tree
    */
    virtual YWidget * createTree( YWidget * parent, YWidgetOpt & opt,
				  const YCPString & label );

    /**
     * Creates a table widget
    */
    virtual YWidget * createTable( YWidget * parent, YWidgetOpt & opt,
				   vector<string> header );

    /**
     * Creates a progress bar
    */
    virtual YWidget * createProgressBar( YWidget * parent, YWidgetOpt & opt,
					 const YCPString & label,
					 const YCPInteger & maxprogress,
					 const YCPInteger & progress);

    /**
     * Creates an image widget from a YCP byteblock
    */
    virtual YWidget * createImage( YWidget * parent, YWidgetOpt & opt,
				   YCPByteblock imagedata,
				   YCPString defaulttext);

    /**
     * Creates an image widget from a YCP byteblock
    */
    virtual YWidget * createImage( YWidget * parent, YWidgetOpt & opt,
				   YCPString filename,
				   YCPString defaulttext);

    /**
     * Creates an image widget from a predefined set of images
    */
    virtual YWidget * createImage( YWidget * parent, YWidgetOpt & opt,
				   ImageType img,
				   YCPString defaulttext );

    /**
     * Creates an IntField widget.
    */
    virtual YWidget * createIntField( YWidget * parent, YWidgetOpt & opt,
				      const YCPString & label,
				      int minValue, int maxValue,
				      int initialValue );

    /**
     * Creates the PackageSelector widget (i.e. a widget tree).
     */
    virtual YWidget * createPackageSelector( YWidget *parent,
					     YWidgetOpt &opt,
					     const YCPString & floppyDevice );

    /**
     * Fills the PackageSelector widget.
     */
    virtual YCPValue runPkgSelection( YWidget * packageSelector );
    
    /**
     * Creates a special subwidget used for Package Selection (which doesn't exist in QT-UI).
     */
    virtual YWidget * createPkgSpecial( YWidget *parent,
					YWidgetOpt &opt,
					const YCPString &subwidget );
    
    /**
     * UI-specific setLanguage() function.
     * Returns YCPVoid() if OK and YCPNull() on error.
     * This default implementation does nothing.
    */
    virtual YCPValue setLanguage( const YCPTerm & term );

    /**
     * UI-specific setConsoleFont() function.
     * Returns YCPVoid() if OK and YCPNull() on error.
    */
    virtual YCPValue setConsoleFont ( const YCPString & console_magic,
				      const YCPString & font,
				      const YCPString & screen_map,
				      const YCPString & unicode_map,
				      const YCPString & encoding );


    /**
     *
     * Open a directory selection box and prompt the user for an existing directory.
     * [Inherited from YUIInterpreter]
     *
     * 'startDir' is the initial directory that is displayed.
     *
     * 'headline' is an explanatory text for the directory selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected directory name
     * or 'nil' (YCPVoid()) if the user canceled the operation. 
     **/
    virtual YCPValue askForExistingDirectory ( const YCPString & startDir,
					       const YCPString & headline )
	{ return YCPVoid(); }
    
    /**
     * Open a file selection box and prompt the user for an existing file.
     * [Inherited from YUIInterpreter]
     * 
     * 'startWith' is the initial directory or file.
     * 
     * 'filter' is one or more blank-separated file patterns, e.g. "*.png *.jpg"
     * 
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     * 
     * Returns the selected file name
     * or 'nil' (YCPVoid()) if the user canceled the operation.
     **/
    virtual YCPValue askForExistingFile	( const YCPString & startWith,
					  const YCPString & filter,
					  const YCPString & headline )
	{ return YCPVoid(); }
	

    /**
     * Open a file selection box and prompt the user for a file to save data to.
     * Automatically asks for confirmation if the user selects an existing file.
     * [Inherited from YUIInterpreter]
     * 
     * 'startWith' is the initial directory or file.
     * 
     * 'filter' is one or more blank-separated file patterns, e.g. "*.png *.jpg"
     * 
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     * 
     * Returns the selected file name
     * or 'nil' (YCPVoid()) if the user canceled the operation.
     **/
    virtual YCPValue askForSaveFileName	( const YCPString & startWith,
					  const YCPString & filter,
					  const YCPString & headline )
		{ return YCPVoid(); }

    
    /**
     * UI-specific redrawScreen method.
     * This default implementation does nothing.
    */
    virtual void redrawScreen() { Refresh(); }

    /**
     * UI-specific getDisplayInfo() functions.
     * See UI builtin GetDisplayInfo() doc for details.
     **/
    virtual int  getDisplayWidth()           { return cols(); }
    virtual int  getDisplayHeight()          { return lines(); }
    virtual int  getDisplayDepth()           { return -1; }
    virtual long getDisplayColors()          { return NCattribute::colors(); }
    virtual int  getDefaultWidth()           { return cols(); }
    virtual int  getDefaultHeight()          { return lines(); }
    virtual bool textMode()                  { return true; }
    virtual bool hasImageSupport()           { return false; }
    virtual bool hasIconSupport()            { return false; }
    virtual bool hasFullUtf8Support()        { return false; }

  private:

    bool setLanguage( string lang );
};

#endif // Y2NCursesUI_h

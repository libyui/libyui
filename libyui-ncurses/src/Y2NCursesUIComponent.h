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

   File:       Y2NCursesUIComponent.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
// -*- c++ -*-

#ifndef Y2NCursesUIComponent_h
#define Y2NCursesUIComponent_h

#include <iosfwd>

#include <Y2.h>
#include "Y2UIComponent.h"
#include "NCurses.h"

class Y2UIComponent;
class NCDialog;

/**
 * @short YaST2 Component: NCursesUI user interface
 * The YaST2 Component realizes a NCursesUI based user interface with an
 * embedded YCP interpreter.
 */
class Y2NCursesUIComponent : public NCurses, public Y2UIComponent
{
   /**
    * Used to store the server options until the server is launched
    */
   int argc;

   /**
     * Used to store the server options until the server is launched
     */
   char **argv;

    /**
     * since we're defining our own setCallback/getCallback function
     * we must save the callback pointer ourselfs. See Y2Component.
     */
    Y2Component *m_callback;

    /**
     * This is false, if not threads should be used.
     */
    bool with_threads;

public:
   /**
    * Initialize data.
     */
   Y2NCursesUIComponent(int argc, char **argv, bool with_threads, Y2Component *callback);

   /**
     * Cleans up.
     */
   ~Y2NCursesUIComponent();

   /**
     * The name of this component
     */
   string name() const;

   /**
     * Is called by the genericfrontend, when the session is finished.
     * Close the user interace here.
     */
   void result(const YCPValue &result);

   /**
     * Is called by the genericfrontend after it parsed the commandline.
     * gives the QT UI its commandline options. We store it here and
     * wait until we create the interpreter in @ref #evaluate.
     */
   void setServerOptions(int argc, char **argv);

#if 0
    /**
     * Functions to pass callback information
     * The callback is a pointer to a Y2Component with
     * a valid evaluate() function.
     */

    Y2Component *getCallback (void) const;
    void setCallback (Y2Component *callback);
#endif

  protected:

    virtual bool want_colors();
    virtual void init_title();

   /**
    * Global reference to the UI
    **/
   static Y2NCursesUIComponent * _ui;
    
  public:

//    Y2NCursesUI( bool with_threads, const char * macro_file, Y2Component *callback );
//    virtual ~Y2NCursesUI();

   /**
    * Access the global Y2NCursesUI.
    */
    static Y2NCursesUIComponent * ui() { return _ui; }
    
    /**
     * Idle around until fd_ycp is readable
    */
    virtual void idleLoop( int fd_ycp );

    /**
     * Go into event loop until user input is available or until the specified
     * timeout (in milliseconds) has elapsed. 
     *
     * Reimplemented from YUIInterpreter.
     */
    virtual YEvent * userInput( unsigned long timeout_millisec );

    /**
     * Check the event queue for user input. Don't wait.
     *
     * Reimplemented from YUIInterpreter.
     */
    virtual YEvent * pollInput();

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

    virtual YCPValue setKeyboard ( );
    
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
					       const YCPString & headline );
    
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
					  const YCPString & headline );
	

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
					  const YCPString & headline );

    
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
    
    virtual bool hasFullUtf8Support();

  private:

    bool setLanguage( string lang );
};

#endif // Y2NCursesUIComponent_h


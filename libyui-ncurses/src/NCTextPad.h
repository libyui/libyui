/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCTextPad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTextPad_h
#define NCTextPad_h

#include <iosfwd>
#include <list>

#include "NCPad.h"
#include "NCtext.h"

using std::list;


class NCTextPad : public NCPad
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTextPad & OBJ );

    NCTextPad & operator=( const NCTextPad & );
    NCTextPad( const NCTextPad & );

private:

    list<unsigned>	     lines;
    list<unsigned>::iterator cline;

    wpos curs;
    bool curson;

    void cursor( bool on );
    void cursorOn()  { cursor( true ); }

    void cursorOff() { cursor( false ); }

    bool insert( wint_t key );
    bool delch( bool previous = false );
    bool openLine();

    void assertSze( wsze minsze );
    void assertWidth( unsigned minw );
    void assertHeight( unsigned minh );

    // specifies how much characters can be inserted. -1 for unlimited input
    int InputMaxLength;

protected:

    virtual int setpos( const wpos & newpos );

public:

    NCTextPad( int lines, int cols, const NCWidget & p );
    virtual ~NCTextPad();

public:

    virtual void resize( wsze nsze );
    virtual int setpos();
    virtual wpos CurPos() const;
    virtual bool handleInput( wint_t key );

    void setText( const NCtext & ntext );
    wstring getText() const;

    // limits  the input to numberOfChars characters and truncates the text
    // if appropriate
    void setInputMaxLength( int nr );
};


#endif // NCTextPad_h

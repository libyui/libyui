/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt objects		       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2CharValidator.h

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt object - it can be used independently of YaST2.

/-*/


#ifndef QY2CharValidator_h
#define QY2CharValidator_h

#include <qvalidator.h>


class QY2CharValidator : public QValidator
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * Create a validator object that checks for input characters being any one
     * of 'validChars'. If this is empty, any input is accepted.
     **/
    QY2CharValidator( const QString &	initialValidChars,
		      QObject *		parent	= 0,
		      const char *	name	= 0 );

    /**
     * Destructor.
     **/
    ~QY2CharValidator();

    /**
     * Check user input.
     *
     * Reimplemented from @ref QValidator.
     **/
    virtual State validate( QString & input, int & pos ) const;

    /**
     * Set the valid input characters.
     * If 'newValidChars' is empty, any input is accepted.
     **/
    void setValidChars( const QString &	newValidChars )
	{ _validChars = newValidChars; }

    /**
     * Retrieve the valid input characters.
     **/
    QString validChars() const { return _validChars; }

protected:

    QString _validChars;
};

#endif // QY2CharValidator_h

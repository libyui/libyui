/*
  Copyright (C) 2023 SUSE LLC

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

#ifndef NCHttpApplication_h
#define NCHttpApplication_h

#include <yui/ncurses/NCApplication.h>

class NCHttpApplication: public NCApplication
{

  protected:

    friend class YNCHttpUI;

    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    NCHttpApplication();

    /**
     * Destructor.
     **/
    virtual ~NCHttpApplication();


  public:

    virtual int runInTerminal( const std::string & command ) override;
};


#endif // NCHttpApplication_h

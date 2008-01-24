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

  File:	      YFrameBuffer.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "framebuffer"
#include "YUILog.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "YFrameBuffer.h"


YFrameBuffer::YFrameBuffer( int width, int height, int depth )
{
    _width	= width;
    _height	= height;
    _depth	= depth;

    _fb		= 0;
    _fb_fd	= -1;
    _fb_len	= 0;

    switch ( _depth )
    {
	case 15:
	case 16: _bytesPerPixel = 2;
	    break;

	case 24:
	case 32: _bytesPerPixel = 4;
	    break;

	default:
	    yuiWarning() << "Pixel depth " << depth << " not supported" << endl;
	    _bytesPerPixel = 0;
	    break;
    }

    if ( _bytesPerPixel > 0 )
	mmapFB();
}


YFrameBuffer::~YFrameBuffer()
{
    munmapFB();
}


void YFrameBuffer::mmapFB()
{
    if ( _fb )
    {
	yuiError() << "Framebuffer already mmap()'ed" << endl;
	return;
    }

    _fb_fd = open( "/dev/fb0", O_RDWR );

    if ( _fb_fd < 0 )
    {
	yuiError() << "Can't open /dev/fb0 - errno " << errno << ": " << strerror( errno ) << endl;
	return;
    }

    _fb_len 	= _width * _height * _bytesPerPixel;
    _fb		= mmap( 0,			// start
			_fb_len,		// length
			PROT_READ | PROT_WRITE,	// prot
			MAP_SHARED,		// flags
			_fb_fd,			// file descriptor
			0 );			// offset

    if ( _fb == MAP_FAILED )
    {
	yuiError() << "mmap() failed for /dev/fb0 - errno " << errno << ": " << strerror( errno ) << endl;
	_fb = 0;
	close( _fb_fd );
    }

    _fb16 = (UINT16 *) _fb;
    _fb32 = (UINT32 *) _fb;
}


void YFrameBuffer::munmapFB()
{
    if ( ! _fb )
	return;

    munmap( _fb, _fb_len );
    close( _fb_fd );

    _fb	   	= 0;
    _fb16	= 0;
    _fb32	= 0;
    _fb_fd 	= -1;
}


void YFrameBuffer::setPixel( int x, int y, FBPixel pixel )
{
    if ( ! _fb )
	return;

    if ( x < _width && y < _height )
    {
	if ( _bytesPerPixel == 2 )
	{
	    pixel &= 0xFFFF;
	    _fb16[ y * _width + x ] = (UINT16) pixel;
	}
	else if ( _bytesPerPixel == 4 )
	{
	    _fb32[ y * _width + x ] = (UINT32) pixel;
	}
    }
}


YFrameBuffer::FBPixel
YFrameBuffer::pixel( int x, int y )
{
    if ( ! _fb )
	return 0;

    if ( x < _width && y < _height )
    {
	if ( _bytesPerPixel == 2 )
	{
	    return (FBPixel) _fb16[ y * _width + x ];
	}
	else if ( _bytesPerPixel == 4 )
	{
	    return (FBPixel) _fb32[ y * _width + x ];
	}
    }

    return 0;
}





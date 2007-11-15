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

  File:	      YQFrameBuffer.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YFrameBuffer_h
#define YFrameBuffer_h

class YFrameBuffer
{
public:

    typedef unsigned short UINT16;
    typedef unsigned long UINT32;
    typedef UINT32 FBPixel;

    /**
     * Constructor
     **/
    YFrameBuffer( int screenWidth, int screenHeight, int bitDepth );

    /**
     * Destructor
     **/
    virtual ~YFrameBuffer();

    /**
     * Set the framebuffer pixel at (x, y)
     **/
    void setPixel( int x, int y, FBPixel pixel );

    /**
     * Get the framebuffer pixel value at (x, y)
     **/
    FBPixel pixel( int x, int y );

    /**
     * Returns 'true' if the framebuffer is valid, i.e. can be used.
     **/
    bool valid()	const { return _fb != 0; }

    int width()		const { return _width;		}
    int height()	const { return _height;		}
    int depth()		const { return _depth;		}
    int bytesPerPixel() const { return _bytesPerPixel;	}


protected:

    /**
     * mmap() the framebuffer device
     **/
    void mmapFB();

    /**
     * release the mmaped framebuffer
     **/
    void munmapFB();

    
    //
    // Data members
    //

    int		_width;
    int		_height;
    int		_depth;

    void *	_fb;
    int		_fb_fd;
    UINT32 *	_fb32;
    UINT16 * 	_fb16;
    size_t	_fb_len;
    int		_bytesPerPixel;
};


#endif // ifndef YFrameBuffer_h

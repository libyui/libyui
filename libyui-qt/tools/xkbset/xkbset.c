/*
 * xkbset -	Set the X keyboard type.
 *		Requires the XFree86 XKB and XVidMode extensions.
 *
 *		This program is part of both SaX and YaST2.
 *
 * Author:	Marcus Schaefer <ms@suse.de>
 */




/*------------------------*/
/*   INCLUDES             */
/*------------------------*/
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xmu/StdSel.h>
#include <X11/Xmd.h>
#include <X11/extensions/xf86vmode.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBgeom.h>
#include <X11/extensions/XKM.h>
#include <X11/extensions/XKBfile.h>
#include <X11/extensions/XKBui.h>
#include <X11/extensions/XKBrules.h>
#include <X11/Xproto.h>
#include <X11/Xfuncs.h>


/*-----------------------------*/
/*        VARIABLES            */
/*-----------------------------*/
int MajorVersion, MinorVersion;
int EventBase, ErrorBase;
int dot_clock, mode_flags;


#define MINMAJOR 0
#define MINMINOR 5


#define V_FLAG_MASK	0x1FF;
#define V_PHSYNC	0x001 
#define V_NHSYNC	0x002
#define V_PVSYNC	0x004
#define V_NVSYNC	0x008
#define V_INTERLACE	0x010 
#define V_DBLSCAN	0x020
#define V_CSYNC		0x040
#define V_PCSYNC	0x080
#define V_NCSYNC	0x100

typedef enum 
{ 
 HDisplay, HSyncStart, HSyncEnd, HTotal,
 VDisplay, VSyncStart, VSyncEnd, VTotal, Flags, 
 InvertVclk, BlankDelay1, BlankDelay2, EarlySc,
 PixelClock, HSyncRate, VSyncRate, fields_num 
} fields;

typedef struct 
{
  fields  me;
  fields  use;
  int 	  val;
  int	  lastpercent;
  int 	  range;
  Widget  textwidget;
  Widget  scrollwidget;
} ScrollData;


/*-------------------*/
/*   FUNCTIONS       */
/*-------------------*/


int LoadKBD(Display *display, int argc, char **argv)
{
  XkbComponentNamesRec    getcomps;
  XkbDescPtr              xkb;
  Bool                    loadit = True;

  if(argc < 7 || argc > 8) {
    printf("wrong number of arguments\n");
    return -1;
  }

  /*
  printf("1: %s\n",argv[1]);
  printf("2: %s\n",argv[2]);
  printf("3: %s\n",argv[3]);
  printf("4: %s\n",argv[4]);
  printf("5: %s\n",argv[5]);
  printf("6: %s\n",argv[6]);
  printf("7: %s\n",argv[7]);
  */

  if(argc == 8) {
    if(!strcmp(argv[7], "load"))
      loadit = True;
    else if(!strcmp(argv[7], "noload"))
      loadit = False;
    else {
     printf("Wrong parameter %s\n", argv[7]);
     return -1;
    }
  }

  getcomps.keymap   = argv[1];
  getcomps.keycodes = argv[2];
  getcomps.compat   = argv[3];
  getcomps.types    = argv[4];
  getcomps.symbols  = argv[5];
  getcomps.geometry = argv[6];
  xkb = XkbGetKeyboardByName(display, XkbUseCoreKbd, &getcomps, XkbGBN_AllComponentsMask, 0, loadit);

  if(!xkb) {
    printf("Failed to set keyboard description\n");
    return -1;
  }

  if(!xkb->names->geometry) xkb->names->geometry = xkb->geom->name;

  return 0;
}

/*-------------------------*/
/*   MAIN                  */
/*-------------------------*/

int main(int argc, char **argv)
{
  Display *display;

  if(!(display = XOpenDisplay(getenv("DISPLAY")))) {
    return fprintf(stderr, "unable to open display\n"), 11;
  }

  if(!XF86VidModeQueryVersion(display, &MajorVersion, &MinorVersion)) {
    fprintf(stderr, "Unable to query video extension version\n");
    return 2;
  }

  if(!XF86VidModeQueryExtension(display, &EventBase, &ErrorBase)) {
    fprintf(stderr, "Unable to query video extension information\n");
    return 2;
  }

  if(
     MajorVersion < MINMAJOR ||
    (MajorVersion == MINMAJOR && MinorVersion < MINMINOR)
  ) {
    fprintf(stderr,
      "Xserver is running an old XFree86-VidModeExtension version (%d.%d)\n",
      MajorVersion, MinorVersion
    );
    fprintf(stderr,"Minimum required version is %d.%d\n", MINMAJOR, MINMINOR);
    return 2;
  }
  
  /* printf("Load keyboard\n"); */
  LoadKBD(display, argc, argv);

  return 0;
}

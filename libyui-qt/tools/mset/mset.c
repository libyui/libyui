#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/xf86misc.h>

int set_mouse(Display *, int, char **);
int get_mouse(Display *);

/*
 * *** List is not up to date!!! ***
 */
static char *msetable[] = {
  "None", "Microsoft", "MouseSystems", "MMSeries", "Logitech", "BusMouse",
  "Mouseman", "PS/2", "MMHitTab", "GlidePoint", "IntelliMouse",
  "ThinkingMouse", "IMPS/2", "ThinkingMousePS/2", "MouseManPlusPS/2",
  "GlidePointPS/2", "NetMousePS/2", "NetScrollPS/2", "SysMouse", "Auto",
  "Xqueue", "OSMouse"
};

/*---------------------------------------------------------------------------*/
/* Implement mouse apply                                                     */
/* Usage: set_mouse <device> <mousetype>                                     */
/*                 <baudrate> <samplerate> <resolution> <buttons>            */ 
/*                 on|off <timeout> on|off [ClearDTR] [ClearRTS] [ReOpen]    */
/*---------------------------------------------------------------------------*/
int set_mouse(Display *display, int argc, char **argv)
{
  XF86MiscMouseSettings mseinfo;
  int i;

  mseinfo.device = argv[1];

  mseinfo.type = -1;
  for(i = 1; i < sizeof msetable / sizeof *msetable; i++) {
    if(!strcasecmp(msetable[i], argv[2])) mseinfo.type = i - 1;
  }
  if(!strcasecmp("Xqueue", argv[2])) mseinfo.type = MTYPE_XQUEUE;
  if(!strcasecmp("OSMouse", argv[2])) mseinfo.type = MTYPE_OSMOUSE;
  if(mseinfo.type == -1) {
    return fprintf(stderr, "invalid mouse type\n"), 3;
  }

  mseinfo.baudrate   = atoi(argv[3]);
  mseinfo.samplerate = atoi(argv[4]);
  mseinfo.resolution = atoi(argv[5]);
  mseinfo.buttons    = atoi(argv[6]);

  if(!strcasecmp(argv[7], "on"))
    mseinfo.emulate3buttons = 1;
  else if(!strcasecmp(argv[7], "off"))
    mseinfo.emulate3buttons = 0;
  else 
    return fprintf(stderr, "button option must be either on or off\n"), 4;

  mseinfo.emulate3timeout = atoi(argv[8]);

  if(!strcasecmp(argv[9], "on"))
    mseinfo.chordmiddle = 1;
  else if(!strcasecmp(argv[9], "off"))
    mseinfo.chordmiddle = 0;
  else 
    return fprintf(stderr, "chordmiddle option must be either on or off\n"), 5;

  mseinfo.flags = 0;

  for(i = 10; i < argc; i++) {
    if(!strcasecmp(argv[i], "cleardtr"))
      mseinfo.flags |= MF_CLEAR_DTR;
    else if(!strcasecmp(argv[i], "clearrts"))
      mseinfo.flags |= MF_CLEAR_RTS;
    else if(!strcasecmp(argv[i], "reopen"))
      mseinfo.flags |= MF_REOPEN;
    else
      return fprintf(stderr, "flag must be one of ClearDTR,ClearRTS, or ReOpen\n"), 6;
  }

  XSync(display, False);
  XF86MiscSetMouseSettings(display, &mseinfo);
  XSync(display, False);

  XFlush(display);

  return 0;
}


int get_mouse(Display *display)
{
  XF86MiscMouseSettings mseinfo;
  char tmpbuf[256], *name;

  if(!XF86MiscGetMouseSettings(display, &mseinfo)) {
    return fprintf(stderr, "unable to get mouse settings\n"), 20;
  }

  if(mseinfo.type == MTYPE_XQUEUE) name = "Xqueue";
  else if(mseinfo.type == MTYPE_OSMOUSE) name = "OSMouse";
  else if(mseinfo.type < 0 || (mseinfo.type >= sizeof msetable / sizeof *msetable))
    name = "Unknown";
  else
    name = msetable[mseinfo.type+1];

  sprintf(tmpbuf, "%s %s %d %d %d %d %s %d %s",
    mseinfo.device == NULL? "{}": mseinfo.device,
    name, mseinfo.baudrate, mseinfo.samplerate,
    mseinfo.resolution, mseinfo.buttons,
    mseinfo.emulate3buttons? "on": "off",
    mseinfo.emulate3timeout,
    mseinfo.chordmiddle? "on": "off"
  );

  if(mseinfo.flags & MF_CLEAR_DTR)
    strcat(tmpbuf, " ClearDTR");
  if(mseinfo.flags & MF_CLEAR_RTS)
    strcat(tmpbuf, " ClearRTS");

  if(mseinfo.device) XFree(mseinfo.device);

  printf("%s\n", tmpbuf);

  return 0;
}


int main (int argc, char **argv)
{
  Display *display;

  if(argc != 1 && (argc < 9 || argc > 12)) {
    fprintf(stderr, "usage: mset <device> <mousetype> <baudrate> <samplerate> <resolution> <buttons> on|off <timeout> on|off [ClearDTR] [ClearRTS] [ReOpen]\n");
    return 10;
  }

  if(!(display = XOpenDisplay(getenv("DISPLAY")))) {
    return fprintf(stderr, "unable to open display\n"), 11;
  }

  return argc == 1 ? get_mouse(display) : set_mouse(display, argc, argv);
}

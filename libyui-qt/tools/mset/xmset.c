/**************
FILE          : xmset.c
***************
PROJECT       : SaX ( SuSE advanced X configuration )
              :
BELONGS TO    : Configuration tool XFree86 version 4.0
              :  
              :
BESCHREIBUNG  : xmset change the mouse properties on the
              : fly of a running X-Server
              : 
              :
STATUS        : development
**************/

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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
#include <X11/extensions/xf86misc.h>
#include <X11/Xos.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/kdev_t.h>
#include <sys/types.h>
#include <fcntl.h>

#define TRUE  1
#define FALSE 0
#define LINK  "/dev/pointer0"

/*****************************************
  Type definitions used in this functions
*****************************************/
typedef char str[256];

/***********************
  Variables
***********************/
int EventBase, ErrorBase;
int MajorVersion, MinorVersion;

struct mouse {
 char link[256];
 char device[256];
 char type[256];
 int  baudrate;
 int  samplerate;
 int  resolution;
 int  buttons;
 char emu3button[256];
 int  emu3timeout;
 char chordmiddle[256];
 char flag[256];
};

struct devnode {
 char  name[256];    /* name of the device file */
 int   major;        /* major number            */
 int   minor;        /* minor number            */
 dev_t device;       /* device struct for mknod */
 int   mode;         /* char(0) or block(1) dev */
 uid_t user;         /* user id                 */
 gid_t group;        /* group id                */
 mode_t protect;     /* protection mode         */
};


static char *msetable[] = { 
 "None", 
 "Microsoft", 
 "MouseSystems", 
 "MMSeries",
 "Logitech", 
 "BusMouse", 
 "Mouseman", 
 "PS/2",
 "MMHitTab", 
 "GlidePoint", 
 "IntelliMouse",
 "ThinkingMouse", 
 "IMPS/2", 
 "ThinkingMousePS/2",
 "MouseManPlusPS/2", 
 "GlidePointPS/2", 
 "NetMousePS/2", 
 "NetScrollPS/2", 
 "SysMouse",
 "Auto", 
 "Xqueue", 
 "OSMouse" 
};

static int (*savErrorFunc)();
static int errorOccurred;

#define MSETABLESIZE    (sizeof(msetable)/sizeof(char *))

int strsplit(str string,char deli,str left,str right);

int devstat(struct devnode *dev) {
 char name[256];
 int error;
 struct stat device;

 strcpy(name,dev->name);
 error = stat(name,&device);

 dev->major   = MAJOR(device.st_rdev);
 dev->minor   = MINOR(device.st_rdev);
 dev->device  = MKDEV(dev->major,dev->minor);
 dev->user    = device.st_uid;
 dev->group   = device.st_gid;
 dev->protect = device.st_mode;

 if (S_ISBLK(device.st_mode)) {
  dev->mode = 1;
 }
 if (S_ISCHR(device.st_mode)) {
  dev->mode = 0;
 }
 return(error);
}


int mkdev(struct devnode *dev) {
 mode_t mode;
 int error;

 /* S_IFBLK|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP */
 if (dev->mode == 1) {
  mode = S_IFBLK;
 }
 if (dev->mode == 0) {
  mode = S_IFCHR;
 }

 error = mknod(dev->name,mode,dev->device);
 chmod(dev->name,dev->protect);
 chown(dev->name,dev->user,dev->group);
 return(error);
}


/*************************************************
 special string comparison Note, the characters 
 '_', ' ', and '\t' are ignored 
*************************************************/
int StrCaseCmp(char* s1, char* s2) {
 char c1, c2;

 if (*s1 == 0) {
  if (*s2 == 0) {
   return(0);
  }
  else {
   return(1);
  }
 }
 while (*s1 == '_' || *s1 == ' ' || *s1 == '\t') {
  s1++;
 }
 while (*s2 == '_' || *s2 == ' ' || *s2 == '\t') {
  s2++;
 }
 c1 = (isupper(*s1) ? tolower(*s1) : *s1);
 c2 = (isupper(*s2) ? tolower(*s2) : *s2);
 while (c1 == c2) {
  if (c1 == '\0') {
   return(0);
  }
  s1++; s2++;
  while (*s1 == '_' || *s1 == ' ' || *s1 == '\t') {
   s1++;
  }
  while (*s2 == '_' || *s2 == ' ' || *s2 == '\t') {
   s2++;
  }
  c1 = (isupper(*s1) ? tolower(*s1) : *s1);
  c2 = (isupper(*s2) ? tolower(*s2) : *s2);
 }
 return(c1 - c2);
}


/***************************
 Implement mouse apply                                         
 Usage: 
 SetMouse ( 
  display       -> display
  device        -> device 
  mousetype     -> Protocol
  baudrate      -> Baudrate
  samplerate    -> Samplerate
  resolution    -> Resolution 
  buttons       -> Buttons
  on|off        -> Emulate3Buttons
  timeout       -> Emulate3Timeout
  on|off        -> ChordMiddle
   
  you may set one of this flags
  -----------------------------
  [ClearDTR]    -> Flag
  [ClearRTS]    -> Flag
  [ReOpen]      -> apply immediatelly 
 )
**************************/
int SetMouse(Display *dpy, struct mouse* argv) {
 XF86MiscMouseSettings mseinfo;
 int i;

 mseinfo.device = argv->device;
 mseinfo.type   = -1;
 for (i = 1; i < sizeof(msetable)/sizeof(char *); i++) {
  if (!StrCaseCmp(msetable[i], argv->type)) {
   mseinfo.type = i - 1;
  }
 }

 /* some special cases... */
 if (!StrCaseCmp("Xqueue", argv->type))
  mseinfo.type = MTYPE_XQUEUE;
 else if (!StrCaseCmp("OSMouse", argv->type))
  mseinfo.type = MTYPE_OSMOUSE;
 if (mseinfo.type == -1) {
  return(-1);
 }

 mseinfo.baudrate   = argv->baudrate;
 mseinfo.samplerate = argv->samplerate;
 mseinfo.resolution = argv->resolution;
 if (argv->buttons != 0) {
  mseinfo.buttons    = argv->buttons;
 }
 if (!StrCaseCmp(argv->emu3button, "on")) {
  mseinfo.emulate3buttons = 1;
 } else {
  mseinfo.emulate3buttons = 0;
 }
 mseinfo.emulate3timeout = argv->emu3timeout;

 if (!StrCaseCmp(argv->chordmiddle, "on")) {
  mseinfo.chordmiddle = 1;
 } else {
  mseinfo.chordmiddle = 0;
 }

 mseinfo.flags = 0;
 if (!StrCaseCmp(argv->flag,      "cleardtr"))
  mseinfo.flags |= MF_CLEAR_DTR;
 else if (!StrCaseCmp(argv->flag, "clearrts"))
  mseinfo.flags |= MF_CLEAR_RTS;
 else if (!StrCaseCmp(argv->flag, "reopen")) 
  mseinfo.flags |= MF_REOPEN;
 else {
  mseinfo.flags |= MF_REOPEN; 
 }

 XSync(dpy, False);
 errorOccurred = 0;
 XF86MiscSetMouseSettings(dpy, &mseinfo);
 XSync(dpy, False);
 XSetErrorHandler(savErrorFunc);
 if (errorOccurred) {
  return(-1);
 }
 return(0);
}


int GetMouse(Display *dpy) {
 XF86MiscMouseSettings mseinfo;
 char tmpbuf[200] = {0};

 if (!XF86MiscGetMouseSettings(dpy, &mseinfo)) {
  return(-1);
 } else {
  char *name;
  if (mseinfo.type == MTYPE_XQUEUE)
   name = "Xqueue";
  else if (mseinfo.type == MTYPE_OSMOUSE)
   name = "OSMouse";
  else if (mseinfo.type < 0 || (mseinfo.type >= MSETABLESIZE))
   name = "Unknown";
  else
   name = msetable[mseinfo.type+1];
  snprintf (
   tmpbuf, sizeof(tmpbuf), "%s %s %d %d %d %d %s %d %s",
   mseinfo.device==NULL? "{}": mseinfo.device, 
   name,
   mseinfo.baudrate, 
   mseinfo.samplerate,
   mseinfo.resolution, 
   mseinfo.buttons,
   mseinfo.emulate3buttons? "on": "off",
   mseinfo.emulate3timeout,
   mseinfo.chordmiddle? "on": "off"
  );
  if (mseinfo.flags & MF_CLEAR_DTR)
   strncat(tmpbuf, " ClearDTR", sizeof(tmpbuf)-strlen(tmpbuf)-1);
  if (mseinfo.flags & MF_CLEAR_RTS)
   strncat(tmpbuf, " ClearRTS", sizeof(tmpbuf)-strlen(tmpbuf)-1);

  if (mseinfo.device) {
   XFree(mseinfo.device);
  }
  printf("%s\n",tmpbuf);
  return(0);
 }
}

Display* XOpen(char name[256]) {
 Display *dpy;
 char display[256] = "";

 if (strcmp(name,"null") == 0) {
  strcpy(display,getenv("DISPLAY"));
 } else {
  strcpy(display,name);
 }
 if(!(dpy = XOpenDisplay(display))) {
  /*fprintf(stderr, "unable to open display: %s\n",getenv("DISPLAY"));*/
  return(NULL);
 }
 return(dpy);
}


void usage(void) {
 exit(0);
}

/****************
  main
****************/
int main (int argc, char*argv[]) {
 Display* dpy;
 int c;
 int DEBUG = FALSE;
 int error;
 int show   = FALSE;
 int apply  = FALSE;
 
 char link  [256] = LINK;
 char mydisp[256] = "";
 char option[256] = "";
 char left  [256] = "";
 char dummy [256] = "";
 struct mouse   *pointer;
 struct devnode *dev;

 pointer = (struct mouse*)   malloc(sizeof(struct mouse));
 dev     = (struct devnode*) malloc(sizeof(struct devnode));

 /*****************************
   get commandline options...
 *****************************/
 while (1) {
  int option_index = 0;
  static struct option long_options[] =
  {
   {"show"     , 0 , 0 , 's'},
   {"apply"    , 1 , 0 , 'a'},
   {"display"  , 1 , 0 , 'd'},
   {"help"     , 0 , 0 , 'h'},
   {"link"     , 1 , 0 , 'l'},
   {0          , 0 , 0 , 0  }
  };

  c = getopt_long (argc, argv, "hsa:d:l:",long_options, &option_index);
  if (c == -1)
   break;

  switch (c) {
    case 0:
     if (DEBUG) {
      fprintf (stderr,"option %s", long_options[option_index].name);
      if (optarg)
       fprintf (stderr," with arg %s", optarg);
      fprintf (stderr,"\n");
     }
     break;

    case 'h':
     usage();

    case 's':
     show  = TRUE;
     break;

    case 'a':
     apply = TRUE;
     strcpy(option,optarg);
     break;

    case 'l':
     strcpy(link,optarg);
     break;

    case 'd':
     strcpy(mydisp,optarg);
     break;

    default:
     exit(1);
  }
 }


 /****************
   test link
 ****************/
 strcpy(dev->name,link);
 error = devstat(dev);
 if (error < 0) {
  strsplit(option,':',left,dummy);  
  //printf("xmset: warning could not stat file: %s\n",link);
  printf("xmset: create link %s -> %s\n",link,left);
  symlink(left,link);
  //exit(1);
 } 

 /*******************
   open display...
 *******************/
 if (strcmp(mydisp,"") == 0) {
  strcpy(mydisp,"null");
 }
 dpy = XOpen(mydisp);
 if(dpy == NULL) {
  fprintf(stderr,"xmset: could not initialize display: %s\n",mydisp);
  exit(1);
 }


 if (apply == TRUE) {
  strcpy(pointer->link,link);
  strsplit(option,':',left,option);  
  strcpy(pointer->device,left);
  strsplit(option,':',left,option);
  strcpy(pointer->type,left);
  strsplit(option,':',left,option);
  pointer->baudrate = atoi(left);
  strsplit(option,':',left,option);
  pointer->samplerate = atoi(left);
  strsplit(option,':',left,option);
  pointer->resolution = atoi(left);
  strsplit(option,':',left,option);
  pointer->buttons = atoi(left);
  strsplit(option,':',left,option);
  if (strcmp(left,"on") == 0) {
   strcpy(pointer->emu3button,"on");
  } else {
   strcpy(pointer->emu3button,"off");
  }
  strsplit(option,':',left,option);
  pointer->emu3timeout = atoi(left);
  strsplit(option,':',left,option);
  if (strcmp(left,"on") == 0) {
   strcpy(pointer->chordmiddle,"on");
  } else {
   strcpy(pointer->chordmiddle,"off");
  }
  strsplit(option,':',left,option);
  strcpy(pointer->flag,left);

  /*
  printf(
   "%s %s %s %d %d %d %d %s %d %s %s\n", 
   pointer->link,
   pointer->device,
   pointer->type,
   pointer->baudrate,
   pointer->samplerate,
   pointer->resolution,
   pointer->buttons,
   pointer->emu3button,
   pointer->emu3timeout,
   pointer->chordmiddle,
   pointer->flag
  );
  */

  /* relink the /dev/sax link to the new device... */
  remove(link);
  symlink(pointer->device,link);

  /* set mouse parameters... */
  SetMouse(dpy,pointer);
 }


 if (show == TRUE) {
  GetMouse(dpy);
 }
 return(0);
}

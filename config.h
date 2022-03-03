/* See LICENSE file for copyright and license details. */

#include "mpdcontrol.c"

#define TERMINAL "st"
#define BROWSER "google-chrome-stable"
#define FILE_EXPLORER "nautilus"
#define SCREENSHOT "flameshot gui"
#define FULLSS "flameshot full -c -p ~/screenshots"
#define NETWORK_MANAGER "networkmanager_dmenu"
#define VOLUME_UP "amixer set Master 3+ && pkill -RTMIN+2 goblocks"
#define VOLUME_DOWN "amixer set Master 3- && pkill -RTMIN+2 goblocks"
#define VOLUME_TOGGLE "amixer set Master toggle && pkill -RTMIN+2 goblocks"
#define ROFI_RUN "~/.rofi/run.sh"
#define NOTIF_HISTORY "dunstctl history-pop"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Fira Mono for Powerline:size=9:antialias=true", "JoyPixels:pixelsize=9:antialias=true:autohint=true"};
static const char dmenufont[]       = "Fira Mono for Powerline:size=9:antialias=true";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#2a4158";
static const char col_selborder[]   = "#fe9635";
static const char col_white[]       = "#ffffff";
static const char col_gray5[]       = "#a89984";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_gray2, col_gray5 },
};

/* tagging */
#define MAX_TAGNAME_LEN 14		/* excludes TAG_PREPEND */
#define MAX_TAGLEN 16
#define TAG_PREPEND "%1i:"		/* formatted as 2 chars */
#define MONS_TAGGED 2
#define TAGMON(mon) (mon->num < MONS_TAGGED ? mon->num : MONS_TAGGED-1) 
static char tags[][MONS_TAGGED][MAX_TAGLEN] = {
       /* monitor 0, monitor 1, ... */
       { "1",        "1" },
       { "2",        "2" },
       { "3",        "3" },
       { "4",        "4" },
       { "5",        "5" },
       { "6",        "6" },
       { "7",        "7" },
       { "8",        "8" },
       { "9",        "9" },
};
 

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "pritunl",  NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "|||",      col },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", "--command", "tmux" };
static const char *roficmd[]  = { "rofi", "-show", "combi" };
static const char *notifhistorycmd[] = { "dunstctl", "history-pop" };

static Key keys[] = {
	/* modifier                     key        function        argument */
        /* { MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } }, */
        /* START_KEYS */
	{ MODKEY,                       XK_p,      spawn,          {.v = roficmd} },
	{ MODKEY,                       XK_r,      spawn,          SHCMD(ROFI_RUN) },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd} },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD(SCREENSHOT) },
	{ MODKEY|ShiftMask,             XK_f,      spawn,          SHCMD(FULLSS) },
	{ MODKEY,                       XK_w,      spawn,          SHCMD(BROWSER) },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD(FILE_EXPLORER) },
	{ MODKEY|ShiftMask,             XK_n,      spawn,          SHCMD(NETWORK_MANAGER) },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          SHCMD("slock") },
	{ MODKEY|ControlMask,           XK_grave,  spawn,          SHCMD(NOTIF_HISTORY) },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|Mod1Mask,              XK_h,      incrgaps,       {.i = +5 } },
	{ MODKEY|Mod1Mask,              XK_l,      incrgaps,       {.i = -5 } },
	{ MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod1Mask,              XK_equal,  defaultgaps,    {0} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },
        { MODKEY,                       XK_c,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_q,      setlayout,      {.v = &layouts[6]} },
	{ MODKEY,                       XK_a,      setlayout,      {.v = &layouts[7]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_F10,     spawn,         SHCMD(VOLUME_TOGGLE) },
	{ MODKEY,                       XK_F11,     spawn,         SHCMD(VOLUME_DOWN) },
	{ MODKEY,                       XK_F12,     spawn,         SHCMD(VOLUME_UP) },
	{ MODKEY,                       XK_Escape, mpdcontrol,     { 0 } },
        { MODKEY,                       XK_n,      nametag,        {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
        /* END_KEYS */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },};


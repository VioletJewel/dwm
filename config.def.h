/* See LICENSE file for copyright and license details. */

/* appearance */
#include <time.h>
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 8;        /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "FiraCode Nerd Font Mono:pixelsize=14" };
static const char dmenufont[]       = "FiraCode Nerd Font Mono:pixelsize=14";

struct theme {
  const char *name;
	const char *fg;
	const char *bg;
	const char *active;
	const char *active2;
	const char *inactive;
};

enum themenames {
  tokyonight,
  kanagawa,
};

static const struct theme themes[] = {
	{ "tokyonight-moon" , "#c8d3f5", "#222436", "#c099ff", "#82aaff", "#636da6", },
	{ "kanagawa-wave"   , "#dcd7ba", "#1f1f28", "#957fb8", "#7e9cd8", "#727169", },
	{ "catppuccin-mocha", "#cdd6f4", "#1e1e2e", "#957fb8", "#89b4fa", "#9399b2", },
	{ "gruvbox8"        , "#ebdbb2", "#282828", "#d3869b", "#83a598", "#928374", },
};

static struct theme *theme = (struct theme*)themes;

/* tagging */
static const char *tags[] = { ">_", "www", ">_", ">_", ">_", ">_", ">_", ">_", "chat" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	// /* class                 instance    title       tags mask     isfloating   monitor */
	// { "Gimp",                NULL,       NULL,       0,            1,           -1 },
	{ "mercury-default",        NULL,       NULL,       1 << 1,       0,           -1 },
	{ "discord",                NULL,       NULL,       1 << 8,       0,           -1 },
	{ "irc",                    NULL,       NULL,       1 << 7,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[D]",      deck },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,      {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,       {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define TERM "st"

/* commands */
static char dmenumon[2] = "0"; /* component of getdmenucmd return, manipulated in spawn() */
static const char *termcmd[]  = { TERM, NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      dmenu,          {0} },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_s,      setscheme,      {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },

	{ MODKEY|Mod1Mask,              XK_u,      spawn,          SHCMD("/usr/bin/xbacklight -set 1"  ) },
	{ MODKEY|Mod1Mask,              XK_i,      spawn,          SHCMD("/usr/bin/xbacklight -dec 5"  ) },
	{ MODKEY|Mod1Mask,              XK_o,      spawn,          SHCMD("/usr/bin/xbacklight -inc 5"  ) },
	{ MODKEY|Mod1Mask,              XK_p,      spawn,          SHCMD("/usr/bin/xbacklight -set 100") },

	{ MODKEY|Mod1Mask,              XK_h,      spawn,          SHCMD("/usr/bin/pactl set-sink-mute @DEFAULT_SINK@ toggle;") },
	{ MODKEY|Mod1Mask,              XK_j,      spawn,          SHCMD("/usr/bin/pactl set-sink-volume @DEFAULT_SINK@ -5%;") },
	{ MODKEY|Mod1Mask,              XK_k,      spawn,          SHCMD("/usr/bin/pactl set-sink-volume @DEFAULT_SINK@ +5%;") },
	{ MODKEY|Mod1Mask,              XK_l,      spawn,          SHCMD("/usr/bin/notify-send -h 'string:synchronous:volume' \"$(/usr/bin/pactl get-sink-volume '@DEFAULT_SINK@' | head -n1 | awk '{ printf \"%s | %s\", $5, $12 }';[ \"$(/usr/bin/pactl get-sink-mute '@DEFAULT_SINK@' | cut -d' ' -f2)\" = 'yes' ] && echo -n ' (muted)')\"") },

	{ MODKEY,                       XK_s,      spawn,          SHCMD("ekraneto plena") },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("ekraneto elektu") },
	{ MODKEY|ControlMask,           XK_s,      spawn,          SHCMD("ekraneto fenestro") },

	{ MODKEY,                       XK_w,      spawn,          {.v = (const char*[]){ "/usr/bin/firefox", NULL } } },
	{ MODKEY|ShiftMask,             XK_i,      spawn,          {.v = (const char*[]){ TERM, "-c", "irc", "-e", "weechat", NULL } } },

	{ MODKEY,                       XK_BackSpace, spawn,       SHCMD("xautolock -enable; xautolock -locknow;") },
	{ MODKEY|ShiftMask,             XK_BackSpace, spawn,       SHCMD("[ -f /tmp/.lock.disabled ] && { rm -f /tmp/.lock.disabled; xautolock -enable; notify-send 'slock enabled'; } || { touch /tmp/.lock.disabled; xautolock -disable; notify-send 'slock disabled'; };") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};


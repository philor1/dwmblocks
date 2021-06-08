// Modify the given location of files as per your system
const char *cpu_hwmon_name = "hwmon4";				// Found in /sys/class/hwmon/hw.....
const char *network_wifi_name = "wlp3s0b1";			// Found in /sys/class/net/wl....
const char *network_lan_name = "enp0s25";			// Found in /sys/class/net/en....
const char *backlight_driver_name = "intel_backlight";		// Found in /sys/class/backlight/.......
const char *power_adapter_name = "AC";				// Found in /sys/class/power_supply/ADP.....
const char *power_battery_name = "BAT0";			// Found in /sys/class/power_supply/BAT.....

//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Isfunc*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	{ " Vol",		1,		"volume",					0,			1 },
	{ " Bkl",		1,		"xbacklight",				0,			2 },
	{ " Std",		1,		"ltime",					1,			3 },
	{ " Day",		1,		"datet",					3600,		4 },
	{ " Cpu",		1,		"cpustat",					1,			5 },
	{ " Ram",		1,		"memstat",					1,			6 },
	{ " Bat",		1,		"power",					1,			9 },
	{ " Wifi",		1,		"wifistat",					1,			7 },
	{ " Lan",		1,		"netlan",					1,			8 },
	{ " Upd",		1,		"updatestat",				0,	    	10 },
//	{ " Usb",		1,		"busb",						0,			11 },
//	{ " Fon",		1,		"android",					0,			12 },
//	{ " Tm",		1,		"transmission",				60,			13 },
	{ " >>> ",		0,      "block-config", 			0,          14 },
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = " ";
static unsigned int delimLen = 3;

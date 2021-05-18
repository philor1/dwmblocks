static char adapter[64];
static char battery[64];
static char cpu_temp[64];
static char brightness[64];
static char lan_opstate[64];
static char wifi_opstate[64];
static char wifi_rxbytes[64];
static char max_brightness[64];
static long double rxBytes;
static long double rxBytes1;
static long double rxBytesdiff;

const char *getstsmods(int clk, const char *fak, char *value) {

int i = strlen(value);
time_t t;
FILE * file;




//__________________________________________________________________________________
//			MEMORY
//----------------------------------------------------------

if (strcmp(fak, "memstat") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: system("PID=$(ps aux | grep 'st -c dbar -n psmem' | grep -v grep | awk '{print $2}') && kill $PID || st -c dbar -n psmem -e psmem &"); break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
	}
	file = fopen("/proc/meminfo","r");
	if (file != NULL)
	{
		int memusage;
		long double memTotal, memFree;
		char *membar[1];
		fscanf(file, "MemTotal: %Lf kB\nMemFree: %Lf", &memTotal, &memFree);
		fclose(file);
		memusage = ((memTotal - memFree) / memTotal) * 100;
		if (memusage >= 0 && memusage <= 15)
			membar[1] = "▁";
		else if (memusage > 15 && memusage <= 30)
			membar[1] = "▂";
		else if (memusage > 30 && memusage <= 45)
			membar[1] = "▃";
		else if (memusage > 45 && memusage <= 60)
			membar[1] = "▄";
		else if (memusage > 60 && memusage <= 75)
			membar[1] = "▅";
		else if (memusage > 75 && memusage <= 90)
			membar[1] = "▆";
		else if (memusage > 75 && memusage <= 90)
			membar[1] = "▇";
		else
			membar[1] = "█";
		if ( memusage <= 33)
			sprintf(value+i, "[%d%^c%s^%s^d^]",memusage, color2, membar[1] );
		else if ( memusage > 33 && memusage <= 66)
			sprintf(value+i, "[%d%^c%s^%s^d^]",memusage, color3, membar[1] );
		else
			sprintf(value+i, "[%d%^c%s^%s^d^]",memusage, color1, membar[1] );
	}
}


//__________________________________________________________________________________
//			CPU
//----------------------------------------------------------

if (strcmp(fak, "cpustat") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: system("PID=$(ps aux | grep 'st -c dbar -n htop' | grep -v grep | awk '{print $2}') && kill $PID || st -c dbar -n htop -e htop &"); break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
	}
	file = fopen("/proc/stat","r");
	if (file != NULL) {
		#define CPU_NBR 4
		int cputemp;
		size_t len = 0;
		char *line = NULL;
		int j;
		long int idle_time, other_time;
		char cpu_name[8];
		static int new_cpu_usage[CPU_NBR][4];
		static int old_cpu_usage[CPU_NBR][4];
		int cpu_percent[CPU_NBR];
		char *cpu_bar[CPU_NBR];
		for(j = 0; j < CPU_NBR; ++j) {
			getline(&line,&len,file);
			sscanf(line, "%s %d %d %d %d", cpu_name,
					&new_cpu_usage[j][0], &new_cpu_usage[j][1], &new_cpu_usage[j][2], &new_cpu_usage[j][3]);
			if(line != NULL) {
				free(line);
				line = NULL;
			}
			idle_time = new_cpu_usage[j][3] - old_cpu_usage[j][3];
			other_time = new_cpu_usage[j][0] - old_cpu_usage[j][0] + new_cpu_usage[j][1] - old_cpu_usage[j][1]
						+ new_cpu_usage[j][2] - old_cpu_usage[j][2];
			if(idle_time + other_time != 0) {
				cpu_percent[j] = (other_time*100)/(idle_time + other_time);

				if (cpu_percent[j] == 0)
					cpu_bar[j] = "▁";
				else if (cpu_percent[j] > 0 && cpu_percent[j] <= 15)
					cpu_bar[j] = "▂";
				else if (cpu_percent[j] > 15 && cpu_percent[j] <= 30)
					cpu_bar[j] = "▃";
				else if (cpu_percent[j] > 30 && cpu_percent[j] <= 45)
					cpu_bar[j] = "▄";
				else if (cpu_percent[j] > 45 && cpu_percent[j] <= 60)
					cpu_bar[j] = "▅";
				else if (cpu_percent[j] > 60 && cpu_percent[j] <= 80)
					cpu_bar[j] = "▆";
				else if (cpu_percent[j] > 80 && cpu_percent[j] <= 100)
					cpu_bar[j] = "▇";
				else
					cpu_bar[j] = "█";
			} else {
				cpu_percent[j] = 0;
				cpu_bar[j] = "▁";
			}
			old_cpu_usage[j][0] = new_cpu_usage[j][0];
			old_cpu_usage[j][1] = new_cpu_usage[j][1];
			old_cpu_usage[j][2] = new_cpu_usage[j][2];
			old_cpu_usage[j][3] = new_cpu_usage[j][3];
		}
		fclose(file);
		sprintf(cpu_temp, "/sys/class/hwmon/%s/temp2_input", cpu_hwmon_name);
		file = fopen(cpu_temp,"r");
		if (file != NULL) {
			long double cpuTemp;
			fscanf(file, "%Lf", &cpuTemp);
			fclose(file);
			cputemp = cpuTemp / 1000;
		} else
			cputemp = 0;
	if (cputemp >= 70)
		sprintf(value+i, "[ %s %s %s %s ]  Tmp[^c%s^%d˚C^d^]", cpu_bar[0], cpu_bar[1], cpu_bar[2], cpu_bar[3], color1, cputemp);
	else
		sprintf(value+i, "[ %s %s %s %s ]  Tmp[%d˚C]", cpu_bar[0], cpu_bar[1], cpu_bar[2], cpu_bar[3], cputemp);
	}
}


//__________________________________________________________________________________
//			DATE
//----------------------------------------------------------

if (strcmp(fak, "datet") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: system("PID=$(ps aux | grep 'st -c dbar -n calen' | grep -v grep | awk '{print $2}') && kill $PID || st -c dbar -n calen -e calen &"); break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
	}
	time(&t);
	struct tm *date = localtime(&t);
	sprintf(value+i, "[%02d-%02d]", date->tm_mday, date->tm_mon+1);
}


//__________________________________________________________________________________
//			TIME
//----------------------------------------------------------

if (strcmp(fak, "ltime") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: system("PID=$(ps aux | grep 'st -c dbar -n calcurse' | grep -v grep | awk '{print $2}') && kill $PID || st -c dbar -n calcurse -e calcurse &"); break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
	}
	time(&t);
	struct tm *time = localtime(&t);
	sprintf(value+i, "[%02d:%02d]", time->tm_hour, time->tm_min);
}


//__________________________________________________________________________________
//			WIFI
//----------------------------------------------------------

if (strcmp(fak, "wifistat") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: system("networkmanager_dmenu"); break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
	}
	file = fopen(wifi_opstate,"r");
	if (file != NULL) {
		char wifiState[4];
		fscanf(file, "%s", wifiState);
		fclose(file);
		if (*(wifiState+0) != 'd') {
			file = fopen("/tmp/internetav","r");
			if (file != NULL)
			{
				int internetav;
				fscanf(file, "%d", &internetav);
				fclose(file);
				if (internetav != 0) {
					int wifispeed = 0;
					file = fopen(wifi_rxbytes,"r");
					if (file != NULL) {
						fscanf(file, "%Lf", &rxBytes);
						fclose(file);
					}
					rxBytesdiff = rxBytes - rxBytes1;
					rxBytes1 = rxBytes;
					wifispeed = rxBytesdiff / 1024;
					if (wifispeed < 10)
					sprintf(value+i, "[▾  %dkB/s]", wifispeed);
					else if (wifispeed >= 10 && wifispeed < 100)
					sprintf(value+i, "[▾ %dkB/s]", wifispeed);
					else if (wifispeed >= 100 && wifispeed < 1000)
					sprintf(value+i, "[▾%dkB/s]", wifispeed);
					else
					sprintf(value+i, "[%dkB/s]", wifispeed);
				} else {
					sprintf(value+i, "%s", "[----kB/s]");
				}
			} else {
				sprintf(value+i, "%s", "[----kB/s]");
			}
		} else
			sprintf(value+i, "%s", "[---off--]");
	}
}


//__________________________________________________________________________________
//			NETWORK
//----------------------------------------------------------

if (strcmp(fak, "netlan") == 0)
{
	file = fopen(lan_opstate, "r");
	if (file != NULL) {
		char lanopstate[4];
		fscanf(file, "%s", lanopstate);
		fclose(file);
		if (*(lanopstate+0) != 'd') {
			file = fopen("/tmp/internetav","r");
			if (file != NULL) {
				int internetav;
				fscanf(file, "%d", &internetav);
				fclose(file);
				if (internetav != 0) {
					sprintf(value+i, "%s", "[╩]");
				} else {
					sprintf(value+i, "%s", "[╧]");
				}
			} else {
				sprintf(value+i, "%s", "[╧]");
			}
		} else {
			sprintf(value+i, "%s", "[═]");
		}
	}
}


//__________________________________________________________________________________
//			UPDATE
//----------------------------------------------------------

if (strcmp(fak, "updatestat") == 0)
{
	file = fopen("/home/philipp/.cache/updates","r");
	if (file != NULL) {
		int u = 0;
		fscanf(file, "%d", &u);
		fclose(file);
		if (u != 0)
			sprintf(value+i, "[%d]", u);
		else
			value[0] = '\0';
	} else
		value[0] = '\0';
}


//__________________________________________________________________________________
//			ANDROID
//----------------------------------------------------------

if (strcmp(fak, "android") == 0)
{
	file = popen("mtp-detect 2>/dev/null | awk '/Model/' | wc -l", "r");
	if (file != NULL) {
		int devs = 0;
		fscanf(file, "%d", &devs);
		pclose(file);
		if (devs != 0)
			sprintf(value+i, "[%d]", devs);
		else
			value[0] = '\0';
	}
}


//__________________________________________________________________________________
//			USB
//----------------------------------------------------------

if (strcmp(fak, "busb") == 0)
{
	file = popen("lsblk -lo RM,TYPE | grep '1 disk' | wc -l", "r");
	if (file != NULL) {
		int devs = 0;
		char c1[128], c2[128], vendor[32], mp[128]="\0";
		fscanf(file, "%d", &devs);
		pclose(file);
		if (devs != 0) {
			for (int l = 1; l <= devs; l++) {
				sprintf(c1, "lsblk -lo RM,TYPE,NAME,VENDOR | grep '1 disk' | sed '%dq;d'", l);
				file = popen(c1, "r");
				if (file != NULL) {
					fscanf(file, " 1 disk %s %s", c1, vendor);
					pclose(file);
				}
				sprintf(c2, "lsblk -lo RM,TYPE,MOUNTPOINT /dev/%s | grep '1 part' | cut -d '/' -f4", c1);
				file = popen(c2, "r");
				if (file != NULL) {
					fscanf(file, " 1 part %s", mp);
					pclose(file);
				}
				if (mp[0] != '\0')
					sprintf(value+i, "[%s#]", vendor);
				else
					sprintf(value+i, "[%s]", vendor);
			}
		}
		else {
			value[0] = '\0';
		}
	}
}


//__________________________________________________________________________________
//			TRANSMISSION
//----------------------------------------------------------

if (strcmp(fak, "transmission") == 0)
{
	file = popen("transmission-remote -l | wc -l", "r");
	if (file != NULL) {
		int torrents = 0;
		fscanf(file, "%d", &torrents);
		pclose(file);
		if (torrents > 2) {
			int count = 2;
			char trans[64];
			char pri[8*torrents];
			char percentdone[8];
			while (count < torrents)
			{
				sprintf(trans, "transmission-remote -l | sed '%dq;d' | awk '{ print $2 }'", count);
				file = popen(trans, "r");
				fscanf(file, "%s", percentdone);
				fclose(file);
				sprintf(pri+strlen(pri), "%s:", percentdone);
				count++;
			}
			pri[strlen(pri)-1] = '\0';
			sprintf(value+i, "{%s}", pri);
		} else
			value[0] = '\0';
	}
}


//__________________________________________________________________________________
//			BACKLIGHT
//----------------------------------------------------------

if (strcmp(fak, "xbacklight") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: system("backlight_control +7"); break;
		case 5: system("backlight_control -7"); break;
	}
	file = fopen(max_brightness, "r");
	if (file != NULL) {
		int light;
		int max_light;
		fscanf(file, "%d", &max_light);
		fclose(file);
		file = fopen(brightness, "r");
		fscanf(file, "%d", &light);
		fclose(file);
		light = (light * 100) / max_light;
		if (light == 0)
			sprintf(value+i, "%s",   "[----------]");
		else if (light > 0 && light <= 1)
			sprintf(value+i, "%s",   "[■---------]");
		else if (light > 1 && light <= 2)
			sprintf(value+i, "%s",   "[■■--------]");
		else if (light > 2 && light <= 4)
			sprintf(value+i, "%s",   "[■■■-------]");
		else if (light > 4 && light <= 5)
			sprintf(value+i, "%s",   "[■■■■------]");
		else if (light > 5 && light <= 7)
			sprintf(value+i, "%s",   "[■■■■■-----]");
		else if (light > 7 && light <= 11)
			sprintf(value+i, "%s",   "[■■■■■■----]");
		else if (light > 11 && light <= 17)
			sprintf(value+i, "%s",   "[■■■■■■■---]");
		else if (light > 17 && light <= 30)
			sprintf(value+i, "%s",   "[■■■■■■■■--]");
		else if (light > 30 && light <= 60)
			sprintf(value+i, "%s",   "[■■■■■■■■■-]");
		else
			sprintf(value+i, "^c%s^%s^d^", color1,  "[■■■■■■■■■■]");
	}
}


//__________________________________________________________________________________
//			VOLUME
//----------------------------------------------------------

if (strcmp(fak, "volume") == 0)
{
	switch (clk) {
		case 0: break;
		case 1: system("audiosel -o &"); break;
		case 2: system("audiosel -s &"); break;
		case 3: system("/usr/bin/amixer set Master toggle"); break;
		case 4: system("/usr/bin/amixer -q sset Master 5%+"); break;
		case 5: system("/usr/bin/amixer -q sset Master 5%-"); break;
	}
	file = popen("amixer sget Master | tail -n 1 | cut -d ' ' -f8 | cut -b 3", "r");
	if (file != NULL) {
		char mute[1];
		fscanf(file, "%s", mute);
		fclose(file);
		if (mute[0] != 'f'){

			int vol;
			file = popen("amixer sget Master | tail -n 1 | cut -d ' ' -f6 | tr -d -c 0-9", "r");
			fscanf(file, "%d", &vol);
			fclose(file);

			int server;
			file = popen("pidof pipewire || echo 0", "r");
			fscanf(file, "%d", &server);
			fclose(file);

			if (vol == 0)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[----------]");
			else if (vol > 0 && vol <= 7)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■---------]");
			else if (vol > 7 && vol <= 18)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■--------]");
			else if (vol > 18 && vol <= 30)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■-------]");
			else if (vol > 30 && vol <= 41)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■■------]");
			else if (vol > 41 && vol <= 53)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■■■-----]");
			else if (vol > 53 && vol <= 64)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■■■■----]");
			else if (vol > 64 && vol <= 76)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■■■■■---]");
			else if (vol > 76 && vol <= 87)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■■■■■■--]");
			else if (vol > 87 && vol <= 99)
				sprintf(value+i, "%s%s", server == 0 ? "(alsa)" : "(pipe)", "[■■■■■■■■■-]");
			else if (vol > 99 && vol <= 100)
				sprintf(value+i, "%s^c%s^%s^d^", server == 0 ? "(alsa)" : "(pipe)", color1, "[■■■■■■■■■■]");
			else
				sprintf(value+i, "[%d]", vol);
		} else
			sprintf(value+i, "^c%s^(mute)^d^%s", color1, "[----------]");
	}
}


//__________________________________________________________________________________
//			POWER
//----------------------------------------------------------

if (strcmp(fak, "power") == 0)
{
	int status;
	int online;
	char *batbar[1];
	file = fopen(battery, "r");
	if (file != NULL)
		fscanf(file, "%d", &status);
	else
		status = 0;
	fclose(file);
//	if (status == 0)
//		sprintf(value+i, "[%d] %s %s", &status, "◉", "[▨▨▨▨▨▨◽]");
//		break;
	file = fopen(adapter, "r");
	if (file != NULL)
		fscanf(file, "%d", &online);
	else
		online = 0;
	fclose(file);
		if (status >= 0 && status <= 10)
			batbar[1] = "▁";
		else if (status > 10 && status <= 25)
			batbar[1] = "▂";
		else if (status > 25 && status <= 40)
			batbar[1] = "▃";
		else if (status > 40 && status <= 55)
			batbar[1] = "▄";
		else if (status > 55 && status <= 70)
			batbar[1] = "▅";
		else if (status > 70 && status <= 85)
			batbar[1] = "▆";
		else if (status > 85 && status <= 95)
			batbar[1] = "▇";
		else
			batbar[1] = "█";

	if (online != 0)
		sprintf(value+i, "^b%s^^c%s^[%d%%%s]^d^", color3, color0, status, batbar[1]);
	else
	{
		if (status < 15)
			sprintf(value+i, "^b%s^^c%s^[%d%%%s]^d^", color2, color0, status, batbar[1]);
		else
		{
			if (status >= 15 && status < 33)
				sprintf(value+i, "[%d%^c%s^%s^d^]", status, color2, batbar[1]);
			if (status >= 33 && status < 66)
				sprintf(value+i, "[%d%^c%s^%s^d^]", status, color4, batbar[1]);
			if (status >= 66)
				sprintf(value+i, "[%d%^c%s^%s^d^]", status, color3, batbar[1]);
		}
	}
}



i = strlen(value);
if (i != 0) //return if block and command output are both empty
	if (delim[0] != '\0') {
		//only chop off newline if one is present at the end
		i = value[i-1] == '\n' ? i-1 : i;
		strncpy(value+i, delim, delimLen);
	}
}



/* ----------------------------------------------------------------------------------
 [▫▫▫▫▫▫▫▫▫▫]	[----------]
 [▪▫▫▫▫▫▫▫▫▫]	[■---------]
 [▪▪▫▫▫▫▫▫▫▫]	[■■--------]
 [▪▪▪▫▫▫▫▫▫▫]	[■■■-------]
 [▪▪▪▪▫▫▫▫▫▫]	[■■■■------]
 [▪▪▪▪▪▫▫▫▫▫]	[■■■■■-----]
 [▪▪▪▪▪▪▫▫▫▫]	[■■■■■■----]
 [▪▪▪▪▪▪▪▫▫▫]	[■■■■■■■---]
 [▪▪▪▪▪▪▪▪▫▫]	[■■■■■■■■--]
 [▪▪▪▪▪▪▪▪▪▫]	[■■■■■■■■■-]
 [▪▪▪▪▪▪▪▪▪▪]	[■■■■■■■■■■]

 [ ▾ ■ □ ▢ ▣ ▨ ▫ ▪ ▯ ◻ ◼ ◽◾ ⬤  ◉ ● ○ ◍ ◌ ◎ ]	[▁▂▃▄▅▆▇█]
---------------------------------------------------------------------------------- */


/* 	POWER --------------------------------------------------------------------------
		if (status >= 0 && status <= 10)
			batbar[1] = "[□□□□□□◽]";
		else if (status > 10 && status <= 25)
			batbar[1] = "[▣□□□□□◽]";
		else if (status > 25 && status <= 40)
			batbar[1] = "[▣▣□□□□◽]";
		else if (status > 40 && status <= 55)
			batbar[1] = "[▣▣▣□□□◽]";
		else if (status > 55 && status <= 70)
			batbar[1] = "[▣▣▣▣□□◽]";
		else if (status > 70 && status <= 85)
			batbar[1] = "[▣▣▣▣▣□◽]";
		else if (status > 85 && status <= 95)
			batbar[1] = "[▣▣▣▣▣▣◽]";
		else
			batbar[1] = "[▣▣▣▣▣▣◾]";
	if (online != 0)
		sprintf(value+i, "[%d] %s %s", status, "◉", batbar[1]);
	else
		sprintf(value+i, "[%d] %s %s", status, "◎", batbar[1]);
----------------------------------------------------------------------------------- */


/*		VOLUME - pulseaudio... ------------------------------------------------------
			if (vol == 0)
				sprintf(value+i, "%s", "(-.-) [----------]");
				sprintf(value+i, "(alsa)^c%s^%s^d^", color1, "[■■■■■■■■■■]");
			else if (vol > 200 && vol <= 8000)
				sprintf(value+i, "%s", "(puls)[■---------]");
			else if (vol > 8000 && vol <= 15000)
				sprintf(value+i, "%s", "(puls)[■■--------]");
			else if (vol > 15000 && vol <= 21000)
				sprintf(value+i, "%s", "(puls)[■■■-------]");
			else if (vol > 21000 && vol <= 28000)
				sprintf(value+i, "%s", "(puls)[■■■■------]");
			else if (vol > 28000 && vol <= 34000)
				sprintf(value+i, "%s", "(puls)[■■■■■-----]");
			else if (vol > 34000 && vol <= 41000)
				sprintf(value+i, "%s", "(puls)[■■■■■■----]");
			else if (vol > 41000 && vol <= 47500)
				sprintf(value+i, "%s", "(puls)[■■■■■■■---]");
			else if (vol > 47500 && vol <= 54000)
				sprintf(value+i, "%s", "(puls)[■■■■■■■■--]");
			else if (vol > 54000 && vol <= 60500)
				sprintf(value+i, "%s", "(puls)[■■■■■■■■■-]");
			else if (vol > 60500 && vol <= 66000)
				sprintf(value+i, "(puls)^c%s^%s^d^", color1, "[■■■■■■■■■■]");
*/ /* 	VOLUME - monocolor-theme...
				sprintf(value+i, "%s", "[     ]");
			else if (vol > 0 && vol <= 10)
				sprintf(value+i, "%s", "[░    ]");
			else if (vol > 10 && vol <= 25)
				sprintf(value+i, "%s", "[░░   ]");
			else if (vol > 25 && vol <= 40)
				sprintf(value+i, "%s", "[░░░  ]");
			else if (vol > 40 && vol <= 55)
				sprintf(value+i, "%s", "[░░░░ ]");
			else if (vol > 55 && vol <= 65)
				sprintf(value+i, "%s", "[░░░░░]");
			else if (vol > 65 && vol <= 75)
				sprintf(value+i, "%s", "[█░░░░]");
			else if (vol > 75 && vol <= 80)
				sprintf(value+i, "%s", "[██░░░]");
			else if (vol > 80 && vol <= 88)
				sprintf(value+i, "%s", "[███░░]");
			else if (vol > 88 && vol <= 92)
				sprintf(value+i, "%s", "[████░]");
			else if (vol > 92 && vol <=100)
				sprintf(value+i, "%s", "[█████]");
-------------------------------------------------------------------------------- */


/* 	BACKLIGHT --------------------------------------------------------------------
		if (light >= 0 && light <= 10)
			sprintf(value+i, "%s", "{░    }");
		else if (light > 10 && light <= 20)
			sprintf(value+i, "%s", "{░░   }");
		else if (light > 20 && light <= 30)
			sprintf(value+i, "%s", "{░░░  }");
		else if (light > 30 && light <= 40)
			sprintf(value+i, "%s", "{░░░░ }");
		else if (light > 40 && light <= 50)
			sprintf(value+i, "%s", "{░░░░░}");
		else if (light > 50 && light <= 60)
			sprintf(value+i, "%s", "{█░░░░}");
		else if (light > 60 && light <= 70)
			sprintf(value+i, "%s", "{██░░░}");
		else if (light > 70 && light <= 80)
			sprintf(value+i, "%s", "{███░░}");
		else if (light > 80 && light <= 90)
			sprintf(value+i, "%s", "{████░}");
		else
			sprintf(value+i, "%s", "{█████}");
---------------------------------------------------------------------------------- */


/* 		CPU -----------------------------------------------------------------------
				if (cpu_percent[j] == 0)
					cpu_bar[j] = "▫▫▫▫▫";
				else if (cpu_percent[j] > 0 && cpu_percent[j] <= 25)
					cpu_bar[j] = "▪▫▫▫▫";
				else if (cpu_percent[j] > 25 && cpu_percent[j] <= 50)
					cpu_bar[j] = "▪▪▫▫▫";
				else if (cpu_percent[j] > 50 && cpu_percent[j] <= 75)
					cpu_bar[j] = "▪▪▪▫▫";
				else if (cpu_percent[j] > 75 && cpu_percent[j] <= 100)
					cpu_bar[j] = "▪▪▪▪▫";
				else
					cpu_bar[j] = "▪▪▪▪▪";
				} else {
					cpu_percent[j] = 0;
					cpu_bar[j] = "▫▫▫▫▫";
				}
------------------------------------------------------------------------------------ */


/* 	MEMORY ---------------------------------------------------------------------------
		if (memusage >= 0 && memusage <= 20)
			membar[1] = "◽◽◽◽◽";
		else if (memusage > 20 && memusage <= 40)
			membar[1] = "◾◽◽◽◽";
		else if (memusage > 40 && memusage <= 60)
			membar[1] = "◾◾◽◽◽";
		else if (memusage > 60 && memusage <= 80)
			membar[1] = "◾◾◾◽◽";
		else if (memusage > 80 && memusage <= 90)
			membar[1] = "◾◾◾◾◽";
		else
			membar[1] = "◾◾◾◾◾";
		sprintf(value+i, "[%s]", membar[1]);
------------------------------------------------------------------------------------ */

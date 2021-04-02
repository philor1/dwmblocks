#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<stdint.h>
#include<X11/Xlib.h>
#ifndef NO_X
#include<X11/Xlib.h>
#endif
#ifdef __OpenBSD__
#define SIGPLUS			SIGUSR1+1
#define SIGMINUS		SIGUSR1-1
#else
#define SIGPLUS			SIGRTMIN
#define SIGMINUS		SIGRTMIN
#endif
#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH 124
#define MIN( a, b ) ( ( a < b) ? a : b )
#define STATUSLENGTH (LENGTH(blocks) * CMDLENGTH + 1)

typedef struct {
	char* icon;
	unsigned int isFunc;
	char* command;
	unsigned int interval;
	unsigned int signal;
} Block;
#ifndef __OpenBSD__
void dummysighandler(int num);
#endif
void sighandler(int num);
void buttonhandler(int sig, siginfo_t *si, void *ucontext);
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getstatus(char *str, char *last);
void statusloop();
void termhandler();
void pstdout();
#ifndef NO_X
void setroot();
static void (*writestatus) () = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif

#include "colors.c"
#include "config.h"
#include "comp.c"

static char statusbar[LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[2][STATUSLENGTH];
static char button[] = "\0";
static int statusContinue = 1;
static int returnStatus = 0;

int gcd(int a, int b)
{
	int temp;
	while (b > 0){
		temp = a % b;

		a = b;
		b = temp;
	}
	return a;
}

//opens process *cmd and stores output in *output
void getcmd(const Block *block, char *output)
{
	if (block->signal) {
		output[0] = block->signal;
		output++;
	}
	int isfunc = block->isFunc;
	if (isfunc != 1) {	// If the block has IsFunc set to 0
		strcpy(output, block->icon);
		FILE *cmdf;
		if (*button) {
			setenv("BUTTON", button, 1);
			cmdf = popen(block->command,"r");
			*button = '\0';
			unsetenv("BUTTON");
		} else {
			cmdf = popen(block->command,"r");
		}
        if (!cmdf)
                return;
        int i = strlen(block->icon);
        fgets(output+i, CMDLENGTH-i-delimLen, cmdf);
        i = strlen(output);
		if (i == 0) {
			//return if block and command output are both empty
			pclose(cmdf);
			return;
		}
		if (delim[0] != '\0') {
			//only chop off newline if one is present at the end
			i = output[i-1] == '\n' ? i-1 : i;
			strncpy(output+i, delim, delimLen);
		} else
			output[i++] = '\0';
        pclose(cmdf);
	} else {
		strcpy(output, block->icon);
		if (*button) {
			int c = *button - '0';
			getstsmods(c, block->command, output);
			*button = '\0';
		} else {
			getstsmods(0, block->command, output);
		}

	}
}

void getcmds(int time)
{
	const Block* current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
			getcmd(current,statusbar[i]);
	}
}

void getsigcmds(unsigned int signal)
{
	const Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if (current->signal == signal)
			getcmd(current,statusbar[i]);
	}
}

void setupsignals()
{
	struct sigaction sa;
#ifndef __OpenBSD__
	    /* initialize all real time signals with dummy handler */
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
        signal(i, dummysighandler);
#endif

	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].signal > 0) {
			signal(SIGMINUS+blocks[i].signal, sighandler);
			sigaddset(&sa.sa_mask, SIGRTMIN+blocks[i].signal); // ignore signal when handling SIGUSR1
		}
	}
	sa.sa_sigaction = buttonhandler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);
}

int getstatus(char *str, char *last)
{
	strcpy(last, str);
	str[0] = '\0';
	for (unsigned int i = 0; i < LENGTH(blocks); i++)
		strcat(str, statusbar[i]);
	str[strlen(str)-strlen(delim)] = '\0';
	return strcmp(str, last);	//0 if they are the same
}

#ifndef NO_X
void setroot()
{
	if (!getstatus(statusstr[0], statusstr[1]))	//Only set root if text has changed.
		return;
	XStoreName(dpy, root, statusstr[0]);
	XFlush(dpy);
}

int setupX()
{
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmblocks: Failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}
#endif

void pstdout()
{
	if (!getstatus(statusstr[0], statusstr[1]))	//Only write out if text has changed.
		return;
	printf("%s\n",statusstr[0]);
	fflush(stdout);
}


void statusloop()
{
	setupsignals();
	unsigned int interval = -1;
	for(int i = 0; i < LENGTH(blocks); i++){
		if(blocks[i].interval){
			interval = gcd(blocks[i].interval, interval);
		}
	}
	unsigned int i = 0;
	int interrupted = 0;
	struct timespec sleeptime = {interval, 0};
	struct timespec tosleep = sleeptime;
	getcmds(-1);
	while (1) {
		interrupted = nanosleep(&tosleep, &tosleep);
		if(interrupted == -1){
			continue;
		}
		getcmds(i++);
		writestatus();
		if (!statusContinue)
			break;
		tosleep = sleeptime;
	}
}

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void dummysighandler(int signum)
{
    return;
}
#endif

void sighandler(int signum)
{
	getsigcmds(signum-SIGPLUS);
	writestatus();
}

void buttonhandler(int sig, siginfo_t *si, void *ucontext)
{
	*button = '0' + si->si_value.sival_int & 0xff;
	getsigcmds(si->si_value.sival_int >> 8);
	writestatus();
}

void termhandler()
{
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	sprintf(wifi_opstate, "/sys/class/net/%s/operstate", network_wifi_name);
	sprintf(wifi_rxbytes, "/sys/class/net/%s/statistics/rx_bytes", network_wifi_name);
	sprintf(lan_opstate, "/sys/class/net/%s/operstate", network_lan_name);
	sprintf(max_brightness, "/sys/class/backlight/%s/max_brightness", backlight_driver_name);
	sprintf(brightness, "/sys/class/backlight/%s/brightness", backlight_driver_name);
	sprintf(battery, "/sys/class/power_supply/%s/capacity", power_battery_name);
	sprintf(adapter, "/sys/class/power_supply/%s/online", power_adapter_name);
	for (int i = 0; i < argc; i++) { //Handle command line arguments
		if (!strcmp("-d",argv[i]))
			strncpy(delim, argv[++i], delimLen);
		else if (!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
#ifndef NO_X
	if (!setupX())
		return 1;
#endif
	delimLen = MIN(delimLen, strlen(delim));
	delim[delimLen++] = '\0';
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
#ifndef NO_X
	XCloseDisplay(dpy);
#endif
	return 0;
}

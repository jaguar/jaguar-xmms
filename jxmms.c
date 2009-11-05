#include "xchat-plugin.h"
#include "xmmsctrl.h"
#include <glib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h> 
#include <stdlib.h>
#include <ctype.h>

int get_song();
int get_version();

int session = 0;

char* convert_rate(int rate, char* ratestr);
char* convert_time(int session, int pos, char* timestr); 
char* convert_milliseconds(int time, char* timestr); 
int display_song(int dsp_fn);

static xchat_plugin *ph;   /* plugin handle */

#define PNAME "jaguar-xmms"
#define PDESC "XMMS Now Playing Script"
#define PVERSION "0.1"

#define IS_XMMS_RUNNING() \
	if (!xmms_remote_is_running(session)) { \
		xchat_print(ph, "Xmms is not running, exiting.."); \
		return XCHAT_EAT_ALL; \
	}  

int color_b1 = 0;
int color_b2 = 0;
int color_info = 4;
int color_title = 4;
int color_sep = 14;
int color_text = 0;
int color_bar1 = 0;
int color_bar2 = 4;
int color_bar3 = 0;


int play_pause(char *word[], char *word_eol[], void *userdata) {
	IS_XMMS_RUNNING()
	xmms_remote_play_pause(session);
	return XCHAT_EAT_ALL;
}

int xmms_next(char *word[], char *word_eol[], void *userdata) {
	IS_XMMS_RUNNING()
	xmms_remote_playlist_next(session);
	return XCHAT_EAT_ALL;
}

int xmms_prev(char *word[], char *word_eol[], void *userdata) {
	IS_XMMS_RUNNING()
	xmms_remote_playlist_prev(session);
	return XCHAT_EAT_ALL;
}

int get_song_fn(char *word[], char *word_eol[], void *userdata) {
	display_song(1);
	return XCHAT_EAT_ALL;
}
int get_song(char *word[], char *word_eol[], void *userdata) {
	display_song(0);
	return XCHAT_EAT_ALL;
}

int display_song(int dsp_fn) {
	IS_XMMS_RUNNING()

	int rate, nch, freq, vol;
	struct stat;
	int session=0;
	char ratestr[200];
	char song_path[300];
	char song_name[300];
	char * song_filename;
	char * format;
	char * pch;
	char formatstr[25];
	char song_time[100];
	char freqhz[100];
	int song_position = xmms_remote_get_playlist_pos(session);
	int playlist_length = xmms_remote_get_playlist_length(session);
	/* grab the bitrate, freq and number of channels */
	xmms_remote_get_info(session, &rate, &freq, &nch);
	
	vol = xmms_remote_get_main_volume(session);

	convert_rate(rate, ratestr);
	convert_time(session, song_position, song_time);

	sprintf(song_path, "%s", xmms_remote_get_playlist_file(session, song_position));
	sprintf(song_name, "%s",xmms_remote_get_playlist_title(session, xmms_remote_get_playlist_pos(session))); 	
	sprintf(freqhz, "%02d.%.01d", freq / 1000, freq % 1000);

	char msg[1024];
	/* Search in reverse for / */	
	song_filename = strrchr(song_path,'/') +1;
	format = strrchr(song_filename, '.')+1;
	sprintf(formatstr, " [%s]", format);

	/* Clear the extension from the song_filename */
	pch = strstr(song_filename, format) -1;
	strncpy(pch, "", strlen(format));
	
	if (dsp_fn) {
		if (song_filename) {
			sprintf(msg, "ME playing: %s (%s) (%s kHz) %s (%i/%i)", song_filename, ratestr,freqhz,song_time,song_position+1,playlist_length);
		}
	}
	else {
		sprintf(msg, "ME playing: %s (%s) (%s kHz) %s (%i/%i)", song_name, ratestr,freqhz,song_time,song_position+1,playlist_length);
	}
	strcat(msg, formatstr);
	xchat_command(ph,msg);		

	return XCHAT_EAT_ALL;
}

int xchat_plugin_init(xchat_plugin *plugin_handle,
                      char **plugin_name,
                      char **plugin_desc,
                      char **plugin_version,
                      char *arg)
{
	/* we need to save this for use with any xchat_* functions */
	ph = plugin_handle;
	/* tell xchat our info */
	*plugin_name = PNAME;
	*plugin_desc = PDESC;
	*plugin_version = PVERSION;

	xchat_hook_command(ph, "xmms", XCHAT_PRI_NORM, get_song,"Usage: xmms - Displays song information as a /ME ", 0);
	xchat_hook_command(ph, "xmmsfn", XCHAT_PRI_NORM, get_song_fn,"Usage: xmms - Displays song information as a /ME ", 0);
	xchat_hook_command(ph, "xmms-playpause", XCHAT_PRI_NORM, play_pause, "Usage: xmms-playpause - Plays or pauses Xmms", 0);
	xchat_hook_command(ph, "xmms-next", XCHAT_PRI_NORM, xmms_next, "Usage: xmms-next - Moves to the next song", 0);
	xchat_hook_command(ph, "xmms-prev", XCHAT_PRI_NORM, xmms_prev, "Usage: xmms-prev - Moves to the previous song", 0);
	
	char msg[200];
	sprintf(msg, "\003%d[\003%d\002Jaguar-XMMS Now Playing Script v.%s\017\003%d]\003%d by \003%d\002 Jaguar\017 \003%dloaded successfully!", color_b2, color_title, PVERSION, color_b2, color_text, color_info, color_text);
	xchat_print(ph, msg);

	return 1;
}


void xchat_plugin_get_info(char **name, char **desc, char **version, void **reserved)
{
   *name = PNAME;
   *desc = PDESC;
   *version = PVERSION;
}

int get_version(session) {
	int version;
	version = xmms_remote_get_version(session);
	return version;
}
char* convert_rate(int rate, char* ratestr) {
	rate = rate / 1000;
	sprintf(ratestr, "%i Kbps", rate);
	return ratestr;
}

char* convert_time(int session, int pos, char* timestr) {
	
	int cur_pos = xmms_remote_get_output_time(session);
	int playlist_time = xmms_remote_get_playlist_time(session,pos);
	char time_curf[100];
	char time_playf[100];
	sprintf(timestr, "(%s/%s)", convert_milliseconds(cur_pos, time_curf), convert_milliseconds(playlist_time, time_playf));
	return timestr;
}

char* convert_milliseconds(int time, char* timestr) {

	int hours = time / (1000*60*60);
	int minutes = (time % (1000*60*60)) / (1000*60);
	int seconds = ((time % (1000*60*60)) % (1000*60)) / 1000;

	if (hours >= 1) {
		sprintf(timestr, "%02i:%02i:%02i", hours, minutes, seconds);
	}
	else {
		sprintf(timestr, "%02i:%02i", minutes, seconds);	
	}

	return timestr;
}

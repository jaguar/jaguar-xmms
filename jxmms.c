#include <glib.h>
#include "xmmsctrl.h"
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h> /* struct */

int get_song();
int get_version();

char* convert_rate(int rate, char* ratestr);
char* convert_time(int session, int pos, char* timestr); 
char* convert_milliseconds(int time, char* timestr); 

int main (void) {

	int session=0;

	if (!xmms_remote_is_running(session)) { 
		printf("Xmms is not running, exiting..\n");
		return 0;
	}
	
	get_song();
	
	return 1;
}

int get_version(session) {
	int version;
	version = xmms_remote_get_version(session);
	return version;
}

void play_pause(session) {
	xmms_remote_play_pause(session);
	return;
}

void xmms_next(session) {
	xmms_remote_playlist_next(session);
	return;
}

void xmms_prev(session) {
	xmms_remote_playlist_prev(session);
	return;
}

int get_song(void) {

	int rate, nch, freq, vol;
	struct stat;
	int session=0;
	char ratestr[200];
	char song_filename[300];
	char song_name[300];
	char song_time[100];
	char freqhz[100];
	int song_position = xmms_remote_get_playlist_pos(session);
	int playlist_length = xmms_remote_get_playlist_length(session);

	/* stat(xmms_remote_get_playlist_file(session, song_position), &filestat); */
	/* grab the bitrate, freq and number of channels */
	xmms_remote_get_info(session, &rate, &freq, &nch);
	
	/* Get the volume. if it's 100%, change it to 110% for l33tness :) */
	vol = xmms_remote_get_main_volume(session);
	/* if (vol == 100) { vol = 110; } */
	convert_rate(rate, ratestr);
	convert_time(session, song_position, song_time);
	sprintf(song_filename, "%s", xmms_remote_get_playlist_file(session, song_position));
	sprintf(song_name, "%s",xmms_remote_get_playlist_title(session, xmms_remote_get_playlist_pos(session))); 	
	sprintf(freqhz, "%02d.%.01d", freq / 1000, freq % 1000);

/*
	printf("song: %s Volume: %i rate: %s freq: %s kHz songtime: %s file loc: %s", song_name, vol, ratestr, freqhz, song_time, song_filename);
	printf(" playlist: [%i/%i]\n", song_position+1, playlist_length);
*/
	printf("playing: %s (%s) (%s kHz) %s (%i/%i)\n", song_name, ratestr,freqhz,song_time,song_position+1,playlist_length);
		
	/* ugly as fuck, i know :) */
	/* xchat_commandf(ph,now_playing_string, xmms_remote_get_playlist_title(session, xmms_remote_get_playlist_pos(session))); */

	return 0;
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

	

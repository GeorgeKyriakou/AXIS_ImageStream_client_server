#include <glib.h>
#define _GNU_SOURCEinclude <gio/gio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>
#include <capture.h>
#include <axsdk/axhttp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define file "/tmp/jpg.jpg"

char frames[2];
char resolution[7];
char* camera;
char* concat(char* s1, char* s2) {
	char *combine = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(combine, s1);
	strcat(combine, s2);
	return combine;
}
int main(void) {

	int process;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int newsockfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	unsigned char buffer[256];
	//char* array = "resolution=640x480&fps=25";

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5005);

	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	for (;;) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		bzero(buffer, 256);
		syslog(LOG_INFO, "Connection established");
		process = fork();
		if (process < 0) {
			syslog(LOG_INFO, "Error on fork");
			exit(1);
		}
		if (process == 0) {
			close(sockfd);
	
			read(newsockfd, &resolution[0], 7);     //read data from client
			read(newsockfd, &frames[0], 2);     //(resolution, fps)
			char* res = &resolution[0];
			char* fps = &frames[0];
			char* con1 = concat("resolution=", res);
			char* con2 = concat(con1, "&fps=");
			camera = concat(con2, fps);

			syslog(LOG_INFO, "Resolution received");
			syslog(LOG_INFO, "FPS received");
			syslog(LOG_INFO, res);
			syslog(LOG_INFO, fps);

			media_stream *stream;
			stream = capture_open_stream(IMAGE_JPEG, camera);
			syslog(LOG_INFO, "stream Open");
			while (1) {
				media_frame *frame;
				frame = capture_get_frame(stream);
				syslog(LOG_INFO, "capture frame");
				void *data;
				data = capture_frame_data(frame);
				syslog(LOG_INFO, "capture data");
				size_t frame_size = capture_frame_size(frame);
				syslog(LOG_INFO, "capture frame_size %d", frame_size);
				int size = htonl((int) frame_size);
				syslog(LOG_INFO, "%d", frame_size);
				//unsigned char image[frame_size];
				syslog(LOG_INFO, "trying to send frame size");
				write(newsockfd, &size, sizeof(int));  //send size to client
				syslog(LOG_INFO, "sent frame size");
				write(newsockfd, data, frame_size);
				syslog(LOG_INFO, "received frame size");
				capture_frame_free(frame);
			}
			capture_close_stream(stream);
			exit(0);
		} else {
			close(newsockfd);
		}
	}
	close(sockfd);
	return (0);
}


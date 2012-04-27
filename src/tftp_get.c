#define _WIN32_WINNT 0x0501
#include <allegro5/allegro.h>

#include "tftp_get.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WINPC
#undef UNICODE
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define my_shutdown(a, b) closesocket(a)
#define SHUT_RDWR 0xf008a7
#define socklen_t int
#else
#include <sys/socket.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#define my_shutdown shutdown
#endif

#include <sys/stat.h>

#define SERVER "nooskewl.com"
#define PORT "69"
#define BLKSIZE 32768
#define BLKSIZE_S "32768"

#define NUM_FILES 42 // FIXME: change this as needed
#define EXPECTED_LIST_SIZE (NUM_FILES * 81) // 60 chars filename 20 chars length + 1 \n

static char DOWNLOAD_PATH[1000];

static volatile bool stop = false;
static bool is_downloading = false;

static int sock = -1;
static struct sockaddr saddr;
static socklen_t saddr_len;

static int get(char *buf, int len)
{
	int n = recvfrom(
		sock,
		buf,
		len,
		0,
		&saddr,
		&saddr_len
	);

	if (n < 1) {
		return 0;
	}

	return n;
}

static int put(const char *buf, int len)
{
	int n = sendto(
		sock,
		buf,
		len,
		0,
		&saddr,
		saddr_len
	);
	return n;
}

static bool connect_to_server(void)
{
	struct addrinfo hints, *res;

	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	int r;

	if ((r = getaddrinfo(SERVER, PORT, &hints, &res)) != 0) {
		return false;
	}
	
	// make a socket:
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == INVALID_SOCKET) {
		freeaddrinfo(res);
		return false;
	}

#if 0
#ifdef WINPC
	DWORD nonBlocking = 1;
	if (ioctlsocket(sock, FIONBIO, &nonBlocking) != 0) {
		printf("failed to set non-blocking socket\n");
		return false;
	}
#else
	int nonBlocking = 1;
	if (fcntl(sock, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
		printf("failed to set non-blocking socket\n");
		return false;
	}
#endif
#endif

	saddr_len = sizeof(struct sockaddr);
	memcpy(&saddr, res->ai_addr, saddr_len);

	freeaddrinfo(res);

	return true;
}

static void shutdown_connection(void)
{
	shutdown(sock, SHUT_RDWR);
}

// return size
static int get_rrq(char *buf, const char *filename)
{
	buf[0] = 0;
	buf[1] = 1;
	buf += 2;
	strcpy(buf, filename);
	buf[strlen(filename)] = 0;
	buf += strlen(filename)+1;
	strcpy(buf, "octet");
	buf[strlen("octet")] = 0;
	buf += strlen("octet")+1;
	strcpy(buf, "blksize");
	buf[strlen("blksize")] = 0;
	buf += strlen("blksize")+1;
	char *bsize = BLKSIZE_S;
	strcpy(buf, bsize);
	buf[strlen(bsize)] = 0;

	return strlen(filename) + strlen("octet") + strlen("blksize") + strlen(bsize) + 6;
}

static void mkack(char *buf)
{
	buf[0] = 0;
	buf[1] = 4;
}

static int get16bits(const char *buf)
{
	unsigned char *b1 = (unsigned char *)buf;
	unsigned char *b2 = (unsigned char *)buf+1;
	return ((int)(*b1) << 8) | (int)(*b2);
}

static int download_file(const char *filename)
{
	char fn[1000];
	char buf[4+BLKSIZE];
	int sz, total_size = 0;
	int last_blocknum = -1;
	
	if (!connect_to_server()) {
		return -1;
	}

	sz = get_rrq(buf, filename);
	if (put(buf, sz) != sz) {
		shutdown_connection();
		return -1;
	}

	// read option acknowledgement
	sz = get(buf, 2+strlen("blksize")+1+strlen(BLKSIZE_S)+1);
	if ((sz != (2+strlen("blksize")+1+strlen(BLKSIZE_S)+1)) || get16bits(buf) != 6) {
		shutdown_connection();
		return -1;
	}

	buf[0] = 0;
	buf[1] = 4;
	buf[2] = 0;
	buf[3] = 0;
	if (put(buf, 4) != 4) {
		shutdown_connection();
		return -1;
	}

	sprintf(fn, "%s/%s", DOWNLOAD_PATH, filename);
	FILE *f = fopen(fn, "wb");

	while (1) {
		if (stop) {
			fclose(f);
			shutdown_connection();
			return -1;
		}
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset);
		struct timeval tv;
		tv.tv_sec = 60;
		tv.tv_usec = 0;
		if (select(sock+1, &fdset, 0, 0, &tv) == 0) {
			fclose(f);
			shutdown_connection();
			return -1;
		}
		sz = get(buf, 4+BLKSIZE);
		if (sz != 4+BLKSIZE) {
			break;
		}
		int blocknum = get16bits(buf+2);
		if (last_blocknum == -1 || blocknum != last_blocknum) {
			fwrite(buf+4, BLKSIZE, 1, f);
			total_size += BLKSIZE;
		}
		last_blocknum = blocknum;
		mkack(buf);
		if (put(buf, 4) != 4) {
			fclose(f);
			shutdown_connection();
			return -1;
		}
	}

	if (sz > 4) {
		fwrite(buf+4, sz-4, 1, f);
		total_size += sz - 4;
		mkack(buf);
		if (put(buf, 4) != 4) {
			fclose(f);
			shutdown_connection();
			return -1;
		}
	}

	fclose(f);
	shutdown_connection();

	return total_size;
}

static void download_list(char **filenames, int *lengths)
{
	int i = 0;

	while (filenames[i]) {
		int len = download_file(filenames[i]);
		if (stop) {
			stop = false;
			return;
		}
		if (len != lengths[i])
			continue;
		i++;
	}
}

static bool download_all(void)
{
	char fn[1000];
	sprintf(fn, "%s/%s", DOWNLOAD_PATH, "list.txt");
	FILE *f = fopen(fn, "r");
	size_t read;
	char buf[100];
	int sz;
	char **filenames = NULL;
	int *lengths = NULL;
	int count = 0;
	while ((read = fread(buf, 1, 80, f)) == 80) {
		fgetc(f); // skip, fseek malfunctioning on windows oO
		buf[80] = 0;
		if (sscanf(buf, "%s %d", fn, &sz) != 2) {
			fclose(f);
			return false;
		}
		char fn2[1000];
		sprintf(fn2, "%s/%s", DOWNLOAD_PATH, fn);
		ALLEGRO_FILE *f2 = al_fopen(fn2, "rb");
		if (f2) {
			int sz2 = al_fsize(f2);
			al_fclose(f2);
			if (sz2 == sz) {
				continue;
			}
		}
		count++;
		if (filenames == NULL) {
			filenames = malloc(2 * sizeof(char *));
			lengths = malloc(2 * sizeof(int));
		}
		else {
			filenames = realloc(filenames, (count+1) * sizeof(char *));
			lengths = realloc(lengths, (count+1) * sizeof(int));
		}
		filenames[count-1] = strdup(fn);
		lengths[count-1] = sz;
	}
	if (filenames != NULL) {
		filenames[count] = NULL;
		lengths[count] = 0;

		download_list(filenames, lengths);

		free(filenames);
		free(lengths);
	}

	fclose(f);

	return true;
}

static void *hqm_go_thread(void *arg)
{
	(void)arg;

	is_downloading = true;

	mkdir(DOWNLOAD_PATH, 0755);

#ifdef WINPC
	WSADATA crap;
	WSAStartup(MAKEWORD(2, 2), &crap);
#endif

	int len = download_file("list.txt");
	if (len != EXPECTED_LIST_SIZE) {
		is_downloading = false;
		return NULL;
	}

	float percent;
	int status = hqm_get_status(&percent);

	download_all();

#ifdef WINPC
	WSACleanup();
#endif

	is_downloading = false;

	return NULL;
}

bool hqm_is_downloading(void)
{
	return is_downloading;
}

void hqm_go(void)
{
	al_run_detached_thread(hqm_go_thread, NULL);
}

void hqm_stop(void)
{
	stop = true;
}

const char *hqm_status_string(int status)
{
	const char *strs[] = {
		"Complete",
		"Partial",
		"Not started"
	};

	return strs[status];
}

int hqm_get_status(float *percent)
{
	if (percent)
		*percent = 0.0f;

	char fn[1000];
	sprintf(fn, "%s/%s", DOWNLOAD_PATH, "list.txt");

	ALLEGRO_FILE *f = al_fopen(fn, "r");

	if (f == NULL || al_fsize(f) != EXPECTED_LIST_SIZE) {
		if (f) al_fclose(f);
		goto nuthin;
	}

	size_t read;
	char buf[100];
	int sz;
	int count = 0;
	while ((read = al_fread(f, buf, 80)) == 80) {
		al_fseek(f, 1, ALLEGRO_SEEK_CUR);
		buf[80] = 0;
		if (sscanf(buf, "%s %d", fn, &sz) != 2) {
			if (count == 0) {
				al_fclose(f);
				goto nuthin;
			}
			al_fclose(f);
			goto partial;
		}
		char fn2[1000];
		sprintf(fn2, "%s/%s", DOWNLOAD_PATH, fn);
		ALLEGRO_FILE *f2 = al_fopen(fn2, "rb");
		if (f2) {
			int sz2 = al_fsize(f2);
			al_fclose(f2);
			if (sz2 != sz) {
				if (count == 0) {
					al_fclose(f);
					goto nuthin;
				}
				goto partial;
			}
		}
		else {
			if (count == 0) {
				al_fclose(f);
				goto nuthin;
			}
			goto partial;
		}
		count++;
	}

	if (count == NUM_FILES) {
		al_fclose(f);
		if (percent)
			*percent = 1.0f;
		return HQM_STATUS_COMPLETE;
	}

partial:
	al_fclose(f);
	if (percent)
		*percent = (float)count / NUM_FILES;
	return HQM_STATUS_PARTIAL;

nuthin:
	return HQM_STATUS_NOTSTARTED;
}

#ifdef ALLEGRO_WINDOWS
#define mkdir(a, b) mkdir(a)
#endif

void hqm_set_download_path(const char *path)
{
	strcpy(DOWNLOAD_PATH, path);
	while (DOWNLOAD_PATH[strlen(DOWNLOAD_PATH)-1] == '/' ||
			DOWNLOAD_PATH[strlen(DOWNLOAD_PATH)-1] == '\\') {
		DOWNLOAD_PATH[strlen(DOWNLOAD_PATH)-1] = 0;
	}
}

void hqm_delete(void)
{
	ALLEGRO_FS_ENTRY *dir = al_create_fs_entry(DOWNLOAD_PATH);
	if (!dir) return;
	if (!al_open_directory(dir)) return;

	ALLEGRO_FS_ENTRY *file;
	while ((file = al_read_directory(dir)) != NULL) {
		al_remove_fs_entry(file);
		al_destroy_fs_entry(file);
	}

	al_close_directory(dir);
	al_remove_fs_entry(dir);
	al_destroy_fs_entry(dir);
}

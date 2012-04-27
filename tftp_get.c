#define _WIN32_WINNT 0x0501
#include <allegro5/allegro.h>

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

#define SERVER "nooskewl.com"
#define PORT "69"
#define BLKSIZE 32768
#define BLKSIZE_S "32768"

#define NUM_FILES 42 // FIXME: change this as needed
#define EXPECTED_LIST_SIZE (NUM_FILES * 81) // 60 chars filename 20 chars length + 1 \n

#define DOWNLOAD_PATH "." // FIXME: change me

static volatile bool stop = false;

static int sock = -1;
static struct sockaddr saddr;
static socklen_t saddr_len;

static int get(char *buf, int len)
{
	int n;
	int i;

	for (i = 0; i < 60/0.1; i++) {
		n = recvfrom(
			sock,
			buf,
			len,
			0,
			&saddr,
			&saddr_len
		);
		printf("n=%d\n", n);
#ifdef WINPC
		int e = WSAGetLastError();
		printf("WSAGetLastError()=%d\n", e);
		if (n == -1 && e == WSAEWOULDBLOCK) {
#else
		printf("errno=%d EAGAIN=%d\n", errno, EAGAIN);
		if (n == -1 && errno == EAGAIN) {
#endif
			printf("HERE\n");
			al_rest(0.1);
			continue;
		}
		break;
	}

	if (n < 1) return 0;

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

	saddr_len = sizeof(struct sockaddr);
	memcpy(&saddr, res->ai_addr, saddr_len);

	freeaddrinfo(res);

	return true;
}

#if 0
static bool connect_to_server(void)
{
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		return false;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(0);

	if (bind(sock, (const struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0) {
		return false;
	}

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

	unsigned int a = 64;
	unsigned int b = 85;
	unsigned int c = 168;
	unsigned int d = 26;
	unsigned short port = 69;

	unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
	unsigned short destination_port = port;

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = htonl(destination_address);
	srvaddr.sin_port = htons(destination_port);

	return true;
}
#endif

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

int get16bits(const char *buf)
{
	unsigned char *b1 = (unsigned char *)buf;
	unsigned char *b2 = (unsigned char *)buf+1;
	return ((int)(*b1) << 8) | (int)(*b2);
}

static int download_file(const char *filename)
{
	char buf[4+BLKSIZE];
	int sz, total_size = 0;
	int last_blocknum = -1;
	
	if (!connect_to_server()) {
		printf("couldn't connect\n");
		return -1;
	}

	printf("connected\n");

	sz = get_rrq(buf, filename);
	if (put(buf, sz) != sz) {
		printf("read request fail\n");
		shutdown_connection();
		return -1;
	}

	printf("sent rrq\n");

	// read option acknowledgement
	sz = get(buf, 2+strlen("blksize")+1+strlen(BLKSIZE_S)+1);
	if ((sz != (2+strlen("blksize")+1+strlen(BLKSIZE_S)+1)) || get16bits(buf) != 6) {
		shutdown_connection();
		return -1;
	}

	printf("got oack\n");

	buf[0] = 0;
	buf[1] = 4;
	buf[2] = 0;
	buf[3] = 0;
	if (put(buf, 4) != 4) {
		shutdown_connection();
		return -1;
	}

	printf("downloading %s\n", filename);
	
	FILE *f = fopen(filename, "wb");

	while (1) {
		if (stop) {
			fclose(f);
			shutdown_connection();
			stop = false;
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

void download_list(char **filenames, int *lengths)
{
	int i = 0;

	while (filenames[i]) {
		int len = download_file(filenames[i]);
		if (stop) {
			return;
		}
		if (len != lengths[i])
			continue;
		i++;
	}
}

bool download_all(void)
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
	while ((read = fread(buf, 80, 1, f)) == 1) {
		fseek(f, 1, SEEK_CUR);
		buf[80] = 0;
		if (sscanf(buf, "%s %d", fn, &sz) != 2) {
			fclose(f);
			return false;
		}
		ALLEGRO_FILE *f2 = al_fopen(fn, "rb");
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

		printf("Downloading %d files\n", count);

		download_list(filenames, lengths);

		free(filenames);
		free(lengths);
	}
	else {
		printf("Already done!\n");
	}

	fclose(f);

	return true;
}

static const char *get_status(float *percent)
{
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
		ALLEGRO_FILE *f2 = al_fopen(fn, "rb");
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
		*percent = 1.0f;
		return "Complete";
	}

partial:
	al_fclose(f);
	*percent = (float)count / NUM_FILES;
	return "Partially downloaded";

nuthin:
	return "Nothing downloaded";
}

void *test_thread(void *arg)
{
	(void)arg;
	al_rest(30);
	stop = true;
	return NULL;
}

int main(void)
{
	al_init();

#ifdef WINPC
	WSADATA crap;
	WSAStartup(MAKEWORD(2, 2), &crap);
#endif

	al_run_detached_thread(test_thread, NULL);
	
	int len = download_file("list.txt");
	if (len != EXPECTED_LIST_SIZE) {
		printf("Connection error\n");
		return 1;
	}

	const char *status;
	float percent;
	status = get_status(&percent);
	printf("Status: %s (%d percent)\n", status, (int)(100*percent));

	bool ret = download_all();
	if (ret) printf("Success!\n");
	else printf("Error!\n");

#ifdef WINPC
	WSACleanup();
#endif

	return 0;
}

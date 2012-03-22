#include "monster2.hpp"
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/platform/aintiphone.h>
#include "apple.h"

// bonus content

static std::map< std::string, std::vector<std::string> > available_content;
static bool downloading = false;
static std::string download_listname = "";
static bool file_finished = false;
static bool stop_early = false;
static std::vector<std::string> download_queue;
static ALLEGRO_MUTEX *download_mutex;
static NSURLDownload *download;

@interface Download_Delegate
-(void) downloadDidFinish:(NSURLDownload *)download;
@end

@implementation Download_Delegate
-(void) downloadDidFinish:(NSURLDownload *)download
{
	file_finished = true;
}
@end

static Download_Delegate *download_delegate;

static void stop_downloads(void)
{
	if (!downloading)
		return;

	al_lock_mutex(download_mutex);
	[download cancel];
	download_queue.clear();
	stop_early = true;
	downloading = false;
	file_finished = false;
	al_unlock_mutex(download_mutex);
}

void delete_content(std::string listname)
{
	if (downloading && listname == download_listname)
		stop_downloads();

	char output[1000];

	std::vector<std::string> &v = available_content[listname];

	for (int i = 0; i < v.size() && downloading; i++) {
		char size[1000], name[1000];
		sscanf(v[i].c_str(), "%s %s", size, name);
		strcpy(output, getUserResource("bonus/%s", name));
		if (al_filename_exists(output)) {
			al_remove_filename(output);
		}
	}
}

static bool download_file(NSURL *url, const char *output_name)
{
	NSURLRequest *request = [NSURLRequest requestWithURL:url];

	al_lock_mutex(download_mutex);
	if (!downloading) {
		return false;
	}
	download = [[NSURLDownload alloc] init];
	download = [[NSURLDownload alloc] initWithRequest:request delegate:download_delegate];
	NSString *str = [[NSString alloc] initWithUTF8String:output_name];
	[download setDestination:str allowOverwrite:TRUE];
	al_unlock_mutex(download_mutex);

	while (!file_finished && !stop_early) {
		al_rest(0.1);
	}

	bool ret = stop_early ? false : true;

	stop_early = false;
	file_finished = false;
	
	[download release];
	[str release];

	return ret;
}

static void *download_proc(void *arg)
{
	std::string listname = *(std::string *)arg;

	char url[1000];
	char output[1000];

	std::vector<std::string> &v = available_content[listname];

	for (int i = 0; i < v.size() && downloading; i++) {
		if (download_queue.size() <= 0)
			break;
		char size[1000], name[1000];
		sscanf(v[i].c_str(), "%s %s", size, name);
		strcpy(output, getUserResource("bonus/%s", name));
		if (al_filename_exists(output)) {
			ALLEGRO_FS_ENTRY *e;
			e = al_create_fs_entry(output);
			int esize = al_get_fs_entry_size(e);
			al_destroy_fs_entry(e);
			if (atoi(size) == esize) {
				continue;
			}
		}
		sprintf(url, "http://www.nooskewl.com/monster2-bonus-content/%s", v[i].c_str());
		NSString *url_path = [[NSString alloc] initWithUTF8String:url];
		NSURL *URL = [[NSURL alloc] initWithString:url_path];
		download_file(URL, output);
		[url_path release];
		[URL release];
	}

	downloading = false;
	
	return NULL;
}

void download_list(void)
{
	available_content.clear();

	char url[1000];
	char output[1000];

	sprintf(url, "http://www.nooskewl.com/monster2-bonus-content/list.txt");
	strcpy(output, getUserResource("bonus/list.txt"));
	NSString *url_path = [[NSString alloc] initWithUTF8String:url];
	NSURL *URL = [[NSURL alloc] initWithString:url_path];
	if (download_file(URL, output)) {
		FILE *f = fopen(output, "rb");
		char buf[1000];
		std::string listname = "";
		while (fgets(buf, 1000, f)) {
			printf("%s", buf); // FIXME
			while (isspace(buf[strlen(buf)-1]))
				buf[strlen(buf)-1] = 0;
			if (buf[0] == '\t')
				available_content[listname].push_back(std::string(buf));
			else
				listname = std::string(buf);
		}
		fclose(f);
	}
	
	[url_path release];
	[URL release];
}

void download_content(std::string name)
{
	download_list();

	if (downloading) {
		playPreloadedSample("error.ogg");
		return;
	}

	download_listname = name;
	
	al_run_detached_thread(download_proc, &name);
}

void init_downloads(void)
{
	download_mutex = al_create_mutex();
	if (!al_filename_exists(getUserResource("bonus/")))
		al_make_directory(getUserResource("bonus"));
	download_delegate = [[Download_Delegate alloc] init];
}

void shutdown_downloads(void)
{
	stop_downloads();
	al_destroy_mutex(download_mutex);
	[download_delegate release];
}


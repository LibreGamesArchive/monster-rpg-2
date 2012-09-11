#include <stdio.h>
#define CURL_STATICLIB
#include <curl/curl.h>

#include "ftpget.h"

struct FtpFile {
  const char *filename;
  FILE *stream;
  long written;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) {
    /* open file for writing */
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1; /* failure, can't open file to write */
  }
  size_t written = fwrite(buffer, size, nmemb, out->stream);
  out->written += written;
  return written;
}

int getfile(const char *url, const char *outfilename)
{
  CURL *curl;
  CURLcode res;
  struct FtpFile ftpfile={
    outfilename,
    NULL,
    0
  };

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if(curl) {
    /*
     * You better replace the URL with one that works!
     */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    /* Define our callback to get called when there's data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

    /* Switch on full protocol/debug output */
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if(CURLE_OK != res) {
      /* we failed */
      return -1;
    }
  }

  if(ftpfile.stream)
    fclose(ftpfile.stream); /* close the local file */

  curl_global_cleanup();

  return ftpfile.written;
}

/*
int main(void)
{
	printf(
		"%d\n",
		getfile("ftp://nooskewl.com/underwater_final.flac", "crap.flac")
	);

	return 0;
}
*/

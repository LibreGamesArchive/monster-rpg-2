#ifndef FTPGET_H
#define FTPGET_H

#ifdef __cplusplus
extern "C" {
#endif

// returns number of bytes read and written to file
int getfile(const char *url, const char *outfilename);

#ifdef __cplusplus
}
#endif

#endif

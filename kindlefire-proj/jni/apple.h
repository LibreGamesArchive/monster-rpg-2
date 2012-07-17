#ifndef __APPLE_H
#define __APPLE_H

void init_downloads(void);
void shutdown_downloads(void);
void delete_content(std::string listname);
void download_content(std::string name);
void download_list(void);

#endif

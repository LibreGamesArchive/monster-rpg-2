#define HQM_STATUS_COMPLETE   0
#define HQM_STATUS_PARTIAL    1
#define HQM_STATUS_NOTSTARTED 2

#include "debug.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int hqm_get_status(float *percent); // 0-1.0
const char *hqm_status_string(int status);
void hqm_set_download_path(const char *path); // with or without trailing /
                                              // also makes directory
void hqm_go(void);
void hqm_stop(void);
bool hqm_is_downloading(void);
void hqm_delete(void);

#ifdef __cplusplus
}
#endif


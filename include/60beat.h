#ifdef __cplusplus
extern "C" {
#endif

void sb_start(void);
bool is_sb_connected(void);
void get_sb_state(bool *l, bool *r, bool *u, bool *d, bool *b1, bool *b2, bool *b3);
void sb_stop(void);

#ifdef __cplusplus
}
#endif

#define my_load_bitmap my_tmp_load_bmp
static inline ALLEGRO_BITMAP *my_tmp_load_bmp(const char *filename)
{
	char fn[2000];
	strcpy(fn, filename);
	char ext[10];
	char *p = fn + (strlen(fn)-1);
	while (*p != '.') p--;
	// Watch this! FIXME
#if 1
	strcpy(p, ".tga");
#endif
	strncpy(ext, p, 10);

	int sz;
	unsigned char *bytes = slurp_file(fn, &sz);

	if (!bytes)
		return NULL;

	ALLEGRO_FILE *fp = al_open_memfile(bytes, sz, "rb");
	ALLEGRO_BITMAP *b = NULL;

	if (fp) {
		b = al_load_bitmap_f(fp, ext);
		al_fclose(fp);
	}

	delete[] bytes;

	return b;
}

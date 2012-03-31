// FIXME!
#ifdef ALLEGRO_ANDROID
#define my_load_bitmap my_tmp_load_bmp
static inline ALLEGRO_BITMAP *my_tmp_load_bmp(const char *filename)
{
	ALLEGRO_DEBUG("opening file");
	char fn[2000];
	strcpy(fn, filename);
	char ext[10];
	char *p = fn + (strlen(fn)-1);
	while (*p != '.') p--;
#ifdef ALLEGRO_ANDROID_XXX
	strcpy(p, ".tga");
#endif
	strncpy(ext, p, 10);

	ALLEGRO_FILE *fp = al_fopen(fn, "rb");
	ALLEGRO_DEBUG("fp=%p", fp);
	ALLEGRO_BITMAP *b = NULL;

	if (fp) {
		ALLEGRO_DEBUG("loading bitmap for REALLY REAL");
		b = al_load_bitmap_f(fp, ext);
		ALLEGRO_DEBUG("loaded %p", b);
		al_fclose(fp);
		ALLEGRO_DEBUG("file closed");
	}
	ALLEGRO_DEBUG("my_load returning");
	return b;
}
#else
#define my_load_bitmap al_load_bitmap
#endif


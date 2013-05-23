#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(int argc, char **argv)
{
	const char *infilename = argv[1];
	const char *outfilename = argv[2];

	al_init();
	al_init_image_addon();

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);

	ALLEGRO_BITMAP *in = al_load_bitmap(infilename);
	FILE *out = fopen(outfilename, "wb");

	for (int y = 0; y < al_get_bitmap_height(in); y++) {
		for (int x = 0; x < al_get_bitmap_width(in); x++) {
			ALLEGRO_COLOR c = al_get_pixel(in, x, y);
			unsigned char r, g, b, a;
			al_unmap_rgba(c, &r, &g, &b, &a);
			if (a == 0) r = 0, g = 0, b = 0;
			fputc(r, out);
			fputc(g, out);
			fputc(b, out);
			fputc(a, out);
		}
	}

	fclose(out);
}


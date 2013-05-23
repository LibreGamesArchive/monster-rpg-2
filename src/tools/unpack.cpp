#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include "mapping.h"

int main(int argc, char **argv)
{
	const char *infilename = argv[1];
	const char *outfilename = argv[2];

	al_init();
	al_init_image_addon();

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	ALLEGRO_BITMAP *in = al_load_bitmap(infilename);
	ALLEGRO_BITMAP *out = al_create_bitmap(512, 2048);

	al_set_target_bitmap(out);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	for (int i = 0; i < 4096; i++) {
		int j = mapping[i];
		int sx = j % (1024/16);
		int sy = j / (1024/16);
		int dx = i % (512/16);
		int dy = i / (512/16);
		al_draw_bitmap_region(
			in,
			sx*16, sy*16,
			16, 16,
			dx*16, dy*16,
			0
		);
	}

	al_save_bitmap(outfilename, out);
}

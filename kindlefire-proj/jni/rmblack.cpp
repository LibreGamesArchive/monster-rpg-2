#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(int argc, char **argv)
{
	al_init();
	al_init_image_addon();

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

	ALLEGRO_BITMAP *b = al_load_bitmap(argv[1]);

	int w = al_get_bitmap_width(b);
	int h = al_get_bitmap_height(b);

	al_set_target_bitmap(b);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			ALLEGRO_COLOR c= al_get_pixel(b, x, y);
			if (c.r == 0 && c.g == 0 && c.b == 0 && c.a == 1)
				al_put_pixel(x, y, al_map_rgba(0, 0, 0, 0));
		}
	}

	al_save_bitmap("test.png", b);
}


#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	al_init();
	al_init_image_addon();

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	ALLEGRO_BITMAP *tilemap = al_load_bitmap(argv[1]);
	ALLEGRO_BITMAP *result = al_create_bitmap(1024, 1024);

	al_set_target_bitmap(result);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	int tx = 0, ty = 0;
	int mx = 0, my = 0;

	int mapping[(512/16)*(2048/16)];
	memset(mapping, 0, sizeof(mapping));
	int i = 0;
	mapping[i++] = 0;
	mx = 16;

	for (int y = 0; y < 2048/16; y++) {
		int x;
		if (y == 0) x = 1;
		else x = 0;
		for (; x < 512/16; x++) { // 1 == skip first alpha img
			int ox = x * 16;
			int oy = y * 16;
			bool all_alpha = true;
			// FIXME: tile index 96 is all alpha and still in the packed.png. keep for legacy reasons.
			if (i == 96) {
				all_alpha = false;
			}
			else {
				for (int py = 0; py < 16; py++) {
					for (int px = 0; px < 16; px++) {
						ALLEGRO_COLOR pix = al_get_pixel(tilemap, px+ox, py+oy);
						unsigned char r, g, b, a;
						al_unmap_rgba(pix, &r, &g, &b, &a);
						if (!(a == 0)) {
							all_alpha = false;
							break;
						}
					}
					if (!all_alpha) break;
				}
			}
			if (!all_alpha) {
				mapping[(y*32)+x] = i++;
				if (ox == 0 && oy == (3*16)) {
				}
				else {
					al_draw_bitmap_region(tilemap, ox, oy,
						16, 16, mx, my, 0);
				}
				mx += 16;
				if (mx == 1024) {
					mx = 0;
					my += 16;
				}
			}
		}
	}

	printf("int mapping[4096] = {\n\t");
	for (int i = 0; i < 4096; i++) {
		if ((i % 10) == 0)
			printf(",\n\t");
		else
			printf(", ");
		printf("%4d", mapping[i]);
	}
	printf("\n};\n");

	al_save_bitmap("packed.png", result);
}

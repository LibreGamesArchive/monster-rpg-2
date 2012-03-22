#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

const int TOP = 175;
const int MIDDLE = 255;

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage: infile.png outfile.png\n");
		return 0;
	}

	al_init();
	al_init_image_addon();

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);

	ALLEGRO_BITMAP *inbmp = al_load_bitmap(argv[1]);

	al_set_target_bitmap(inbmp);

	int x, y;
	for (y = 0; y < al_get_bitmap_height(inbmp);) {
		for (x = 0; x < al_get_bitmap_width(inbmp); x++) {
			ALLEGRO_COLOR pixel = al_get_pixel(inbmp, x, y);
			if (pixel.a == 0) {
				int h = 1;
				do {
					ALLEGRO_COLOR p = al_get_pixel(inbmp, x, y+h);
					if (p.a == 0 || (p.r == 1 && p.g == 1 && p.b == 1))
						h++;
					else
						break;
				} while (1);
				printf("h=%d\n", h);
				
				for (int r = 0; r < h; r++) {
					ALLEGRO_COLOR c;
					if (r < h/2) {
						float p = (float)r / (h/2);
						int diff = MIDDLE-TOP;
						int v = diff*p + TOP;
						c = al_map_rgb(v, v, v);
						printf("v=%d\n", v);
					}
					else {
						float p = 1 - ((float)(r-h/2) / (h/2));
						int diff = MIDDLE-TOP;
						int v = diff*p + TOP;
						c = al_map_rgb(v, v, v);
						printf("v=%d\n", v);
					}
					for (int xx = 0; xx < al_get_bitmap_width(inbmp); xx++) {
						ALLEGRO_COLOR p = al_get_pixel(inbmp, xx, y+r);
						if (p.r == 1 && p.g == 1 && p.b == 1 && p.a == 1) {
							al_put_pixel(xx, y+r, c);
						}
					}
				}
				y += h+1;
				goto loop;
			}
		}
		y++;
loop:;
	}

	al_save_bitmap(argv[2], inbmp);
}
END_OF_MAIN()

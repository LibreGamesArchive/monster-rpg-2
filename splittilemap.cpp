#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(int argc, char **argv)
{
	al_init();
	al_init_image_addon();

	ALLEGRO_BITMAP *tm = al_load_bitmap("tilemapfixed.png");
	int w = al_get_bitmap_width(tm);
	int h = al_get_bitmap_height(tm);
	int wt = w / 16;
	int ht = h / 16;
	ALLEGRO_BITMAP *b = al_create_bitmap(16, 16);
	al_set_target_bitmap(b);

	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wt; x++) {
			al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
			al_draw_bitmap_region(tm, x*16, y*16, 16, 16, 0, 0, 0);
			char name[200];
			sprintf(name, "out/%d-%d.png", x, y);
			al_save_bitmap(name, b);
		}
	}
}

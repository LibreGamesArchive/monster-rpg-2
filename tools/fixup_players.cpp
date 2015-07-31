#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

const int SUBW = 45;
const int SUBH = 40;

int furthest_left(ALLEGRO_BITMAP *bmp)
{
	int f = SUBW-1;
	for (int y = 0; y < SUBH; y++) {
		int thisf = SUBW-1;
		for (int x = 0; x < SUBW; x++) {
			ALLEGRO_COLOR pixel = al_get_pixel(bmp, x, y);
			unsigned char r, g, b;
			al_unmap_rgb(pixel, &r, &g, &b);
			if (r == 255 && g == 0 && b == 255)
				continue;
			thisf = x;
			break;
		}
		if (thisf < f)
			f = thisf;
	}

	return f;
}

int main(int argc, char **argv)
{
	const char *if_name = argv[1];
	const char *of_name = argv[2];

	al_init();
	al_init_image_addon();

	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

	ALLEGRO_BITMAP *i_bmp = al_load_bitmap(if_name);
	int w, h;
	w = al_get_bitmap_width(i_bmp);
	h = al_get_bitmap_height(i_bmp);
	ALLEGRO_BITMAP *o_bmp = al_create_bitmap(w, h);
	al_set_target_bitmap(o_bmp);
	al_clear_to_color(al_map_rgb(255, 0, 255));

	int nanims = h/SUBH;
	int nframes = w/SUBW;

	for (int anim = 0; anim < nanims; anim++) {
		int f = SUBW-1;
		for (int i = 0; i < 4; i++) {
			ALLEGRO_BITMAP *sub = al_create_sub_bitmap(i_bmp,
				i*SUBW, anim*SUBH, SUBW, SUBH);
			int thisf = furthest_left(sub);
			if (thisf < SUBW-1 && thisf < f)
				f = thisf;
			al_destroy_bitmap(sub);
		}
		for (int i = 0; i < 4; i++) {
			ALLEGRO_BITMAP *sub = al_create_sub_bitmap(i_bmp,
				i*SUBW, anim*SUBH, SUBW, SUBH);
			al_draw_bitmap(sub,
				i*SUBW-(f/2),
				anim*SUBH, 0);
			al_destroy_bitmap(sub);
		}
	}

	al_save_bitmap(of_name, o_bmp);
}
END_OF_MAIN()


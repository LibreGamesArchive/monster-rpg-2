#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <cctype>

int main(int argc, char **argv)
{
	const char *text = argv[1];

	al_init();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

	ALLEGRO_FONT *font = al_load_ttf_font("DejaVuSans.ttf", 9, ALLEGRO_TTF_MONOCHROME);

	ALLEGRO_BITMAP *cursor = al_load_bitmap("cursor.png");

	int len = al_get_text_width(font, text) + al_get_bitmap_width(cursor) + 3 + 2;
	int h = al_get_font_line_height(font)+2;

	ALLEGRO_BITMAP *bmp = al_create_bitmap(len, h);

	al_set_target_bitmap(bmp);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(cursor)+3, 0, 0, text);

	for (int y = 0; y < h/2-2; y++) {
		for (int x = 0; x < len; x++) {
			ALLEGRO_COLOR col = al_get_pixel(bmp, x, y);
			float f = (float)y / (h/2-2);
			int c = 125 + (f * 130);
			if (col.a != 0) {
				al_put_pixel(x, y, al_map_rgb(c, c, c));
			}
			int yy = h-y-1;
			col = al_get_pixel(bmp, x, yy);
			if (col.a == 0) continue;
			al_put_pixel(x, yy, al_map_rgb(c, c, c));
		}
	}

	ALLEGRO_BITMAP *bmp2 = al_create_bitmap(len, h);
	al_set_target_bitmap(bmp2);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	al_draw_text(font, al_map_rgb(0, 0, 0), al_get_bitmap_width(cursor)+3+1, 1, 0, text);
	al_draw_text(font, al_map_rgb(0, 0, 0), al_get_bitmap_width(cursor)+3, 1, 0, text);
	al_draw_text(font, al_map_rgb(0, 0, 0), al_get_bitmap_width(cursor)+3+1, 0, 0, text);
	al_draw_bitmap(bmp, 0, 0, 0);

	char buf[1000];
	sprintf(buf, "%s.png", text);
	buf[0] = tolower(buf[0]);
	al_save_bitmap(buf, bmp2);

	al_draw_bitmap(cursor, 0, (h-al_get_bitmap_height(cursor))/2, 0);
	sprintf(buf, "%ssel.png", text);
	buf[0] = tolower(buf[0]);
	al_save_bitmap(buf, bmp2);
}

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(int argc, char **argv)
{
	al_init();
	al_init_image_addon();

	ALLEGRO_BITMAP *b = al_load_bitmap(argv[1]);
	al_convert_mask_to_alpha(b, al_map_rgb(255, 0, 255));
	al_save_bitmap(argv[2], b);
}


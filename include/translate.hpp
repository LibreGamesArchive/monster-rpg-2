#ifndef TRANS_HPP
#define TRANS_HPP

#include <allegro5/allegro.h>
#include <vector>

extern std::vector<std::string> pre_translated_strings;
extern std::vector<ALLEGRO_USTR *> post_translated_strings;

std::string get_language_name(int index);
std::string get_language_friendly_name(int index);
void cache_all_glyphs();
void load_translation(const char *filename);
void load_translation_tags(void);
const char *_t(const char *tag);
void destroy_translation(void);

#endif // TRANS_HPP

#include "monster2.hpp"

/*
 * In the translation files, \n is replaced with ^. The load functions
 * convert it back to a single newline character.
 */

std::vector<std::string> pre_translated_strings;
std::vector<ALLEGRO_USTR *> post_translated_strings;

static std::string translation_languages[][2] = {
	{ "English", "English" },
	{ "Dutch", "Nederlands (Dutch)" },
	{ "French", "Français (French)" },
	{ "German", "Deutsch (German)" },
	{ "Greek", "ελληνικά (Greek)" },
	{ "Italian", "Italien (Italian)" },
	{ "Polish", "Polski (Polish)" },
	{ "Portuguese-Brazil", "Portuguese (Brazil)" },
	{ "Portuguese-Europe", "Portuguese (Europe)" },
	{ "Spanish", "Español (Spanish)" },
	{ "Russian", "русский (Russian)" },
	{ "", "" }
};

std::string get_language_name(int index) {
	return translation_languages[index][0];
}

std::string get_language_friendly_name(int index)
{
	return translation_languages[index][1];
}

void load_translation_tags(void)
{
	ALLEGRO_FILE *f = al_fopen(getResource("English.utf8"), "r");
	if (!f) {
		native_error("Couldn't load English.utf8.");
	}

	char buf[5000];

	while (al_fgets(f, buf, 5000) != NULL) {
		if (buf[strlen(buf)-1] == 0xa)
			buf[strlen(buf)-1] = 0;
		if (buf[strlen(buf)-1] == 0xd)
			buf[strlen(buf)-1] = 0;
		for (int i = 0; buf[i]; i++) {
			if (buf[i] == '^') buf[i] = '\n';
		}
		pre_translated_strings.push_back(std::string(buf));
	}

	al_fclose(f);
}

static void ustr_replace_all(ALLEGRO_USTR *ustr, int32_t c1, int32_t c2)
{
	for (int i = al_ustr_find_chr(ustr, 0, c1); i >= 0; i = al_ustr_find_chr(ustr, i, c1)) {
		al_ustr_set_chr(ustr, i, c2);
	}
}

void destroy_translation(void)
{
	std::vector<ALLEGRO_USTR *> &v = post_translated_strings;

	for (int i = 0; i < (int)v.size(); i++) {
		al_ustr_free(v[i]);
	}
	v.clear();
}

static std::string get_entire_translation(const char *filename)
{
	int sz;
	unsigned char *bytes = slurp_file(getResource("%s.utf8", filename), &sz);
	if (!bytes) {
		native_error("Load error.", filename);
	}

	ALLEGRO_FILE *f = al_open_memfile(bytes, sz, "rb");

	ALLEGRO_USTR *ustr;

	std::string whole_translation;

	while ((ustr = al_fget_ustr(f)) != NULL) {
		// remove newline
		int size = (int)al_ustr_size(ustr);
		const char *cstr = al_cstr(ustr);
		int count = 0;
		if (cstr[strlen(cstr)-1] == 0xa)
			count++;
		if (cstr[strlen(cstr)-2] == 0xd)
			count++;
		if (count > 0) {
			al_ustr_remove_range(ustr, size-count, size);
		}
		ustr_replace_all(ustr, '^', '\n');
		whole_translation += al_cstr(ustr);
	}

	al_fclose(f);
	delete[] bytes;

	return whole_translation;
}

std::string get_all_glyphs()
{
	std::string characters;

	for (int i = 0; translation_languages[i][0] != ""; i++) {
		characters += get_entire_translation(translation_languages[i][0].c_str());
	}

	return characters;
}

void load_translation(const char *filename)
{
	destroy_translation();

	int sz;
	unsigned char *bytes = slurp_file(getResource("%s.utf8", filename), &sz);
	if (!bytes) {
		native_error("Load error.", filename);
	}

	ALLEGRO_FILE *f = al_open_memfile(bytes, sz, "rb");

	ALLEGRO_USTR *ustr;

	while ((ustr = al_fget_ustr(f)) != NULL) {
		// remove newline
		int size = (int)al_ustr_size(ustr);
		const char *cstr = al_cstr(ustr);
		int count = 0;
		if (cstr[strlen(cstr)-1] == 0xa)
			count++;
		if (cstr[strlen(cstr)-2] == 0xd)
			count++;
		if (count > 0) {
			al_ustr_remove_range(ustr, size-count, size);
		}
		ustr_replace_all(ustr, '^', '\n');
		post_translated_strings.push_back(ustr);
	}

	al_fclose(f);
	delete[] bytes;
}

const char *_t(const char *_tag)
{
	#define N 20
	static char s[N][5000];
	static int i = 0;
	char *p;

	std::string tag = std::string(_tag);

	for (int i = 0; i < (int)pre_translated_strings.size(); i++) {
		if (i >= (int)post_translated_strings.size())
			break;
		if (pre_translated_strings[i] == tag) {
			tag = std::string(al_cstr(post_translated_strings[i]));
			break;
		}
	}

	strcpy(s[i], tag.c_str());
	p = s[i];

	i++;
	i %= N;

	return p;
}


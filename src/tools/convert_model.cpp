#include <zlib.h>
#include <cstdio>
#include <allegro5/allegro5.h>
#include <vector>
#include <string>
#include <sstream>

struct V3 {
	float x, y, z;
	ALLEGRO_COLOR color;
	float u, v;
};

struct MODEL {
	std::vector< std::vector<V3> > verts;
};

class WriteError {
};

/*
 * Write 32 bits, little endian.
 */
void iputl(long l, gzFile f)
{
	if (gzputc(f, (int)(l & 0xFF)) == -1) {
		throw WriteError();
	}
	if (gzputc(f, (int)((l >> 8) & 0xFF)) == -1) {
		throw WriteError();
	}
	if (gzputc(f, (int)((l >> 16) & 0xFF)) == -1) {
		throw WriteError();
	}
	if (gzputc(f, (int)((l >> 24) & 0xFF)) == -1) {
		throw WriteError();
	}
}

void my_pack_putc(int c, gzFile f)
{
	if (gzputc(f, c) == EOF)
		throw WriteError();
}

MODEL *load_model(const char *filename)
{
	union {
		float fl;
		uint32_t in;
	};

	/* Keep me */
	char b[1000];
	sprintf(b, "%s.new", filename);
	gzFile out = NULL;
	out = gzopen(b, "wb9");

	FILE *f = fopen(filename, "r");
	if (!f)
		return NULL;
	
	char buf[1000];
	MODEL *m = new MODEL;

	while (fgets(buf, 1000, f)) {
		std::vector<V3> v;
		buf[strlen(buf)-1] = 0; // remove \n
		std::string bufS(buf);
		std::stringstream ss(bufS);
		std::string word;
		while (!ss.eof()) {
			int r, g, b;
			ss >> word;
			r = atoi(word.c_str());
			ss >> word;
			g = atoi(word.c_str());
			ss >> word;
			b = atoi(word.c_str());
			float x, y, z;
			ss >> word;
			x = atof(word.c_str());
			ss >> word;
			y = atof(word.c_str());
			ss >> word;
			z = atof(word.c_str());
			V3 v3;
			v3.x = x;
			v3.y = y;
			v3.z = z;
			ALLEGRO_COLOR color;
			color.r = (float)r/255.0f;
			color.g = (float)g/255.0f;
			color.b = (float)b/255.0f;
			color.a = 1.0f;
			v3.color = color;
			v.push_back(v3);
		}
		m->verts.push_back(v);
	}
	fclose(f);

	for (unsigned int i = 0; i < m->verts.size(); i++) {
		my_pack_putc((unsigned char)m->verts[i].size(), out);
		for (unsigned int j = 0; j < m->verts[i].size(); j++) {
			V3 v = m->verts[i][j];
			my_pack_putc((unsigned char)(v.color.r*255), out);
			my_pack_putc((unsigned char)(v.color.g*255), out);
			my_pack_putc((unsigned char)(v.color.b*255), out);
			fl = v.x;
			iputl(in, out);
			fl = v.y;
			iputl(in, out);
			fl = v.z;
			iputl(in, out);
		}
	}
	gzclose(out);

	return m;
}

int main(int argc, char **argv)
{
	load_model(argv[1]);
}
END_OF_MAIN()

#ifndef _3D_HPP
#define _3D_HPP

struct V3 {
	float x, y, z;
	MCOLOR color;
	float u, v;
};

void mesa_frustum(float *transform,
	float left, float right,
	float bottom, float top,
	float nearval, float farval);
void mesa_rotate(float *transform,
	float angle, float x, float y, float z );
void mesa_scale(float *transform, float x, float y, float z);
void mesa_translate(float *mat, float x, float y, float z);

void enable_cull_face(bool ccw = false);
void disable_cull_face(void);
void enable_zbuffer(bool less_equal = true);
void disable_zbuffer(void);

struct TMPMODEL {
	std::vector< std::vector<V3> > verts;
};

struct MODEL {
	std::vector<ALLEGRO_VERTEX *> verts;
	std::vector<int> num_verts;
	~MODEL() {
		for (size_t i = 0; i < verts.size(); i++) {
			delete[] verts[i];
		}
	}
};

void set_projection(float near, float far, bool reverse_y = false, bool rotate = false);

bool archery(bool for_points);

void volcano_scene(void);

#endif

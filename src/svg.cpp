/*
 * Licensed under the MIT License,
 *                http://opensource.org/licenses/mit-license.php
 * Copyright 2013 Trent Gamblin <trent@nooskewl.com>
 * Copyright 2008 James Bursa <james@semichrome.net>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <cmath>

extern "C" {
#include "svgtiny.h"
}

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <vector>

const int CONVEX = 1;
const int CONCAVE = -1;

int polygon_is_convex(const std::vector<float> &v)
{
	int i;
	int x0, x1, x2;
	int y0, y1, y2;
	int flag = 0;
	double z;

	int n = v.size();

	if (n < 6)
		return(0);

	for (i = 0; i < n; i += 2) {
		x0 = v[i % n];
		y0 = v[(i + 1) % n];
		x1 = v[(i + 2) % n];
		y1 = v[(i + 3) % n];
		x2 = v[(i + 4) % n];
		y2 = v[(i + 5) % n];

		z = (x1 - x0) * (y2 - y1);
		z -= (y1 - y0) * (x2 - x1);

		if (z < 0)
			flag |= 1;
		else if (z > 0)
			flag |= 2;
		if (flag == 3)
			return(CONCAVE);
	}
	if (flag != 0)
		return(CONVEX);
	else
		return(0);
}

static bool real_polygon_is_clockwise(const std::vector<float> &v)
{
	int convex = polygon_is_convex(v);

	if (convex == 0) {
		return false;
	}
	
	int n = v.size();

	if (convex == CONVEX) {
		for (int i = 0; i < n/2; i++) {
			float x1, y1;
			if (i == 0) {
				x1 = v[n-2];
				y1 = v[n-1];
			}
			else {
				x1 = v[i*2-2];
				y1 = v[i*2-1];
			}
			float x2 = v[i*2+0];
			float y2 = v[i*2+1];
			float x3 = v[(i*2+2) % n];
			float y3 = v[(i*2+3) % n];
			float cross = (x2-x1)*(y3-y2)-(y2-y1)*(x3-x2);
			if (cross > 0) return false;
		}
		return true;
	}
	else {
		float sum = 0;
		for (int i = 0; i < n; i += 2) {
			float x1 = v[i % n];
			float y1 = v[(i + 1) % n];
			float x2 = v[(i + 2) % n];
			float y2 = v[(i + 3) % n];

			sum += (x1*y2) - (x2*y1);
		}
		sum /= 2;

		return sum < 0;
	}
}

ALLEGRO_BITMAP *load_svg(const char *filename, float scale)
{
	FILE *fd;
	struct stat sb;
	char *buffer;
	size_t size;
	size_t n;
	struct svgtiny_diagram *diagram;
	svgtiny_code code;

	double start = al_get_time();

	/* load file into memory buffer */
	fd = fopen(filename, "rb");
	if (!fd) {
		return NULL;
	}

	if (stat(filename, &sb)) {
		return NULL;
	}
	size = sb.st_size;

	buffer = (char *)malloc(size);
	if (!buffer) {
		return NULL;
	}

	n = fread(buffer, 1, size, fd);
	if (n != size) {
		return NULL;
	}

	fclose(fd);

	/* create svgtiny object */
	diagram = svgtiny_create();
	if (!diagram) {
		return NULL;
	}

	/* parse */
	code = svgtiny_parse(diagram, buffer, size, filename, 1000, 1000);
	if (code != svgtiny_OK) {
		fprintf(stderr, "svgtiny_parse failed: ");
		switch (code) {
		case svgtiny_OUT_OF_MEMORY:
			fprintf(stderr, "svgtiny_OUT_OF_MEMORY");
			break;
		case svgtiny_LIBDOM_ERROR:
			fprintf(stderr, "svgtiny_LIBDOM_ERROR");
			break;
		case svgtiny_NOT_SVG:
			fprintf(stderr, "svgtiny_NOT_SVG");
			break;
		case svgtiny_SVG_ERROR:
			fprintf(stderr, "svgtiny_SVG_ERROR: line %i: %s",
					diagram->error_line,
					diagram->error_message);
			break;
		default:
			fprintf(stderr, "unknown svgtiny_code %i", code);
			break;
		}
		fprintf(stderr, "\n");
	}

	free(buffer);

	ALLEGRO_BITMAP *bmp;
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();

	bmp = al_create_bitmap(scale*diagram->width, scale*diagram->height);
	al_set_target_bitmap(bmp);
	al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));

	for (unsigned int i = 0; i != diagram->shape_count; i++) {
		std::vector< std::vector<float> > points;
		points.push_back(std::vector<float>());
		int subshape = 0;
		ALLEGRO_COLOR fill;
		ALLEGRO_COLOR stroke;
		if (diagram->shape[i].fill == svgtiny_TRANSPARENT)
			fill = al_map_rgba_f(0, 0, 0, 0);
		else {
			float fill_opacity = diagram->shape[i].fill_opacity;
			char buf[100];
			sprintf(buf, "#%.6x ", diagram->shape[i].fill);
			fill = al_color_html(buf);
			fill.a = fill_opacity;
		}
		if (diagram->shape[i].stroke == svgtiny_TRANSPARENT)
			stroke = al_map_rgba_f(0, 0, 0, 0);
		else {
			float stroke_opacity = diagram->shape[i].stroke_opacity;
			char buf[100];
			sprintf(buf, "#%.6x ", diagram->shape[i].stroke);
			stroke = al_color_html(buf);
			stroke.a = stroke_opacity;
		}
		float stroke_width = scale * diagram->shape[i].stroke_width;
		float last_x = 0, last_y = 0;
		bool last_was_move = false;
		if (diagram->shape[i].path) {
			for (unsigned int j = 0;
					j != diagram->shape[i].path_length; ) {
				switch ((int) diagram->shape[i].path[j]) {
				case svgtiny_PATH_LINE:
					if (last_was_move) {
						points[subshape].push_back(last_x);
						points[subshape].push_back(last_y);
					}
					last_x = scale*diagram->shape[i].path[j+1];
					last_y = scale*diagram->shape[i].path[j+2];
					points[subshape].push_back(last_x);
					points[subshape].push_back(last_y);
					last_was_move = false;
					j += 3;
					break;
				case svgtiny_PATH_MOVE:
					last_x = scale*diagram->shape[i].path[j+1];
					last_y = scale*diagram->shape[i].path[j+2];
					last_was_move = true;
					j += 3;
					break;
				case svgtiny_PATH_CLOSE:
					last_x = points[subshape][0];
					last_y = points[subshape][1];
					points[subshape].push_back(last_x);
					points[subshape].push_back(last_y);
					points.push_back(std::vector<float>());
					subshape++;
					last_was_move = false;
					j += 1;
					break;
				case svgtiny_PATH_BEZIER: {
					float spline[8];
					spline[0] = last_x;
					spline[1] = last_y;
					spline[2] = scale * diagram->shape[i].path[j + 1];
					spline[3] = scale * diagram->shape[i].path[j + 2];
					spline[4] = scale * diagram->shape[i].path[j + 3];
					spline[5] = scale * diagram->shape[i].path[j + 4];
					spline[6] = scale * diagram->shape[i].path[j + 5];
					spline[7] = scale * diagram->shape[i].path[j + 6];
					float dx, dy, dist;
					dx = spline[6] - spline[0];
					dy = spline[7] - spline[1];
					dist = sqrt(dx*dx + dy*dy);
					int npts = sqrt(dist*2) * 1.3; // trial and error yields this number
					if (npts < 1) npts = 1;
					float *out = (float *)malloc(npts*2*sizeof(float));
					al_calculate_spline(out, 2*sizeof(float), spline, 0, npts);
					for (int k = last_was_move ? 0 : 1; k < npts; k++) {
						points[subshape].push_back(out[k*2]);
						points[subshape].push_back(out[k*2+1]);
					}
					last_x = out[(npts-1)*2];
					last_y = out[(npts-1)*2+1];
					free(out);
					last_was_move = false;
					j += 7;
					break;
				}
				default:
					j += 1;
				}
			}
		}

		for (int j = 0; j < subshape+1; j++) {
			bool reverse = !real_polygon_is_clockwise(points[j]);

			float *v = (float *)malloc(points[j].size()*sizeof(float));

			int count = 0;

			for (int k = 0; k < (int)points[j].size()/2; k++) {
				int l;
				if (reverse) {
					l = points[j].size() - ((k+1)*2);
				}
				else {
					l = k*2;
				}

				v[k*2] = points[j][l];
				v[k*2+1] = points[j][l+1];

				count++;
			}

			al_draw_filled_polygon(v, count, fill);

			al_draw_polyline(v, count, ALLEGRO_LINE_JOIN_NONE, ALLEGRO_LINE_CAP_NONE, stroke, stroke_width, 0.0);

			free(v);
		}
	}

	double end = al_get_time();

	//printf("elapsed: %f seconds\n", end-start);

	svgtiny_free(diagram);

	al_set_target_bitmap(old_target);

	return bmp;
}


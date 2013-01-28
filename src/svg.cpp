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
#include <allegro5/allegro_opengl.h>

#include <vector>

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
		fprintf(stderr, "Unable to allocate %lld bytes\n",
				(long long) size);
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
		fprintf(stderr, "svgtiny_create failed\n");
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

	int diagram_w = scale*diagram->width;
	int diagram_h = scale*diagram->height;

	GLuint fb;
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	GLuint ColorBufferID;
	glGenRenderbuffersEXT(1, &ColorBufferID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, ColorBufferID);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 4, GL_RGBA8, diagram_w, diagram_h);
	GLuint DepthBufferID;
	glGenRenderbuffersEXT(1, &DepthBufferID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DepthBufferID);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 4, GL_DEPTH24_STENCIL8_EXT, diagram_w, diagram_h);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, ColorBufferID);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, DepthBufferID);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, DepthBufferID);
	GLint status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	GLint old_vp[4];

	glGetIntegerv(GL_VIEWPORT, old_vp);

	glViewport(0, 0, diagram_w, diagram_h);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, diagram_w, diagram_h, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

/*
	ALLEGRO_TRANSFORM t, backup1, backup2;
	al_copy_transform(&backup1, al_get_projection_transform(al_get_current_display()));
	al_copy_transform(&backup2, al_get_current_transform());
	al_identity_transform(&t);
	al_orthographic_transform(&t, 0, 0, 1, diagram_w, diagram_h, 1000);
	al_set_projection_transform(al_get_current_display(), &t);
	al_identity_transform(&t);
	al_use_transform(&t);
*/

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

    		glClearStencil(0.0);
		glClear(GL_STENCIL_BUFFER_BIT);
    		glEnable(GL_STENCIL_TEST);

		glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		float x1 = -1;
		float y1 = -1;

		glColor3f(1, 1, 1);

		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < points.size(); j++) {
			for (int k = 0; k < (int)points[j].size()/2-1; k++) {
				float x2 = points[j][k*2];
				float y2 = points[j][k*2+1];
				float x3 = points[j][(k+1)*2];
				float y3 = points[j][(k+1)*2+1];
				glVertex2f(x1, y1);
				glVertex2f(x2, y2);
				glVertex2f(x3, y3);
			}
		}
		glEnd();

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 1);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glColor4f(fill.r, fill.g, fill.b, fill.a);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(0, diagram_h);
			glVertex2f(0, 0);
			glVertex2f(diagram_w, 0);
			glVertex2f(diagram_w, diagram_h);
		glEnd();

		glDisable(GL_STENCIL_TEST);

		for (size_t j = 0; j < points.size(); j++) {
			al_draw_polyline(&points[j][0], points[j].size()/2, ALLEGRO_LINE_JOIN_NONE, ALLEGRO_LINE_CAP_NONE, stroke, stroke_width, 0.0);
		}
	}

	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	int old_format = al_get_new_bitmap_format();
	
	//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);

	ALLEGRO_BITMAP *out = al_create_bitmap(diagram_w, diagram_h);

	al_set_target_bitmap(out);

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, fb);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, al_get_opengl_fbo(out));

	int rw, rh;
	al_get_opengl_texture_size(out, &rw, &rh);

	glBlitFramebufferEXT(0, 0, diagram_w, diagram_h, 0, 0, diagram_w, diagram_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	al_set_target_bitmap(old_target);
	al_set_new_bitmap_format(old_format);

	double end = al_get_time();

	//printf("elapsed: %f seconds\n", end-start);

	glDeleteFramebuffersEXT(1, &fb);
	glDeleteRenderbuffersEXT(1, &ColorBufferID);
	glDeleteRenderbuffersEXT(1, &DepthBufferID);

	svgtiny_free(diagram);

	glViewport(old_vp[0], old_vp[1], old_vp[2], old_vp[3]);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	return out;
}


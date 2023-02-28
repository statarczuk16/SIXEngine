/*
  kiss_sdl widget toolkit
  Copyright (c) 2016 Tarvo Korrovits <tkorrovi@mail.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would
     be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

  kiss_sdl version 1.2.4
*/

#include "kiss_sdl.h"


kiss_font kiss_textfont, kiss_buttonfont;
kiss_image kiss_normal, kiss_prelight, kiss_active, kiss_bar,
	kiss_up, kiss_down, kiss_left, kiss_right, kiss_vslider,
	kiss_hslider, kiss_selected, kiss_unselected, kiss_combo;
int kiss_screen_width, kiss_screen_height;
int kiss_textfont_size = 30;
int kiss_buttonfont_size = 30;
int kiss_click_interval = 140;
int kiss_progress_interval = 50;
int kiss_slider_padding = 2;
int kiss_edge = 2;
int kiss_border = 6;
double kiss_spacing = 0.5;
SDL_Color kiss_white = {255, 255, 255, 255};
SDL_Color kiss_black = {0, 0, 0, 255};
SDL_Color kiss_green = {0, 150, 0, 255};
SDL_Color kiss_blue = {0, 0, 255, 255};
SDL_Color kiss_lightblue = {200, 225, 255, 255};
SDL_Color kiss_ivory = { 255,255,240 };
SDL_Color kiss_sand = { 203, 193, 172 };
SDL_Color kiss_sand_dark = { 134, 117, 83 };
FC_Font* fc_font;

unsigned int kiss_getticks(void)
{
	return SDL_GetTicks();
}

/* Can be rewritten for proportional fonts */
int kiss_maxlength(kiss_font font, int width, char *str1, char *str2)
{
	char buf[KISS_MAX_LENGTH];
	int n, i;

	n = 0;
	if (!str1 && !str2) return -1;
	kiss_string_copy(buf, KISS_MAX_LENGTH, str1, str2);
	/* Maximum length + 1 for '\0', by the rule */
	for (i = 0; buf[i]; i += kiss_utf8next(buf, i))
		if (++n * font.advance > width)
			return i + 1;
	return i + 1;
}

/* Works also with proportional fonts */
int kiss_textwidth(kiss_font font, char *str1, char *str2)
{
	char buf[KISS_MAX_LENGTH];
	int width;

	if (!str1 && !str2) return -1;
	kiss_string_copy(buf, KISS_MAX_LENGTH, str1, str2);
	TTF_SizeUTF8(font.font, buf, &width, NULL);
	return width;
}

/* Works also with proportional fonts */
int kiss_textheight(kiss_font font, char* str1, char* str2)
{
	char buf[KISS_MAX_LENGTH];
	int height;

	if (!str1 && !str2) return -1;
	kiss_string_copy(buf, KISS_MAX_LENGTH, str1, str2);
	TTF_SizeUTF8(font.font, buf, NULL, &height);
	return height;
}

int kiss_renderimage(SDL_Renderer *renderer, kiss_image image,
	int x, int y, SDL_Rect *clip)
{
	SDL_Rect dst;

	if (!renderer || !image.image) return -1;
	kiss_makerect(&dst, x, y, image.w, image.h);
	if (clip) dst.w = clip->w;
	if (clip) dst.h = clip->h;
	int success = SDL_RenderCopy(renderer, image.image, NULL, &dst);
	return 0;
}



int kiss_rendertext(SDL_Renderer *renderer, char *text, int x, int y,
	kiss_font font, SDL_Color color)
{
	//SDL_Surface *surface;
	//kiss_image image;

	if (!text || !renderer || !font.font) return -1;
	if (strnlen(text, 2) == 0)
	{
		return 0;
	}
	/**
	surface = TTF_RenderUTF8_Blended(font.font, text, color);
	image.image = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(image.image, NULL, NULL, &image.w, &image.h);
	if (surface) SDL_FreeSurface(surface);
	kiss_renderimage(renderer, image, x, y, NULL);
	SDL_DestroyTexture(image.image);
	**/
	
	FC_Draw(fc_font, renderer, x, y, text);
	return 0;
}

int kiss_fillrect(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color)
{
	if (!renderer || !rect) return -1;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, rect);
	return 0;
}

int kiss_decorate(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color,
	int edge)
{
	SDL_Rect outlinerect;
	int d, i;

	d = 2 * edge;
	if (!renderer || !rect || rect->w < d + 6 || rect->h < d + 6)
		return -1;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (i = 0; i < 2; i++) {
		kiss_makerect(&outlinerect, rect->x + edge + i,
			rect->y + edge + i, rect->w - d - i - i,
			rect->h - d - i - i);
		SDL_RenderDrawRect(renderer, &outlinerect);
	}
	return 0;
}

int kiss_image_new(kiss_image *image, char *fname, kiss_array *a,
	SDL_Renderer* renderer)
{
	char buf[KISS_MAX_LENGTH];

	if (!image || !fname) return -1;
	kiss_string_copy(buf, KISS_MAX_LENGTH, RESDIR, fname);
	if (!(image->image = IMG_LoadTexture(renderer, buf))) {
		fprintf(stderr, "Cannot load image %s\n", fname);
		return -1;
	}
	if (a) kiss_array_append(a, TEXTURE_TYPE, image->image);
	SDL_QueryTexture(image->image, NULL, NULL, &image->w, &image->h);
	image->magic = KISS_MAGIC;
	return 0;
}

int kiss_font_new(kiss_font *font, char *fname, kiss_array *a, int size)
{
	char buf[KISS_MAX_LENGTH];

	if (!font || !fname) return -1;
	kiss_string_copy(buf, KISS_MAX_LENGTH, RESDIR, fname);
	if (!(font->font = TTF_OpenFont(buf, size))) {
		fprintf(stderr, "Cannot load font %s\n", fname);
		return -1;
	}
	if (a) kiss_array_append(a, FONT_TYPE, font->font);
	font->fontheight = TTF_FontHeight(font->font);
	font->spacing = (double) kiss_spacing * font->fontheight;
	font->lineheight = font->fontheight + font->spacing;
	font->ascent = TTF_FontAscent(font->font);
	TTF_GlyphMetrics(font->font, 'W', NULL, NULL, NULL, NULL,
		&(font->advance));
	font->magic = KISS_MAGIC;
	return 0;
}

SDL_Renderer* kiss_init(char* title, kiss_array *a, int w, int h, const char * resource_folder, const char * tile_set)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Rect srect;
	int r;

	#define KISS_MAX_FILE_PATH 100


	char tile_set_folder[KISS_MAX_FILE_PATH] = "";
	snprintf(tile_set_folder, KISS_MAX_FILE_PATH+1, "%s/%s/", resource_folder, tile_set);
	if (strnlen(tile_set_folder, KISS_MAX_FILE_PATH + 1) > KISS_MAX_FILE_PATH)
	{
		printf("kiss_init: Fatal: tile map path: %s is greater than KISS_MAX_FILE_PATH (%d)", tile_set_folder, KISS_MAX_FILE_PATH);
		abort();
	}
	
	printf("kiss_init with resource path: %s", tile_set_folder);

	char kiss_normal_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_normal_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_normal",tile_set,".png");

	char kiss_font_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_font_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_font",tile_set,".ttf");

	char kiss_prelight_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_prelight_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_prelight",tile_set,".png");

	char kiss_active_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_active_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_active",tile_set,".png");

	char kiss_bar_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_bar_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_bar",tile_set,".png");

	char kiss_vslider_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_vslider_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_vslider",tile_set,".png");

	char kiss_hslider_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_hslider_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_hslider",tile_set,".png");

	char kiss_up_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_up_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_up",tile_set,".png");

	char kiss_down_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_down_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_down",tile_set,".png");

	char kiss_left_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_left_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_left",tile_set,".png");

	char kiss_right_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_right_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_right",tile_set,".png");

	char kiss_combo_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_combo_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_combo",tile_set,".png");

	char kiss_selected_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_selected_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_selected",tile_set,".png");

	char kiss_unselected_path[KISS_MAX_FILE_PATH] = "";
	snprintf(kiss_unselected_path, KISS_MAX_FILE_PATH , "%s%s_%s%s", tile_set_folder, "kiss_unselected",tile_set,".png");


	r = 0;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GetDisplayBounds(0, &srect);
	if (!a || w > srect.w || h > srect.h) {
		SDL_Quit();
		return NULL;
	}
	kiss_screen_width = w;
	kiss_screen_height = h;
	
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	kiss_array_new(a);
	window = SDL_CreateWindow(title, srect.w / 2 - w / 2,
		srect.h / 2 - h / 2, w, h, SDL_WINDOW_SHOWN);
	if (window) kiss_array_append(a, WINDOW_TYPE, window);
	renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer) kiss_array_append(a, RENDERER_TYPE, renderer);
	r += kiss_font_new(&kiss_textfont, kiss_font_path, a, kiss_textfont_size);
	r += kiss_font_new(&kiss_buttonfont, kiss_font_path, a, kiss_buttonfont_size);
	r += kiss_image_new(&kiss_normal, kiss_normal_path, a, renderer);
	r += kiss_image_new(&kiss_prelight, kiss_prelight_path, a, renderer);
	r += kiss_image_new(&kiss_active, kiss_active_path, a, renderer);
	r += kiss_image_new(&kiss_bar, kiss_bar_path, a, renderer);
	r += kiss_image_new(&kiss_vslider, kiss_vslider_path, a, renderer);
	r += kiss_image_new(&kiss_hslider, kiss_hslider_path, a, renderer);
	r += kiss_image_new(&kiss_up, kiss_up_path, a, renderer);
	r += kiss_image_new(&kiss_down, kiss_down_path, a, renderer);
	r += kiss_image_new(&kiss_left, kiss_left_path, a, renderer);
	r += kiss_image_new(&kiss_right, kiss_right_path, a, renderer);
	r += kiss_image_new(&kiss_combo, kiss_combo_path, a, renderer);
	r += kiss_image_new(&kiss_selected, kiss_selected_path, a, renderer);
	r += kiss_image_new(&kiss_unselected, kiss_unselected_path, a, renderer);

	if (r) {
		kiss_clean(a);
		return NULL;
	}
	fc_font = FC_CreateFont();
	FC_LoadFont(fc_font, renderer, kiss_font_path, kiss_textfont_size, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);
	
	return renderer;	
}

int kiss_clean(kiss_array *a)
{
	int i;

	if (!a) return -1;
	if (a->length)
		for (i = a->length - 1; i >= 0; i--) {
			if (kiss_array_id(a, i) == FONT_TYPE)
				TTF_CloseFont((TTF_Font *)
					kiss_array_data(a, i));
			else if (kiss_array_id(a, i) == TEXTURE_TYPE)
				SDL_DestroyTexture((SDL_Texture *)
					kiss_array_data(a, i));
			else if (kiss_array_id(a, i) == RENDERER_TYPE)
				SDL_DestroyRenderer((SDL_Renderer *)
					kiss_array_data(a, i));
			else if (kiss_array_id(a, i) == WINDOW_TYPE)
				SDL_DestroyWindow((SDL_Window *)
					kiss_array_data(a, i));
			else if (kiss_array_id(a, i) == ARRAY_TYPE)
				kiss_array_free((kiss_array *)
					kiss_array_data(a, i));
			else
				free (a->data[i]);
		}
	a->length = 0;
	kiss_array_free(a);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}


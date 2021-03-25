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


int determine_text_render_position(SDL_Rect* parent_rect, h_alignment ha, v_alignment va, int* out_x, int* out_y, int text_width, int text_height)
{
	if (parent_rect == NULL)
	{
		printf("kiss_widgets::determine_text_render_position null input argument");
		abort();
	}

	*out_x = parent_rect->x;
	*out_y = parent_rect->y;
	//determine horizontal alignment
	switch (ha)
	{
	case HA_CENTER:
	{
		//set x position to middle of parent window
		*out_x = parent_rect->x + (parent_rect->w / 2) - (text_width / 2);
		break;
	}
	case HA_COLUMN:
	{
		
	}
	case HA_NONE:
	{
		
	}
	default:
	{
		//offset ui rect by the position of parent window
		*out_x = parent_rect->x + text_width / 10;
		break;
	}
	}
	//determine vertical alignment
	switch (va)
	{
	case VA_CENTER:
	{
		//set y position to middle of parent window
		*out_y = parent_rect->y + (parent_rect->h / 2) - (text_height / 1.5);
		break;
	}
	default:
	{
		//offset ui rect by the position of parent window
		*out_y = parent_rect->y + text_height / 10;
		break;
	}
	}
	return 0;

}

int determine_render_position(SDL_Rect *ui_rect, SDL_Rect *parent_rect, SDL_Rect *return_rect, h_alignment ha, v_alignment va, scale_to_parent_width sp_w, int column, int row)
{
	if (ui_rect == NULL || parent_rect == NULL || return_rect == NULL)
	{
		printf("kiss_widgets::determine_render_position null input argument");
		abort();
	}

	return_rect->h = ui_rect->h;
	return_rect->x = ui_rect->x;
	return_rect->w = ui_rect->w;
	return_rect->y = ui_rect->y;

	//optional scaling of ui element to its parent window width
	switch (sp_w)
	{
		case SP_FILL:
		{
			return_rect->w = parent_rect->w;
			break;
		}
		case SP_FILL_WITH_BUFFER:
		{
			return_rect->w = parent_rect->w - 20;
			break;
		}
		case SP_HALF:
		{
			return_rect->w = (parent_rect->w / 2) - 20;
			break;
		}
		case SP_THIRD:
		{
			return_rect->w = (parent_rect->w / 3) - 20;
			break;
		}
		case SP_FOURTH:
		{
			return_rect->w = (parent_rect->w / 4) - 20;
			break;
		}
		case SP_NONE:
		{
			return_rect->w = ui_rect->w;
			break;
		}
		default:
		{
			return_rect->w = ui_rect->w;
			break;
		}
	}

	
	//determine horizontal alignment
	switch (ha)
	{
		case HA_CENTER:
		{
			//set x position to middle of parent window
			return_rect->x = parent_rect->x + (parent_rect->w / 2) - (return_rect->w/2);
			break;
		}
		case HA_COLUMN:
		{
			return_rect->x = parent_rect->x + 15 + (((return_rect->w) + 10) * column);
			break;
		}
		case HA_NONE:
		{
			//offset ui rect by the position of parent window
			return_rect->x = ui_rect->x + parent_rect->x;
			break;
		}
		default:
		{
			//offset ui rect by the position of parent window
			return_rect->x = ui_rect->x + parent_rect->x;
			break;
		}
	}
	//determine vertical alignment
	switch (va)
	{
		case VA_CENTER:
		{
			//set y position to middle of parent window
			return_rect->y = parent_rect->y + (parent_rect->h / 2) - (return_rect->h / 2);
			break;
		}
		case VA_ROW:
		{
			//use the size of the UI element as the row size + a bit of offset
			return_rect->y = parent_rect->y + 10 + (((return_rect->h)+5) * row );
			break;
		}
		case VA_NONE:
		{
			//offset ui rect by the position of parent window
			return_rect->y =  parent_rect->y + ui_rect->y;
			break;
		}
		default:
		{
			//offset ui rect by the position of parent window
			return_rect->y = parent_rect->y + ui_rect->y;
			break;
		}
	}
	return 0;
	
}

int kiss_window_new(kiss_window *window, kiss_window *wdw, int decorate,
	int x, int y, int w, int h)
{
	if (!window) return -1;
	window->bg = kiss_ivory;
	kiss_makerect(&window->rect, x, y, w, h);
	window->decorate = decorate;
	window->visible = 0;
	window->focus = 1;
	window->wdw = wdw;
	return 0;
}

int kiss_window_event(kiss_window *window, SDL_Event *event, int *draw)
{
	if (!window || !window->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!window->focus && (!window->wdw ||
		(window->wdw && !window->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&window->rect))
		return 1;
	else if (event->type == SDL_MOUSEMOTION &&
		kiss_pointinrect(event->motion.x, event->motion.y,
			&window->rect))
	{
		return 1;
	}

	return 0;
}

int kiss_window_draw(kiss_window *window, SDL_Renderer *renderer)
{
	if (window && window->wdw) window->visible = window->wdw->visible;
	if (!window || !window->visible || !renderer) return 0;
	kiss_fillrect(renderer, &window->rect, window->bg);
	if (window->decorate)
		kiss_decorate(renderer, &window->rect, kiss_sand_dark, kiss_edge);
	return 1;
}

int kiss_label_new(kiss_label *label, kiss_window *wdw, char *text,
	int x, int y)
{
	if (!label || !text) return -1;
	if (label->font.magic != KISS_MAGIC) label->font = kiss_textfont;
	label->textcolor = kiss_black;
	kiss_makerect(&label->rect, x, y, 150, 50);
	kiss_string_copy(label->text, KISS_MAX_LABEL, text, NULL);
	label->visible = 0;
	label->wdw = wdw;
	return 0;
}

int kiss_label_draw(kiss_label *label, SDL_Renderer *renderer)
{
	char buf[KISS_MAX_LABEL], *p;
	int len, y, x;

	if (label && label->wdw)
	{
		label->visible = label->wdw->visible;
		//pass by ref - sets button->r_rect
		(void)determine_render_position(&label->rect, &label->wdw->rect, &label->r_rect, label->h_align, label->v_align, label->parent_scale, label->column, label->row);
		//pass by ref - sets textx and texty
		(void)determine_text_render_position(&label->r_rect, label->txt_h_align, VA_NONE, &x, &y, kiss_textwidth(label->font, label->text, NULL), label->r_rect.h);
	}
	if (!label || !label->visible || !renderer)
	{
		return 0;
	}
	y = label->r_rect.y + label->font.spacing / 2;
	len = strlen(label->text);
	if (len > KISS_MAX_LABEL - 2)
	{
		label->text[len - 1] = '\n';
	}
	else
	{
		strcat(label->text, "\n");
	}
	for (p = label->text; *p; p = strchr(p, '\n') + 1)
	{
		kiss_string_copy(buf, strcspn(p, "\n") + 1, p, NULL);
		kiss_rendertext(renderer, buf, x, y,
			label->font, label->textcolor);
		y += label->font.lineheight;
	}
	
	label->text[len] = 0;
	return 1;
}

int kiss_button_new(kiss_button *button, kiss_window *wdw, char *text,
	int x, int y)
{
	if (!button || !text) return -1;
	if (button->font.magic != KISS_MAGIC) button->font = kiss_buttonfont;
	if (button->normalimg.magic != KISS_MAGIC)
		button->normalimg = kiss_normal;
	if (button->activeimg.magic != KISS_MAGIC)
		button->activeimg = kiss_active;
	if (button->prelightimg.magic != KISS_MAGIC)
		button->prelightimg = kiss_prelight;
	kiss_makerect(&button->rect, x, y, button->normalimg.w,
		button->normalimg.h);
	button->textcolor = kiss_white;
	kiss_string_copy(button->text, KISS_MAX_LENGTH, text, NULL);
	button->textx = x + button->normalimg.w / 2 -
		kiss_textwidth(button->font, text, NULL) / 2;
	button->texty = y + button->normalimg.h / 2 -
		button->font.fontheight / 2;
	button->active = 0;
	button->prelight = 0;
	button->visible = 0;
	button->focus = 0;
	button->wdw = wdw;
	button->v_align = HA_NONE;
	button->h_align = VA_NONE;
	button->parent_scale = SP_NONE;
	return 0;
}


int kiss_button_new_uc(kiss_button* button, kiss_window* wdw, char* text,
	int x, int y, int w, int h)
{
	if (!button || !text) return -1;
	if (button->font.magic != KISS_MAGIC) button->font = kiss_buttonfont;
	if (button->normalimg.magic != KISS_MAGIC)
		button->normalimg = kiss_normal;
	if (button->activeimg.magic != KISS_MAGIC)
		button->activeimg = kiss_active;
	if (button->prelightimg.magic != KISS_MAGIC)
		button->prelightimg = kiss_prelight;
	kiss_makerect(&button->rect, x, y, w,
		h);
	kiss_makerect(&button->r_rect, x, y, w, h);
	button->textcolor = kiss_black;
	kiss_string_copy(button->text, KISS_MAX_LENGTH, text, NULL);
	button->text_width = kiss_textwidth(button->font, text, NULL);
	button->textx = x + button->normalimg.w / 2 -
		button->text_width / 2;
	//button->textx = x + button->rect.w / ;
	button->texty = y + button->normalimg.h / 2 -
		button->font.fontheight / 2;
	button->active = 0;
	button->prelight = 0;
	button->visible = 0;
	button->focus = 0;
	button->wdw = wdw;
	button->v_align = HA_NONE;
	button->h_align = VA_NONE;
	button->parent_scale = SP_NONE;
	button->row = 0;
	button->column = 0;
	button->txt_v_align = VA_CENTER;
	button->txt_h_align = HA_CENTER;
	return 0;
}


int kiss_button_event(kiss_button *button, SDL_Event *event, int *draw)
{
	if (!button || !button->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!button->focus && (!button->wdw ||
		(button->wdw && !button->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&button->r_rect)) {
		button->active = 1;
		*draw = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP &&
		kiss_pointinrect(event->button.x, event->button.y,
		&button->r_rect) && button->active) {
		button->active = 0;
		*draw = 1;
		return 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		kiss_pointinrect(event->motion.x, event->motion.y,
		&button->r_rect)) {
		button->prelight = 1;
		*draw = 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		!kiss_pointinrect(event->motion.x, event->motion.y,
		&button->r_rect)) {
		button->prelight = 0;
		*draw = 1;
		if (button->active) {
			button->active = 0;
			return 1;
		}
	}
	return 0;
}

int kiss_button_draw(kiss_button *button, SDL_Renderer *renderer)
{

	if (button && button->wdw)
	{
		button->visible = button->wdw->visible;
		//pass by ref - sets button->r_rect
		(void) determine_render_position(&button->rect, &button->wdw->rect, &button->r_rect, button->h_align, button->v_align, button->parent_scale, button->column, button->row);
		//pass by ref - sets textx and texty
		(void) determine_text_render_position(&button->r_rect, button->txt_h_align, button->txt_v_align, &button->textx, &button->texty, button->text_width, button->font.fontheight);


	}
	if (!button || !button->visible || !renderer)
	{
		return 0;
	}
	 
	if (button->active)
	{
		kiss_renderimage(renderer, button->activeimg, button->r_rect.x,
			button->r_rect.y, &button->r_rect);
	}
	else if (button->prelight && !button->active)
	{
		kiss_renderimage(renderer, button->prelightimg,
			button->r_rect.x, button->r_rect.y, &button->r_rect);
	}
	else
	{
		kiss_renderimage(renderer, button->normalimg, button->r_rect.x,
			button->r_rect.y, &button->r_rect);
	}
	kiss_rendertext(renderer, button->text, button->textx, button->texty,
		button->font, button->textcolor);
	//kiss_rendertext(renderer, button->text, button->r_rect.x, button->r_rect.y,
	//	button->font, button->textcolor);
	return 1;
}

int kiss_selectbutton_new(kiss_selectbutton *selectbutton, kiss_window *wdw,
	int x, int y)
{
	if (!selectbutton) return -1;
	if (selectbutton->selectedimg.magic != KISS_MAGIC)
		selectbutton->selectedimg = kiss_selected;
	if (selectbutton->unselectedimg.magic != KISS_MAGIC)
		selectbutton->unselectedimg = kiss_unselected;
	kiss_makerect(&selectbutton->rect, x, y, selectbutton->selectedimg.w,
		selectbutton->selectedimg.h);
	selectbutton->selected = 0;
	selectbutton->focus = 0;
	selectbutton->wdw = wdw;
	return 0;
}

int kiss_selectbutton_event(kiss_selectbutton *selectbutton,
	SDL_Event *event, int *draw)
{
	if (!selectbutton || !selectbutton->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!selectbutton->focus && (!selectbutton->wdw ||
		(selectbutton->wdw && !selectbutton->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&selectbutton->rect)) {
		selectbutton->selected ^= 1;
		*draw = 1;
		return 1;
	}
	return 0;
}

int kiss_selectbutton_draw(kiss_selectbutton *selectbutton,
	SDL_Renderer *renderer)
{
	if (selectbutton && selectbutton->wdw)
		selectbutton->visible = selectbutton->wdw->visible;
	if (!selectbutton || !selectbutton->visible || !renderer) return 0;
	if (selectbutton->selected)
		kiss_renderimage(renderer, selectbutton->selectedimg,
			selectbutton->rect.x, selectbutton->rect.y, NULL);
	else
		kiss_renderimage(renderer, selectbutton->unselectedimg,
			selectbutton->rect.x, selectbutton->rect.y, NULL);
	return 1;
}

int kiss_vscrollbar_new(kiss_vscrollbar *vscrollbar, kiss_window *wdw,
	int x, int y, int h)
{
	if (!vscrollbar) return -1;
	if (vscrollbar->up.magic != KISS_MAGIC)
		vscrollbar->up = kiss_up;
	if (vscrollbar->down.magic != KISS_MAGIC)
		vscrollbar->down = kiss_down;
	if (vscrollbar->vslider.magic != KISS_MAGIC)
		vscrollbar->vslider = kiss_vslider;
	if (vscrollbar->up.h + vscrollbar->down.h + 2 * kiss_edge +
		2 * kiss_slider_padding + vscrollbar->vslider.h > h)
		return -1;
	kiss_makerect(&vscrollbar->uprect, x, y + kiss_edge,
		vscrollbar->up.w, vscrollbar->up.h + kiss_slider_padding);
	kiss_makerect(&vscrollbar->downrect, x, y + h - vscrollbar->down.h -
		kiss_slider_padding - kiss_edge, vscrollbar->down.w,
		vscrollbar->down.h + kiss_slider_padding);
	kiss_makerect(&vscrollbar->sliderrect, x, y + vscrollbar->uprect.h +
		kiss_edge, vscrollbar->vslider.w, vscrollbar->vslider.h);
	vscrollbar->maxpos = h - 2 * kiss_slider_padding - 2 * kiss_edge -
		vscrollbar->up.h - vscrollbar->down.h -
		vscrollbar->vslider.h;
	vscrollbar->fraction = 0.;
	vscrollbar->step = 0.1;
	vscrollbar->upclicked = 0;
	vscrollbar->downclicked = 0;
	vscrollbar->sliderclicked = 0;
	vscrollbar->lasttick = 0;
	vscrollbar->visible = 0;
	vscrollbar->focus = 0;
	vscrollbar->wdw = wdw;
	return 0;
}

static void vnewpos(kiss_vscrollbar *vscrollbar, double step, int *draw)
{
	*draw = 1;
	vscrollbar->fraction += step;
	vscrollbar->lasttick = kiss_getticks();
	if (vscrollbar->fraction < -0.000001) vscrollbar->fraction = 0.;
	if (vscrollbar->fraction > 0.999999) vscrollbar->fraction = 1.;
	vscrollbar->sliderrect.y = vscrollbar->uprect.y +
		vscrollbar->uprect.h + (int) (vscrollbar->fraction *
		vscrollbar->maxpos + 0.5);
	if (vscrollbar->fraction > 0.000001 &&
		vscrollbar->fraction < 0.999999)
		return;
	vscrollbar->upclicked = 0;
	vscrollbar->downclicked = 0;
}

int kiss_vscrollbar_event(kiss_vscrollbar *vscrollbar, SDL_Event *event,
	int *draw)
{
	if (!vscrollbar || !vscrollbar->visible) return 0;
	if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) {
		vscrollbar->upclicked = 0;
		vscrollbar->downclicked = 0;
		vscrollbar->lasttick = 0;
	} else if (vscrollbar->upclicked && kiss_getticks() >
		vscrollbar->lasttick + kiss_click_interval) {
		vnewpos(vscrollbar, -vscrollbar->step, draw);
		return 1;
	} else if (vscrollbar->downclicked && kiss_getticks() >
		vscrollbar->lasttick + kiss_click_interval) {
		vnewpos(vscrollbar, vscrollbar->step, draw);
		return 1;
	}
	if (!event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!vscrollbar->focus && (!vscrollbar->wdw ||
		(vscrollbar->wdw && !vscrollbar->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&vscrollbar->uprect) && vscrollbar->step > 0.000001) {
		if (vscrollbar->fraction > 0.000001) {
			vscrollbar->upclicked = 1;
			if (vscrollbar->wdw) vscrollbar->wdw->focus = 0;
			vscrollbar->focus = 1;
		}
		vscrollbar->lasttick = kiss_getticks() -
			kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&vscrollbar->downrect) && vscrollbar->step > 0.000001) {
		if (vscrollbar->fraction < 0.999999) {
			vscrollbar->downclicked = 1;
			if (vscrollbar->wdw) vscrollbar->wdw->focus = 0;
			vscrollbar->focus = 1;
		}
		vscrollbar->lasttick = kiss_getticks() -
			kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&vscrollbar->sliderrect) && vscrollbar->step > 0.000001) {
		if (vscrollbar->wdw) vscrollbar->wdw->focus = 0;
		vscrollbar->focus = 1;
		vscrollbar->sliderclicked = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		vscrollbar->upclicked = 0;
		vscrollbar->downclicked = 0;
		vscrollbar->lasttick = 0;
		if (vscrollbar->wdw) vscrollbar->wdw->focus = 1;
		vscrollbar->focus = 0;
		vscrollbar->sliderclicked = 0;
	} else if (event->type == SDL_MOUSEMOTION &&
		(event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
		vscrollbar->sliderclicked) {
		vnewpos(vscrollbar, 1. * event->motion.yrel /
			vscrollbar->maxpos, draw);
		return 1;
	}
	return 0;
}

int kiss_vscrollbar_draw(kiss_vscrollbar *vscrollbar, SDL_Renderer *renderer)
{
	if (vscrollbar && vscrollbar->wdw)
		vscrollbar->visible = vscrollbar->wdw->visible;
	if (!vscrollbar || !vscrollbar->visible || !renderer) return 0;
	vscrollbar->sliderrect.y = vscrollbar->uprect.y +
		vscrollbar->uprect.h + (int) (vscrollbar->fraction *
		vscrollbar->maxpos);
	kiss_renderimage(renderer, vscrollbar->up, vscrollbar->uprect.x,
		vscrollbar->uprect.y, NULL);
	kiss_renderimage(renderer, vscrollbar->down, vscrollbar->downrect.x,
		vscrollbar->downrect.y + kiss_slider_padding, NULL);
	kiss_renderimage(renderer, vscrollbar->vslider,
		vscrollbar->sliderrect.x, vscrollbar->sliderrect.y, NULL);
	return 1;
}

int kiss_hscrollbar_new(kiss_hscrollbar *hscrollbar, kiss_window *wdw,
	int x, int y, int w)
{
	if (!hscrollbar) return -1;
	if (hscrollbar->left.magic != KISS_MAGIC)
		hscrollbar->left = kiss_left;
	if (hscrollbar->right.magic != KISS_MAGIC)
		hscrollbar->right = kiss_right;
	if (hscrollbar->hslider.magic != KISS_MAGIC)
		hscrollbar->hslider = kiss_hslider;
	if (hscrollbar->left.w + hscrollbar->right.w + 2 * kiss_edge +
		2 * kiss_slider_padding + hscrollbar->hslider.w > w)
		return -1;
	kiss_makerect(&hscrollbar->leftrect, x + kiss_edge, y,
		hscrollbar->left.w + kiss_slider_padding,
		hscrollbar->left.h);
	kiss_makerect(&hscrollbar->rightrect, x + w - hscrollbar->right.w -
		kiss_slider_padding - kiss_edge, y,
		hscrollbar->right.w + kiss_slider_padding,
		hscrollbar->right.h);
	kiss_makerect(&hscrollbar->sliderrect, x + hscrollbar->leftrect.w +
		kiss_edge, y, hscrollbar->hslider.w, hscrollbar->hslider.h);
	hscrollbar->maxpos = w - 2 * kiss_slider_padding - 2 * kiss_edge -
		hscrollbar->left.w - hscrollbar->right.w - 
		hscrollbar->hslider.w;
	hscrollbar->fraction = 0.;
	hscrollbar->step = 0.1;
	hscrollbar->leftclicked = 0;
	hscrollbar->rightclicked = 0;
	hscrollbar->sliderclicked = 0;
	hscrollbar->lasttick = 0;
	hscrollbar->visible = 0;
	hscrollbar->focus = 0;
	hscrollbar->wdw = wdw;
	return 0;
}

static void hnewpos(kiss_hscrollbar *hscrollbar, double step, int *draw)
{
	*draw = 1;
	hscrollbar->fraction += step;
	hscrollbar->lasttick = kiss_getticks();
	if (hscrollbar->fraction < -0.000001) hscrollbar->fraction = 0.;
	if (hscrollbar->fraction > 0.999999) hscrollbar->fraction = 1.;
	hscrollbar->sliderrect.x = hscrollbar->leftrect.x +
		hscrollbar->leftrect.w + (int) (hscrollbar->fraction *
		hscrollbar->maxpos + 0.5);
	if (hscrollbar->fraction > 0.000001 &&
		hscrollbar->fraction < 0.999999)
		return;
	hscrollbar->leftclicked = 0;
	hscrollbar->rightclicked = 0;
}

int kiss_hscrollbar_event(kiss_hscrollbar *hscrollbar, SDL_Event *event,
	int *draw)
{
	if (!hscrollbar || !hscrollbar->visible) return 0;
	if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) {
		hscrollbar->leftclicked = 0;
		hscrollbar->rightclicked = 0;
		hscrollbar->lasttick = 0;
	} else if (hscrollbar->leftclicked && kiss_getticks() >
		hscrollbar->lasttick + kiss_click_interval) {
		hnewpos(hscrollbar, -hscrollbar->step, draw);
		return 1;
	} else if (hscrollbar->rightclicked && kiss_getticks() >
		hscrollbar->lasttick + kiss_click_interval) {
		hnewpos(hscrollbar, hscrollbar->step, draw);
		return 1;
	}
	if (!event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!hscrollbar->focus && (!hscrollbar->wdw ||
		(hscrollbar->wdw && !hscrollbar->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&hscrollbar->leftrect)) {
		if (hscrollbar->fraction > 0.000001) {
			hscrollbar->leftclicked = 1;
			if (hscrollbar->wdw) hscrollbar->wdw->focus = 0;
			hscrollbar->focus = 1;
		}
		hscrollbar->lasttick = kiss_getticks() -
			kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&hscrollbar->rightrect) && hscrollbar->step > 0.000001) {
		if (hscrollbar->fraction < 0.999999) {
			hscrollbar->rightclicked = 1;
			if (hscrollbar->wdw) hscrollbar->wdw->focus = 0;
			hscrollbar->focus = 1;
		}
		hscrollbar->lasttick = kiss_getticks() -
			kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&hscrollbar->sliderrect) && hscrollbar->step > 0.000001) {
		if (hscrollbar->wdw) hscrollbar->wdw->focus = 0;
		hscrollbar->focus = 1;
		hscrollbar->sliderclicked = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		hscrollbar->leftclicked = 0;
		hscrollbar->rightclicked = 0;
		hscrollbar->lasttick = 0;
		if (hscrollbar->wdw) hscrollbar->wdw->focus = 1;
		hscrollbar->focus = 0;
		hscrollbar->sliderclicked = 0;
	} else if (event->type == SDL_MOUSEMOTION &&
		(event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
		hscrollbar->sliderclicked) {
		hnewpos(hscrollbar, 1. * event->motion.xrel /
			hscrollbar->maxpos, draw);
		return 1;
	}
	return 0;
}

int kiss_hscrollbar_draw(kiss_hscrollbar *hscrollbar, SDL_Renderer *renderer)
{
	if (hscrollbar && hscrollbar->wdw)
		hscrollbar->visible = hscrollbar->wdw->visible;
	if (!hscrollbar || !hscrollbar->visible || !renderer) return 0;
	hscrollbar->sliderrect.x = hscrollbar->leftrect.x +
		hscrollbar->leftrect.w + (int) (hscrollbar->fraction *
		hscrollbar->maxpos);
	kiss_renderimage(renderer, hscrollbar->left, hscrollbar->leftrect.x,
		hscrollbar->leftrect.y, NULL);
	kiss_renderimage(renderer, hscrollbar->right,
		hscrollbar->rightrect.x + kiss_slider_padding,
		hscrollbar->rightrect.y, NULL);
	kiss_renderimage(renderer, hscrollbar->hslider,
		hscrollbar->sliderrect.x, hscrollbar->sliderrect.y, NULL);
	return 1;
}

int kiss_progressbar_new(kiss_progressbar *progressbar, kiss_window *wdw,
	int x, int y, int w)
{
	if (!progressbar || w < 2 * kiss_border + 1) return -1;
	if (progressbar->bar.magic != KISS_MAGIC)
		progressbar->bar = kiss_bar;
	progressbar->bg = kiss_white;
	kiss_makerect(&progressbar->rect, x, y, w,
		progressbar->bar.h + 2 * kiss_border);
	kiss_makerect(&progressbar->barrect, x + kiss_border,
		y + kiss_border, 0, progressbar->bar.h);
	progressbar->width = w - 2 * kiss_border;
	progressbar->fraction = 0.;
	progressbar->step = 0.02;
	progressbar->lasttick = 0;
	progressbar->run = 0;
	progressbar->visible = 0;
	progressbar->wdw = wdw;
	return 0;
}

int kiss_progressbar_event(kiss_progressbar *progressbar, SDL_Event *event,
	int *draw)
{
	if (!progressbar || !progressbar->visible) return 0;
	if (progressbar->run && kiss_getticks() > progressbar->lasttick +
		kiss_progress_interval) {
		progressbar->fraction += progressbar->step;
		if (progressbar->fraction > 0.999999) {
			progressbar->run = 0;
			progressbar->fraction = 1.;
		}
		progressbar->lasttick = kiss_getticks();
		*draw = 1;
	}
	return 1;
}

int kiss_progressbar_draw(kiss_progressbar *progressbar,
	SDL_Renderer *renderer)
{
	SDL_Rect clip;

	if (progressbar && progressbar->wdw)
		progressbar->visible = progressbar->wdw->visible;
	if (!progressbar || !progressbar->visible || !renderer)
		return 0;
	kiss_fillrect(renderer, &progressbar->rect, progressbar->bg);
	kiss_decorate(renderer, &progressbar->rect, kiss_sand_dark, kiss_edge);
	progressbar->barrect.w = (int) (progressbar->width *
		progressbar->fraction + 0.5);
	kiss_makerect(&clip, 0, 0, progressbar->barrect.w,
		progressbar->barrect.h);
	kiss_renderimage(renderer, progressbar->bar, progressbar->barrect.x,
		progressbar->barrect.y, &clip);
	return 1;
}

int kiss_entry_new(kiss_entry *entry, kiss_window *wdw, int decorate,
	char *text, int x, int y, int w)
{
	if (!entry || !text) return -1;
	if (entry->font.magic != KISS_MAGIC) entry->font = kiss_textfont;
	if (w < 2 * kiss_border + entry->font.advance) return -1;
	entry->bg = kiss_white;
	entry->normalcolor = kiss_black;
	entry->activecolor = kiss_blue;
	entry->textwidth = w - 2 * kiss_border;
	kiss_string_copy(entry->text, kiss_maxlength(entry->font,
		entry->textwidth, text, NULL), text, NULL);
	kiss_makerect(&entry->rect, x, y, w, entry->font.fontheight +
		2 * kiss_border);
	entry->decorate = decorate;
	entry->textx = x + kiss_border;
	entry->texty = y + kiss_border;
	entry->active = 0;
	entry->visible = 0;
	entry->focus = 0;
	entry->wdw = wdw;
	entry->lower_bound = 0;
	entry->upper_bound = 0;
	entry->num_val = 0;
	entry->entry_type = TE_NONE;
	return 0;
}

int kiss_entry_event(kiss_entry *entry, SDL_Event *event, int *draw)
{
	if (!entry || !entry->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!entry->focus && (!entry->wdw ||
		(entry->wdw && !entry->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN && !entry->active &&
		kiss_pointinrect(event->button.x, event->button.y,
		&entry->r_rect)) {
		entry->active = 1;
		SDL_StartTextInput();
		if (entry->wdw) entry->wdw->focus = 0;
		entry->focus = 1;
		*draw = 1;
		return 1;
	}
	else if ((event->type == SDL_KEYDOWN && entry->active &&
		event->key.keysym.scancode == SDL_SCANCODE_RETURN)
	|| (event->type == SDL_MOUSEBUTTONDOWN && entry->active && !kiss_pointinrect(event->button.x, event->button.y, &entry->r_rect)))
	{
		entry->active = 0;
		SDL_StopTextInput();
		if (entry->wdw) entry->wdw->focus = 1;
		entry->focus = 0;
		*draw = 1;
		switch (entry->entry_type)
		{
			case TE_NONE:
			{
				break;
			}
			case TE_FLOAT:
			{
				double temp = atof(entry->text);
				if (temp >= entry->upper_bound)
				{
					temp = entry->upper_bound;
				}
				else if (temp <= entry->lower_bound)
				{
					temp = entry->lower_bound;
				}
				entry->num_val = temp;
				snprintf(entry->text, KISS_MAX_LENGTH, "%g", entry->num_val);
				break;
			}
			case TE_INT:
			{
				int temp = (int)round(atoi(entry->text));
			
				if (temp >= entry->upper_bound)
				{
					temp = entry->upper_bound;
				}
				else if (temp <= entry->lower_bound)
				{
					temp = entry->lower_bound;
				}
				entry->num_val = (int) temp;
				snprintf(entry->text, KISS_MAX_LENGTH, "%d", (int)entry->num_val);
				break;
			}
			default:
			{
				printf("kiss_entry_handle bad entry->entry_type\n");
				abort();
			}
		}
		return 2; //ret 2 if "done"
	} else if (event->type == SDL_TEXTINPUT && entry->active) {
		if (kiss_textwidth(entry->font, entry->text,
			event->text.text) < entry->textwidth &&
			strlen(entry->text) + strlen(event->text.text) <
			KISS_MAX_LENGTH)
			strcat(entry->text, event->text.text);
		*draw = 1;
		return 1;
	} else if (event->type == SDL_TEXTEDITING && entry->active) {
		//todo
		return 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
		kiss_backspace(entry->text);
		*draw = 1;
		return 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		(event->key.keysym.mod & KMOD_CTRL) &&
		event->key.keysym.scancode == SDL_SCANCODE_U) {
		strcpy(entry->text, "");
		*draw = 1;
		return 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN && entry->active &&
		kiss_pointinrect(event->button.x, event->button.y,
		&entry->r_rect)) {
		strcpy(entry->text, "");
		*draw = 1;
		return 1;
	}
	
	return 0;
}

int kiss_entry_draw(kiss_entry *entry, SDL_Renderer *renderer)
{
	SDL_Color color;

	if (entry && entry->wdw)
	{
		entry->visible = entry->wdw->visible;
		(void)determine_render_position(&entry->rect, &entry->wdw->rect, &entry->r_rect, entry->h_align, entry->v_align, entry->parent_scale, entry->column, entry->row);
		//pass by ref - sets textx and texty
		(void)determine_text_render_position(&entry->r_rect, entry->txt_h_align, entry->txt_v_align, &entry->textx, &entry->texty, entry->textwidth, entry->font.fontheight);
	}

	if (!entry || !entry->visible || !renderer)
	{
		return 0;
	}
	kiss_fillrect(renderer, &entry->r_rect, entry->bg);
	color = kiss_sand;
	if (entry->active)
	{
		color = kiss_green;
	}
	if (entry->decorate)
	{
		kiss_decorate(renderer, &entry->r_rect, color, kiss_edge);
	}
	color = entry->normalcolor;
	if (entry->active)
	{
		color = entry->activecolor;
	}
	kiss_rendertext(renderer, entry->text, entry->textx, entry->texty,
		entry->font, color);
	return 1;
}

int kiss_textbox_new(kiss_textbox *textbox, kiss_window *wdw, int decorate,
	kiss_array *a, int x, int y, int w, int h)
{
	if (!textbox || !a) return -1;
	if (textbox->font.magic != KISS_MAGIC) textbox->font = kiss_textfont;
	if (h - 2 * kiss_border < textbox->font.lineheight) return -1;
	textbox->bg = kiss_white;
	textbox->textcolor = kiss_black;
	textbox->hlcolor = kiss_lightblue;
	kiss_makerect(&textbox->rect, x, y, w, h);
	kiss_makerect(&textbox->textrect, x + kiss_border,
		y + kiss_border, w - 2 * kiss_border, h - 2 * kiss_border);
	textbox->decorate = decorate;
	textbox->array = a;
	textbox->firstline = 0;
	textbox->maxlines = (h - 2 * kiss_border) / textbox->font.lineheight;
	textbox->textwidth = w - 2 * kiss_border;
	textbox->highlightline = -1;
	textbox->selectedline = -1;
	textbox->visible = 0;
	textbox->focus = 0;
	textbox->wdw = wdw;
	return 0;
}

static int textbox_numoflines(kiss_textbox *textbox)
{
	int numoflines;

	numoflines = textbox->maxlines;
	if (textbox->array->length - textbox->firstline < textbox->maxlines)
		numoflines = textbox->array->length - textbox->firstline;
	return numoflines;
}

int kiss_textbox_event(kiss_textbox *textbox, SDL_Event *event, int *draw)
{
	int texty, textmaxy, numoflines;

	if (!textbox || !textbox->visible || !event || !textbox->array ||
		!textbox->array->length)
		return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!textbox->focus && (!textbox->wdw ||
		(textbox->wdw && !textbox->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&textbox->textrect)) {
		numoflines = textbox_numoflines(textbox);
		texty = event->button.y - textbox->textrect.y;
		textmaxy = numoflines * textbox->font.lineheight;
		if (texty < textmaxy) {
			textbox->selectedline =
				texty / textbox->font.lineheight;
			return 1;
		}
	} else if (event->type == SDL_MOUSEMOTION &&
		kiss_pointinrect(event->motion.x, event->motion.y,
		&textbox->textrect)) {
		numoflines = textbox_numoflines(textbox);
		texty = event->motion.y - textbox->textrect.y;
		textmaxy = numoflines * textbox->font.lineheight;
		textbox->highlightline = -1;
		if (texty < textmaxy)
			textbox->highlightline =
				texty / textbox->font.lineheight;
		*draw = 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		!kiss_pointinrect(event->motion.x, event->motion.y,
		&textbox->textrect)) {
		textbox->highlightline = -1;
		*draw = 1;
	}
	return 0;
}

int kiss_textbox_draw(kiss_textbox *textbox, SDL_Renderer *renderer)
{
	SDL_Rect highlightrect;
	char buf[KISS_MAX_LENGTH];
	int numoflines, i;

	if (textbox && textbox->wdw)
		textbox->visible = textbox->wdw->visible;
	if (!textbox || !textbox->visible || !renderer) return 0;
	kiss_fillrect(renderer, &textbox->rect, textbox->bg);
	if (textbox->decorate)
		kiss_decorate(renderer, &textbox->rect, kiss_sand,
			kiss_edge);
	if (textbox->highlightline >= 0) {
		kiss_makerect(&highlightrect, textbox->textrect.x,
			textbox->textrect.y +
			textbox->highlightline * textbox->font.lineheight,
			textbox->textrect.w, textbox->font.lineheight);
		kiss_fillrect(renderer, &highlightrect, textbox->hlcolor);
	}
	if (!textbox->array || !textbox->array->length) return 0;
	numoflines = textbox_numoflines(textbox);
	for (i = 0; i < numoflines; i++) {
		kiss_string_copy(buf, kiss_maxlength(textbox->font,
			textbox->textwidth,
			(char *) kiss_array_data(textbox->array,
			textbox->firstline + i), NULL),
			(char *) kiss_array_data(textbox->array,
			textbox->firstline + i), NULL);
		kiss_rendertext(renderer, buf, textbox->textrect.x,
			textbox->textrect.y + i * textbox->font.lineheight +
			textbox->font.spacing / 2, textbox->font,
			textbox->textcolor);
	}
	return 1;
}

int kiss_combobox_new(kiss_combobox *combobox, kiss_window *wdw,
	char *text, kiss_array *a, int x, int y, int w, int h)
{
	if (!combobox || !a || !text) return -1;
	if (combobox->combo.magic != KISS_MAGIC)
		combobox->combo = kiss_combo;
	kiss_entry_new(&combobox->entry, wdw, 1, text, x, y, w);
	strcpy(combobox->text, combobox->entry.text);
	kiss_window_new(&combobox->window, NULL, 0, x,
		y + combobox->entry.rect.h, w +
		combobox->vscrollbar.up.w, h);
	if (kiss_textbox_new(&combobox->textbox, &combobox->window, 1,
		a, x, y + combobox->entry.rect.h, w, h) == -1)
		return -1;
	if (kiss_vscrollbar_new(&combobox->vscrollbar, &combobox->window,
		x + combobox->textbox.rect.w, combobox->textbox.rect.y,
		combobox->textbox.rect.h) == -1)
		return -1;
	combobox->visible = 0;
	combobox->wdw = wdw;
	combobox->vscrollbar.step = 0.;
	if (combobox->textbox.array->length - combobox->textbox.maxlines > 0)
		combobox->vscrollbar.step = 1. /
			(combobox->textbox.array->length -
			combobox->textbox.maxlines);
	return 0;
}

int kiss_combobox_event(kiss_combobox *combobox, SDL_Event *event, int *draw)
{
	int firstline, index;

	if (!combobox || !combobox->visible) return 0;
	if (kiss_vscrollbar_event(&combobox->vscrollbar, event,
		draw) && combobox->textbox.array->length -
		combobox->textbox.maxlines >= 0) {
		combobox->vscrollbar.step = 0.;
		if (combobox->textbox.array->length -
			combobox->textbox.maxlines > 0)
			combobox->vscrollbar.step = 1. /
				(combobox->textbox.array->length -
				combobox->textbox.maxlines);
		firstline = (int) ((combobox->textbox.array->length - 
			combobox->textbox.maxlines) *
			combobox->vscrollbar.fraction + 0.5);
		if (firstline >= 0) combobox->textbox.firstline = firstline;
		*draw = 1;
	}
	if (!event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&combobox->entry.rect)) {
		combobox->window.visible = 1;
		*draw = 1;
	}
	if (kiss_entry_event(&combobox->entry, event, draw)) {
		combobox->window.visible = 0;
		strcpy(combobox->text, combobox->entry.text);
		*draw = 1;
		SDL_StopTextInput();
		return 1;
	} else if (kiss_textbox_event(&combobox->textbox, event, draw)) {
		combobox->window.visible = 0;
		combobox->entry.active = 0;
		if (combobox->entry.wdw) combobox->entry.wdw->focus = 1;
		combobox->entry.focus = 0;
		index = combobox->textbox.firstline +
			combobox->textbox.selectedline;
		kiss_string_copy(combobox->entry.text,
			kiss_maxlength(combobox->textbox.font,
			combobox->entry.textwidth,
			(char *) kiss_array_data(combobox->textbox.array,
			index), NULL),
			(char *) kiss_array_data(combobox->textbox.array,
			index), NULL);
		*draw = 1;
		SDL_StopTextInput();
		return 1;
	}
	return 0;
}

int kiss_combobox_draw(kiss_combobox *combobox, SDL_Renderer *renderer)
{
	if (combobox && combobox->wdw)
		combobox->visible = combobox->wdw->visible;
	if (!combobox || !combobox->visible || !renderer) return 0;
	kiss_renderimage(renderer, combobox->combo,
		combobox->entry.rect.x + combobox->entry.rect.w,
		combobox->entry.rect.y + combobox->entry.rect.h -
		combobox->combo.h - kiss_edge, NULL);
	kiss_entry_draw(&combobox->entry, renderer);
	kiss_window_draw(&combobox->window, renderer);
	kiss_vscrollbar_draw(&combobox->vscrollbar, renderer);
	kiss_textbox_draw(&combobox->textbox, renderer);
	return 1;
}


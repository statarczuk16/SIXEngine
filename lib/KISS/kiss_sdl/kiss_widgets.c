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
#include <math.h>


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
		
		int offset_down = parent_rect->h * 0.5; //set to y to middle of parent window
		int offset_up_text = text_height * 0.6; //offset up to account for height of font
		*out_y = parent_rect->y + offset_down - offset_up_text;
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

int determine_render_position(SDL_Rect *ui_rect, kiss_window* parent_, SDL_Rect *return_rect, h_alignment ha, v_alignment va, scale_to_parent_width sp_w, int column, int row)
{
	if (ui_rect == NULL || return_rect == NULL)
	{
		printf("kiss_widgets::determine_render_position null input argument");
		abort();
	}

	SDL_Rect temp_parent;
	temp_parent.x = 0;
	temp_parent.y = 0;
	temp_parent.h = 0;
	temp_parent.w = 0;

	//if this component has a parent window
	if (parent_ != NULL)
	{
		//take parent width and height from first parent
		kiss_window* parent = parent_;
		temp_parent.w = parent->rect.w;
		temp_parent.h = parent->rect.h;
		temp_parent.x += parent->r_rect.x;
		temp_parent.y += parent->r_rect.y;
		//recursive up the chain to get absolute position of the first parent, since its position is relative to ITS parent
		/**
		while (parent != NULL)
		{
			temp_parent.x += parent->r_rect.x;
			temp_parent.y += parent->r_rect.y;
			parent = parent->wdw;
		}
		**/

	}
	else
	{
		temp_parent.x = 0;
		temp_parent.y = 0;
		temp_parent.w = ui_rect->w;
		temp_parent.h = ui_rect->h;
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
			return_rect->w = temp_parent.w;
			break;
		}
		case SP_FILL_WITH_BUFFER:
		{
			return_rect->w = temp_parent.w - 20;
			break;
		}
		case SP_HALF:
		{
			return_rect->w = (temp_parent.w / 2) - 20;
			break;
		}
		case SP_THIRD:
		{
			return_rect->w = (temp_parent.w / 3) - 20;
			break;
		}
		case SP_FOURTH:
		{
			return_rect->w = (temp_parent.w / 4) - 20;
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
			return_rect->x = temp_parent.x + (temp_parent.w / 2) - (return_rect->w/2);
			break;
		}
		case HA_COLUMN:
		{
			//return_rect->x = temp_parent.x + 15 + (((return_rect->w) + 10) * column);
			return_rect->x = temp_parent.x + 15 + (((return_rect->w) + 10) * column);
			break;
		}
		case HA_NONE:
		{
			//offset ui rect by the position of parent window
			return_rect->x = ui_rect->x + temp_parent.x;
			break;
		}
		default:
		{
			//offset ui rect by the position of parent window
			return_rect->x = ui_rect->x + temp_parent.x;
			break;
		}
	}
	//determine vertical alignment
	switch (va)
	{
		case VA_CENTER:
		{
			//set y position to middle of parent window
			return_rect->y = temp_parent.y + (temp_parent.h / 2) - (return_rect->h / 2);
			break;
		}
		case VA_ROW:
		{
			//use the size of the UI element as the row size + a bit of offset
			return_rect->y = temp_parent.y + 10 + (((return_rect->h)+5) * row );
			break;
		}
		case VA_NONE:
		{
			//offset ui rect by the position of parent window
			return_rect->y =  temp_parent.y + ui_rect->y;
			break;
		}
		default:
		{
			//offset ui rect by the position of parent window
			return_rect->y = temp_parent.y + ui_rect->y;
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



int kiss_label_new_uc(kiss_label* label, kiss_window* wdw, char* text,
	int x, int y, int width, int height)
{
	if (!label || !text)
	{
		return -1;
	}
	if (label->font.magic != KISS_MAGIC)
	{
		label->font = kiss_textfont;
	}
	label->textcolor = kiss_black;
	kiss_makerect(&label->rect, x, y, width, height);
	kiss_string_copy(label->text, KISS_MAX_LABEL, text, NULL);
	label->visible = 1;
	label->wdw = wdw;
	return 0;
}

int kiss_label_new(kiss_label *label, kiss_window *wdw, char *text,
	int x, int y)
{
	if (!label || !text) return -1;
	if (label->font.magic != KISS_MAGIC) label->font = kiss_textfont;
	label->textcolor = kiss_black;
	kiss_makerect(&label->rect, x, y, 150, 50);
	kiss_string_copy(label->text, KISS_MAX_LABEL, text, NULL);
	label->visible = 1;
	label->wdw = wdw;
	return 0;
}

int kiss_window_draw(kiss_window* window, SDL_Renderer* renderer)
{
	
	if (!window)
	{
		return 0;
	}
	int visible = window->visible;
	if (window->wdw)
	{
		visible = (window->wdw->visible && window->visible);
	}

	
	(void)determine_render_position(&window->rect, window->wdw, &window->r_rect, window->h_align, window->v_align, window->parent_scale, window->column, window->row);

		
	
	if (!window || !visible || !renderer)
	{
		return 0;
	}
	kiss_fillrect(renderer, &window->r_rect, window->bg);
	if (window->decorate)
	{
		kiss_decorate(renderer, &window->r_rect, kiss_sand_dark, kiss_edge);
	}
		
	return 1;
}

int substring_length(char str[], int start, int end)
{
	int length = end - start + 1;
	char sub[KISS_MAX_LABEL];
	strncpy(sub, str + start, length);
	sub[length] = '\0';
	return strnlen(sub, KISS_MAX_LABEL);
}

int kiss_label_draw(kiss_label *label, SDL_Renderer *renderer)
{
	char buf[KISS_MAX_LABEL], *p;
	int len, y, x;
	int visible = label->visible;
	int text_width;

	if (label)
	{
		//get the width in units of the label text
		//if it has line breaks, get the width until the first line break
		char* first_line_break_pos = strchr(label->text, '\n');
		if (first_line_break_pos != NULL)
		{
			//get index of first line break pos subtracting address of label (eg, FF) from address of line break (eg, FF+7)
			int first_line_break_index = (int) (first_line_break_pos - label->text);
			char buffer[KISS_MAX_LABEL];
			//make a substring of the label up until first line break
			strncpy(buffer, label->text, first_line_break_index);
			buffer[first_line_break_index] = '\0';
			//get width of the text up until the first line break
			text_width = kiss_textwidth(label->font, buffer, NULL);
		}
		else
		{
			text_width = kiss_textwidth(label->font, label->text, NULL);
		}
		
		visible = label->visible && label->wdw->visible;
		//pass by ref - sets button->r_rect
		(void)determine_render_position(&label->rect, label->wdw, &label->r_rect, label->h_align, label->v_align, label->parent_scale, label->column, label->row);
		//pass by ref - sets textx and texty
		(void)determine_text_render_position(&label->r_rect, label->txt_h_align, VA_CENTER, &x, &y, text_width, kiss_textheight(label->font, label->text, NULL));
		if (x > label->r_rect.x)
		{
			text_width += (x - label->r_rect.x);
		}
		
	}
	if (!label || !visible || !renderer)
	{
		return 0;
	}
	
	
	//if the text is too wide to fit in the label, insert line breaks
	//note text_width variable is only as wide as text until first line break
	double text_width_temp_px_dbl = text_width * 1.05;
	int text_width_temp_px = round(text_width_temp_px_dbl);
	if (text_width_temp_px > label->r_rect.w)
	{
		char* test = "o";
		int char_width_px = kiss_textwidth(label->font, test, NULL);
		
		
		int label_width_chars = label->r_rect.w / char_width_px + 1;
		int split_location = 0;
		size_t label_text_width_chars = strnlen(label->text, KISS_MAX_LABEL);
		
		if (strchr(label->text, ' ') == NULL)
		{
			label->r_rect.w = text_width_temp_px;
		}
		while (text_width_temp_px > label->r_rect.w)
		{
			split_location += label_width_chars;
			if (split_location < label_text_width_chars && split_location > 0)
			{
				while (label->text[split_location] != ' ')
				{
					split_location--;
				}
				label->text[split_location] = '\n';
				char* buffer = label->text + split_location;
				text_width_temp_px = kiss_textwidth(label->font, buffer, NULL);
			}
			else
			{

				label->r_rect.w = text_width_temp_px;
			}
		}
	}
	
	
	

	kiss_decorate(renderer, &label->r_rect, kiss_sand_dark, kiss_edge);

	len = (int)strlen(label->text);
	if (len > KISS_MAX_LABEL - 2)
	{
		label->text[len - 1] = '\n';
	}
	else
	{
		strncat(label->text, "\n", KISS_MAX_LENGTH);
	}
	
	for (p = label->text; *p; p = strchr(p, '\n')+1)
	{
		kiss_string_copy(buf, strcspn(p, "\n") + 1, p, NULL);
		kiss_rendertext(renderer, buf, x, y, label->font, label->textcolor);
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
	button->visible = 1;
	button->focus = 0;
	button->wdw = wdw;
	button->v_align = VA_NONE;
	button->h_align = HA_NONE;
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
	button->visible = 1;
	button->focus = 0;
	button->wdw = wdw;
	button->v_align = VA_NONE;
	button->h_align = HA_NONE;
	button->parent_scale = SP_NONE;
	button->row = 0;
	button->column = 0;
	button->txt_v_align = VA_CENTER;
	button->txt_h_align = HA_CENTER;
	button->enabled = 1;
	return 0;
}


int kiss_button_event(kiss_button *button, SDL_Event *event, int *draw)
{
	int visible = 0;
	visible = (button->wdw->visible && button->visible);
	if (!visible || !event) return 0;
	if (button->enabled == 0)
	{
		return 0;
	}
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
	char buf[KISS_MAX_LABEL], * p;
	int len, y, x;
	int visible = 0;
	if (button)
	{
		visible = (button->wdw->visible && button->visible);
		
		//pass by ref - sets button->r_rect
		(void) determine_render_position(&button->rect, button->wdw, &button->r_rect, button->h_align, button->v_align, button->parent_scale, button->column, button->row);
		//pass by ref - sets textx and texty
		(void) determine_text_render_position(&button->r_rect, button->txt_h_align, button->txt_v_align, &x, &y, kiss_textwidth(button->font, button->text, NULL), kiss_textheight(button->font, button->text, NULL));
	}
	
	if (!button || !visible || !renderer)
	{
		return 0;
	}
	
	if (button->active || button->enabled == 0)
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

	len = (int)strlen(button->text);
	if (len > KISS_MAX_LABEL - 2)
	{
		button->text[len - 1] = '\n';
	}
	else
	{
		strncat(button->text, "\n", KISS_MAX_LENGTH);
	}

	for (p = button->text; *p; p = strchr(p, '\n') + 1)
	{
		kiss_string_copy(buf, strcspn(p, "\n") + 1, p, NULL);
		kiss_rendertext(renderer, buf, x, y, button->font, button->textcolor);
		y += button->font.lineheight;
	}
	button->text[len] = 0;
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
	selectbutton->v_align = VA_NONE;
	selectbutton->h_align = HA_NONE;
	selectbutton->parent_scale = SP_NONE;
	selectbutton->row = 0;
	selectbutton->column = 0;
	selectbutton->txt_v_align = VA_CENTER;
	selectbutton->txt_h_align = HA_CENTER;
	return 0;
}

int kiss_selectbutton_new_uc(kiss_selectbutton* selectbutton, kiss_window* wdw,
	int x, int y, int w, int h)
{
	if (!selectbutton) return -1;
	if (selectbutton->selectedimg.magic != KISS_MAGIC)
		selectbutton->selectedimg = kiss_selected;
	if (selectbutton->unselectedimg.magic != KISS_MAGIC)
		selectbutton->unselectedimg = kiss_unselected;
	kiss_makerect(&selectbutton->rect, x, y, w, h);
	selectbutton->selected = 0;
	selectbutton->visible = 1;
	selectbutton->focus = 0;
	selectbutton->wdw = wdw;
	selectbutton->v_align = VA_NONE;
	selectbutton->h_align = HA_NONE;
	selectbutton->parent_scale = SP_NONE;
	selectbutton->row = 0;
	selectbutton->column = 0;
	selectbutton->txt_v_align = VA_CENTER;
	selectbutton->txt_h_align = HA_CENTER;
	selectbutton->enabled = 1;
	return 0;
}

int kiss_selectbutton_event(kiss_selectbutton *selectbutton,
	SDL_Event *event, int *draw)
{
	int visible = 0;
	visible = (selectbutton->wdw->visible && selectbutton->visible);
	if (!visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!selectbutton->focus && (!selectbutton->wdw ||
		(selectbutton->wdw && !selectbutton->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		kiss_pointinrect(event->button.x, event->button.y,
		&selectbutton->r_rect)) {
		selectbutton->selected ^= 1;
		*draw = 1;
		return 1;
	}
	return 0;
}

int kiss_selectbutton_draw(kiss_selectbutton *selectbutton,	SDL_Renderer *renderer)
{
	int visible = 0;
	if (selectbutton)
	{
		visible = (selectbutton->wdw->visible && selectbutton->visible);
		//pass by ref - sets button->r_rect
		(void)determine_render_position(&selectbutton->rect, selectbutton->wdw, &selectbutton->r_rect, selectbutton->h_align, selectbutton->v_align, selectbutton->parent_scale, selectbutton->column, selectbutton->row);
		//pass by ref - sets textx and texty
		//(void)determine_text_render_position(&selectbutton->r_rect, selectbutton->txt_h_align, selectbutton->txt_v_align, &selectbutton->textx, &selectbutton->texty, selectbutton->text_width, selectbutton->font.fontheight);
	}
	if (!selectbutton || !visible || !renderer)
	{
		return 0;
	}
	if (selectbutton->selected)
	{
		kiss_renderimage(renderer, selectbutton->selectedimg, selectbutton->r_rect.x, selectbutton->r_rect.y, NULL);
	}
	else
	{
		kiss_renderimage(renderer, selectbutton->unselectedimg, selectbutton->r_rect.x, selectbutton->r_rect.y, NULL);
	}
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
	vscrollbar->visible = 1;
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
	int visible = 0;
	visible = (vscrollbar->wdw->visible && vscrollbar->visible);
	if (!vscrollbar || !visible) return 0;
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
	hscrollbar->visible = 1;
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
	int visible = 0;
	visible = (hscrollbar->wdw->visible && hscrollbar->visible);
	if (!hscrollbar || !visible) return 0;
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
	if (!progressbar || w < 2 * kiss_border + 1)
	{
		return -1;
	}
	if (progressbar->bar.magic != KISS_MAGIC)
	{
		progressbar->bar = kiss_bar;
	}
		
	progressbar->bg = kiss_blue;
	kiss_makerect(&progressbar->rect, x, y, w, progressbar->bar.h + 2 * kiss_border);
	kiss_makerect(&progressbar->barrect, x + kiss_border, y + kiss_border, 0, progressbar->bar.h);
	progressbar->width = w - 2 * kiss_border;
	progressbar->fraction = 0.;
	progressbar->step = 0.02;
	progressbar->lasttick = 0;
	progressbar->run = 0;
	progressbar->visible = 1;
	progressbar->wdw = wdw;
	return 0;
}

int kiss_progressbar_new_uc(kiss_progressbar* progressbar, kiss_window* wdw,
	int x, int y, int w, int h)
{
	if (!progressbar || w < 2 * kiss_border + 1)
	{
		return -1;
	}
	if (progressbar->bar.magic != KISS_MAGIC)
	{
		progressbar->bar = kiss_bar;
	}
	if (progressbar->font.magic != KISS_MAGIC)
	{
		progressbar->font = kiss_textfont;
	}
	progressbar->textcolor = kiss_black;

	progressbar->bg = kiss_lightblue;
	kiss_makerect(&progressbar->rect, x, y, w, h);
	kiss_makerect(&progressbar->barrect, x + kiss_border, y + kiss_border, 0, progressbar->bar.h);
	progressbar->width = w - 2 * kiss_border;
	progressbar->fraction = 0.;
	progressbar->step = 0.02;
	progressbar->lasttick = 0;
	progressbar->run = 0;
	progressbar->visible = 1;
	progressbar->wdw = wdw;
	return 0;
}

int kiss_progressbar_event(kiss_progressbar *progressbar, SDL_Event *event,
	int *draw)
{
	int visible = 0;
	visible = (progressbar->wdw->visible && progressbar->visible);
	if (!progressbar || !visible) return 0;
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
	char buf[KISS_MAX_LABEL], * p;
	SDL_Rect clip;
	int len, y, x;
	int visible = progressbar->visible;
	if (progressbar)
	{
		visible = progressbar->visible && progressbar->wdw->visible;
		//pass by ref - sets button->r_rect
		(void)determine_render_position(&progressbar->rect, progressbar->wdw, &progressbar->r_rect, progressbar->h_align, progressbar->v_align, progressbar->parent_scale, progressbar->column, progressbar->row);
		//pass by ref - sets textx and texty
		(void)determine_text_render_position(&progressbar->r_rect, progressbar->txt_h_align, VA_CENTER, &x, &y, kiss_textwidth(progressbar->font, progressbar->text, NULL), kiss_textheight(progressbar->font, progressbar->text, NULL));
	}
	if (!progressbar || !visible || !renderer)
	{
		return 0;
	}
	//y = progressbar->r_rect.y + progressbar->font.spacing + kiss_edge;
	//printf("Progress Row %d Y %d\n", progressbar->row, progressbar->r_rect.y);

	
	progressbar->barrect.x = progressbar->r_rect.x + kiss_edge;
	progressbar->barrect.y = progressbar->r_rect.y + kiss_edge;
	progressbar->barrect.h = progressbar->r_rect.h - 2 * kiss_edge;
	progressbar->barrect.w = progressbar->r_rect.w - 2 *  kiss_edge;
	
	progressbar->fraction = progressbar->value / progressbar->max_value;
	if (progressbar->fraction < 0.0)
	{
		progressbar->fraction = 0.0;
	}
	progressbar->barrect.w = (int)(progressbar->barrect.w * progressbar->fraction);
	if (progressbar->barrect.w < 0)
	{
		progressbar->barrect.w = 0.0;
	}
	
	kiss_fillrect(renderer, &progressbar->barrect, progressbar->bg);
	kiss_decorate(renderer, &progressbar->r_rect, kiss_sand_dark, kiss_edge);
	//
	//kiss_makerect(&clip, 0, 0, progressbar->barrect.w, progressbar->barrect.h);
	//kiss_renderimage(renderer, progressbar->bar, progressbar->barrect.x, progressbar->barrect.y, &clip);

	//char c[100] = "%%";
	//snprintf(progressbar->text, 8, "%2g %s", round(progressbar->fraction * 100.0), c);
	len = (int)strlen(progressbar->text);
	if (len > KISS_MAX_LABEL - 2)
	{
		progressbar->text[len - 1] = '\n';
	}
	else
	{
		strncat(progressbar->text, "\n", KISS_MAX_LENGTH);
	}

	for (p = progressbar->text; *p; p = strchr(p, '\n') + 1)
	{
		kiss_string_copy(buf, strcspn(p, "\n") + 1, p, NULL);
		kiss_rendertext(renderer, buf, x, y, progressbar->font, progressbar->textcolor);
		y += progressbar->font.lineheight;
	}
	progressbar->text[len] = 0;
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
	entry->visible = 1;
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
	int visible = 0;
	visible = (entry->wdw->visible && entry->visible);
	if (!entry || !visible || !event) return 0;
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
					temp = (int)round(entry->upper_bound);
				}
				else if (temp <= entry->lower_bound)
				{
					temp = (int)round(entry->lower_bound);
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
			strncat(entry->text, event->text.text, KISS_MAX_LENGTH);
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
		strncpy(entry->text, "", KISS_MAX_LENGTH);
		*draw = 1;
		return 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN && entry->active &&
		kiss_pointinrect(event->button.x, event->button.y,
		&entry->r_rect)) {
		strncpy(entry->text, "", KISS_MAX_LENGTH);
		*draw = 1;
		return 1;
	}
	
	return 0;
}

int kiss_entry_draw(kiss_entry *entry, SDL_Renderer *renderer)
{
	SDL_Color color;
	int visible = 0;
	if (entry && entry->wdw)
	{
		visible = entry->visible && entry->wdw->visible;
		(void)determine_render_position(&entry->rect, entry->wdw, &entry->r_rect, entry->h_align, entry->v_align, entry->parent_scale, entry->column, entry->row);
		//pass by ref - sets textx and texty
		(void)determine_text_render_position(&entry->r_rect, entry->txt_h_align, entry->txt_v_align, &entry->textx, &entry->texty, entry->textwidth, entry->font.fontheight);
	}

	if (!entry || !visible || !renderer)
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
	textbox->visible = 1;
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
	int visible = 0;
	if (textbox && textbox->wdw)
	{
		visible = textbox->visible && textbox->wdw->visible;
	}
		
	if (!textbox || !visible || !renderer) return 0;
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
	strncpy(combobox->text, combobox->entry.text, KISS_MAX_LENGTH);
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
	combobox->visible = 1;
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
		strncpy(combobox->text, combobox->entry.text, KISS_MAX_LENGTH);
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
	int visible = 0;
	if (combobox && combobox->wdw)
	{
		visible = combobox->visible && combobox->wdw->visible;
	}
		
	if (!combobox || !visible || !renderer) return 0;
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


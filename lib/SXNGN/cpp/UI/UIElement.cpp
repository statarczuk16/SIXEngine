#include <UI/UIElement.h>
#include <Texture.h>


SXNGN::UIElement::UIElement(
	MultiSprite sprite,
	std::shared_ptr< SXNGN::Rendering::RenderingDelegate> rd,
	std::shared_ptr< SXNGN::EventHandling::EventHandlingDelegateUI> ehd)
	: renderering_handler_(rd)
	, event_handler_(ehd)
	, x(x)
	, y(y)
	, ui_tiles_(sprite)
{

}

void SXNGN::UIElement::UIElement::handle_event(SDL_Event& e)
{
	event_handler_->handle_event(this, e);
}

void SXNGN::UIElement::move(int x, int y)
{
	x = x;
	y = y;
}

SXNGN::MultiSprite SXNGN::UIElement::get_sprite_ref()
{
	return ui_tiles_;
}

void SXNGN::UIElement::render(std::shared_ptr<SXNGN::Tile>, std::shared_ptr<SXNGN::Camera> camera)
{

}
#pragma once
#include <ECS_Rendering/RenderingDelegate.h>
#include <ECS_EventHandling/EventHandlingDelegate.h>
#include <Sprite/Tile.h>

namespace SXNGN {
	//Base class of all UI Elements
	class UIElement
	{

	public:

		//Initializes the variables
		UIElement(
			MultiSprite sprite,
			std::shared_ptr< SXNGN::Rendering::RenderingDelegate> rd,
			std::shared_ptr< SXNGN::EventHandling::EventHandlingDelegateUI> ehd
		);
			
		virtual void handle_event(SDL_Event& e);

		virtual void render(std::shared_ptr<SXNGN::Tile>, std::shared_ptr<SXNGN::Camera> camera);

		virtual void move(int x, int y);

		virtual MultiSprite SXNGN::UIElement::get_sprite_ref();

	protected:
		std::shared_ptr<SXNGN::Rendering::RenderingDelegate> renderering_handler_;
		std::shared_ptr<SXNGN::EventHandling::EventHandlingDelegateUI> event_handler_;
		SXNGN::MultiSprite ui_tiles_;
		int x;
		int y;


	};
}

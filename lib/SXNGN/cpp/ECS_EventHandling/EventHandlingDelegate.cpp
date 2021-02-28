#include <ECS_EventHandling/EventHandlingDelegate.h>
#include <Camera.h>
#include <Sprite/Tile.h>
#include <UI/UIElement.h>
#include <Object/Entity.h>


void SXNGN::EventHandling::EventHandlingDelegateUI::attach(std::shared_ptr<UIElement> element)
{
    watched_ui_elements_.push_back(element);
}

void SXNGN::EventHandling::EventHandlingDelegateUI::detach(std::shared_ptr<UIElement> element)
{

}


void SXNGN::EventHandling::ReactiveUI::handle_event(UIElement* element, SDL_Event& e)
{
    element->handle_event(e);
}

 
void SXNGN::EventHandling::NonReactiveUI::handle_event(UIElement* element, SDL_Event& e)
{
            
}

void SXNGN::EventHandling::ReactiveEntity::handle_event(SXNGN::Entity* entity, SDL_Event& e)
{
    entity->handleEvent(e);
}

void SXNGN::EventHandling::NonReactiveEntity::handle_event(SXNGN::Entity* entity, SDL_Event& e)
{

}




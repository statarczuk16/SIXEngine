#pragma once
#include <memory>
#include <SDL.h>
#include <vector>


namespace SXNGN {
    

    class Tile;
    class Camera;
    class UIElement;
    class Entity;

    namespace EventHandling {


        class EventHandlingDelegateUI
        {
        public:
            virtual void handle_event(UIElement* element, SDL_Event& e) = 0;
            virtual void attach(std::shared_ptr<UIElement> element);
            virtual void detach(std::shared_ptr<UIElement> element);
            std::vector< std::shared_ptr<UIElement>> watched_ui_elements_;
        };

        class ReactiveUI : public EventHandlingDelegateUI
        {
        public:
            virtual void handle_event(UIElement* element, SDL_Event& e) override;
        };

        class NonReactiveUI : public EventHandlingDelegateUI
        {
        public:
            virtual void handle_event(UIElement* element, SDL_Event& e) override;
        };


        class EventHandlingDelegateEntity
        {
        public:
            virtual void handle_event(SXNGN::Entity* element, SDL_Event& e) = 0;
        };

        class ReactiveEntity : public EventHandlingDelegateEntity
        {
        public:
            virtual void handle_event(SXNGN::Entity* element, SDL_Event& e) override;
        };

        class NonReactiveEntity : public EventHandlingDelegateEntity
        {
        public:
            virtual void handle_event(SXNGN::Entity* element, SDL_Event& e) override;
        };
    }
}
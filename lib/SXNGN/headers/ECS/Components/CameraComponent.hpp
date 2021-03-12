#pragma once

#include <SDL.h>
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/CameraComponent.hpp>
#include <ECS/Core/Types.hpp>
#include <memory>



namespace SXNGN::ECS::Components {

    struct CameraComponent : ECS_Component
    {
    private:

       
       //CameraComponent::CameraComponent(const CameraComponent& source);
      
       //CameraComponent::CameraComponent(CameraComponent&& source);
       
       static std::shared_ptr<CameraComponent> instance_;
       

    public:

        CameraComponent(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target = -1);

        static std::shared_ptr<CameraComponent> init_instance(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target = -1);
       

        static std::shared_ptr<CameraComponent> get_instance();
      

        static void set_target(Entity target);
      

        static void set_position_actual(SDL_Rect pos_actual);
       

        static void set_position_scaled(SDL_Rect pos_scaled);
       

        static Entity get_target();

        SDL_Rect lens_;
        SDL_Rect position_actual_; //relation to the level
        SDL_Rect position_scaled_;//scaled (old position * scaling factor)
        SDL_Rect screen_bounds_;
        Entity target_;
       
    };

}


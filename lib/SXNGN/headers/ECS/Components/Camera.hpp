#pragma once

#include "SDL.h"
#include <Database.h>
#include <ECS/Components/Camera.hpp>

using ComponentTypeEnum = SXNGN::ECS::Components::ComponentTypeEnum;

namespace SXNGN::ECS::Components {

    struct Camera : ECS_Component
    {
    private:

       
       //Camera::Camera(const Camera& source);
      
       //Camera::Camera(Camera&& source);
       
       static std::shared_ptr<Camera> instance_;
       

    public:

        Camera(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target = -1);

        static std::shared_ptr<Camera> init_instance(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target = -1);
       

        static std::shared_ptr<Camera> get_instance();
      

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


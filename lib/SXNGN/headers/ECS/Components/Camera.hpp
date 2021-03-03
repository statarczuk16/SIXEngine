#pragma once

#include "SDL.h"
#include <ECS/Core/Component.hpp>


using ComponentTypeEnum = SXNGN::ECS::Components::ComponentTypeEnum;

namespace SXNGN::ECS::Components {

    struct Camera : ECS_Component
    {
    private:
        static std::shared_ptr<Camera> instance;

        Camera(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target = -1)
        {
            lens_ = lens;
            position_actual_ = position;
            position_scaled_.x = position_actual_.x * SXNGN::Database::get_scale();
            position_scaled_.y = position_actual_.y * SXNGN::Database::get_scale();
            screen_bounds_ = screen_bounds;
            target = -1;
            component_type = ECS::Components::ComponentTypeEnum::CAMERA;
        }

        Camera(const Camera& source)
        {
            // Disabling copy-ctor
        }

        Camera(Camera&& source)
        {
            // Disabling move-ctor
        }

        SDL_Rect lens_;
        SDL_Rect position_actual_; //relation to the level
        SDL_Rect position_scaled_;//scaled (old position * scaling factor)
        SDL_Rect screen_bounds_;
        Entity target_;


    public:

        static std::shared_ptr<Camera> init_instance(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target = -1)
        {
            instance = std::make_shared<Camera>(lens, position, screen_bounds, target);
        }

        static std::shared_ptr<Camera> get_instance()
        {
            return instance;
        }

        static void set_target(Entity target)
        {
            instance->target_ = target;
        }

        static void set_position_actual(SDL_Rect pos_actual)
        {
            instance->position_actual_ = pos_actual;
        }

        static void set_position_scaled(SDL_Rect pos_scaled)
        {
            instance->position_scaled_ = pos_scaled;
        }

        static Entity get_target()
        {
            return instance->target_;
        }
    };

}


#pragma once

#include "SDL.h"
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/Camera.hpp>

namespace SXNGN::ECS::Components {

    std::shared_ptr<Camera> Camera::instance_;//have to define static privates in cpp as well as h

    Camera::Camera(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target)
    {
        lens_ = lens;
        position_actual_ = position;
        position_scaled_.x = position_actual_.x * SXNGN::Database::get_scale();
        position_scaled_.y = position_actual_.y * SXNGN::Database::get_scale();
        position_scaled_.w = 0;
        position_scaled_.h = 0;
        screen_bounds_ = screen_bounds;
        target_ = target;
        component_type = ECS::Components::ComponentTypeEnum::CAMERA;
    }


    std::shared_ptr<Camera> Camera::init_instance(SDL_Rect lens, SDL_Rect position, SDL_Rect screen_bounds, Entity target)
    {
        instance_ = std::make_shared<Camera>(lens, position, screen_bounds, target);
        return instance_;
    }

    std::shared_ptr<Camera> Camera::get_instance()
    {
        return instance_;
    }

    void Camera::set_target(Entity target)
    {
        instance_->target_ = target;
    }

    void Camera::set_position_actual(SDL_Rect pos_actual)
    {
        instance_->position_actual_ = pos_actual;
    }

    void Camera::set_position_scaled(SDL_Rect pos_scaled)
    {
        instance_->position_scaled_ = pos_scaled;
    }

    Entity Camera::get_target()
    {
        return instance_->target_;
    }

}



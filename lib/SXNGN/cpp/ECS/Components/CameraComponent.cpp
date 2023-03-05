#pragma once

#include "SDL.h"
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/CameraComponent.hpp>

namespace SXNGN::ECS {

    std::shared_ptr<CameraComponent> CameraComponent::instance_;//have to define static privates in cpp as well as h

    CameraComponent::CameraComponent(SDL_FRect lens, SDL_FRect position, SDL_FRect screen_bounds, Entity target)
    {
        lens_ = lens;
        position_actual_ = position;
        position_scaled_.x = position_actual_.x * SXNGN::Database::get_scale();
        position_scaled_.y = position_actual_.y * SXNGN::Database::get_scale();
        position_scaled_.w = 0;
        position_scaled_.h = 0;
        screen_bounds_ = screen_bounds;
        bounded_horizontal_ = false;
        target_ = target;
        component_type = ECS::ComponentTypeEnum::CAMERA_SINGLE;
    }


    std::shared_ptr<CameraComponent> CameraComponent::init_instance(SDL_FRect lens, SDL_FRect position, SDL_FRect screen_bounds, Entity target)
    {
        instance_ = std::make_shared<CameraComponent>(lens, position, screen_bounds, target);
        return instance_;
    }

    std::shared_ptr<CameraComponent> CameraComponent::get_instance()
    {
        return instance_;
    }

    void CameraComponent::set_target(Entity target)
    {
        instance_->target_ = target;
    }

    void CameraComponent::set_position_actual(SDL_FRect pos_actual)
    {
        instance_->position_actual_ = pos_actual;
    }

    void CameraComponent::set_position_scaled(SDL_FRect pos_scaled)
    {
        instance_->position_scaled_ = pos_scaled;
    }

    Entity CameraComponent::get_target()
    {
        return instance_->target_;
    }

}



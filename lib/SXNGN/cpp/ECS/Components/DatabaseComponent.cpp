#pragma once

#include "SDL.h"
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/DatabaseComponent.hpp>

namespace SXNGN::ECS::A {

    std::shared_ptr<DatabaseComponent> DatabaseComponent::instance_;//have to define static privates in cpp as well as h

    DatabaseComponent::DatabaseComponent()
    {
        component_type = ComponentTypeEnum::DATABASE_SINGLE;
    }

    std::shared_ptr<DatabaseComponent> DatabaseComponent::init_instance()
    {
        instance_ = std::make_shared<DatabaseComponent>();
        return instance_;
    }

    std::shared_ptr<DatabaseComponent> DatabaseComponent::get_instance()
    {
        return instance_;
    }

}



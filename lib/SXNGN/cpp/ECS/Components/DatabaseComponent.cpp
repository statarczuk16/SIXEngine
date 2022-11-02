#pragma once

#include "SDL.h"
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/DatabaseComponent.hpp>
#include <iostream>

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
        if (instance_ == nullptr)
        {
            return init_instance();
        }
        return instance_;
    }

    void DatabaseComponent::replace_db(DatabaseComponent* new_db)
    {
       std::shared_ptr<DatabaseComponent> new_instance(new_db);
       instance_ = new_instance;
    }

    void DatabaseComponent::merge_db(DatabaseComponent* new_db)
    {
        for (auto& it : new_db->settings_map) {
            instance_->settings_map[it.first] = it.second;
        }

        for (auto const& entry : instance_->settings_map) 
        {
            std::cout << "{" << entry.first << ", " << entry.second << "}" << std::endl;
        }

        return;
    }

}



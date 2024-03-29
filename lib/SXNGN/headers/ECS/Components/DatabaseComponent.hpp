#pragma once

#include <SDL.h>
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/Components.hpp>
#include <ECS/Core/Types.hpp>
#include <memory>



namespace SXNGN::ECS {

    struct DatabaseComponent : ECS_Component
    {
    private:

       
       static std::shared_ptr<DatabaseComponent> instance_;
       

    public:

        DatabaseComponent();

        static std::shared_ptr<DatabaseComponent> init_instance();
       
        static std::shared_ptr<DatabaseComponent> get_instance();

        static void replace_db(DatabaseComponent* new_db);

        static void merge_db(DatabaseComponent* new_db);

        std::map < std::string, double > settings_map;
        std::map < std::string, sole::uuid > entity_map;
        std::map < std::string, Event_Component > event_map;
       
     
    };

	inline void to_json(json& j, const DatabaseComponent& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::DATABASE_SINGLE]},
			{"settings_map", p.settings_map},
            {"entity_map", p.entity_map},
            {"event_map", p.entity_map}
		};

	}

	inline void from_json(const json& j, DatabaseComponent& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("settings_map").get_to(p.settings_map);
        j.at("entity_map").get_to(p.entity_map);
        j.at("event_map").get_to(p.event_map);
	}

}


#pragma once

#include <SDL.h>
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Core/Types.hpp>
#include <memory>



namespace SXNGN::ECS::A {

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
     
    };

	inline void to_json(json& j, const DatabaseComponent& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::DATABASE_SINGLE]},
			{"settings_map", p.settings_map}
		};

	}

	inline void from_json(const json& j, DatabaseComponent& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("settings_map").get_to(p.settings_map);
	}

}


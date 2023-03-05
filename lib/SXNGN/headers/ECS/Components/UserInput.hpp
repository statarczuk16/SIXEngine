#pragma once

#include <SDL.h>
#include <unordered_set>
#include <mutex>
#include <ECS/Core/Component.hpp>
#include <UI/UserInputUtils.hpp>


namespace SXNGN::ECS {

	enum class User_Input_Tags
	{
		MOUSE_CONTROL, //can be selected with mouse
		WASD_CONTROL, //movement controlled by keyboard
		PLAYER_CONTROL_MOVEMENT,
		PROPERTY_CONTROL_MOVEMENT //movement controlled by gui
	};


	/// <summary>
	/// Contains information about how an entity responds to events
	/// </summary>
	struct User_Input_Tags_Collection : ECS_Component
	{
		User_Input_Tags_Collection()
		{
			component_type = ComponentTypeEnum::INPUT_TAGS;
		}
		std::unordered_set<User_Input_Tags> input_tags_;
		std::string property_tag_ = ""; //used if input tags include PROPERTY_CONTROL_MOVEMENT
	};

	
	inline void to_json(json& j, const User_Input_Tags_Collection& p) {
		j = json{
			{"component_type",component_type_enum_to_string()[ComponentTypeEnum::INPUT_TAGS]},
			{"input_tags_", p.input_tags_},
			{"property_tag_", p.property_tag_},
		};
	}

	inline void from_json(const json& j, User_Input_Tags_Collection& p) {
		auto component_type_enum = component_type_string_to_enum().at(j.at("component_type"));
		p.component_type = component_type_enum;
		j.at("input_tags_").get_to(p.input_tags_);
		j.at("property_tag_").get_to(p.property_tag_);
	}

	/// <summary>
	/// Contains all sdl events (user inputs) entered during a frame
	/// </summary>
	struct User_Input_Cache : ECS_Component
	{
		User_Input_Cache()
		{
			component_type = ComponentTypeEnum::INPUT_CACHE;
		}
		std::vector<SDL_Event> events_;
	};





	/// <summary>
	/// Singleton used to store input state
	/// </summary>
	class User_Input_State : ECS_Component
	{
	private:

		//CameraComponent::CameraComponent(const CameraComponent& source);

		//CameraComponent::CameraComponent(CameraComponent&& source);

		static User_Input_State *instance_;
		static std::mutex lock_;

		User_Input_State() {

		}

	public:
		static User_Input_State* get_instance()
		{
			std::lock_guard<std::mutex> guard(lock_);//Wait until data is available (no other theadss have checked it out
			if (instance_ == nullptr)
			{
				instance_ = new User_Input_State();
			}
			return instance_;
		}

		MouseState mouse_state;
		Click last_right_click;
		Click last_left_click;
		std::unordered_set<Entity> selected_entities;
		bool shift_down = false;
		bool alt_down = false;
		bool ctrl_down = false;

	};
}

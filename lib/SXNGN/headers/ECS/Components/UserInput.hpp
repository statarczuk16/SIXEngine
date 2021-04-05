#pragma once

#include <SDL.h>
#include <unordered_set>
#include <mutex>
#include <ECS/Core/Component.hpp>
#include <UI/UserInputUtils.hpp>


namespace SXNGN::ECS::A {

	enum class User_Input_Tags
	{
		CLICKABLE, //can be selected with mouse
		WASD_CONTROL, //movement controlled by keyboard
		PLAYER_CONTROL_MOVEMENT,
		CURRENTLY_SELECTED
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
	};

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


	};
}

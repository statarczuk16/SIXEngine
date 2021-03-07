#pragma once

#include <SDL.h>
#include <ECS/Core/Component.hpp>
#include <unordered_set>
#include <mutex>
#include <UI/UserInputUtils.hpp>




namespace SXNGN::ECS::Components {

	enum class User_Input_Tags
	{
		CLICKABLE, //can be selected with mouse
		WASD_CONTROL, //movement controlled by keyboard
		PLAYER_CONTROL_MOVEMENT
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
	/// Singleton used to store input state
	/// </summary>
	class User_Input_State : ECS_Component
	{
	private:

		//Camera::Camera(const Camera& source);

		//Camera::Camera(Camera&& source);

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
				instance_ = new User_Input_State;
			}
			return instance_;
		}

		SXNGN::UserInputUtils::MouseState mouse_state;
		SXNGN::UserInputUtils::Click last_right_click;
		SXNGN::UserInputUtils::Click last_left_click;


	};
}
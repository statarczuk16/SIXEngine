#include <ECS/Systems/ParallaxSystem.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Parallax.hpp>




namespace SXNGN::ECS::A 
{

	void Parallax_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Parallax_System Init");
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto it_act = m_actable_entities.begin();
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;
			auto parallax_data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::PARALLAX);
			if (parallax_data)
			{
				Parallax* parallax_ptr = static_cast<Parallax*>(parallax_data);
				parallax_ptr->init_ = false;
			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARALLAX);
		}
	}

	/// <summary>
	/// User Input A Goals: Translate user input to components that need it
	/// </summary>
	/// <param name="dt"></param>
	void Parallax_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		//SDL_LogDebug(1, "User_Input_Begin ");
		auto it_act = m_actable_entities.begin();
		//actable entities for user input system are UserInputCache (vector of sdl events)
		auto total = m_actable_entities.size();
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;
			double speed_horizontal = 0.0;
			double speed_vertical = 0.0;
			auto parallax_data = gCoordinator.CheckOutComponent(entity_actable, ComponentTypeEnum::PARALLAX);
			if (parallax_data)
			{
				std::deque<sole::uuid> parallax_images_new_uuid;;
				std::deque<Entity> parallax_images_new_entity;
				/// First, determine the scroll speed of the images
				Parallax* parallax_ptr = static_cast<Parallax*>(parallax_data);
				std::map<std::string, double>* properties = SXNGN::Database::get_property_map();
				
				if (parallax_ptr->speed_source_horizontal_ != BAD_STRING_RETURN)
				{

					if (properties->count(parallax_ptr->speed_source_horizontal_) > 0)
					{
						speed_horizontal = properties->at(parallax_ptr->speed_source_horizontal_);
						speed_horizontal *= parallax_ptr->speed_multiplier_;
					}
				}
				if (parallax_ptr->speed_source_vertical_ != BAD_STRING_RETURN)
				{
					if (properties->count(parallax_ptr->speed_source_vertical_) > 0)
					{
						speed_vertical = properties->at(parallax_ptr->speed_source_vertical_);
						speed_vertical *= parallax_ptr->speed_multiplier_;
					}
				}

				/// Each image is its own entity in the list 
				// 1. Apply speed to the moveable component
				// 2. Determine width of the image from renderable component 
				// 3. For location components:
				//	  1. Determine if image needs to be wrapped around (if off screen left, move to the right, vice versa) If so add to BACK of queue
				//    2. Else move to FRONT of queue
				// 4. For each location component in queue, start from first image that should be on screen and append the rest to that image

				auto it = parallax_ptr->parallax_images_.begin();
				bool overlap = false;
				double first_onscreen_image_x = DBL_MAX; //the x location of the first image that should be on the screen.
				double image_width_p = 0.0;
				double image_height_p = 0.0;
				while (it != parallax_ptr->parallax_images_.end())
				{
					sole::uuid image_id = *it;
					
					it++;
					Entity image_entity = gCoordinator.GetEntityFromUUID(image_id);
					auto check_out_move = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::MOVEABLE);
					if (check_out_move)
					{
						Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move);
						moveable_ptr->m_vel_x_m_s = speed_horizontal;
						moveable_ptr->m_vel_y_m_s = speed_vertical;
						gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, image_entity);
					}
					auto render_data = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::RENDERABLE);
					
					if(render_data)
					{
						const Renderable* renderable_ptr = static_cast<const Renderable*>(render_data);
						image_width_p = renderable_ptr->tile_map_snip_.w;
						image_height_p = renderable_ptr->tile_map_snip_.h;
						gCoordinator.CheckInComponent(image_entity, ComponentTypeEnum::RENDERABLE);
					}
					
					auto check_out_loc = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::LOCATION);
					if (check_out_loc)
					{
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						if (location_ptr)
						{
							//if images are moving left, when an image escapes left side of screen, put it in the BACK of the queue
							if (speed_horizontal < 0.0)
							{
								if (location_ptr->m_pos_x_m_ + image_width_p < 0.0)
								{
									overlap = true;
									parallax_images_new_entity.push_back(image_entity);
								}
								else
								{
									if (location_ptr->m_pos_x_m_ < first_onscreen_image_x)
									{
										first_onscreen_image_x = location_ptr->m_pos_x_m_;
									}
									parallax_images_new_entity.push_front(image_entity);
								}
							}
							//if images are moving right, when an image escapes the right side of screen, put it in the FRONT of the queue
							else if(speed_horizontal > 0.0)
							{
								if (location_ptr->m_pos_x_m_ > gCoordinator.getGameSettings()->resolution.w)
								{
									overlap = true;
									parallax_images_new_entity.push_front(image_entity);
								}
								else
								{
									if (location_ptr->m_pos_x_m_ < first_onscreen_image_x)
									{
										first_onscreen_image_x = location_ptr->m_pos_x_m_;
									}
									parallax_images_new_entity.push_back(image_entity);
								}
							}
							
							gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, image_entity);
						}
						else
						{
							
						}
								
					}
					
				}
				
				if (overlap || (parallax_ptr->init_ == false && parallax_images_new_entity.size() > 0))
				{
					parallax_ptr->init_ = true;
					//now have list, in order, of images to be rendered, their width, and starting x position
					auto it2 = parallax_images_new_entity.begin();
					int idx = 0;
					while (it2 != parallax_images_new_entity.end())
					{
						Entity image_entity = *it2;
						it2++;
						auto check_out_loc = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::LOCATION);
						if (check_out_loc)
						{
							Location* location_ptr = static_cast<Location*>(check_out_loc);
							if (location_ptr)
							{
								location_ptr->m_pos_x_m_ = first_onscreen_image_x + (image_width_p * idx) - 1.0;
							}
							gCoordinator.CheckInComponent(image_entity, ComponentTypeEnum::LOCATION);
						}
						idx++;

					}
				}
				
				

				/// Second, 
			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARALLAX);
			
			
		}
	}
			

		
		
		

}

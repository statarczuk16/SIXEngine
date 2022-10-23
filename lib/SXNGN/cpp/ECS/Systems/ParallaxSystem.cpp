#include <ECS/Systems/ParallaxSystem.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <ECS/Components/Parallax.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>




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
		std::shared_ptr<SDL_Rect> overworld_viewport = gCoordinator.get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
		auto camera = CameraComponent::get_instance();
		auto camera_lens_unscaled = ECS_Utils::determine_camera_lens_unscaled(camera);
		double screen_left_bound = camera_lens_unscaled.x;
		double screen_right_bound = camera_lens_unscaled.x + camera_lens_unscaled.w;
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
				//	  1. Determine if image needs to be wrapped around (if off screen left, move to the right, vice versa) If so remove from queue and save
				//    2. Else push back onto the queue to preserve original order
				// 4. Push the saved image entity to be wrapped around onto the front or back of the queue (if it is the new leftmost or rightmost image respectively)
				// 5. If new rightmost or newleftmost, calculate new position of leftmost or rightmost based on being left or right of its neighbor
				// 6. If a redraw is warranted (init, new right image, new left image) refresh the locations by drawing each image in the queue in order, starting from leftmost position

				auto it = parallax_ptr->parallax_images_.begin();
				bool new_leftmost_image = false;
				bool new_rightmost_image = false;
				double image_width_p = 0.0;
				double image_height_p = 0.0;
				Entity new_right_entity = -1;
				Entity new_left_entity = -1;
				//SDL_Log("Parallax Entity %d", entity_actable);
				while (it != parallax_ptr->parallax_images_.end())
				{
					sole::uuid image_id = *it;
					
					it++;
					Entity image_entity = gCoordinator.GetEntityFromUUID(image_id);
					//SDL_Log("Contains image with Entity %d", image_entity);
					if (!(gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::MOVEABLE) && gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::RENDERABLE) && gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::LOCATION)))
					{
						continue;
					}
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
							//std::cout << "Image " << image_entity << " has location " << location_ptr->m_pos_x_m_ << std::endl;
							//if images are moving left, when an image escapes left side of screen, put it in the BACK of the queue
							if (speed_horizontal < 0.0)
							{
								if (location_ptr->m_pos_x_m_ + image_width_p < screen_left_bound && new_right_entity == -1)
								{
									new_rightmost_image = true;
									new_right_entity = image_entity;
								}
								else
								{
									parallax_images_new_entity.push_back(image_entity);
								}
							}
							//if images are moving right, when an image escapes the right side of screen, put it in the FRONT of the queue
							else if(speed_horizontal > 0.0)
							{
								if (location_ptr->m_pos_x_m_ > screen_right_bound && new_right_entity == -1)
								{
									new_leftmost_image = true;
									new_left_entity = image_entity;
								}
								else
								{
									parallax_images_new_entity.push_back(image_entity);
								}
							}
							else
							{
								parallax_images_new_entity.push_back(image_entity);
							}
							
							gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, image_entity);
						}	
					}
					else
					{

					}
					
				}


				
				if (new_rightmost_image)
				{
					
					//push the new found rightmost image onto the back of the queue
					parallax_images_new_entity.push_back(new_right_entity);
					//if there is a new image in the end of the queue, it should be the rightmost image, so change its location to be so
					//by placing it one image width to the right of the image next to it in the queue
					auto it_new_right = parallax_images_new_entity.end() - 1;
					Entity image_entity_new_right = *it_new_right;
					auto it_prev_rightmost = it_new_right - 1;
					Entity image_entity_prev_rightmost = *it_prev_rightmost;

					double prev_rightmost_x;
					double rightmost_x;
					auto check_out_loc = gCoordinator.CheckOutComponent(image_entity_prev_rightmost, ComponentTypeEnum::LOCATION);
					if (check_out_loc)
					{
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						if (location_ptr)
						{
							prev_rightmost_x = location_ptr->m_pos_x_m_;
						}
						gCoordinator.CheckInComponent(image_entity_prev_rightmost, ComponentTypeEnum::LOCATION);
					}
					check_out_loc = gCoordinator.CheckOutComponent(image_entity_new_right, ComponentTypeEnum::LOCATION);
					if (check_out_loc)
					{
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						if (location_ptr)
						{
							location_ptr->m_pos_x_m_ = (prev_rightmost_x + image_width_p);
							rightmost_x = location_ptr->m_pos_x_m_;
							SDL_Log("New rightmost image: %d at %f", new_right_entity, rightmost_x);
						}
						gCoordinator.CheckInComponent(image_entity_new_right, ComponentTypeEnum::LOCATION);
					}
				}

				if (new_leftmost_image)
				{
					//push the new found leftmost image onto the back of the queue
					parallax_images_new_entity.push_front(new_left_entity);
					//if there is a new image in the front of the queue, it should be the leftmost image, so change its location to be so
					//by placing it one image width to the left of the image next to it in the queue
					auto it_new_left = parallax_images_new_entity.begin();
					Entity image_entity_new_left = *it_new_left;
					auto it_prev_leftmost = it_new_left + 1;
					Entity image_entity_prev_leftmost = *it_prev_leftmost;

					double prev_leftmost_x;
					double leftmost_x;
					auto check_out_loc = gCoordinator.CheckOutComponent(image_entity_prev_leftmost, ComponentTypeEnum::LOCATION);
					if (check_out_loc)
					{
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						if (location_ptr)
						{
							prev_leftmost_x = location_ptr->m_pos_x_m_;
							
						}
						gCoordinator.CheckInComponent(image_entity_prev_leftmost, ComponentTypeEnum::LOCATION);
					}
					check_out_loc = gCoordinator.CheckOutComponent(image_entity_new_left, ComponentTypeEnum::LOCATION);
					if (check_out_loc)
					{
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						if (location_ptr)
						{
							location_ptr->m_pos_x_m_ = (prev_leftmost_x - image_width_p);
							leftmost_x = location_ptr->m_pos_x_m_;
							SDL_Log("New leftmost image: %d at %f", new_left_entity, leftmost_x);
						}
						gCoordinator.CheckInComponent(new_left_entity, ComponentTypeEnum::LOCATION);
					}
				}
				
				//want to reset positions of the images in any of these cases
				bool redraw = new_leftmost_image || new_rightmost_image || parallax_ptr->init_ == false;
				
					
				if (redraw && (parallax_ptr->parallax_images_.size() == parallax_images_new_entity.size()))
				{
					
					parallax_ptr->parallax_images_.clear(); //replace this with the new queue generated
					
					//dont want to "blow" the initilizaton redrwaw if the images aren't in the system yet
					if (parallax_images_new_entity.size() > 0)
					{
						parallax_ptr->init_ = true;
					}
					
					auto it2 = parallax_images_new_entity.begin();
					int idx = 0;
					double start_x;
					while (it2 != parallax_images_new_entity.end())
					{
						Entity image_entity = *it2;
						parallax_ptr->parallax_images_.push_back(gCoordinator.GetUUIDFromEntity(image_entity));
						it2++;
						auto check_out_loc = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::LOCATION);
						if (check_out_loc)
						{
							Location* location_ptr = static_cast<Location*>(check_out_loc);
							if (location_ptr)
							{
								if (idx == 0)
								{
									start_x = location_ptr->m_pos_x_m_;
								}
								std::cout << "Image " << image_entity << " moved from " << location_ptr->m_pos_x_m_;
								location_ptr->m_pos_x_m_ = start_x + (image_width_p * idx - 1.0);
								std::cout << " to " << location_ptr->m_pos_x_m_ << std::endl;
							}
							gCoordinator.CheckInComponent(image_entity, ComponentTypeEnum::LOCATION);
						}
						idx++;

					}
						
				}
			}
			gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARALLAX);
			
			
		}
	}
			

		
		
		

}

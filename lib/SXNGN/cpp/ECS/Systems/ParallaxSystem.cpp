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
	void Parallax_System::Update(double dt)
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
		float screen_left_bound = camera_lens_unscaled.x;
		float screen_right_bound = camera_lens_unscaled.x + camera_lens_unscaled.w;
		auto db_comp = DatabaseComponent::get_instance();
		auto properties = &db_comp->settings_map;
		double stop_or_go = 0.0;
		auto db = DatabaseComponent::get_instance();
		double focus_y = -1;
		
		if (properties->count(SXNGN::OVERWORLD_GO) > 0)
		{
			stop_or_go = properties->at(SXNGN::OVERWORLD_GO);
		}

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

				Entity snap_source_entity = gCoordinator.GetEntityFromUUID(parallax_ptr->snap_source_y_);
				if (snap_source_entity != SXNGN::BAD_ENTITY)
				{
					auto focus_data = gCoordinator.CheckOutComponent(snap_source_entity, ComponentTypeEnum::LOCATION);
					if (focus_data)
					{
						Location* focus_ptr = static_cast<Location*>(focus_data);
						double offset = parallax_ptr->snap_source_y_offset_;
						focus_y = focus_ptr->m_pos_y_m_ + offset;
						gCoordinator.CheckInComponent(snap_source_entity, ComponentTypeEnum::LOCATION);
					}
				}


				if (parallax_ptr->speed_source_horizontal_ != BAD_STRING_RETURN)
				{

					if (properties->count(parallax_ptr->speed_source_horizontal_) > 0)
					{
						speed_horizontal = properties->at(parallax_ptr->speed_source_horizontal_);
						speed_horizontal *= parallax_ptr->speed_multiplier_;
						speed_horizontal *= stop_or_go;
						if (parallax_ptr->speed_sign_ != 0)
						{
							speed_horizontal *= parallax_ptr->speed_sign_;
						}
					}
				}
				if (parallax_ptr->speed_source_vertical_ != BAD_STRING_RETURN)
				{
					if (properties->count(parallax_ptr->speed_source_vertical_) > 0)
					{
						speed_vertical = properties->at(parallax_ptr->speed_source_vertical_);
						speed_vertical *= parallax_ptr->speed_multiplier_;
						speed_vertical *= stop_or_go;
					}
				}

				Entity new_leftmost_image = -1;
				Entity new_rightmost_image = -1;
				double x_to_staple_new_rightmost = -1.0;
				double x_to_staple_new_leftmost = -1.0;
				double image_width_p = 0.0;
				double image_height_p = 0.0;
				Entity new_right_entity = -1;
				Entity new_left_entity = -1;
				//SDL_Log("Parallax Entity %d", entity_actable);
				
				
				auto it = parallax_ptr->parallax_images_.begin();
				if (true)
				{
					sole::uuid image_id = *it;

					it++;
					Entity image_entity = gCoordinator.GetEntityFromUUID(image_id);
					//SDL_Log("Contains image with Entity %d", image_entity);
					if (!(gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::MOVEABLE) && gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::RENDERABLE) && gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::LOCATION)))
					{
						//skip because missing components necessary for scrolling
						//std::cout << "Cannot Parallax Scroll" << std::endl;
					}
					else
					{


						auto render_data = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::RENDERABLE);
						if (render_data)
						{
							const Renderable* renderable_ptr = static_cast<const Renderable*>(render_data);
							image_width_p = renderable_ptr->tile_map_snip_.w;// *renderable_ptr->scale_x_;
							image_height_p = renderable_ptr->tile_map_snip_.h;
							gCoordinator.CheckInComponent(image_entity, ComponentTypeEnum::RENDERABLE);
						}
						auto check_out_move = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::MOVEABLE);
						if (check_out_move)
						{
							Moveable* moveable_ptr = static_cast<Moveable*>(check_out_move);
							moveable_ptr->m_vel_x_m_s = speed_horizontal;
							moveable_ptr->m_vel_y_m_s = speed_vertical;
							gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, image_entity);
						}

						auto check_out_loc = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::LOCATION);
						if (check_out_loc)
						{
							Location* location_ptr = static_cast<Location*>(check_out_loc);

							//this is the first image in the queue. If the camera is further left than this image, grab the image from the back of the queue and staple it on
							//at the new leftmost image.
							if (location_ptr)
							{
								if (screen_left_bound < location_ptr->m_pos_x_m_)
								{
									auto current_rightmost_image = parallax_ptr->parallax_images_.back();
									parallax_ptr->parallax_images_.pop_back();
									parallax_ptr->parallax_images_.push_front(current_rightmost_image);
									new_leftmost_image = gCoordinator.GetEntityFromUUID(current_rightmost_image);
									x_to_staple_new_leftmost = location_ptr->m_pos_x_m_ - image_width_p;
								}

							}
							gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, image_entity);
						}
					}
				}

				it = parallax_ptr->parallax_images_.begin();
				if (focus_y != -1)
				{
					while (it != parallax_ptr->parallax_images_.end())
					{
						sole::uuid image_id = *it;
						it++;
						Entity image_entity = gCoordinator.GetEntityFromUUID(image_id);
						auto check_out_loc = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::LOCATION);
						if (check_out_loc)
						{
							Location* location_ptr = static_cast<Location*>(check_out_loc);
							location_ptr->m_pos_y_m_ = focus_y - image_height_p + 64;
							gCoordinator.CheckInComponent(image_entity, ComponentTypeEnum::LOCATION);
						}
					}
				}

				it = parallax_ptr->parallax_images_.end() - 1;
				if (it != parallax_ptr->parallax_images_.begin())
				{
					sole::uuid image_id = *it;
					Entity image_entity = gCoordinator.GetEntityFromUUID(image_id);
					//SDL_Log("Contains image with Entity %d", image_entity);
					if (!(gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::MOVEABLE) && gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::RENDERABLE) && gCoordinator.EntityHasComponent(image_entity, ComponentTypeEnum::LOCATION)))
					{
						//skip because missing components necessary for scrolling
						//std::cout << "Cannot Parallax Scroll" << std::endl;
					}
					else
					{
						auto render_data = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::RENDERABLE);
						if (render_data)
						{
							const Renderable* renderable_ptr = static_cast<const Renderable*>(render_data);
							image_width_p = renderable_ptr->tile_map_snip_.w;// *renderable_ptr->scale_x_; disabled the scaling because want the images to overlap a bit because of a gap between
							image_height_p = renderable_ptr->tile_map_snip_.h;
							gCoordinator.CheckInComponent(image_entity, ComponentTypeEnum::RENDERABLE);
						}
						auto check_out_loc = gCoordinator.CheckOutComponent(image_entity, ComponentTypeEnum::LOCATION);
						if (check_out_loc)
						{
							Location* location_ptr = static_cast<Location*>(check_out_loc);

							//this is the last image in the queue. If the camera is further right than this image, grab the image from the front of the queue and staple it on
							//at the new rightmost image.
							if (location_ptr)
							{
								if (screen_right_bound > location_ptr->m_pos_x_m_ + image_width_p)
								{
									auto current_leftmost_image = parallax_ptr->parallax_images_.front();
									parallax_ptr->parallax_images_.pop_front();
									parallax_ptr->parallax_images_.push_back(current_leftmost_image);
									new_rightmost_image = gCoordinator.GetEntityFromUUID(current_leftmost_image);
									x_to_staple_new_rightmost = location_ptr->m_pos_x_m_ + image_width_p;
								}
							}
							gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, image_entity);
						}
					}

					if (new_rightmost_image != -1)
					{
						auto check_out_loc = gCoordinator.CheckOutComponent(new_rightmost_image, ComponentTypeEnum::LOCATION);
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						location_ptr->m_pos_x_m_ = x_to_staple_new_rightmost;
						gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, new_rightmost_image);

					}
					if (new_leftmost_image != -1)
					{
						auto check_out_loc = gCoordinator.CheckOutComponent(new_leftmost_image, ComponentTypeEnum::LOCATION);
						Location* location_ptr = static_cast<Location*>(check_out_loc);
						location_ptr->m_pos_x_m_ = x_to_staple_new_leftmost;
						gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, new_leftmost_image);
					}
				}


				gCoordinator.CheckInComponent(entity_actable, ComponentTypeEnum::PARALLAX);
			}
		}
	}
}
			
			
			

		
		
		



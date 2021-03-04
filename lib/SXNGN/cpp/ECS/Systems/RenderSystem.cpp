#include <ECS/Systems/RenderSystem.hpp>
#include <ECS/Components/Renderable.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <fstream>
#include <Database.h>
#include <gameutils.h>

#include <Collision.h>


using Pre_Sprite_Factory = SXNGN::ECS::Components::Pre_Sprite_Factory;
using Sprite_Factory = SXNGN::ECS::Components::Sprite_Factory;
using Pre_Renderable = SXNGN::ECS::Components::Pre_Renderable;
using Renderable = SXNGN::ECS::Components::Renderable;
using ECS_Camera = SXNGN::ECS::Components::Camera;


	void Renderer_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Renderer_System Init");


	}

	bool Renderer_System::object_in_view(ECS_Camera camera, SDL_Rect object_bounds)
	{
		//Have to scale up object to compare to the camera
		object_bounds.x *= SXNGN::Database::get_scale();
		object_bounds.y *= SXNGN::Database::get_scale();
		object_bounds.w *= SXNGN::Database::get_scale();
		object_bounds.h *= SXNGN::Database::get_scale();
		SDL_Rect scaled_camera_lens = determine_camera_lens_scaled(camera);

		//Note object_bounds.w * 2 -- extra buffer so objects a bit out of camera range are still rendered
		//dont want stuff to disappear//appear while the player can still see them
		if (SXNGN::CollisionChecks::checkCollisionBuffer(scaled_camera_lens, object_bounds, object_bounds.w * 2))
		{
			return true;
		}
		return false;
	}

	SDL_Rect Renderer_System::determine_camera_lens_scaled(ECS_Camera camera)
	{
		SDL_Rect return_view;
		SDL_Rect position_scaled = camera.position_scaled_;
		SDL_Rect screen_bounds = camera.screen_bounds_;
		SDL_Rect lens = camera.lens_;

		//bounding box centers are at the top left of the box.
		//If the camera is tracking a target at (10,10), then the top left of the camera vision square should not be the target
		//instead the top left vision square of the camera should be some ways left and some ways above the tracked target
		//such that a tracked target is in the center of the view (rather than being at the top left)
		return_view.x = position_scaled.x - (lens.w / 2);
		return_view.y = position_scaled.y - (lens.h / 2);
		return_view.w = lens.w;
		return_view.h = lens.h;

		SDL_Rect screen_bounds_scaled = screen_bounds;

		screen_bounds_scaled.x *= SXNGN::Database::get_scale();
		screen_bounds_scaled.y *= SXNGN::Database::get_scale();
		screen_bounds_scaled.w *= SXNGN::Database::get_scale();
		screen_bounds_scaled.h *= SXNGN::Database::get_scale();


		//don't allow camera top-right view square point to leave screen
		if (return_view.x < screen_bounds_scaled.x)
		{
			return_view.x = screen_bounds_scaled.x;
		}
		if (return_view.x > (screen_bounds_scaled.x + screen_bounds_scaled.w))
		{
			return_view.x = screen_bounds_scaled.x + screen_bounds_scaled.w;
		}

		if (return_view.y < screen_bounds_scaled.y)
		{
			return_view.y = screen_bounds_scaled.y;
		}
		if (return_view.y > (screen_bounds_scaled.y + screen_bounds_scaled.h))
		{
			return_view.y = screen_bounds_scaled.y + screen_bounds_scaled.h;
		}

		return return_view;
	}

	SDL_Rect Renderer_System::determine_camera_lens_unscaled(ECS_Camera camera)
	{
		SDL_Rect current_view = determine_camera_lens_scaled(camera);
		current_view.x /= SXNGN::Database::get_scale();
		current_view.y /= SXNGN::Database::get_scale();
		return current_view;
	}


	void Renderer_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		ECS_Camera camera = *ECS_Camera::get_instance();
		//if (camera = nullptr)
		//{
		//	return;
		//}
		//Iterate through entities this system manipulates/converts
		//(Renders Renderables to screen)
		SDL_Rect render_quad = { 0,0,0,0 };
		for (auto const& entity : m_actable_entities)
		{
			//Search for Pre-Renderables
			ECS_Component* data = gCoordinator.TryGetComponent(entity, ComponentTypeEnum::RENDERABLE);
			Renderable* renderable_ptr;
			if (data && data->get_component_type() == ComponentTypeEnum::RENDERABLE)
			{
				renderable_ptr = (Renderable*)data;
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderable of Entity ID %2d is not a renderable!", entity);
				continue;
			}
			Renderable renderable = *renderable_ptr;

			//Mutex go here - don't need pointer anymore
			//Do not ever free any component ptr retrieved from coordinator!!!

			//If the renderable is on screen (within camera lens)
			if (object_in_view(camera, renderable.bounding_box_))
			{

				SDL_Rect camera_lens = determine_camera_lens_unscaled(camera);
				//get the position of this object with respect to the camera lens
				int texture_pos_wrt_cam_x = renderable.bounding_box_.x - camera_lens.x;
				int texture_pos_wrt_cam_y = renderable.bounding_box_.y - camera_lens.y;

				//inefficient, but here for debug/readbility
				renderable.bounding_box_.x = texture_pos_wrt_cam_x;
				renderable.bounding_box_.y = texture_pos_wrt_cam_y;


				renderable.sprite_map_texture_
					->render2(renderable.bounding_box_, renderable.tile_map_snip_);
			}
		
			if (renderable.sprite_map_texture_ != nullptr)
			{
				renderable.sprite_map_texture_->render2(renderable.bounding_box_, renderable.tile_map_snip_);
			}
			else
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture of Entity ID %2d is null!", entity);
				
			}
			


		}
	}


	void Sprite_Factory_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Sprite_Factory_System Init");


	}


	void Sprite_Factory_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;
		//Iterate through entities this system manipulates/converts
		//(Mostly Pre_Renderables) -> Renderables
		for (auto const& entity : m_actable_entities)
		{
			//Search for Pre-Renderables
			ECS_Component *data = gCoordinator.TryGetComponent(entity, ComponentTypeEnum::PRE_RENDERABLE);
			Pre_Renderable *pre_renderable_ptr;
			if (data && data->get_component_type() == ComponentTypeEnum::PRE_RENDERABLE)
			{
				pre_renderable_ptr = (Pre_Renderable*) data;
			}
			else
			{
				continue;
			}
			Pre_Renderable pre_renderable = *pre_renderable_ptr;

			std::string factory_name = pre_renderable.sprite_factory_name;
			std::string sprite_type = pre_renderable.sprite_factory_sprite_type;

			SXNGN::ECS::Components::Sprite_Factory sprite_factory_component;
			//fixme - move sprite factories into singleton components
			bool found_sprite_factory;
			//Entities of interest should include the Sprite_Factories
			for (auto const& entity : m_entities_of_interest)
			{
				ECS_Component *data = gCoordinator.TryGetComponent(entity, ComponentTypeEnum::SPRITE_FACTORY);
				Sprite_Factory *factory;
				if (data!=nullptr && data->get_component_type() == ComponentTypeEnum::SPRITE_FACTORY)
				{
					factory = (Sprite_Factory*)data;
					if (factory->sprite_factory_name == factory_name)
					{
						found_sprite_factory = true;
						sprite_factory_component = *factory;
						break;
					}
				}
				else
				{
					continue;
				}
			}

			//If there's no Sprite_Factory of name requested by Pre_Renderable, skip this Pre_Renderable
			//But don't delete... maybe someone will create that Sprite_Factory in the future
			if (!found_sprite_factory)
			{
				printf("Sprite Factory doesn't exist: %s\n", pre_renderable.sprite_factory_name.c_str());
				continue;
			}
			else
			{
				//See if the factory has the data needed to process the sprite type of the Pre_Renderable
				if (sprite_factory_component.tile_name_string_to_rect_map_.count(pre_renderable.sprite_factory_sprite_type) > 0)
				{
					Renderable* renderable_component = new Renderable();
					auto tile_snip_box = sprite_factory_component.tile_name_string_to_rect_map_[pre_renderable.sprite_factory_sprite_type];
					SDL_Rect collision_box;
					collision_box.x = pre_renderable.x;
					collision_box.y = pre_renderable.y;
					collision_box.w = tile_snip_box->w;
					collision_box.h = tile_snip_box->h;

					renderable_component->bounding_box_ = collision_box;
					renderable_component->tile_map_snip_ = *tile_snip_box;
					auto sprite_sheet_texture = gCoordinator.get_texture_manager()->get_texture(pre_renderable.sprite_factory_name);
					if (!sprite_sheet_texture)
					{
						printf("Sprite Factory::Texture Manager does not have texture: %s", pre_renderable.sprite_factory_name.c_str());
						abort();
					}
					renderable_component->sprite_map_texture_ = sprite_sheet_texture;
					renderable_component->tile_name_ = pre_renderable.sprite_factory_sprite_type + "_renderable";

					//Build the apocalypse map sprite factory
					auto new_renderable_entity = gCoordinator.CreateEntity();
					
					gCoordinator.AddComponent(new_renderable_entity, renderable_component);

					entities_to_cleanup.push_back(entity);;

				}
				

			}
		}

		for (Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
	}

	void Sprite_Factory_Creator_System::Init()
	{


	}


	void Sprite_Factory_Creator_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		for (auto const& entity : m_actable_entities)
		{

			//go from a map file to a string
			std::map<int, std::string> tile_name_int_to_string_map;
			//where that string can go to a rect that is a tilesheet clip coordinate
			std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map;

			size_t  tile_width, tile_height;

			ECS_Component* data = gCoordinator.TryGetComponent(entity, ComponentTypeEnum::PRE_SPRITE_FACTORY);
			Pre_Sprite_Factory* pre_factory_ptr;
			if (data && data->get_component_type() == ComponentTypeEnum::PRE_SPRITE_FACTORY)
			{
				pre_factory_ptr = (Pre_Sprite_Factory*)data;
			}
			else
			{
				//fixme error here
				continue;
			}

			Pre_Sprite_Factory pre_factory = *pre_factory_ptr;

			std::string tile_manifest_path = pre_factory.tile_manifest_path_;

			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Sprite_Factory_Creator_System::Creating Sprite_Factory from %s", tile_manifest_path.c_str());

			std::ifstream in(tile_manifest_path);

			if (!in)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Sprite_Factory_Creator_System::Fatal: Cannot open tile manifest : % s", tile_manifest_path.c_str());
				entities_to_cleanup.push_back(entity);
				continue;
			}
			//Open the map
			std::string str;
			int tile_name_idx = 0;
			char delimiter = ',';
			std::string w = "";
			std::vector < std::vector< std::string> > manifest_vector;
			std::vector< std::string>  line_split_results;
			while (std::getline(in, str))
			{
				line_split_results.clear();
				w = "";
				for (auto rem : str)
				{
					if (rem == delimiter)
					{
						if (w != "")
						{
							line_split_results.push_back(w);
						}

						w = "";
					}
					else
					{
						w = w + rem;
					}

				}
				if (w != "")
				{
					line_split_results.push_back(w);
				}
				manifest_vector.push_back(line_split_results);
			}


			for (auto manifest_entry : manifest_vector)
			{

				if (manifest_entry.empty())
				{
					//std::cout << "Empty line..." << std::endl;
					continue;
				}
				std::string manifest_entry_type_str = manifest_entry.at(0);
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, " Read Type: %s", manifest_entry_type_str.c_str());
				SXNGN::ECS::Components::TileManifestType manifest_tile_type;
				if (SXNGN::ECS::Components::manifest_string_to_type_map_().count(manifest_entry_type_str) > 0)
				{
					manifest_tile_type = SXNGN::ECS::Components::manifest_string_to_type_map_().at(manifest_entry_type_str);
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Uknown manifest entry type: %s", manifest_entry_type_str.c_str());
					continue;
				}

				switch (manifest_tile_type)
				{
				case SXNGN::ECS::Components::TileManifestType::META_HEIGHT:
				{
					if (manifest_entry.size() == 2)
					{
						tile_height = std::stoi(manifest_entry.at(1));
					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_HEIGHT");
					}
					break;
				}
				case SXNGN::ECS::Components::TileManifestType::META_WIDTH:
				{
					if (manifest_entry.size() == 2)
					{
						tile_width = std::stoi(manifest_entry.at(1));
					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_WIDTH");
					}
					break;
				}
				case SXNGN::ECS::Components::TileManifestType::TERRAIN:
				{
					if (manifest_entry.size() == 5)
					{
						std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
						int x_grid = std::stoi(manifest_entry.at(1));
						int y_grid = std::stoi(manifest_entry.at(2));
						std::string tile_name = manifest_entry.at(3);
						int int_to_name = std::stoi(manifest_entry.at(4));
						entry->x = x_grid * tile_width;
						entry->y = y_grid * tile_height;
						entry->w = tile_width;
						entry->h = tile_height;
						tile_name_string_to_rect_map[tile_name] = entry;
						tile_name_int_to_string_map[int_to_name] = tile_name;
					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: TERRAIN");
					}
					break;
				}
				case SXNGN::ECS::Components::TileManifestType::UNIT:
				{
					if (manifest_entry.size() == 4)
					{
						std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
						int x_grid = std::stoi(manifest_entry.at(1));
						int y_grid = std::stoi(manifest_entry.at(2));
						std::string tile_name = manifest_entry.at(3);
						entry->x = x_grid * tile_width;
						entry->y = y_grid * tile_height;
						entry->w = tile_width;
						entry->h = tile_height;
						tile_name_string_to_rect_map[tile_name] = entry;
					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: UNIT");
					}
					break;
				}
				case SXNGN::ECS::Components::TileManifestType::META_SPRITE_SHEET_SOURCE:
				{
					if (manifest_entry.size() == 2)
					{
						std::string texture_path = SXNGN::BAD_STRING_RETURN;
						std::string relative_sprite_sheet_path = (manifest_entry.at(1));
						//find the tile sheet texture png. expect to find in the same folder as manifest.txt
						texture_path = Gameutils::get_file_in_folder(pre_factory.tile_manifest_path_,relative_sprite_sheet_path);
						if (texture_path == SXNGN::BAD_STRING_RETURN)
						{
							SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_SPRITE_SHEET_SOURCE: Bad texture path: %s", relative_sprite_sheet_path.c_str());
							abort();
						}

						bool texture_loaded_successfully = gCoordinator.get_texture_manager()->load_texture_from_path(pre_factory.name_,texture_path);
						if (!texture_loaded_successfully)
						{
							SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading texture for sprite factory:%s Texture: %s", pre_factory.name_.c_str(), relative_sprite_sheet_path.c_str());
							abort();
						}

					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_SPRITE_SHEET_SOURCE");
					}
					break;
				}

				}
			}



			in.close();
			
			entities_to_cleanup.push_back(entity);



			auto sprite_factory_entity = gCoordinator.CreateEntity();
			SXNGN::ECS::Components::Sprite_Factory* sprite_factory_component = new Sprite_Factory();
			sprite_factory_component->sprite_factory_name = pre_factory.name_,
				sprite_factory_component->tile_height_ = tile_height;
			sprite_factory_component->tile_width_ = tile_width;
			sprite_factory_component->tile_name_int_to_string_map_ = tile_name_int_to_string_map;
			sprite_factory_component->tile_name_string_to_rect_map_ = tile_name_string_to_rect_map;

			gCoordinator.AddComponent(sprite_factory_entity, sprite_factory_component);

		}

		for (Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}
		

	}

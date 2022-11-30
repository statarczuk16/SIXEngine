#include <ECS/Systems/RenderSystem.hpp>
#include <ECS/Components/SpriteFactory.hpp>
#include <ECS/Core/Coordinator.hpp>
#include <fstream>
#include <Database.h>
#include <gameutils.h>
#include <ECS/Utilities/ECS_Utils.hpp>
#include <array>

#include <Collision.h>


using Pre_Sprite_Factory = SXNGN::ECS::A::Pre_Sprite_Factory;
using Sprite_Factory = SXNGN::ECS::A::Sprite_Factory;
using Pre_Renderable = SXNGN::ECS::A::Pre_Renderable;
using Renderable = SXNGN::ECS::A::Renderable;
using ECS_Camera = SXNGN::ECS::A::CameraComponent;
using Gameutils = SXNGN::Gameutils;

using RenderLayer = SXNGN::ECS::A::RenderLayer;

namespace SXNGN::ECS::A {
	void Renderer_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Renderer_System Init");
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Renderer_System Init");


	}

	void Renderer_System::Draw_GUI()
	{
		//Get the coordinator the state manager
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		//get the singleton that holds the UI elements per game state
		auto ui = UICollectionSingleton::get_instance();

		std::set<ComponentTypeEnum> active_game_states = gCoordinator.GetActiveGameStates();
		//For each active game state, draw the corresponding UI elements
		for (auto state : active_game_states)
		{
			auto game_state_ui = ui->state_to_ui_map_.find(state);
			if (game_state_ui != ui->state_to_ui_map_.end())
			{
				Draw_GUI_Components(game_state_ui->second);
			}

		}
		
	}

	void Renderer_System::Draw_GUI_Component(SDL_Renderer* gRenderer, std::shared_ptr<UIContainerComponent> component_in_layer)
	{
		switch (component_in_layer->type_)
		{
			case UIType::WINDOW:
			{
				kiss_window_draw(component_in_layer->window_.get(), gRenderer);
				for (auto component_in_window : component_in_layer->child_components_)
				{
					Draw_GUI_Component(gRenderer, component_in_window);
				}
				break;
			}
			case UIType::BUTTON:
			{
				kiss_button_draw(component_in_layer->button_, gRenderer);
				break;
			}
			case UIType::LABEL:
			{
				kiss_label_draw(component_in_layer->label_, gRenderer);
			}
			case UIType::COMBOBOX:
			{
				break;
			}
			case UIType::ENTRY:
			{
				kiss_entry_draw(component_in_layer->entry_, gRenderer);
				break;
			}
			case UIType::HSCROLLBAR:
			{
				break;
			}
			case UIType::PROGRESSBAR:
			{
				break;
			}
			case UIType::SELECT_BUTTON:
			{
				kiss_selectbutton_draw(component_in_layer->selectbutton_, gRenderer);
				break;
			}
			case UIType::TEXTBOX:
			{
				break;
			}
			case UIType::VSCROLLBAR:
			{
				break;
			}
			default:
			{
				printf("RenderSystem::Draw_GUI_Components:: Unknown UI Component Type");
				abort();
			}
		}
	}
	
	void Renderer_System::Draw_GUI_Components(std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>> layer_to_components)
	{
		auto coordinator = Database::get_coordinator();
		SDL_Renderer* gRenderer = coordinator->Get_Renderer();
		std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>>::iterator it = layer_to_components.begin();
		while( it != layer_to_components.end())
		{
			for (auto component_in_layer : it->second)
			{
				Draw_GUI_Component(gRenderer, component_in_layer);
			}
			it++;
		}

	}

	

	void Renderer_System::Update(float dt)
	{
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::shared_ptr<ECS_Camera> camera_ptr = ECS_Camera::get_instance();
		Entity camera_target = camera_ptr->get_target();
		auto location_data = gCoordinator.CheckOutComponent(camera_target, ComponentTypeEnum::LOCATION);
		if (location_data)
		{
			Location* location = static_cast<Location*>(location_data);
			Coordinate camera_target_position = location->GetPixelCoordinate();
			SDL_FRect camera_target_position_rect;
			camera_target_position_rect.x = camera_target_position.x;
			camera_target_position_rect.y = camera_target_position.y;
			camera_ptr->set_position_actual(camera_target_position_rect);
			gCoordinator.CheckInComponent(camera_target, ComponentTypeEnum::LOCATION);
		}
		
		auto user_input_state = User_Input_State::get_instance();
		
		//Iterate through entities this system manipulates/converts
		//(Renders Renderables to screen)
		SDL_Rect render_quad = { 0,0,0,0 };
		auto it = m_actable_entities.begin();
		std::vector< std::vector< Entity > > renderables_per_layer;
		for (int i = 0; i <= (int)RenderLayer::TOP_LAYER; i++)
		{
			renderables_per_layer.push_back(std::vector<Entity>());
		}
		std::array<ECS_Component*, MAX_ENTITIES>& all_renderables = gCoordinator.CheckOutAllData(ComponentTypeEnum::RENDERABLE);
		std::array<ECS_Component*, MAX_ENTITIES>& all_locations = gCoordinator.CheckOutAllData(ComponentTypeEnum::LOCATION);
		//for (auto const& entity : m_actable_entities)
		while (it != m_actable_entities.end())
		{
			auto const& entity = *it;
			it++;


			//Get renderable
			ECS_Component* render_data = all_renderables[entity];//gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::RENDERABLE);
			Renderable* renderable_ptr = static_cast<Renderable*>(render_data);

			ECS_Component* location_data = all_locations[entity];//gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::RENDERABLE);
			Location* location_ptr = static_cast<Location*>(location_data);

			bool draw_outline = user_input_state->selected_entities.count(entity);
			if (draw_outline)
			{
				renderable_ptr->outline = true;
			}
			else
			{
				renderable_ptr->outline = false;
			}

			renderables_per_layer.at((int)renderable_ptr->render_layer_).push_back(entity);
		}
			//gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, entity);
		
		

		auto view_port = gCoordinator.get_state_manager()->getStateViewPort(ComponentTypeEnum::MAIN_GAME_STATE);
		
		SDL_RenderSetViewport(gCoordinator.Get_Renderer(), view_port.get());
		/**
		SDL_SetRenderDrawColor(gCoordinator.get_texture_manager()->get_renderer(), 153, 145, 128, 0);
		SDL_Rect rectangle;
		
		rectangle.x = 0;
		rectangle.y = 0;
		rectangle.w = gCoordinator.get_state_manager()->getGameSettings()->resolution.w;
		rectangle.h = gCoordinator.get_state_manager()->getGameSettings()->resolution.h;;
		SDL_RenderFillRect(gCoordinator.get_texture_manager()->get_renderer(), &rectangle);
		**/

		//Order of these matters. UI should appear over ground, etc

		for (std::vector<Entity> entities_this_layer : renderables_per_layer)
		{
			for (auto renderable_entity : entities_this_layer)
			{
				Render(renderable_entity, all_renderables[renderable_entity], all_locations[renderable_entity], camera_ptr);
			}
		}
		

		SDL_Rect normalViewPort;
		normalViewPort.x = 0;
		normalViewPort.y = 0;
		normalViewPort.w = gCoordinator.get_state_manager()->getGameSettings()->resolution.w;
		normalViewPort.h = gCoordinator.get_state_manager()->getGameSettings()->resolution.h;
		SDL_RenderSetViewport(gCoordinator.Get_Renderer(), &normalViewPort);
		//This draws the UI components in the UI Singleton, main menu buttons, etc. Precedence over renderable_ui_layer, which might be context menus in the gameplay window 
		//whereas the singleton holds "constant" stuff corresponding to the game state
		Draw_GUI();

		gCoordinator.CheckInAllData(ComponentTypeEnum::RENDERABLE);
		gCoordinator.CheckInAllData(ComponentTypeEnum::LOCATION);
	}

	void Renderer_System::Render(Entity entity, ECS_Component* render_data, ECS_Component* location_data, std::shared_ptr<ECS_Camera> camera)
	{
		//Get renderable
		auto gCoordinator = *SXNGN::Database::get_coordinator();

		const Collisionable* collisionable = nullptr;
		auto collisionable_data = gCoordinator.GetComponentReadOnly(entity, ComponentTypeEnum::COLLISION);
		if (collisionable_data)
		{
			collisionable = static_cast<const Collisionable*>(collisionable_data);;
		}

		Renderable* renderable_ptr = static_cast<Renderable*>(render_data);

		Location* location_ptr = static_cast<Location*>(location_data);

		

		if (renderable_ptr->sprite_batch_snips_.empty() == false)
		{
			int sprite_width = renderable_ptr->sprite_batch_snips_[0][0].w;
			int sprite_height = renderable_ptr->sprite_batch_snips_[0][0].h;
			float x = location_ptr->m_pos_x_m_;
			float y = location_ptr->m_pos_y_m_; - sprite_height;
			for (auto sprite_row : renderable_ptr->sprite_batch_snips_)
			{
				y += sprite_height;
				x = location_ptr->m_pos_x_m_;
				for (auto sprite : sprite_row)
				{
					x += sprite_width;
					SDL_FRect bounding_box;
					bounding_box.x = x;
					bounding_box.y = y;
					bounding_box.w = sprite.w * renderable_ptr->scale_x_;
					bounding_box.h = sprite.h * renderable_ptr->scale_y_;
					//If the renderable is on screen (within camera lens)
					if (ECS_Utils::object_in_view(camera, bounding_box))
					{

						SDL_FRect camera_lens = ECS_Utils::determine_camera_lens_unscaled(camera);
						//get the position of this object with respect to the camera lens
						float texture_pos_wrt_cam_x = bounding_box.x - camera_lens.x;
						float texture_pos_wrt_cam_y = bounding_box.y - camera_lens.y;

						//inefficient, but here for debug/readbility
						SDL_FRect render_quad;
						render_quad.x = texture_pos_wrt_cam_x;
						render_quad.y = texture_pos_wrt_cam_y;
						render_quad.w = bounding_box.w;
						render_quad.h = bounding_box.h;

						//SDL_Log("Rendering entity %d with location %f,%f at %d,%d", entity, location_ptr->m_pos_x_m_, location_ptr->m_pos_y_m_, render_quad.x, render_quad.y);

						if (renderable_ptr->sprite_map_texture_ != nullptr)
						{
							renderable_ptr->sprite_map_texture_
								->renderf(render_quad, sprite, 0.0, nullptr, SDL_FLIP_NONE, renderable_ptr->outline);
						}
						else
						{
							SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture of Renderable is null! %s", renderable_ptr->renderable_name_);
							abort();
						}
					}
				}
			}
		}
		else
		{
			SDL_FRect bounding_box;
			bounding_box.x = location_ptr->m_pos_x_m_;
			bounding_box.y = location_ptr->m_pos_y_m_;
			bounding_box.w = renderable_ptr->tile_map_snip_.w * renderable_ptr->scale_x_;
			bounding_box.h = renderable_ptr->tile_map_snip_.h * renderable_ptr->scale_y_;
			//If the renderable is on screen (within camera lens)
			if (ECS_Utils::object_in_view(camera, bounding_box))
			{

				SDL_FRect camera_lens = ECS_Utils::determine_camera_lens_unscaled(camera);
				//get the position of this object with respect to the camera lens
				float texture_pos_wrt_cam_x = bounding_box.x - camera_lens.x;
				float texture_pos_wrt_cam_y = bounding_box.y - camera_lens.y;

				//inefficient, but here for debug/readbility
				SDL_FRect render_quad;
				render_quad.x = round(texture_pos_wrt_cam_x);
				render_quad.y = round(texture_pos_wrt_cam_y);
				render_quad.w = bounding_box.w;
				render_quad.h = bounding_box.h;

				//SDL_Log("Rendering entity %d with location %f,%f at %d,%d", entity, location_ptr->m_pos_x_m_, location_ptr->m_pos_y_m_, render_quad.x, render_quad.y);

				if (renderable_ptr->sprite_map_texture_ != nullptr)
				{
					renderable_ptr->sprite_map_texture_
						->renderf(render_quad, renderable_ptr->tile_map_snip_, 0.0, nullptr, SDL_FLIP_NONE, renderable_ptr->outline);
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture of Renderable is null! %s", renderable_ptr->renderable_name_);
					abort();
				}
				if (collisionable != nullptr && collisionable->collision_shape_ == CollisionShape::CIRCLE)
				{
					renderable_ptr->sprite_map_texture_->render_circle(render_quad.x + collisionable->radius_, render_quad.y + collisionable->radius_, collisionable->radius_);
				}

				if (renderable_ptr->draw_debug_ && renderable_ptr->display_string_debug_ != "")
				{
					auto gCoordinator = SXNGN::Database::get_coordinator();
					FC_Font* debug_font = gCoordinator->get_texture_manager()->get_debug_font();
					int x = render_quad.x;
					int y = render_quad.y;
					renderable_ptr->sprite_map_texture_->render_text(x, y, debug_font, renderable_ptr->display_string_debug_);
				}
				auto gCoordinator = SXNGN::Database::get_coordinator();
				FC_Font* debug_font = gCoordinator->get_texture_manager()->get_debug_font();
				int x = render_quad.x;
				int y = render_quad.y;
				renderable_ptr->sprite_map_texture_->render_text(x, y, debug_font, "x");

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

		auto it = m_actable_entities.begin();
		//for (auto const& entity : m_actable_entities)
		while (it != m_actable_entities.end())
		{
			auto const& entity = *it;
			it++;
			//Search for Pre-Renderables
			auto read_only_pre_render = gCoordinator.GetComponentReadOnly(entity, ComponentTypeEnum::PRE_RENDERABLE);
			const Pre_Renderable* pre_renderable_ptr;
			if (read_only_pre_render)
			{
				pre_renderable_ptr = static_cast<const Pre_Renderable*>(read_only_pre_render);
			}
			else
			{
				continue;
			}
			Pre_Renderable pre_renderable = *pre_renderable_ptr;

			std::string factory_name = pre_renderable.sprite_factory_name_;
			std::string sprite_type = pre_renderable.sprite_factory_sprite_type_;

			std::shared_ptr<Sprite_Factory> sprite_factory_component;
			
			
	
			auto sprite_factory_holder = SpriteFactoryHolder::get_instance();
			auto sprite_factory_it = sprite_factory_holder->sprite_factories_.find(factory_name);
			if (sprite_factory_it != sprite_factory_holder->sprite_factories_.end())
			{
				sprite_factory_component = sprite_factory_it->second;
			}
			else
			{
				printf("Sprite Factory doesn't exist: %s\n", pre_renderable.sprite_factory_name_.c_str());
				continue;
			}

			
			//if the batch is used, only use batch and ignore other fields
			if (pre_renderable.sprite_batch_.size() > 0)
			{
				auto sprite_sheet_texture = gCoordinator.get_texture_manager()->get_texture(pre_renderable.sprite_factory_name_);
				if (!sprite_sheet_texture)
				{
					printf("Sprite Factory::Texture Manager does not have texture: %s", pre_renderable.sprite_factory_name_.c_str());
					abort();
				}
				std::vector < std::vector < SDL_Rect > > sprite_batch_snips_;
				for (std::vector<std::string> sprite_row : pre_renderable.sprite_batch_)
				{
					std::vector<SDL_Rect> tile_map_row;
					for (std::string sprite_name : sprite_row)
					{
						if (sprite_factory_component->tile_name_string_to_rect_map_.count(sprite_name) > 0)
						{
							auto tile_snip_box = sprite_factory_component->tile_name_string_to_rect_map_[sprite_name];
							tile_map_row.push_back(*tile_snip_box);
						}
					}
					sprite_batch_snips_.push_back(tile_map_row);
				}
				SDL_Rect first_snip = sprite_batch_snips_[0][0];
				Renderable* renderable_component = new Renderable(
					first_snip,
					pre_renderable.sprite_factory_name_,
					pre_renderable.sprite_factory_sprite_type_,
					sprite_sheet_texture,
					pre_renderable.render_layer_,
					pre_renderable.name_
				);
				renderable_component->sprite_batch_ = pre_renderable.sprite_batch_;
				renderable_component->sprite_batch_snips_ = sprite_batch_snips_;
				renderable_component->scale_x_ = pre_renderable.scale_x_;
				renderable_component->scale_y_ = pre_renderable.scale_y_;

				renderable_component->render_layer_ = pre_renderable.render_layer_;
				if (renderable_component->render_layer_ == SXNGN::ECS::A::RenderLayer::UNKNOWN)
				{
					//printf("RenderSystem:: %s has unknown RenderLayer", renderable_component->renderable_name_.c_str());
					abort();
				}

				//Build the apocalypse map sprite factory

				gCoordinator.AddComponent(entity, renderable_component);
				gCoordinator.RemoveComponent(entity, pre_renderable.get_component_type());
			}
			//See if the factory has the render_data needed to process the sprite type of the Pre_Renderable
			else if (sprite_factory_component->tile_name_string_to_rect_map_.count(pre_renderable.sprite_factory_sprite_type_) > 0)
			{
				auto sprite_sheet_texture = gCoordinator.get_texture_manager()->get_texture(pre_renderable.sprite_factory_name_);
				if (!sprite_sheet_texture)
				{
					printf("Sprite Factory::Texture Manager does not have texture: %s", pre_renderable.sprite_factory_name_.c_str());
					abort();
				}

				auto tile_snip_box = sprite_factory_component->tile_name_string_to_rect_map_[pre_renderable.sprite_factory_sprite_type_];
				Renderable* renderable_component = new Renderable(
					*tile_snip_box,
					pre_renderable.sprite_factory_name_,
					pre_renderable.sprite_factory_sprite_type_,
					sprite_sheet_texture,
					pre_renderable.render_layer_,
					pre_renderable.name_
				);
				renderable_component->scale_x_ = pre_renderable.scale_x_;
				renderable_component->scale_y_ = pre_renderable.scale_y_;

				renderable_component->render_layer_ = pre_renderable.render_layer_;
				if (renderable_component->render_layer_ == SXNGN::ECS::A::RenderLayer::UNKNOWN)
				{
					//printf("RenderSystem:: %s has unknown RenderLayer", renderable_component->renderable_name_.c_str());
					abort();
				}

				//Build the apocalypse map sprite factory

				gCoordinator.AddComponent(entity, renderable_component);
				gCoordinator.RemoveComponent(entity, pre_renderable.get_component_type());

			}
			else
			{
				SDL_LogError(1, "Error: Sprite Type %s was not in manifest", pre_renderable.sprite_factory_sprite_type_);
				abort();
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
		auto it = m_actable_entities.begin();
		
		//for (auto const& entity : m_actable_entities)
		while (it != m_actable_entities.end())
		{
			auto const& entity = *it;
			it++;

			//go from a map file to a string
			std::map<int, std::string> tile_name_int_to_string_map;
			//where that string can go to a rect that is a tilesheet clip coordinate
			std::map< std::string, std::shared_ptr<SDL_Rect>> tile_name_string_to_rect_map;

			size_t  tile_width, tile_height;

			const ECS_Component* data = gCoordinator.GetComponentReadOnly(entity, ComponentTypeEnum::PRE_SPRITE_FACTORY);
			const Pre_Sprite_Factory* pre_factory_ptr;
			if (data)
			{
				pre_factory_ptr = static_cast<const Pre_Sprite_Factory*>(data);
			}
			else
			{
				abort();
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
				if(str[0] == '#')
				{
					//this is a comment
					continue;
				}
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
				SXNGN::ECS::A::TileManifestType manifest_tile_type;
				if (SXNGN::ECS::A::manifest_string_to_type_map_().count(manifest_entry_type_str) > 0)
				{
					manifest_tile_type = SXNGN::ECS::A::manifest_string_to_type_map_().at(manifest_entry_type_str);
				}
				else
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Uknown manifest entry type_: %s", manifest_entry_type_str.c_str());
					continue;
				}

				switch (manifest_tile_type)
				{
				case SXNGN::ECS::A::TileManifestType::META_HEIGHT:
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
				case SXNGN::ECS::A::TileManifestType::META_WIDTH:
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
				case SXNGN::ECS::A::TileManifestType::TERRAIN:
				{
					if (manifest_entry.size() >= 4)
					{
						
						int idx = 1;
						std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
						int x_grid = std::stoi(manifest_entry.at(idx++));
						int y_grid = std::stoi(manifest_entry.at(idx++));

						//next could be tile name or specify an end point for tile snip, indicating this sprite is made up of multiple tiles
						std::string tile_name = manifest_entry.at(idx++);
						
						int sprite_size_w = 1;
						int sprite_size_h = 1;
						if (manifest_entry.size() > idx)
						{
							sprite_size_w = std::stoi(manifest_entry.at(idx++));
						}
						if (manifest_entry.size() > idx)
						{
							sprite_size_h = std::stoi(manifest_entry.at(idx++));
						}
						entry->x = (int) (x_grid * tile_width);
						entry->y = (int) (y_grid * tile_height);
						entry->w = (int) tile_width * sprite_size_w;
						entry->h = (int) tile_height * sprite_size_h;
						tile_name_string_to_rect_map[tile_name] = entry;
						
					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: TERRAIN");
					}
					break;
				}
				case SXNGN::ECS::A::TileManifestType::UNIT:
				{
					if (manifest_entry.size() == 4)
					{
						std::shared_ptr<SDL_Rect> entry = std::make_shared<SDL_Rect>();
						int x_grid = std::stoi(manifest_entry.at(1));
						int y_grid = std::stoi(manifest_entry.at(2));
						std::string tile_name = manifest_entry.at(3);
						entry->x = (int) (x_grid * tile_width);
						entry->y = (int) (y_grid * tile_height);
						entry->w = (int) tile_width;
						entry->h = (int) tile_height;
						tile_name_string_to_rect_map[tile_name] = entry;
					}
					else
					{
						SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: UNIT");
					}
					break;
				}
				case SXNGN::ECS::A::TileManifestType::META_SPRITE_SHEET_SOURCE:
				{
					if (manifest_entry.size() == 2)
					{
						std::string texture_path = SXNGN::BAD_STRING_RETURN;
						std::string relative_sprite_sheet_path = (manifest_entry.at(1));
						//find the tile sheet texture png. expect to find in the same folder as manifest.txt
						texture_path = Gameutils::get_file_in_folder(pre_factory.tile_manifest_path_, relative_sprite_sheet_path);
						if (texture_path == SXNGN::BAD_STRING_RETURN)
						{
							SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading TileManifest: META_SPRITE_SHEET_SOURCE: Bad texture path: %s", relative_sprite_sheet_path.c_str());
							abort();
						}

						bool texture_loaded_successfully = gCoordinator.get_texture_manager()->load_texture_from_path(pre_factory.name_, texture_path);
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


			//Destroy the Pre_Sprite_Factory Entity
			//Add the created Sprite_Factory to the Singleton SpriteFactoryHolder
			std::shared_ptr<SXNGN::ECS::A::Sprite_Factory> sprite_factory_component = std::make_shared<Sprite_Factory>();
			sprite_factory_component->sprite_factory_name_ = pre_factory.name_,
				sprite_factory_component->tile_height_ = tile_height;
			sprite_factory_component->tile_width_ = tile_width;
			//sprite_factory_component->tile_name_int_to_string_map_ = tile_name_int_to_string_map;
			sprite_factory_component->tile_name_string_to_rect_map_ = tile_name_string_to_rect_map;

			auto sprite_factory_holder = SpriteFactoryHolder::get_instance();
			sprite_factory_holder->sprite_factories_[pre_factory.name_] = sprite_factory_component;

			//gCoordinator.RemoveComponent(entity, pre_factory.get_component_type());
			gCoordinator.DestroyEntity(entity);

		}

		for (Entity entity_to_clean : entities_to_cleanup)
		{
			gCoordinator.DestroyEntity(entity_to_clean);
		}


	}
}
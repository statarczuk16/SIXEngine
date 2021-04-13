#include <ECS/Utilities/Entity_Builder_Utils.hpp>


namespace SXNGN {
	namespace ECS {
		namespace A {

			Entity Entity_Builder_Utils::Create_Tile(Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, A::CollisionType collision_type, std::string name)
			{
				auto tile_entity = coordinator.CreateEntity();
				Sint32 x_pixels = x_grid * SXNGN::BASE_TILE_WIDTH;
				Sint32 y_pixels = x_grid * SXNGN::BASE_TILE_HEIGHT;
				SXNGN::ECS::A::Pre_Renderable* pre_renderable = new SXNGN::ECS::A::Pre_Renderable(x_pixels, y_pixels, sprite_sheet, sprite_name, A::RenderLayer::GROUND_LAYER, name);
				coordinator.AddComponent(tile_entity, pre_renderable);

				SDL_Rect collision_box;
				collision_box.x = x_pixels;
				collision_box.y = y_pixels;
				collision_box.w = SXNGN::BASE_TILE_WIDTH;
				collision_box.h = SXNGN::BASE_TILE_HEIGHT;

				SXNGN::ECS::A::Collisionable* collisionable = Create_Collisionable(collision_box, collision_type);

				coordinator.AddComponent(tile_entity, collisionable);

				A::Tile* tile = new A::Tile(x_grid, y_grid);
				coordinator.AddComponent(tile_entity, tile);

				coordinator.AddComponent(tile_entity, Create_Gamestate_Component_from_Enum(game_state));

				return tile_entity;
			}

			Entity Entity_Builder_Utils::Create_Person(A::Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, bool player_controlled, std::string name)
			{
				auto person_entity = coordinator.CreateEntity();
				SDL_LogDebug(1, "Created Person Entity: %s: ID: %d", name.c_str(), person_entity);
				Sint32 x_pixels = x_grid * SXNGN::BASE_TILE_WIDTH;
				Sint32 y_pixels = x_grid * SXNGN::BASE_TILE_HEIGHT;
				SXNGN::ECS::A::Pre_Renderable* pre_renderable = new SXNGN::ECS::A::Pre_Renderable(x_pixels, y_pixels, sprite_sheet, sprite_name, A::RenderLayer::OBJECT_LAYER, name);
				coordinator.AddComponent(person_entity, pre_renderable);


				SXNGN::ECS::A::Moveable* moveable = Create_Moveable(x_pixels, y_pixels, 50, A::MoveableType::VELOCITY);
				coordinator.AddComponent(person_entity, moveable);

				SDL_Rect collision_box;
				collision_box.x = int(round(moveable->m_pos_x_m));
				collision_box.x = int(round(moveable->m_pos_y_m));
				collision_box.w = SXNGN::BASE_TILE_WIDTH;
				collision_box.h = SXNGN::BASE_TILE_HEIGHT;

				SXNGN::ECS::A::Collisionable* collisionable = Create_Collisionable(collision_box, CollisionType::ELASTIC, CollisionTag::PERSON);


				coordinator.AddComponent(person_entity, collisionable);
				SXNGN::ECS::A::User_Input_Tags_Collection* input_tags_comp = new A::User_Input_Tags_Collection();
				input_tags_comp->input_tags_.insert(SXNGN::ECS::A::User_Input_Tags::CLICKABLE);
				if (player_controlled)
				{
					
					input_tags_comp->input_tags_.insert(SXNGN::ECS::A::User_Input_Tags::WASD_CONTROL);
					input_tags_comp->input_tags_.insert(SXNGN::ECS::A::User_Input_Tags::PLAYER_CONTROL_MOVEMENT);
				}
				coordinator.AddComponent(person_entity, input_tags_comp);
				coordinator.AddComponent(person_entity, Create_Gamestate_Component_from_Enum(game_state));

				return person_entity;

			}

			Entity Entity_Builder_Utils::Create_Event(Coordinator coordinator, ComponentTypeEnum game_state, Event_Component trigger_event, std::string name)
			{
				auto event_entity = coordinator.CreateEntity();
				
				Event_Component* event_component = new Event_Component(trigger_event);
				coordinator.AddComponent(event_entity, event_component);
				coordinator.AddComponent(event_entity, Create_Gamestate_Component_from_Enum(game_state));
				return event_entity;
			}

			Entity Entity_Builder_Utils::Create_Mouse_Event(Coordinator coordinator, ComponentTypeEnum game_state, Click click)
			{
				auto event_entity = coordinator.CreateEntity();

				Event_Component* event_component = new Event_Component();
				SXNGN_MouseEvent mouse_event;
				mouse_event.click = click;
				event_component->e.common.type = EventType::MOUSE;
				event_component->e.mouse_event = mouse_event;
				event_component->e.mouse_event.type = MouseEventType::CLICK;

				Collisionable* collisionable = new Collisionable();
				SDL_Rect click_box;
				click_box.h = 1;
				click_box.w = 1;
				click_box.x = mouse_event.click.x / Database::get_scale();
				click_box.y = mouse_event.click.y / Database::get_scale();
				collisionable->collision_box_ = click_box;
				collisionable->collision_tag_ = CollisionTag::EVENT;
				collisionable->collision_type_ = CollisionType::IMMOVEABLE;
				collisionable->buffer_pixels = 0;
				coordinator.AddComponent(event_entity, collisionable);
				coordinator.AddComponent(event_entity, event_component);
				coordinator.AddComponent(event_entity, Create_Gamestate_Component_from_Enum(game_state));
				return event_entity;
			}

			Entity Entity_Builder_Utils::Create_Selection_Event(Coordinator coordinator, ComponentTypeEnum game_state, std::vector<Entity> clicked, std::vector<Entity> dclicked, std::vector<Entity> boxed )
			{
				auto select_entity = coordinator.CreateEntity();

				Event_Component* event_component = new Event_Component();
				SXNGN_Selection selection;
				selection.clicked_entities = clicked;
				selection.double_click_entities = dclicked;
				selection.boxed_entities = boxed;
				event_component->e.common.type = EventType::SELECTION;
				event_component->e.select_event = selection;
				coordinator.AddComponent(select_entity, event_component);
				coordinator.AddComponent(select_entity, Create_Gamestate_Component_from_Enum(game_state));
				return select_entity;
			}


			/**
			A::Pre_Renderable* Entity_Builder_Utils::Create_Pre_Renderable(Sint32 x, Sint32 y, std::string sprite_sheet, std::string sprite_name, A::RenderLayer render_layer)
			{
				SXNGN::ECS::A::Pre_Renderable* pre_renderable = new A::Pre_Renderable();
				pre_renderable->sprite_factory_name_ = sprite_sheet;
				pre_renderable->sprite_factory_sprite_type_ = sprite_name;
				pre_renderable->render_layer_ = render_layer;
				pre_renderable->x = x;
				pre_renderable->y = y;

				return pre_renderable;
			}
			**/
			A::Moveable* Entity_Builder_Utils::Create_Moveable(double pos_x_m, double pos_y_m, Sint32 speed_m_s, A::MoveableType movement_type)
			{
				SXNGN::ECS::A::Moveable* moveable = new A::Moveable();
				moveable->m_pos_x_m = pos_x_m;
				moveable->m_pos_y_m = pos_y_m;
				moveable->moveable_type_ = movement_type;
				moveable->m_speed_m_s = speed_m_s;
				return moveable;
			}

			Collisionable* Entity_Builder_Utils::Create_Collisionable(SDL_Rect collision_box, A::CollisionType collision_type, CollisionTag collision_tag)
			{
				A::Collisionable* collision = new A::Collisionable();
				collision->collision_box_ = collision_box;
				collision->collision_type_ = collision_type;
				collision->collision_tag_ = collision_tag;
				return collision;
			}

			A::Tile* Entity_Builder_Utils::Create_Tile(Sint32 x_grid, Sint32 y_grid)
			{
				A::Tile* tile = new A::Tile(x_grid, y_grid);
				return tile;
			}

			
		}
	}
}
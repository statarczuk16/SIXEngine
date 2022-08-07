#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <ECS/Utilities/ECS_Utils.hpp>


namespace SXNGN {
	namespace ECS {
		namespace A {

			Entity Entity_Builder_Utils::Create_Tile(Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, A::CollisionType collision_type, std::string name)
			{
				auto tile_entity = coordinator.CreateEntity();
				Sint32 x_pixels = x_grid * SXNGN::BASE_TILE_WIDTH;
				Sint32 y_pixels = y_grid * SXNGN::BASE_TILE_HEIGHT;
				SXNGN::ECS::A::Pre_Renderable* pre_renderable = new SXNGN::ECS::A::Pre_Renderable(sprite_sheet, sprite_name, A::RenderLayer::GROUND_LAYER, name);
				coordinator.AddComponent(tile_entity, pre_renderable);

				SDL_Rect collision_box;
				collision_box.x = x_pixels;
				collision_box.y = y_pixels;
				collision_box.w = SXNGN::BASE_TILE_WIDTH;
				collision_box.h = SXNGN::BASE_TILE_HEIGHT;

				Collisionable* collisionable = new Collisionable(collision_box.w, collision_box.h);

				coordinator.AddComponent(tile_entity, collisionable);

				Tile* tile = new Tile(-1);
				coordinator.AddComponent(tile_entity, tile);

				Location* location = new Location(collision_box.x, collision_box.y);

				coordinator.AddComponent(tile_entity, location);

				coordinator.AddComponent(tile_entity, collisionable);

				coordinator.AddComponent(tile_entity, Create_Gamestate_Component_from_Enum(game_state));

				return tile_entity;
			}

			Entity Entity_Builder_Utils::Create_Person(A::Coordinator coordinator, ComponentTypeEnum game_state, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, bool player_controlled, std::string name)
			{
				auto person_entity = coordinator.CreateEntity();
				SDL_LogDebug(1, "Created Person Entity: %s: ID: %d", name.c_str(), person_entity);
				Sint32 x_pixels = x_grid * SXNGN::BASE_TILE_WIDTH;
				Sint32 y_pixels = y_grid * SXNGN::BASE_TILE_HEIGHT;
				Pre_Renderable* pre_renderable = new SXNGN::ECS::A::Pre_Renderable(sprite_sheet, sprite_name, A::RenderLayer::OBJECT_LAYER, name);
				coordinator.AddComponent(person_entity, pre_renderable);


				Moveable* moveable = new Moveable(15);
				coordinator.AddComponent(person_entity, moveable);

				Collisionable* collisionable = new Collisionable(SXNGN::BASE_TILE_WIDTH/2.0 - 1, CollisionType::DYNAMIC);
				coordinator.AddComponent(person_entity, collisionable);

				Location* location = new Location(x_pixels, y_pixels);
				coordinator.AddComponent(person_entity, location);


				SXNGN::ECS::A::User_Input_Tags_Collection* input_tags_comp = new User_Input_Tags_Collection();
				input_tags_comp->input_tags_.insert(User_Input_Tags::MOUSE_CONTROL);
				if (player_controlled)
				{
					input_tags_comp->input_tags_.insert(User_Input_Tags::WASD_CONTROL);
					input_tags_comp->input_tags_.insert(User_Input_Tags::PLAYER_CONTROL_MOVEMENT);
				}
				coordinator.AddComponent(person_entity, input_tags_comp);

				Task_Worker_Component* worker = new Task_Worker_Component();
				worker->skill_levels_[TaskSkill::WALKING] = 5;
				worker->skill_enable_[TaskSkill::WALKING] = SkillPriority::MEDIUM;
				coordinator.AddComponent(person_entity, worker);


				coordinator.AddComponent(person_entity, Create_Gamestate_Component_from_Enum(game_state));

				Selectable* selectable = new Selectable();
				coordinator.AddComponent(person_entity, selectable);

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

			Entity Entity_Builder_Utils::Create_Mouse_Wheel_Event(Coordinator coordinator, ComponentTypeEnum game_state, int x, int y)
			{
				auto event_entity = coordinator.CreateEntity();
				Event_Component* event_component = new Event_Component();
				SXNGN_MouseWheelEvent mouse_wheel_event;
				mouse_wheel_event.x_ = x;
				mouse_wheel_event.y_ = y;
				event_component->e.common.type = EventType::MOUSE_WHEEL;
				event_component->e.mouse_wheel_event = mouse_wheel_event;

				coordinator.AddComponent(event_entity, event_component);
				coordinator.AddComponent(event_entity, Create_Gamestate_Component_from_Enum(game_state));
				return event_entity;
			}

			Entity Entity_Builder_Utils::Create_Mouse_Event(Coordinator coordinator, ComponentTypeEnum game_state, Click click, bool shift_click, bool alt_click, bool ctrl_click)
			{
				auto event_entity = coordinator.CreateEntity();
				std::shared_ptr<ECS_Camera> camera_ptr = ECS_Camera::get_instance();
				Event_Component* event_component = new Event_Component();
				SXNGN_MouseEvent mouse_event;
				mouse_event.click = click;
				mouse_event.alt_click = alt_click;
				mouse_event.shift_click = shift_click;
				mouse_event.ctrl_click = ctrl_click;
				event_component->e.common.type = EventType::MOUSE;
				event_component->e.mouse_event = mouse_event;
				event_component->e.mouse_event.type = MouseEventType::CLICK;

				Collisionable* collisionable = new Collisionable();

				//mouse events take place in the game world. If a mouse click were to hit the main UI, it would not have produced a mouse event, rather been handled in the UserInputSystem
				SDL_FRect mouse_click_world_position = ECS_Utils::convert_screen_position_to_world_position(camera_ptr, mouse_event.click.x, mouse_event.click.y);
				
				collisionable->width_ = 1;
				collisionable->height_ = 1;
				//collisionable->collision_box_ = mouse_click_world_position;
				collisionable->collision_tag_ = CollisionTag::EVENT;
				collisionable->collision_type_ = CollisionType::DYNAMIC;
				collisionable->buffer_pixels = 0;

				Location* location = new Location(mouse_click_world_position.x, mouse_click_world_position.y);
				coordinator.AddComponent(event_entity, collisionable);
				coordinator.AddComponent(event_entity, location);
				coordinator.AddComponent(event_entity, event_component);
				coordinator.AddComponent(event_entity, Create_Gamestate_Component_from_Enum(game_state));
				return event_entity;
			}

			Entity Entity_Builder_Utils::Create_Selection_Event(Coordinator coordinator, ComponentTypeEnum game_state, std::vector<Entity> clicked, std::vector<Entity> dclicked, std::vector<Entity> boxed, bool additive, bool subtractive, bool enqueue)
			{
				auto select_entity = coordinator.CreateEntity();

				Event_Component* event_component = new Event_Component();
				SXNGN_Selection selection;
				selection.clicked_entities = clicked;
				selection.double_click_entities = dclicked;
				selection.boxed_entities = boxed;
				selection.additive = additive;
				selection.subtractive = subtractive;
				selection.enqueue = enqueue;
				event_component->e.common.type = EventType::SELECTION;
				event_component->e.select_event = selection;
				coordinator.AddComponent(select_entity, event_component);
				coordinator.AddComponent(select_entity, Create_Gamestate_Component_from_Enum(game_state));
				return select_entity;
			}

			Entity Entity_Builder_Utils::Create_Order_Event(Coordinator coordinator, ComponentTypeEnum game_state, OrderType order_type, std::vector<Entity> clicked, std::vector<Entity> dclicked, std::vector<Entity> boxed, bool additive, bool subtractive, bool enqueue)
			{
				auto order_event_entity = coordinator.CreateEntity();

				Event_Component* event_component = new Event_Component();
				SXNGN_Order order;
				order.clicked_entities = clicked;
				order.double_click_entities = dclicked;
				order.boxed_entities = boxed;
				order.additive = additive;
				order.subtractive = subtractive;
				order.enqueue = enqueue;
				order.order_type = order_type;
				event_component->e.common.type = EventType::ORDER;
				event_component->e.order_event = order;
				coordinator.AddComponent(order_event_entity, event_component);
				coordinator.AddComponent(order_event_entity, Create_Gamestate_Component_from_Enum(game_state));
				return order_event_entity;
			}

			Entity Entity_Builder_Utils::Create_Spawn_Event(Coordinator coordinator, ComponentTypeEnum game_state, int pixel_x, int pixel_y)
			{
				auto spawn_event_entity = coordinator.CreateEntity();

				Event_Component* event_component = new Event_Component();
				SXNGN_SpawnEvent spawn;
				spawn.x_ = pixel_x;
				spawn.y_ = pixel_y;
				event_component->e.common.type = EventType::SPAWN;
				event_component->e.spawn_event = spawn;
				coordinator.AddComponent(spawn_event_entity, event_component);
				coordinator.AddComponent(spawn_event_entity, Create_Gamestate_Component_from_Enum(game_state));
				return spawn_event_entity;
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
			
			A::Moveable* Entity_Builder_Utils::Create_Moveable(double pos_x_m, double pos_y_m, Sint32 speed_m_s, A::MoveableType movement_type)
			{
				SXNGN::ECS::A::Moveable* moveable = new A::Moveable();
				moveable->UpdatePosition(pos_x_m, pos_y_m);
				moveable->m_intended_delta_x_m = 0;
				moveable->m_intended_delta_y_m = 0;
				moveable->moveable_type_ = movement_type;
				moveable->m_speed_m_s = speed_m_s;
				return moveable;
			}

			Collisionable* Entity_Builder_Utils::Create_Collisionable_Square(int width, int height, A::CollisionType collision_type, CollisionTag collision_tag)
			{
				Collisionable* collision = new Collisionable();
				collision->width_ = width;
				collision->height_ = height;
				collision->collision_type_ = collision_type;
				collision->collision_tag_ = collision_tag;
				return collision;
			}

			A::Tile* Entity_Builder_Utils::Create_Tile(Sint32 x_grid, Sint32 y_grid)
			{
				A::Tile* tile = new A::Tile(x_grid, y_grid);
				return tile;
			}
			**/

			
		}
	}
}
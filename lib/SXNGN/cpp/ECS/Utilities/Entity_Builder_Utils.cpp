#include <ECS/Utilities/Entity_Builder_Utils.hpp>


namespace SXNGN {
	namespace ECS {

		Entity Entity_Builder_Utils::Create_Tile(Coordinator coordinator, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, Components::CollisionType collision_type, std::string name )
		{
			auto tile_entity = coordinator.CreateEntity();
			Sint32 x_pixels = x_grid * SXNGN::BASE_TILE_WIDTH;
			Sint32 y_pixels = x_grid * SXNGN::BASE_TILE_HEIGHT;
			SXNGN::ECS::Components::Pre_Renderable* pre_renderable = new SXNGN::ECS::Components::Pre_Renderable(x_pixels, y_pixels, sprite_sheet, sprite_name, Components::RenderLayer::GROUND_LAYER, name);
			coordinator.AddComponent(tile_entity, pre_renderable);

			SDL_Rect collision_box;
			collision_box.x = x_pixels;
			collision_box.y = y_pixels;
			collision_box.w = SXNGN::BASE_TILE_WIDTH;
			collision_box.h = SXNGN::BASE_TILE_HEIGHT;

			SXNGN::ECS::Components::Collisionable* collisionable = Create_Collisionable(collision_box, collision_type);

			coordinator.AddComponent(tile_entity, collisionable);

			Components::Tile* tile = new Components::Tile(x_grid, y_grid);
			coordinator.AddComponent(tile_entity, tile);

			return tile_entity;
		}

		Entity Entity_Builder_Utils::Create_Person(Coordinator coordinator, Sint32 x_grid, Sint32 y_grid, std::string sprite_sheet, std::string sprite_name, bool player_controlled, std::string name)
		{
			auto person_entity = coordinator.CreateEntity();
			Sint32 x_pixels = x_grid * SXNGN::BASE_TILE_WIDTH;
			Sint32 y_pixels = x_grid * SXNGN::BASE_TILE_HEIGHT;
			SXNGN::ECS::Components::Pre_Renderable* pre_renderable = new SXNGN::ECS::Components::Pre_Renderable(x_pixels, y_pixels, sprite_sheet, sprite_name, Components::RenderLayer::OBJECT_LAYER, name);
			coordinator.AddComponent(person_entity, pre_renderable);


			SXNGN::ECS::Components::Moveable* moveable = Create_Moveable(x_pixels, y_pixels, 7, Components::MoveableType::VELOCITY);
			coordinator.AddComponent(person_entity, moveable);

			SDL_Rect collision_box;
			collision_box.x = int(round(moveable->m_pos_x_m));
			collision_box.x = int(round(moveable->m_pos_y_m));
			collision_box.w = SXNGN::BASE_TILE_WIDTH;
			collision_box.w = SXNGN::BASE_TILE_HEIGHT;

			SXNGN::ECS::Components::Collisionable* collisionable = Create_Collisionable(collision_box, Components::CollisionType::IMMOVEABLE);

			coordinator.AddComponent(person_entity, collisionable);
			if (player_controlled)
			{
				SXNGN::ECS::Components::User_Input_Tags_Collection* input_tags_comp = new Components::User_Input_Tags_Collection();
				input_tags_comp->input_tags_.insert(SXNGN::ECS::Components::User_Input_Tags::WASD_CONTROL);
				input_tags_comp->input_tags_.insert(SXNGN::ECS::Components::User_Input_Tags::PLAYER_CONTROL_MOVEMENT);
				coordinator.AddComponent(person_entity, input_tags_comp);

			}

			return person_entity;
		
		}

		/**
		Components::Pre_Renderable* Entity_Builder_Utils::Create_Pre_Renderable(Sint32 x, Sint32 y, std::string sprite_sheet, std::string sprite_name, Components::RenderLayer render_layer)
		{
			SXNGN::ECS::Components::Pre_Renderable* pre_renderable = new Components::Pre_Renderable();
			pre_renderable->sprite_factory_name_ = sprite_sheet;
			pre_renderable->sprite_factory_sprite_type_ = sprite_name;
			pre_renderable->render_layer_ = render_layer;
			pre_renderable->x = x;
			pre_renderable->y = y;

			return pre_renderable;
		}
		**/
		Components::Moveable* Entity_Builder_Utils::Create_Moveable(double pos_x_m, double pos_y_m, Sint32 speed_m_s, Components::MoveableType movement_type)
		{
			SXNGN::ECS::Components::Moveable* moveable = new Components::Moveable();
			moveable->m_pos_x_m = pos_x_m;
			moveable->m_pos_y_m = pos_y_m;
			moveable->moveable_type_ = movement_type;
			moveable->m_speed_m_s = speed_m_s;
			return moveable;
		}

		Components::Collisionable* Entity_Builder_Utils::Create_Collisionable(SDL_Rect collision_box, Components::CollisionType collision_type)
		{
			Components::Collisionable* collision = new Components::Collisionable();
			collision->collision_box_ = collision_box;
			collision->collision_type_ = collision_type;
			return collision;
		}

		Components::Tile* Entity_Builder_Utils::Create_Tile(Sint32 x_grid, Sint32 y_grid)
		{
			Components::Tile* tile = new Components::Tile(x_grid, y_grid);
			return tile;
		 }
	}
}
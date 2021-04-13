#include <ECS/Utilities/ECS_Utils.hpp>

namespace SXNGN {
	namespace ECS {
		namespace A {


			std::shared_ptr<SDL_Rect> ECS_Utils::GetEntityPosition(Entity entity)
			{
				auto gCoordinator = *SXNGN::Database::get_coordinator();
				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::MOVEABLE))
				{
					auto moveable_data = gCoordinator.GetComponentReadOnly(entity, ComponentTypeEnum::MOVEABLE);
					if (moveable_data)
					{
						const Moveable* moveable = static_cast<const Moveable*>(moveable_data);
						std::shared_ptr<SDL_Rect> pos = std::make_shared<SDL_Rect>();
						pos->x = moveable->m_pos_x_m;
						pos->y = moveable->m_pos_y_m;
						return pos;

					}
				}
				return nullptr;
			 }

			void ECS_Utils::ChangeEntityPositionLastGood(Entity entity)
			{
				auto gCoordinator = *SXNGN::Database::get_coordinator();

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::MOVEABLE))
				{
					auto moveable_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::MOVEABLE);
					if (moveable_data)
					{
						Moveable* moveable = static_cast<Moveable*>(moveable_data);
						auto last_x = moveable->m_prev_pos_x_m;
						auto last_y = moveable->m_prev_pos_y_m;

						gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity);

						ChangeEntityPosition(entity, last_x, last_y, true);
					}
				}
			}

			void ECS_Utils::ChangeEntityPosition(Entity entity, double x, double y, bool confident)
			{
				
				auto gCoordinator = *SXNGN::Database::get_coordinator();

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::MOVEABLE))
				{
					auto moveable_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::MOVEABLE);
					if (moveable_data)
					{
						Moveable* moveable = static_cast<Moveable*>(moveable_data);
						if (moveable->m_pos_x_m == x && moveable->m_pos_y_m == y)
						{
							gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity);
							return;
						}
						if (!confident)
						{
							moveable->m_prev_pos_x_m = moveable->m_pos_x_m;
							moveable->m_prev_pos_y_m = moveable->m_pos_y_m;
						}
						
						moveable->m_pos_x_m = x;
						moveable->m_pos_y_m = y;

						gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, entity);
					}
				}

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::RENDERABLE))
				{
					auto moveable_renderbox = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::RENDERABLE);
					if (moveable_renderbox)
					{
						Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
						render_ptr->x_ = int(round(x));
						render_ptr->y_ = int(round(y));
						gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, entity);
					}
				}

				if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::COLLISION))
				{
					auto collisionable_data = gCoordinator.CheckOutComponent(entity, ComponentTypeEnum::COLLISION);
					if (collisionable_data)
					{
						Collisionable* collision_ptr = static_cast<Collisionable*>(collisionable_data);
						collision_ptr->collision_box_.x = int(round(x));
						collision_ptr->collision_box_.y = int(round(y));
						collision_ptr->resolved_ = false;
						gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, entity);
					}
				}
			}
		}
	}
}
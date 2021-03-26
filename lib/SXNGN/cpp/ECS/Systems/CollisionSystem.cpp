
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/CollisionSystem.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <Collision.h>

namespace SXNGN::ECS::A {

	void Collision_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Event_System Init");
	}

	void Collision_System::Update(float dt)
	{

		auto gCoordinator = *SXNGN::Database::get_coordinator();
		std::vector<Entity> entities_to_cleanup;

		//Iterate through entities this system manipulates/converts
		auto it_act = m_actable_entities.begin();
		int idx = 0;
		std::deque <std::pair<Entity,const Collisionable*>> entity_to_collisionable;
		std::deque<std::pair<Entity, Entity>> confirmed_collisions;
		//actable entities for event system are entities with event component
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;

			//thread safe checkout of data
			auto data = gCoordinator.GetComponentReadOnly(entity_actable, ComponentTypeEnum::COLLISION);
			const Collisionable* collisionable_ptr = static_cast<const Collisionable*>(data);
			entity_to_collisionable.push_back(std::make_pair(idx, collisionable_ptr));

			idx++;
		}

		for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_a = entity_to_collisionable.begin(); it_coll_a != entity_to_collisionable.end()-1; ++it_coll_a)
		{ 
			for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_b = it_coll_a+1; it_coll_b != entity_to_collisionable.end(); ++it_coll_b)
			{
				int collision_buffer_pixels = (*it_coll_a).second->buffer_pixels + (*it_coll_a).second->buffer_pixels;
				if (CollisionChecks::checkCollisionBuffer((*it_coll_a).second->collision_box_, (*it_coll_a).second->collision_box_, collision_buffer_pixels))
				{
					confirmed_collisions.push_back(std::make_pair((*it_coll_a).first, (*it_coll_b).first));
				}
			}
		}

		for (auto c_collision : confirmed_collisions)
		{

			auto data1 = gCoordinator.CheckOutComponent(c_collision.first, ComponentTypeEnum::COLLISION);
			auto data2 = gCoordinator.CheckOutComponent(c_collision.second, ComponentTypeEnum::COLLISION);
			if (data1.first && data2.first)
			{
				Collisionable* collisionable_1 = static_cast<Collisionable*>(data1.first);
				Collisionable* collisionable_2 = static_cast<Collisionable*>(data2.first);

				//if the first collisionable is an event collision
				if (collisionable_1->collision_tag_ == CollisionTag::EVENT)
				{
					//get the entity's event component
					auto event_data = gCoordinator.GetComponentReadOnly(c_collision.first, ComponentTypeEnum::EVENT);
					if (event_data)
					{
						
						const Event_Component* event_ptr = static_cast<const Event_Component*>(event_data);
						//if the event is a mouse click
						if (event_ptr->e.common.type == EventType::MOUSE)
						{
							//see how second collisionable reacts to mouse input
							auto input_data = gCoordinator.GetComponentReadOnly(c_collision.second, ComponentTypeEnum::INPUT_TAGS);
							if (input_data)
							{ 
								const User_Input_Tags_Collection* input_tags_ptr = static_cast<const User_Input_Tags_Collection*>(input_data);
								//if it is clickable
								std::vector<Entity> clicked, std::vector<Entity> double_click_entities, std::vector<Entity> boxed_entities;
								if (input_tags_ptr->input_tags_.count(User_Input_Tags::CLICKABLE))
								{
									switch (event_ptr->e.mouse_event.type)
									{
										//if it's a click
										case MouseEventType::CLICK:
										{
											if (event_ptr->e.mouse_event.click.double_click)
											{
												double_click_entities.push_back(c_collision.second);
											}
											else
											{
												clicked.push_back(c_collision.second);
											}
								
											break;
										}
										//or selection box
										case MouseEventType::BOX:
										{
											boxed_entities.push_back(c_collision.second);
											break;
										}
										default :
										{
											SDL_LogCritical(1, "CollisionSystem: Unknown Mouse Collision Event");
											abort();
										}	
								}
									Entity_Builder_Utils::Create_Selection_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, clicked, double_click_entities, boxed_entities);
							}
						}
					}

					if (collisionable_2->collision_tag_ == CollisionTag::EVENT)
					{

					}
					else if (collisionable_2->collision_tag_ == CollisionTag::PERSON)
					{

					}
				}


			}
		}



	}
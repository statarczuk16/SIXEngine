
#include<Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/CollisionSystem.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <Collision.h>
#include <vector>
#include <ECS/Utilities/ECS_Utils.hpp>

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
		std::deque <std::pair<Entity, const Collisionable*>> entity_to_collisionable_unresolved;
		std::deque <std::pair<Entity, const Collisionable*>> entity_to_collisionable_all;
		std::deque<std::pair<Entity, Entity>> confirmed_collisions;
		std::deque<std::pair<Entity, Entity>> confirmed_collisions_2;
		//actable entities for event system are entities with event component
		while (it_act != m_actable_entities.end())
		{
			auto const& entity_actable = *it_act;
			it_act++;


			//draw distinction between resolved and unresolved collisionables. We don't need to check collision of resolved collisions - ones that we have already detected collision between and they have not moved (become "unresolved") since the check
			auto data = gCoordinator.GetComponentReadOnly(entity_actable, ComponentTypeEnum::COLLISION);
			const Collisionable* collisionable_ptr = static_cast<const Collisionable*>(data);
			if (collisionable_ptr->resolved_ == false)
			{
				entity_to_collisionable_unresolved.push_back(std::make_pair(entity_actable, collisionable_ptr));
			}
			entity_to_collisionable_all.push_back(std::make_pair(entity_actable, collisionable_ptr));

		}


		if (!entity_to_collisionable_unresolved.empty())
		{
			int check_num = 0;
			auto start = std::chrono::high_resolution_clock::now();
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
			if (entity_to_collisionable_unresolved.size() > 500)
			{
				//method 1: iterate through all collisionables, but only do the check if one is unresolved.
				//Advantage: since inner and outer loop use the same index, can place inner loop iterator at out loop iterator + 1 and cut out all redundant checks
				// (if A hitting B, don't need to check B hitting A)
				//Disadvantage: Have to at least *iterate* through a lot of resolved collisions, possibly, even if we don't have to check the collision.
				//Should work better when there are many unresolved (moving) objects
				check_num = 0;
				start = std::chrono::high_resolution_clock::now();
				for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_a = entity_to_collisionable_all.begin(); it_coll_a != entity_to_collisionable_all.end() - 1; ++it_coll_a)
				{
					for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_b = it_coll_a + 1; it_coll_b != entity_to_collisionable_all.end(); ++it_coll_b)
					{
						if ((*it_coll_a).second->resolved_ == true && (*it_coll_b).second->resolved_ == true)
						{
							continue;
						}
						int collision_buffer_pixels = (*it_coll_a).second->buffer_pixels + (*it_coll_b).second->buffer_pixels;
						//does obj a touch obj b
						if (CollisionChecks::checkCollisionBuffer((*it_coll_a).second->collision_box_, (*it_coll_b).second->collision_box_, collision_buffer_pixels))
						{
							confirmed_collisions.push_back(std::make_pair((*it_coll_a).first, (*it_coll_b).first));

						}
						check_num++;
					}
				}
				stop = std::chrono::high_resolution_clock::now();
				if (true)//(!quiet)
				{
					duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
					//SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Collision Method 1: Checked %d Unresolved Collisionables in %d checks and found %d collisions in %d ms", entity_to_collisionable_unresolved.size(), check_num, confirmed_collisions.size(), duration);
				}
			}
			else
			{
				//method 2: only iterate through unresolved collisionables, though check them for collision against ALL collisionables
				//Advantage: Small outer loop to iterate through (only unresolved collisions)
				//Disadvantage: Does not cut out redundant checks because indexes aren't the same (see advantage for above method)
				//Should work better when there are not many unresolved collisions (moving objects)
				check_num = 0;
				start = std::chrono::high_resolution_clock::now();
				for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_a = entity_to_collisionable_unresolved.begin(); it_coll_a != entity_to_collisionable_unresolved.end(); ++it_coll_a)
				{
					for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_b = entity_to_collisionable_all.begin(); it_coll_b != entity_to_collisionable_all.end(); ++it_coll_b)
					{
						if ((*it_coll_a).first == (*it_coll_b).first) //don't check collision against self
						{
							continue;
						}
						int collision_buffer_pixels = (*it_coll_a).second->buffer_pixels + (*it_coll_b).second->buffer_pixels;
						//does obj a touch obj b
						if (CollisionChecks::checkCollisionBuffer((*it_coll_a).second->collision_box_, (*it_coll_b).second->collision_box_, collision_buffer_pixels))
						{
							//SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Collision Method 2: Entity %d hit Entity %d", (*it_coll_a).first, (*it_coll_b).first);

							confirmed_collisions.push_back(std::make_pair((*it_coll_a).first, (*it_coll_b).first));

						}
						check_num++;
					}
				}
				stop = std::chrono::high_resolution_clock::now();
				if (true)//(!quiet)
				{
					duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
					//SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Collision Method 2: Checked %d Unresolved Collisionables in %d checks and found %d collisions in %d ms", entity_to_collisionable_unresolved.size(), check_num, confirmed_collisions_2.size(), duration);
				}
			}
		}

		std::vector<Entity> collisonables_to_delete;
		for (auto c_collision : confirmed_collisions)
		{

			auto data1 = gCoordinator.CheckOutComponent(c_collision.first, ComponentTypeEnum::COLLISION);
			auto data2 = gCoordinator.CheckOutComponent(c_collision.second, ComponentTypeEnum::COLLISION);

			if (data1 && data2)
			{
				Collisionable* collisionable_1 = static_cast<Collisionable*>(data1);
				Collisionable* collisionable_2 = static_cast<Collisionable*>(data2);
				Entity entity_1 = c_collision.first;
				Entity entity_2 = c_collision.second;
				int collision_result = 0;

				//if the first collisionable is an event collision
				
				if (collisionable_1->collision_tag_ == CollisionTag::EVENT || collisionable_2->collision_tag_ == CollisionTag::EVENT)
				{
					if (collisionable_1->collision_tag_ == CollisionTag::EVENT)
					{
						collision_result = HandleCollisionEvent(collisionable_1, entity_1, collisionable_2, entity_2, gCoordinator);
					}
					else
					{
						collision_result = HandleCollisionEvent(collisionable_2, entity_2, collisionable_1, entity_1, gCoordinator);
					}
				}
				if (collisionable_1->collision_tag_ == CollisionTag::UNKNOWN || collisionable_2->collision_tag_ == CollisionTag::UNKNOWN)
				{
					collision_result = HandleCollisionGeneric(collisionable_2, entity_2, collisionable_1, entity_1, gCoordinator);
				}

				else if (collisionable_1->collision_tag_ == CollisionTag::PERSON || collisionable_2->collision_tag_ == CollisionTag::PERSON)
				{
					if (collisionable_1->collision_tag_ == CollisionTag::PERSON && collisionable_2->collision_tag_ == CollisionTag::PERSON)
					{
						collision_result = HandleCollisionPersonPerson(collisionable_1, entity_1, collisionable_2, entity_2, gCoordinator);
					}
					else if (collisionable_1->collision_tag_ == CollisionTag::PROJECTILE)
					{
						collision_result = HandleCollisionPersonProjectile(collisionable_1, entity_1, collisionable_2, entity_2, gCoordinator);
					}
					else if (collisionable_2->collision_tag_ == CollisionTag::PROJECTILE)
					{
						collision_result = HandleCollisionPersonProjectile(collisionable_2, entity_2, collisionable_1, entity_1, gCoordinator);
					}
					else if (collisionable_1->collision_tag_ == CollisionTag::TILE)
					{
						collision_result = HandleCollisionPersonTile(collisionable_1, entity_1, collisionable_2, entity_2, gCoordinator);
					}
					else if (collisionable_2->collision_tag_ == CollisionTag::TILE)
					{
						collision_result = HandleCollisionPersonTile(collisionable_2, entity_2, collisionable_1, entity_1, gCoordinator);
					}
					else if (collisionable_1->collision_tag_ == CollisionTag::OBJECT)
					{
						collision_result = HandleCollisionPersonObject(collisionable_1, entity_1, collisionable_2, entity_2, gCoordinator);
					}
					else if (collisionable_2->collision_tag_ == CollisionTag::OBJECT)
					{
					collision_result = HandleCollisionPersonObject(collisionable_2, entity_2, collisionable_1, entity_1, gCoordinator);
					}
				}


				//check the data back in
				gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, c_collision.first);
				gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, c_collision.second);
				if (collision_result == 1)
				{
					collisonables_to_delete.push_back(c_collision.first);
				}
				else if (collision_result == 2)
				{
					collisonables_to_delete.push_back(c_collision.second);
				}
				else if (collision_result == 3)
				{
					collisonables_to_delete.push_back(c_collision.first);
					collisonables_to_delete.push_back(c_collision.second);
				}


			}//if both collisionables exist


		}//for all confiermed collisions

		for (auto entity : collisonables_to_delete)
		{
			gCoordinator.DestroyEntity(entity);
		}
	}


	int Collision_System::HandleCollisionEvent(Collisionable* event, Entity event_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator)
	{
		//get the entity's event component
		auto event_data = gCoordinator.GetComponentReadOnly(event_entity, ComponentTypeEnum::EVENT);
		if (event_data)
		{

			const Event_Component* event_ptr = static_cast<const Event_Component*>(event_data);
			//if the event is a mouse click and the other entity has input tags
			if (event_ptr->e.common.type == EventType::MOUSE && gCoordinator.EntityHasComponent(other_entity, ComponentTypeEnum::INPUT_TAGS))
			{
				//see how second collisionable reacts to mouse input
				auto input_data = gCoordinator.GetComponentReadOnly(other_entity, ComponentTypeEnum::INPUT_TAGS);

				const User_Input_Tags_Collection* input_tags_ptr = static_cast<const User_Input_Tags_Collection*>(input_data);
				//if it is clickable
				std::vector<Entity> clicked;
				std::vector<Entity> double_click_entities;
				std::vector<Entity> boxed_entities;
				if (input_tags_ptr && input_tags_ptr->input_tags_.count(User_Input_Tags::MOUSE_CONTROL))
				{
					switch (event_ptr->e.mouse_event.type)
					{
						//if it's a click
					case MouseEventType::CLICK:
					{

						if (event_ptr->e.mouse_event.click.double_click)
						{
							SDL_LogDebug(1, "Entity %d Double Clicked\n", other_entity);
							double_click_entities.push_back(other_entity);
						}
						else
						{
							SDL_LogDebug(1, "Entity %d Clicked\n", other_entity);
							clicked.push_back(other_entity);
						}

						break;
					}
					//or selection box
					case MouseEventType::BOX:
					{
						SDL_LogDebug(1, "Entity %d Mouse Boxed\n", other_entity);
						boxed_entities.push_back(other_entity);
						break;
					}
					default:
					{
						SDL_LogCritical(1, "CollisionSystem: Unknown Mouse Collision Event");
						abort();
					}
					}//switch mouse event type

					bool additive = false;
					bool subtractive = false;
					bool enqueue = false;
					//priority of modified mouse clicks in this order
					if (event_ptr->e.mouse_event.shift_click)
					{
						enqueue = true;
					}
					else if (event_ptr->e.mouse_event.ctrl_click)
					{
						additive = true;
					}
					else if (event_ptr->e.mouse_event.alt_click)
					{
						subtractive = true;
					}

					//dragged box or left click is selection
					if (event_ptr->e.mouse_event.type == MouseEventType::BOX || event_ptr->e.mouse_event.click.button == MOUSE_BUTTON::LEFT)
					{
						//create event for user input system - tell it what entities were selected by a mouse event
						Entity_Builder_Utils::Create_Selection_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, clicked, double_click_entities, boxed_entities, additive, subtractive, enqueue);
					}
					//right click is an order 
					else if (event_ptr->e.mouse_event.click.button == MOUSE_BUTTON::RIGHT)
					{
						//create event for user input system - tell it what the target of the order is 
						//todo different types of orders besides MOVE
						Entity_Builder_Utils::Create_Order_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, OrderType::MOVE, clicked, double_click_entities, boxed_entities, additive, subtractive, enqueue);
					}
					
					event->resolved_ = true;
				}//if clickable
			}//if mouse event type
		} //if event data present
		return 0;

	}//namespace


	int Collision_System::HandleCollisionPersonPerson(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator)
	{
		//todo if same faction, let pass, else do not.
		person->resolved_ = true;
		other->resolved_ = true;
		return 0;

	}

	int Collision_System::HandleCollisionPersonProjectile(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator)
	{
		//todo 
		person->resolved_ = true;
		other->resolved_ = true;
		return 2;
	}

	int Collision_System::HandleCollisionPersonTile(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator)
	{
		//todo 
		person->resolved_ = true;
		other->resolved_ = true;
		if (other->collision_type_ == CollisionType::IMMOVEABLE)
		{
			auto person_moveable = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::MOVEABLE);
			if (person_moveable)
			{
				Moveable* moveable = static_cast<Moveable*>(person_moveable);
				moveable->m_pos_x_m = moveable->m_prev_pos_x_m;
				moveable->m_pos_y_m = moveable->m_prev_pos_y_m;
				if (gCoordinator.EntityHasComponent(person_entity, ComponentTypeEnum::RENDERABLE))
				{
					auto moveable_renderbox = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::RENDERABLE);
					if (moveable_renderbox)
					{
						Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
						render_ptr->x_ = int(round(moveable->m_pos_x_m));
						render_ptr->y_ = int(round(moveable->m_pos_y_m));
						gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, person_entity);
					}
				}

			}
		}
		return 0;
	}

	int Collision_System::HandleCollisionPersonObject(Collisionable* person, Entity person_entity, Collisionable* other, Entity other_entity, Coordinator gCoordinator)
	{
		//todo 
		person->resolved_ = true;
		other->resolved_ = true;
		if (other->collision_type_ == CollisionType::IMMOVEABLE)
		{
			auto person_moveable = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::MOVEABLE);
			if (person_moveable)
			{
				Moveable* moveable = static_cast<Moveable*>(person_moveable);
				moveable->m_pos_x_m = moveable->m_prev_pos_x_m;
				moveable->m_pos_y_m = moveable->m_prev_pos_y_m;
				if (gCoordinator.EntityHasComponent(person_entity, ComponentTypeEnum::RENDERABLE))
				{
					auto moveable_renderbox = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::RENDERABLE);
					if (moveable_renderbox)
					{
						Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
						render_ptr->x_ = int(round(moveable->m_pos_x_m));
						render_ptr->y_ = int(round(moveable->m_pos_y_m));
						gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, person_entity);
					}
				}

			}
		}
		return 0;
	}

	//catch all, inefficient, method to handle collisions between types that don't categorize into other methods
	//simple for now. Only handles when one moving object hits an immoveable object and calculates where to stop the moveable object
	int Collision_System::HandleCollisionGeneric(Collisionable* first, Entity first_entity, Collisionable* second, Entity second_entity, Coordinator gCoordinator)
	{
		//for next frame, mark that the collisionson these objects have been handled (will be true at end of this function)
		first->resolved_ = true;
		second->resolved_ = true;
		Collisionable* immoveable_obj = nullptr;
		Collisionable* moveable_obj = nullptr;
		Entity immoveable_entity;
		Entity moveable_entity;
		//one of the collisionables is going to be treated as immoveable. not supporting stuff bouncing off each other yet
		if (first->collision_type_ == CollisionType::IMMOVEABLE)
		{
			immoveable_entity = first_entity;
			immoveable_obj = first;
		}
		else if (second->collision_type_ == CollisionType::IMMOVEABLE)
		{
			immoveable_entity = second_entity;
			immoveable_obj = second;
		}
		//if one of the objects is immoveable
		if (immoveable_obj)
		{
			//determine which object is the moveable one 
			if (first->collision_type_ != CollisionType::IMMOVEABLE && first->collision_type_ != CollisionType::NONE)
			{
				moveable_entity = first_entity;
				moveable_obj = first;
			}
			else if (second->collision_type_ != CollisionType::IMMOVEABLE && second->collision_type_ != CollisionType::NONE)
			{
				moveable_entity = second_entity;
				moveable_obj = second;
			}

			if (moveable_obj)
			{
				gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, moveable_entity);
				//ECS_Utils::ChangeEntityPositionLastGood(moveable_entity);
				auto prev_moveable_data = gCoordinator.GetComponentReadOnly(moveable_entity, ComponentTypeEnum::MOVEABLE);
				const Moveable* prev_moveable = static_cast<const Moveable*>(prev_moveable_data);
				SDL_Rect prev_moveable_box;
				prev_moveable_box.x = prev_moveable->m_prev_pos_x_m;
				prev_moveable_box.y = prev_moveable->m_prev_pos_y_m;
				prev_moveable_box.w = moveable_obj->collision_box_.w;
				prev_moveable_box.h = moveable_obj->collision_box_.h;
				//get where the moveable was last frame, where it wants to be this frame, and combine with immoveable obj location to find where it should smack into a wall and stop
				SDL_Rect new_moveable_pos = CollisionChecks::getCollisionLocation(immoveable_obj->collision_box_, moveable_obj->collision_box_, prev_moveable_box);
				//move the moveable entity to where it should smack into a wall
				ECS_Utils::ChangeEntityPosition(moveable_entity, new_moveable_pos.x, new_moveable_pos.y, true);
				//calling function expects to need to check moveable_entity back in, but ChangeEntityPosition also does that. So re-check it out here.
				auto dummy = gCoordinator.CheckOutComponent(moveable_entity, ComponentTypeEnum::COLLISION);
				
			}
		}
		return 0;
	}
}

#define NOMINMAX
#include <Collision.h>
#include <ECS/Core/Coordinator.hpp>
#include <Database.h>
#include <ECS/Systems/CollisionSystem.hpp>
#include <ECS/Utilities/Entity_Builder_Utils.hpp>
#include <ECS/Utilities/Physics_Utils.hpp>
#include <vector>
#include <ECS/Utilities/ECS_Utils.hpp>
#include <v2d_generic.h>


namespace SXNGN::ECS::A {

	void Collision_System::Init()
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Event_System Init");
	}

	void Collision_System::Update(float dt)
	{

		auto gCoordinator = *SXNGN::Database::get_coordinator();
		auto collision_budget_ms_ = SXNGN::Database::get_collision_budget_ms();
		std::vector<Entity> entities_to_cleanup;
		std::vector<Entity> entities_to_resolve;

		

		//Iterate through entities this system manipulates/converts
		
		std::deque <Entity> entity_unresolved;
		std::deque <Entity> entity_all;
		std::deque<std::pair<Entity, Entity>> confirmed_collisions;
		

		std::array<ECS_Component*, MAX_ENTITIES>& all_collisionables = gCoordinator.CheckOutAllData(ComponentTypeEnum::COLLISION);
		std::array<ECS_Component*, MAX_ENTITIES>& all_locations = gCoordinator.CheckOutAllData(ComponentTypeEnum::LOCATION);
		std::array<ECS_Component*, MAX_ENTITIES>& all_moveables = gCoordinator.CheckOutAllData(ComponentTypeEnum::MOVEABLE);
		auto it_act = m_actable_entities.begin();
		//First sweep check out all data needed
		while (it_act != m_actable_entities.end())
		{
			auto const& entity = *it_act;
			it_act++;
			auto data = all_collisionables[entity];
			Collisionable* collisionable_ptr = static_cast<Collisionable*>(data);
			if (gCoordinator.EntityHasComponent(entity, ComponentTypeEnum::LOCATION) == false)
			{
				SDL_LogError(1, "Entity %d with Collision component does not have Location component", entity);
				std::terminate();
			}
			if (collisionable_ptr->resolved_ == false)
			{
				entity_unresolved.push_back(entity);
			}
			entity_all.push_back(entity);
		}

		int resolved_collisions = 0;
		//actable entities for collision system have collision and can move
		//auto last = m_actable_entities.empty() ? m_actable_entities.end() : std::prev(m_actable_entities.end()); // in case s is empty
		auto start = std::chrono::high_resolution_clock::now();
		//auto stop = std::chrono::high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
		auto it_unresolved = entity_unresolved.begin();

		
		
		while (it_unresolved != entity_unresolved.end())
		{
			bool outer_collided_with_something = false;
			Entity entity_outer = *it_unresolved;
			Collisionable* collisionable_outer = static_cast<Collisionable*>(all_collisionables[entity_outer]);
			Location* location_outer = static_cast<Location*>(all_locations[entity_outer]);
			
			SDL_Rect collision_box_outer;
			collision_box_outer.x = location_outer->m_pos_x_m_;
			collision_box_outer.y = location_outer->m_pos_y_m_;
			Moveable* moveable_outer = static_cast<Moveable*>(all_moveables[entity_outer]);
			if (moveable_outer)
			{
				collision_box_outer.x += moveable_outer->m_intended_delta_x_m;
				collision_box_outer.y += moveable_outer->m_intended_delta_y_m;
			}
			
			CollisionShape outer_shape = collisionable_outer->collision_shape_;
			if (outer_shape == CollisionShape::CIRCLE)
			{
				collision_box_outer.w = collisionable_outer->radius_;
				collision_box_outer.h = collisionable_outer->radius_;
			}
			else
			{
				collision_box_outer.w = collisionable_outer->width_;
				collision_box_outer.h = collisionable_outer->height_;
			}

			it_unresolved++;
			auto it_all = entity_all.begin();
			

			while (it_all != entity_all.end())
			{
				Entity entity_inner = *it_all;
				Collisionable* collisionable_inner = static_cast<Collisionable*>(all_collisionables[entity_inner]);
				Location* location_inner = static_cast<Location*>(all_locations[entity_inner]);
				SDL_Rect collision_box_inner;
				collision_box_inner.x = location_inner->m_pos_x_m_;
				collision_box_inner.y = location_inner->m_pos_y_m_;
				Moveable* moveable_inner = static_cast<Moveable*>(all_moveables[entity_inner]);
				if (moveable_inner)
				{
					collision_box_inner.x += moveable_inner->m_intended_delta_x_m;
					collision_box_inner.y += moveable_inner->m_intended_delta_y_m;
				}

				CollisionShape inner_shape = collisionable_inner->collision_shape_;
				if (inner_shape == CollisionShape::CIRCLE)
				{
					collision_box_inner.w = collisionable_inner->radius_;
					collision_box_inner.h = collisionable_inner->radius_;
				}
				else
				{
					collision_box_inner.w = collisionable_inner->width_;
					collision_box_inner.h = collisionable_inner->height_;
				}
				
				it_all++;
				if (entity_inner == entity_outer)
				{
					continue;
				}
				
				Uint8 collision_buffer_pixels = collisionable_outer->buffer_pixels + collisionable_inner->buffer_pixels;
				//does obj a touch obj b
				if (inner_shape == CollisionShape::RECTANGLE && outer_shape == CollisionShape::RECTANGLE)
				{
					if (CollisionChecks::checkCollisionRectRectBuffer(collision_box_outer, collision_box_inner, collision_buffer_pixels))
					{
						confirmed_collisions.push_back(std::make_pair(entity_inner, entity_outer));
						resolved_collisions++;
						//int collision_result = HandleCollisionGeneric(collisionable_outer, entity_actable_outer, collisionable_inner, entity_actable_inner, gCoordinator);
						outer_collided_with_something = true;

					}
				}
				else if (inner_shape == CollisionShape::RECTANGLE && outer_shape == CollisionShape::CIRCLE)
				{
					if (CollisionChecks::checkCollisionRectCircleBuffer(collision_box_inner, collision_box_outer, collision_buffer_pixels))
					{
						confirmed_collisions.push_back(std::make_pair(entity_inner, entity_outer));
						resolved_collisions++;
						//int collision_result = HandleCollisionGeneric(collisionable_outer, entity_actable_outer, collisionable_inner, entity_actable_inner, gCoordinator);
						outer_collided_with_something = true;

					}

				}
				else if (inner_shape == CollisionShape::CIRCLE && outer_shape == CollisionShape::RECTANGLE)
				{
					if (CollisionChecks::checkCollisionRectCircleBuffer(collision_box_outer, collision_box_inner, collision_buffer_pixels))
					{
						confirmed_collisions.push_back(std::make_pair(entity_inner, entity_outer));
						resolved_collisions++;
						//int collision_result = HandleCollisionGeneric(collisionable_outer, entity_actable_outer, collisionable_inner, entity_actable_inner, gCoordinator);
						outer_collided_with_something = true;

					}
				}
				else if (inner_shape == CollisionShape::CIRCLE && outer_shape == CollisionShape::CIRCLE)
				{
					if (CollisionChecks::checkCollisionCircleCircleBuffer(collision_box_outer, collision_box_inner, collision_buffer_pixels))
					{
						abort();
						confirmed_collisions.push_back(std::make_pair(entity_inner, entity_outer));
						resolved_collisions++;
						//int collision_result = HandleCollisionGeneric(collisionable_outer, entity_actable_outer, collisionable_inner, entity_actable_inner, gCoordinator);
						outer_collided_with_something = true;

					}
				}
				else
				{

				}
				
				if (!outer_collided_with_something)
				{
					resolved_collisions++;
					collisionable_outer->resolved_ = true;
				}
			}
			auto now = std::chrono::high_resolution_clock::now();
			auto collision_time = std::chrono::duration_cast<std::chrono::milliseconds> (now - start);
			if (collision_time.count() > collision_budget_ms_)
			{
				it_unresolved = entity_unresolved.end();
			}
		}
		
		//gCoordinator.CheckInAllData(ComponentTypeEnum::COLLISION);
		//gCoordinator.CheckInAllData(ComponentTypeEnum::LOCATION);
		gCoordinator.CheckInAllData(ComponentTypeEnum::COLLISION);
		gCoordinator.CheckInAllData(ComponentTypeEnum::LOCATION);
		gCoordinator.CheckInAllData(ComponentTypeEnum::MOVEABLE);

		//stop = std::chrono::high_resolution_clock::now();
		//duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
		std::vector<Entity> collisonables_to_delete;
		for (auto c_collision : confirmed_collisions)
		{
			std::set<Entity> entities_to_delete;

			auto data1 = all_collisionables[c_collision.first];
			auto data2 = all_collisionables[c_collision.second];

			if (data1 && data2)
			{
				Collisionable* collisionable_1 = static_cast<Collisionable*>(data1);
				Collisionable* collisionable_2 = static_cast<Collisionable*>(data2);
				Entity entity_1 = c_collision.first;
				Entity entity_2 = c_collision.second;
				

				//if the first collisionable is an event collision
				
				if (collisionable_1->collision_tag_ == CollisionTag::EVENT || collisionable_2->collision_tag_ == CollisionTag::EVENT)
				{
					entities_to_delete = HandleCollisionEvent(entity_1, collisionable_1, entity_2, collisionable_2);
				}

				else// if (collisionable_1->collision_tag_ == CollisionTag::UNKNOWN || collisionable_2->collision_tag_ == CollisionTag::UNKNOWN)
				{
					entities_to_delete = HandleCollisionDynamicOnStatic(entity_1, collisionable_1, entity_2, collisionable_2);
				}


				for (Entity entity : entities_to_delete)
				{
					collisonables_to_delete.push_back(entity);
				}
				
			}//if both collisionables exist


		}//for all confiermed collisions

		for (auto entity : collisonables_to_delete)
		{
			gCoordinator.DestroyEntity(entity);
		}

		
	}


	std::set<Entity> Collision_System::HandleCollisionEvent(Entity entity1, Collisionable* collisonable1_ptr, Entity entity2, Collisionable* collisonable2_ptr)
	{
		std::set<Entity> entities_to_destroy;
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		Collisionable* event_c = nullptr;
		Collisionable* other_c = nullptr;
		Entity event_e;
		Entity other_e;

		if (collisonable1_ptr->collision_tag_ == CollisionTag::EVENT)
		{
			event_c = collisonable1_ptr;
			other_c = collisonable2_ptr;
			event_e = entity1;
			other_e = entity2;
		}
		else
		{
			event_c = collisonable2_ptr;
			other_c = collisonable1_ptr;
			event_e = entity2;
			other_e = entity1;
		}

		auto event_data = gCoordinator.GetComponentReadOnly(event_e, ComponentTypeEnum::EVENT);
		if (event_data)
		{

			const Event_Component* event_ptr = static_cast<const Event_Component*>(event_data);
			//if the event is a mouse click 
			if (event_ptr->e.common.type == EventType::MOUSE )
			{
				//see how second collisionable reacts to mouse input
				//auto select_data = gCoordinator.GetComponentReadOnly(other_entity, ComponentTypeEnum::SELECTABLE);

				//const Selectable* select_ptr = static_cast<const Selectable*>(select_data);
				//if it is clickable
				std::vector<Entity> clicked;
				std::vector<Entity> double_click_entities;
				std::vector<Entity> boxed_entities;
				if (true)
				{
					switch (event_ptr->e.mouse_event.type)
					{
						//if it's a click
					case MouseEventType::CLICK:
					{

						if (event_ptr->e.mouse_event.click.double_click)
						{
							SDL_LogDebug(1, "Entity %d Double Clicked\n", other_e);
							double_click_entities.push_back(other_e);
						}
						else
						{
							SDL_LogDebug(1, "Entity %d Clicked\n", other_e);
							clicked.push_back(other_e);
						}

						break;
					}
					//or selection box
					case MouseEventType::BOX:
					{
						SDL_LogDebug(1, "Entity %d Mouse Boxed\n", other_e);
						boxed_entities.push_back(other_e);
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
						//get where the moveable currently is before this collision occurs
						ECS_Component* other_l_d = gCoordinator.CheckOutComponent(other_e, ComponentTypeEnum::LOCATION);
						Location* other_l = static_cast<Location*>(other_l_d);
						if (gCoordinator.getSetting("Debug_Spawn_Block").first == 1)
						{
							Entity_Builder_Utils::Create_Spawn_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, other_l->m_pos_x_m_, other_l->m_pos_y_m_);
						}
						else
						{
							if(gCoordinator.EntityHasComponent(other_e, ComponentTypeEnum::SELECTABLE))
							{
								//create event for user input system - tell it what entities were selected by a mouse event
								Entity_Builder_Utils::Create_Selection_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, clicked, double_click_entities, boxed_entities, additive, subtractive, enqueue);
							}
							
						}
						gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, other_e);
						
					}
					//right click is an order 
					else if (event_ptr->e.mouse_event.click.button == MOUSE_BUTTON::RIGHT)
					{
						//create event for user input system - tell it what the target of the order is 
						//todo different types of orders besides MOVE
						Entity_Builder_Utils::Create_Order_Event(gCoordinator, ComponentTypeEnum::CORE_BG_GAME_STATE, OrderType::MOVE, clicked, double_click_entities, boxed_entities, additive, subtractive, enqueue);
					}
					
					event_c->resolved_ = true;
					entities_to_destroy.insert(event_e);
				}//if clickable
			}//if mouse event type
		} //if event data present
		return entities_to_destroy;
	}

	std::set<Entity> Collision_System::HandleCollisionDynamicOnStatic(Entity entity1, Collisionable* collisonable1_ptr, Entity entity2, Collisionable* collisonable2_ptr)
	{
		std::set<Entity> entities_to_destroy;
		auto gCoordinator = *SXNGN::Database::get_coordinator();
		Collisionable* dynamic_c = nullptr;
		Collisionable* static_c = nullptr;
		Entity dynamic_e;
		Entity static_e;

		//find which one is dynamic and call the other static
		//if both are dynamic, one will be treated as static.
		if (collisonable1_ptr->collision_type_ == CollisionType::DYNAMIC)
		{
			dynamic_c = collisonable1_ptr;
			dynamic_e = entity1;
			static_c = collisonable2_ptr;
			static_e = entity2;
		}
		else if (collisonable2_ptr->collision_type_ == CollisionType::DYNAMIC)
		{
			static_c = collisonable1_ptr;
			static_e = entity1;
			dynamic_c = collisonable2_ptr;
			dynamic_e = entity2;
		}
		else
		{
			SDL_LogError(1, "Unknown Collision Type");
			std::terminate();
		}

		//get where the moveable currently is before this collision occurs
		ECS_Component* static_l_d = gCoordinator.CheckOutComponent(static_e, ComponentTypeEnum::LOCATION);
		Location* static_l = static_cast<Location*>(static_l_d);

		//get where the moveable currently is before this collision occurs
		ECS_Component* dynamic_l_d = gCoordinator.CheckOutComponent(dynamic_e, ComponentTypeEnum::LOCATION);
		Location* dynamic_l = static_cast<Location*>(dynamic_l_d);


		Location* loc_1 = nullptr;
		Location* loc_2 = nullptr;
		if (static_e == entity1)
		{
			loc_1 = static_l;
			loc_2 = dynamic_l;
		}
		else
		{
			loc_2 = static_l;
			loc_1 = dynamic_l;
		}

		ECS_Component* dynamic_m_d = gCoordinator.CheckOutComponent(dynamic_e, ComponentTypeEnum::MOVEABLE);
		Moveable* dynamic_m = static_cast<Moveable*>(dynamic_m_d);

		auto shape_1 = collisonable1_ptr->collision_shape_;
		auto shape_2 = collisonable2_ptr->collision_shape_;

		SDL_Rect dynamic_e_position;
		//get current position of dynamic entity
		dynamic_e_position.x = dynamic_l->m_pos_x_m_;
		dynamic_e_position.y = dynamic_l->m_pos_y_m_;
		dynamic_e_position.w = dynamic_c->width_;
		dynamic_e_position.h = dynamic_c->height_;

		SDL_Rect dynamic_e_potential_position = dynamic_e_position;
		//add where movement system intends for it to move to this frame
		dynamic_e_potential_position.x += dynamic_m->m_intended_delta_x_m;
		dynamic_e_potential_position.y += dynamic_m->m_intended_delta_y_m;

		//get current position of static entity
		SDL_Rect static_e_position;
		static_e_position.x = static_l->m_pos_x_m_;
		static_e_position.y = static_l->m_pos_y_m_;
		static_e_position.w = static_c->width_;
		static_e_position.h = static_c->height_;


		if (shape_1 == CollisionShape::RECTANGLE && shape_2 == CollisionShape::RECTANGLE)
		{
			


			//check if the intended position is valid
			std::pair<float, float> distance;
			distance = Physics_Utils::CalculateDistanceTo(dynamic_e_position, static_e_position);
			double xAxisTimeToCollide = dynamic_m->m_vel_x_m_s != 0.0 ? std::abs(distance.first / dynamic_m->m_vel_x_m_s) : 0;
			double yAxisTimeToCollide = dynamic_m->m_vel_y_m_s != 0.0 ? std::abs(distance.second / dynamic_m->m_vel_y_m_s) : 0;

			float shortest_time = 0.0;
			double alternate_dist = 0.0;
			//there is a collision, velocity tells what axis it occurred on

			if (dynamic_m->m_vel_x_m_s != 0.0 && dynamic_m->m_vel_y_m_s == 0)
			{
				//moving in x direction but not y, so time to collision is X
				shortest_time = xAxisTimeToCollide;
				alternate_dist = shortest_time * dynamic_m->m_vel_x_m_s;
				dynamic_m->m_intended_delta_x_m = alternate_dist;//

			}
			else if (dynamic_m->m_vel_y_m_s != 0.0 && dynamic_m->m_vel_x_m_s == 0)
			{
				shortest_time = yAxisTimeToCollide;
				alternate_dist = shortest_time * dynamic_m->m_vel_y_m_s;
				dynamic_m->m_intended_delta_y_m = alternate_dist;//distance.second;// shortest_time* dynamic_m->m_vel_y_m_s;
			}
			else
			{
				// Collision on X and Y axis (eg. slide up against a wall)
				shortest_time = std::min(std::abs(xAxisTimeToCollide), std::abs(yAxisTimeToCollide));
				alternate_dist = shortest_time * dynamic_m->m_vel_x_m_s;
				dynamic_m->m_intended_delta_x_m = alternate_dist;// shortest_time* dynamic_m->m_vel_x_m_s;
				alternate_dist = shortest_time * dynamic_m->m_vel_y_m_s;
				dynamic_m->m_intended_delta_y_m = alternate_dist; //shortest_time * dynamic_m->m_vel_y_m_s;
			}
		}
		else if (shape_1 == CollisionShape::RECTANGLE && shape_2 == CollisionShape::CIRCLE)
		{
			
			SDL_Rect rect;
			SDL_Rect circle_potential;
			SDL_Rect circle_real;
			rect.x = loc_1->m_pos_x_m_;
			rect.y = loc_1->m_pos_y_m_;
			rect.w = collisonable1_ptr->width_;
			rect.h = collisonable1_ptr->height_;

			circle_real.x = loc_1->m_pos_x_m_;
			circle_real.y = loc_1->m_pos_y_m_;
			circle_real.w = collisonable2_ptr->radius_;
			circle_real.h = collisonable2_ptr->radius_;

			circle_potential.x = dynamic_e_potential_position.x;
			circle_potential.y = dynamic_e_potential_position.y;
			circle_potential.w = collisonable2_ptr->radius_;
			circle_potential.h = collisonable2_ptr->radius_;
			
			if (entity2 != dynamic_e)
			{
				abort();
			}
			

			int nX = std::max(rect.x, std::min(rect.x + rect.w, circle_potential.x));
			int nY = std::max(rect.y, std::min(rect.y + rect.h, circle_potential.y));
			vi2d vect;
			vect.x = nX - circle_potential.x;
			vect.y = nY - circle_potential.y;
			int mag = vect.mag();
			int overlap = circle_potential.w - mag;
			vi2d correction = vect.norm() * -1;
			correction *= overlap;
			int new_x = circle_potential.x + correction.x;
			int new_y = circle_potential.y + correction.y;
			int diff_x = new_x - circle_potential.x;
			int diff_y = new_y - circle_potential.y;
			dynamic_m->m_intended_delta_x_m = diff_x;
			dynamic_m->m_intended_delta_y_m = diff_y;

			std::cout << "Rectangle Circle Collision" << std::endl;
			std::cout << "Circle at " << circle_real.x << " " << circle_real.y << " with radius " << circle_real.w << " moving to " << circle_potential.x << " " << circle_potential.y << std::endl;
			std::cout << "but rectangle at " << rect.x << " " << rect.y << " in the way " << std::endl;
			std::cout << "overlap: " << overlap << " correction " << correction << correction.x << " " << correction.y << std::endl;
			std::cout << "new potential move: " << diff_x << " " << diff_y << std::endl;

			

		}
		else if (shape_1 == CollisionShape::CIRCLE && shape_2 == CollisionShape::RECTANGLE)
		{
			
		}
		else if (shape_1 == CollisionShape::CIRCLE && shape_2 == CollisionShape::CIRCLE)
		{
			
		}
		else
		{

		}
		
		gCoordinator.CheckInComponent(ComponentTypeEnum::MOVEABLE, dynamic_e);

		gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, dynamic_e);
		gCoordinator.CheckInComponent(ComponentTypeEnum::LOCATION, static_e);

		return entities_to_destroy;
	}
	


	/**
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
		if (other->collision_type_ == CollisionType::STATIC)
		{
			auto person_moveable = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::MOVEABLE);
			if (person_moveable)
			{
				Moveable* moveable = static_cast<Moveable*>(person_moveable);
				moveable->m_intended_delta_x_m = moveable->m_intended_delta_x_m;
				moveable->UpdatePosition(moveable->m_prev_pos_x_m, moveable->m_prev_pos_y_m);
				
				if (gCoordinator.EntityHasComponent(person_entity, ComponentTypeEnum::RENDERABLE))
				{
					auto moveable_renderbox = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::RENDERABLE);
					if (moveable_renderbox)
					{
						Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
						render_ptr->x_ = int(round(moveable->get_pos_x()));
						render_ptr->y_ = int(round(moveable->get_pos_y()));
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
		if (other->collision_type_ == CollisionType::STATIC)
		{
			auto person_moveable = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::MOVEABLE);
			if (person_moveable)
			{
				Moveable* moveable = static_cast<Moveable*>(person_moveable);
				moveable->UpdatePosition(moveable->m_prev_pos_x_m, moveable->m_prev_pos_y_m);
				
				if (gCoordinator.EntityHasComponent(person_entity, ComponentTypeEnum::RENDERABLE))
				{
					auto moveable_renderbox = gCoordinator.CheckOutComponent(person_entity, ComponentTypeEnum::RENDERABLE);
					if (moveable_renderbox)
					{
						Renderable* render_ptr = static_cast<Renderable*>(moveable_renderbox);
						render_ptr->x_ = int(round(moveable->get_pos_x()));
						render_ptr->y_ = int(round(moveable->get_pos_y()));
						gCoordinator.CheckInComponent(ComponentTypeEnum::RENDERABLE, person_entity);
					}
				}

			}
		}
		return 0;
	}
	**/

}


/** Scrap

auto data2 = gCoordinator.CheckOutComponent(c_collision.second, ComponentTypeEnum::COLLISION);

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
					bool outer_collided_with_something = false;
					for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_b = it_coll_a + 1; it_coll_b != entity_to_collisionable_all.end(); ++it_coll_b)
					{
						if ((*it_coll_a).second->resolved_ == true && (*it_coll_b).second->resolved_ == true)
						{
							continue;
						}
						int collision_buffer_pixels = (*it_coll_a).second->buffer_pixels + (*it_coll_b).second->buffer_pixels;
						//does obj a touch obj b
						if (CollisionChecks::checkCollisionRectRectBuffer((*it_coll_a).second->collision_box_, (*it_coll_b).second->collision_box_, collision_buffer_pixels))
						{
							confirmed_collisions.push_back(std::make_pair((*it_coll_a).first, (*it_coll_b).first));
							outer_collided_with_something = true;

						}
						check_num++;
					}
					if (!outer_collided_with_something && (*it_coll_a).second->resolved_ == false)
					{
						entities_to_resolve.push_back((*it_coll_a).first); //if outer didn't collide with anything, it is resolved.
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
					bool outer_collided_with_something = false;
					for (std::deque <std::pair<Entity, const Collisionable*>>::const_iterator it_coll_b = entity_to_collisionable_all.begin(); it_coll_b != entity_to_collisionable_all.end(); ++it_coll_b)
					{
						if ((*it_coll_a).first == (*it_coll_b).first) //don't check collision against self
						{
							continue;
						}
						int collision_buffer_pixels = (*it_coll_a).second->buffer_pixels + (*it_coll_b).second->buffer_pixels;
						//does obj a touch obj b
						if (CollisionChecks::checkCollisionRectRectBuffer((*it_coll_a).second->collision_box_, (*it_coll_b).second->collision_box_, collision_buffer_pixels))
						{
							//SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Collision Method 2: Entity %d hit Entity %d", (*it_coll_a).first, (*it_coll_b).first);

							confirmed_collisions.push_back(std::make_pair((*it_coll_a).first, (*it_coll_b).first));
							outer_collided_with_something = true;

						}
						check_num++;
					}
					if (!outer_collided_with_something && (*it_coll_a).second->resolved_ == false)
					{
						entities_to_resolve.push_back((*it_coll_a).first); //if outer didn't collide with anything, it is resolved.
					}
					auto now = std::chrono::high_resolution_clock::now();
					auto collision_time = std::chrono::duration_cast<std::chrono::milliseconds> (now - start);
					if (collision_time.count() > collision_budget_ms_)
					{
						break; //budget for time exceeded
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

		for (auto entity_to_resolve : entities_to_resolve)
		{
			auto data1 = gCoordinator.CheckOutComponent(entity_to_resolve, ComponentTypeEnum::COLLISION);
			Collisionable* collisionable_1 = static_cast<Collisionable*>(data1);
			collisionable_1->resolved_ = true;
			gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, entity_to_resolve);
		}

		**/









		/** ////// test
				start = std::chrono::high_resolution_clock::now();
				it_act_outer = m_actable_entities.begin();
				while (it_act_outer != last)
				{
					it_act_outer++;
				}
				stop = std::chrono::high_resolution_clock::now();
				duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);

				start = std::chrono::high_resolution_clock::now();
				it_act_outer = m_actable_entities.begin();
				while (it_act_outer != last)
				{
					auto const& entity_actable_outer = *it_act_outer;
					it_act_outer++;
					auto data_outer = gCoordinator.CheckOutComponent(entity_actable_outer, ComponentTypeEnum::COLLISION);
					Collisionable* collisionable_outer = static_cast<Collisionable*>(data_outer);
					gCoordinator.CheckInComponent(ComponentTypeEnum::COLLISION, entity_actable_outer);
				}
				stop = std::chrono::high_resolution_clock::now();
				duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);

				start = std::chrono::high_resolution_clock::now();
				it_act_outer = m_actable_entities.begin();
				while (it_act_outer != last)
				{
					auto const& entity_actable_outer = *it_act_outer;
					it_act_outer++;
					auto data = gCoordinator.GetComponentReadOnly(entity_actable_outer, ComponentTypeEnum::COLLISION);
					const Collisionable* collisionable_ptr = static_cast<const Collisionable*>(data);
				}
				stop = std::chrono::high_resolution_clock::now();
				duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop - start);
				**/











				/**
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
								**/
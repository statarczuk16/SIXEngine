#include <ECS/Components/Moveable.hpp>
#include <ECS/Utilities/Map_Utils.hpp>

namespace SXNGN::ECS::A {
	
	Moveable::Moveable()
	{
		component_type = ComponentTypeEnum::MOVEABLE;
		new_destination_ = false;
	}

	void Moveable::Update_Destination(Location new_location)
	{
		position_ = new_location;
		at_destination = false;
		while (waypoints.size())
		{
			waypoints.pop();
		}
		new_destination_ = true;

	}

	bool Moveable::Check_At_Destination()
	{
		if (Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, position_, destination_))
		{
			at_destination = true;
		}
	}

	bool Moveable::Check_At_Waypoint()
	{
		if (waypoints.empty())
		{
			return false;
		}

		if (Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, position_, waypoints.front()))
		{
			waypoints.pop();
		}
	}

	void Moveable::SolveDestination(NAVIGATION_TYPE method)
	{
		waypoints = Map_Utils::GetPath(method, position_, destination_);
		new_destination_ = false;
	}

	void Moveable::UpdatePosition(double new_x, double new_y)
	{
		m_pos_x_m = new_x;
		m_pos_y_m = new_y;
		position_.x = (int)round(m_pos_x_m);
		position_.y = (int)round(m_pos_y_m);
	}

	Location Moveable::GetCurrentWaypoint()
	{
		return waypoints.front();
	}

	Location Moveable::GetPosition()
	{
		return waypoints.front();
	}
}
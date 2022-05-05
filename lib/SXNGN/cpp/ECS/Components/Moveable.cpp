#include <ECS/Components/Moveable.hpp>
#include <ECS/Utilities/Map_Utils.hpp>

namespace SXNGN::ECS::A {
	
	Moveable::Moveable()
	{
		component_type = ComponentTypeEnum::MOVEABLE;
		new_destination_ = false;
	}

	void Moveable::Update_Destination(Location new_destination)
	{
		destination_ = new_destination;
		at_destination_ = false;
		while (waypoints_.size())
		{
			waypoints_.pop();
		}
		new_destination_ = true;

	}

	bool Moveable::Check_At_Destination()
	{

		if (Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, position_, destination_) < 5)
		{
			at_destination_ = true;
		}
		else
		{
			at_destination_ = false;
		}
		return at_destination_;
	}

	bool Moveable::Check_At_Waypoint()
	{
		if (waypoints_.empty())
		{
			return false;
		}

		if (Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, position_, waypoints_.front()) <= 5)
		{
			waypoints_.pop();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Moveable::SolveDestination(NAVIGATION_TYPE method)
	{
		new_destination_ = false;
		auto new_waypoints = Map_Utils::GetPath(method, position_, destination_);
		if (new_waypoints.empty())
		{
			return false;
		}
		waypoints_ = new_waypoints;
		return true;
	}

	void Moveable::UpdatePosition(double new_x, double new_y)
	{
		m_pos_x_m = new_x;
		m_pos_y_m = new_y;
		position_.x = (int)round(m_pos_x_m);
		position_.y = (int)round(m_pos_y_m);
		Check_At_Destination();
		Check_At_Waypoint();
	}

	Location Moveable::GetCurrentWaypoint()
	{
		if (waypoints_.empty())
		{
			//return -1,-1 location if no waypoints
			return Location();
		}
		return waypoints_.front();
	}

	Location Moveable::GetPosition()
	{
		return position_;
	}
}
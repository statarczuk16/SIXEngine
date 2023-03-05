#include <ECS/Components/Moveable.hpp>
#include <ECS/Utilities/Map_Utils.hpp>

namespace SXNGN::ECS {
	
	Moveable::Moveable()
	{
		component_type = ComponentTypeEnum::MOVEABLE;
		new_destination_ = false;
	}

	Moveable::Moveable(int speed_m_s)
	{
		component_type = ComponentTypeEnum::MOVEABLE;
		new_destination_ = false;
		m_speed_m_s = speed_m_s;
	}

	void Moveable::Update_Destination(Coordinate new_destination)
	{
		destination_ = new_destination;
		at_destination_ = false;
		while (waypoints_.size())
		{
			waypoints_.pop();
		}
		new_destination_ = true;

	}

	bool Moveable::Check_At_Destination(double x, double y)
	{
		Coordinate location(x, y);
		if (Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, location, destination_) <= SXNGN::AT_DESTINATION_THRESH)
		{
			//SDL_LogDebug(1, "Destination Reached: %d, %d", destination_.x, destination_.y);
			at_destination_ = true;

		}
		else
		{
			at_destination_ = false;
		}
		return at_destination_;
	}

	bool Moveable::Check_At_Waypoint(double x, double y)
	{
		if (waypoints_.empty())
		{
			return false;
		}
		Coordinate location(x, y);
		if (Map_Utils::GetDistance(NAVIGATION_TYPE::MANHATTAN, location, waypoints_.front()) <= SXNGN::AT_DESTINATION_THRESH)
		{
			SDL_LogDebug(1, "Waypoint Reached: %d, %d", waypoints_.front().x / SXNGN::BASE_TILE_WIDTH, waypoints_.front().y / SXNGN::BASE_TILE_HEIGHT);
			waypoints_.pop();

			return true;
		}
		else
		{
			return false;
		}
	}

	bool Moveable::SolveDestination(double x, double y, NAVIGATION_TYPE method)
	{
		Coordinate location(x, y);
		new_destination_ = false;
		auto new_waypoints = Map_Utils::GetPath(method, location, destination_);
		if (new_waypoints.empty())
		{
			return false;
		}
		waypoints_ = new_waypoints;
		return true;
	}

	/**
	void Moveable::UpdatePosition(double new_x, double new_y)
	{
		m_pos_x_m = new_x;
		m_pos_y_m = new_y;
		position_.x = (int)round(m_pos_x_m);
		position_.y = (int)round(m_pos_y_m);
		Check_At_Destination();
		Check_At_Waypoint();
	}
	**/

	unsigned int Moveable::GetNumWaypoints()
	{
		return waypoints_.size();
	}

	Coordinate Moveable::GetCurrentWaypoint()
	{
		if (waypoints_.empty())
		{
			//return -1,-1 location if no waypoints
			return Coordinate();
		}
		return waypoints_.front();
	}


}
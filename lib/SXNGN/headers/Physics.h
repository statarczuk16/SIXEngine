#pragma once
#include <climits>


namespace SXNGN::ECS {
	struct Coordinate;
}

namespace SXNGN {
	class Physics
	{
	public:

		enum class MoveType //type of movement. Friction applies in different ways depending on rolling, sliding, etc.
		{
			//Type 
			WALK,
			ROLL,
			SLIDE
		};

		enum class FrictionType
		{
			STATIC_YES_KINETIC_NO,
			STATIC_YES_KINETIC_YES,
			STATIC_NO_KINETIC_YES,//Friction does NOT apply when a force is applied but DOES apply when object is moving (eg, friction does not stop you from taking a step, but friction stops your forward momentum when walking)
			STATIC_NO_KINETIC_NO
		};

		static double get_friction_acc(double weight_kg, double cof, double cur_vel_m_s, double cur_force_n, MoveType move_type = MoveType::SLIDE);
		static double get_friction_force(double weight_kg, double cof);
		static double get_applied_force_acc(double f_app_n, double weight_kg);
		//static double get_total_acceleration(double f_app_acc_m_s_s, double fr_acc_m_s_s);
		static double adjust_velocity_by_acc(double acc, double vel, double time_step, double max_val = double(INT_MAX));
		static FrictionType move_type_to_friction_type(MoveType move_type);
		

		
	};

	
}
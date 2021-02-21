#pragma once
#include <climits>

namespace SXNGN {
	class Physics
	{
	public:


		static double get_friction_acc(double weight_kg, double cof, double cur_vel_m_s, double cur_force_n);
		static double get_friction_force(double weight_kg, double cof);
		static double get_applied_force_acc(double f_app_n, double weight_kg);
		//static double get_total_acceleration(double f_app_acc_m_s_s, double fr_acc_m_s_s);
		static double adjust_velocity_by_acc(double acc, double vel, double time_step, double max_val = double(INT_MAX));
	};
}
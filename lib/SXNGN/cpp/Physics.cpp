#include "Physics.h"
#include "Constants.h"
#include "gameutils.h"
#include <ECS/Utilities/Map_Utils.hpp>
#include <ECS/Core/Types.hpp>


SXNGN::Physics::FrictionType SXNGN::Physics::move_type_to_friction_type(MoveType move_type)
{
	switch (move_type)
	{
		case MoveType::ROLL:
		{
			return FrictionType::STATIC_YES_KINETIC_YES;
			break;
		}
		case MoveType::WALK:
		{
			return FrictionType::STATIC_NO_KINETIC_YES;
			break;
		}
		case MoveType::SLIDE:
		{
			return FrictionType::STATIC_YES_KINETIC_YES;
			break;
		}
		default: return  FrictionType::STATIC_YES_KINETIC_YES;
	}
}

double SXNGN::Physics::get_friction_acc(double weight_kg, double cof, double cur_vel_m_s, double cur_force_n, MoveType move_type)
{
	double kinetic_friction_acc_m_s_s = 0.0;
	double static_friction_acc_m_s_s = 0.0;
	double static_cof = cof * 1.33; //simplification: static is a bit more than kinetic
	FrictionType friction_type = Physics::move_type_to_friction_type(move_type);
	
	//determine kinetic friction - applies only to moving objects
	if (cur_vel_m_s == 0 || friction_type == FrictionType::STATIC_NO_KINETIC_NO || friction_type == FrictionType::STATIC_YES_KINETIC_NO)
	{
		kinetic_friction_acc_m_s_s = 0.0;
	}
	else
	{
		double friction_force_knetic_n = get_friction_force(weight_kg, cof);
		kinetic_friction_acc_m_s_s = friction_force_knetic_n / weight_kg;
		if(cur_vel_m_s > 0)
		{
			kinetic_friction_acc_m_s_s = kinetic_friction_acc_m_s_s * -1.0;
		}
	}
	
	//determine static friction - applies only to objects on which force is imparted
	if (cur_force_n == 0.0 || abs(cur_vel_m_s) > 0 || friction_type == FrictionType::STATIC_YES_KINETIC_NO || friction_type == FrictionType::STATIC_YES_KINETIC_YES)
	{
		static_friction_acc_m_s_s = 0.0;
	}
	else
	{
		double friction_force_static_n = get_friction_force(weight_kg, static_cof);
		static_friction_acc_m_s_s = friction_force_static_n / weight_kg;
		if (cur_force_n > 0)
		{
			static_friction_acc_m_s_s = static_friction_acc_m_s_s * -1.0;
		}
	}

	switch (friction_type)
	{
		case FrictionType::STATIC_NO_KINETIC_NO: return 0.0;
		case FrictionType::STATIC_NO_KINETIC_YES: return kinetic_friction_acc_m_s_s;
		case FrictionType::STATIC_YES_KINETIC_NO: return static_friction_acc_m_s_s;
		default: 
		{
			if (abs(static_friction_acc_m_s_s) > abs(kinetic_friction_acc_m_s_s))
			{
				return static_friction_acc_m_s_s;
			}
			else
			{
				return kinetic_friction_acc_m_s_s;
			}
		}
	}
}

double SXNGN::Physics::get_friction_force(double weight_kg, double cof)
{
	//normal force is mass * gravity * cosine(theta) where theta = standing angle the surface is inclined to 
	double incline = 1; //just 1 for now
	double normal_force_n = weight_kg * G * incline; //normal force in newtons

	double friction_force_n = cof * normal_force_n;

	return friction_force_n;
 }

double SXNGN::Physics::get_applied_force_acc(double f_app_n, double weight_kg)
{
	return (f_app_n / weight_kg);
}

/**
double SXNGN::Physics::get_total_acceleration(double f_app_acc_m_s_s, double fr_acc_m_s_s)
{
	//friction always acts against movement / acceleration
	double acc_m_s_s;
	if (f_app_acc_m_s_s >= 0)
	{
		// if positive acceleration from applied force, negative acceleration from friction
		 acc_m_s_s = f_app_acc_m_s_s - abs(fr_acc_m_s_s);
	}
	else
	{
		// if negative acceleration from applied force, positive acceleration from friction
		 acc_m_s_s = f_app_acc_m_s_s + abs(fr_acc_m_s_s);
	}

	return acc_m_s_s;

}
**/




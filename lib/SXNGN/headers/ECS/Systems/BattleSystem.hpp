#pragma once

#include "ECS/Core/System.hpp"
#include <memory>
#include <ECS/Components/Battle.hpp>

 
namespace SXNGN::ECS {

	class Event;
	class Battle_System : public System
	{
	public:
		virtual void Init();

		void Update(double dt);
		void Init_Menus(Battle* battle);
		void Enemy_Attack(Battle* battle);
		void ShowVictoryScreen(Battle* battle);
		void UpdateUI(Battle* battle);

	private:


	};
}

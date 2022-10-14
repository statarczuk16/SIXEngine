#pragma once
#include <SDL.h>
#include <memory>
#include <map>
#include <string>

namespace SXNGN {

    namespace ECS::A { class Coordinator; }

    class Database {
    public:
        Database();
        static float scale_;
        static float get_scale();
        static void set_scale(float new_scale);
        static void reduce_scale();
        static void increase_scale();
        static void modify_scale(int scale_mod);
       

        static Uint8 max_fps_;
        static Uint8 get_max_fps();
        static void set_max_fps(Uint8 new_fps);

        static Uint8 collision_budget_ms_;
        static Uint8 get_collision_budget_ms();

        static std::map<std::string, double> property_map_;
        static std::map<std::string, double>* get_property_map();


        static Uint8 get_screen_ticks_per_frame();

        
        static std::shared_ptr<ECS::A::Coordinator> ecs_coordinator_;
        static std::shared_ptr<ECS::A::Coordinator> get_coordinator();
        static void set_coordinator(std::shared_ptr<ECS::A::Coordinator> ecs_coordinator);
       
    };
}
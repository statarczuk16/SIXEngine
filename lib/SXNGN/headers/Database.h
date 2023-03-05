#pragma once
#include <SDL.h>
#include <memory>
#include <map>
#include <string>

namespace SXNGN {

    namespace ECS { class Coordinator; }

    class Database {
    public:
        Database();
        static double scale_;
        static double get_scale();
        static void set_scale(double new_scale);
        static void reduce_scale();
        static void increase_scale();
        static void modify_scale(double scale_mod);
       

        static Uint8 max_fps_;
        static Uint8 get_max_fps();
        static void set_max_fps(Uint8 new_fps);

        static Uint8 collision_budget_ms_;
        static Uint8 get_collision_budget_ms();

        static double get_screen_ms_per_frame();

        
        static std::shared_ptr<ECS::Coordinator> ecs_coordinator_;
        static std::shared_ptr<ECS::Coordinator> get_coordinator();
        static void set_coordinator(std::shared_ptr<ECS::Coordinator> ecs_coordinator);
       
    };
}
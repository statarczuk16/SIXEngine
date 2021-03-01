#pragma once
#include <ECS/Core/Coordinator.hpp>

namespace SXNGN {

    class Database {
    public:
        Database();
        static int scale_;
        static int get_scale();
        static void set_scale(int new_scale);
        static void reduce_scale();
        static void increase_scale();

        static int max_fps_;
        static int get_max_fps();
        static void set_max_fps(int new_fps);

        static int get_screen_ticks_per_frame();

        static std::shared_ptr<Coordinator> ecs_coordinator_;
        static std::shared_ptr<Coordinator> get_coordinator();
        static void set_coordinator(std::shared_ptr<Coordinator> ecs_coordinator);


        
    };
}
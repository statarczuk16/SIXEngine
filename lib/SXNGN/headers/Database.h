#pragma once
#include <SDL.h>
#include <memory>

namespace SXNGN {

    namespace ECS::A { class Coordinator; }

    class Database {
    public:
        Database();
        static Uint8 scale_;
        static Uint8 get_scale();
        static void set_scale(Uint8 new_scale);
        static void reduce_scale();
        static void increase_scale();

        static Uint8 max_fps_;
        static Uint8 get_max_fps();
        static void set_max_fps(Uint8 new_fps);

        static Uint8 get_screen_ticks_per_frame();

        
        static std::shared_ptr<ECS::A::Coordinator> ecs_coordinator_;
        static std::shared_ptr<ECS::A::Coordinator> get_coordinator();
        static void set_coordinator(std::shared_ptr<ECS::A::Coordinator> ecs_coordinator);
       
    };
}
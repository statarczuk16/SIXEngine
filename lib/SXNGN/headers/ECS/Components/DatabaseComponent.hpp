#pragma once

#include <SDL.h>
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Core/Types.hpp>
#include <memory>



namespace SXNGN::ECS::A {

    struct DatabaseComponent : ECS_Component
    {
    private:

       
       static std::shared_ptr<DatabaseComponent> instance_;
       

    public:

        DatabaseComponent();

        static std::shared_ptr<DatabaseComponent> init_instance();
       
        static std::shared_ptr<DatabaseComponent> get_instance();
     
    };

}


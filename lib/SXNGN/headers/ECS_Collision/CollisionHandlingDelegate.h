#pragma once
#include <memory>
#include <vector>



namespace SXNGN {
    

    class Tile;
    class Camera;
    class Entity;

    namespace Collision {


        class CollisionHandlingDelegate
        {
        public:
            virtual bool handle_collision(std::shared_ptr<SXNGN::Entity> this_entity, std::vector<std::shared_ptr<SXNGN::Entity>> entities, std::shared_ptr<SXNGN::Tile> tiles) = 0;
        };

        class HasCollision : public CollisionHandlingDelegate
        {
        public:
            virtual bool  handle_collision(std::shared_ptr<SXNGN::Entity> this_entity, std::vector<std::shared_ptr<SXNGN::Entity>> entities, std::shared_ptr<SXNGN::Tile> tiles) override;
        };

        class NoCollision : public CollisionHandlingDelegate
        {
        public:
            virtual bool  handle_collision(std::shared_ptr<SXNGN::Entity> this_entity, std::vector<std::shared_ptr<SXNGN::Entity>> entities, std::shared_ptr<SXNGN::Tile> tiles) override;
        };
    }
}
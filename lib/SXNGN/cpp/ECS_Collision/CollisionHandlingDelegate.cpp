#include <ECS_Collision/CollisionHandlingDelegate.h>
#include <Camera.h>
#include <Sprite/Tile.h>
#include <UI/UIElement.h>
#include <Collision.h>



bool SXNGN::Collision::HasCollision::handle_collision(std::shared_ptr<SXNGN::Entity> this_entity,std::vector<std::shared_ptr<SXNGN::Entity>> entities, std::shared_ptr<SXNGN::Tile> tiles)
{
    /**
    for (auto entity : entities)
    {
        if (SXNGN::Collision::checkCollision(this_entity, entity)
        {
            return true;
        }
    }

    for (auto tile : tiles)
    {
        if (SXNGN::Collision::checkCollision(this_entity, tile)
        {
            return true;
        }
    }


    **/
    return false;
   
}

  

bool SXNGN::Collision::NoCollision::handle_collision(std::shared_ptr<SXNGN::Entity> this_entity, std::vector<std::shared_ptr<SXNGN::Entity>> entities, std::shared_ptr<SXNGN::Tile> tiles)
{
    return false;
}

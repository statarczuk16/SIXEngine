#include <ECS_Rendering/RenderingDelegate.h>
#include <Camera.h>
#include <Sprite/Tile.h>



void SXNGN::Rendering::NotVisible::draw(std::shared_ptr<SXNGN::Camera> camera, std::shared_ptr<SXNGN::Tile> tile)
{

}

 
void SXNGN::Rendering::Visible::draw(std::shared_ptr<SXNGN::Camera> camera, std::shared_ptr<SXNGN::Tile> tile)
{
            tile->render(camera);
}

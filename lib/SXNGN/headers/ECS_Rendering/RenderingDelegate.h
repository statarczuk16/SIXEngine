#pragma once
#include <memory>

namespace SXNGN {

    class Tile;
    class Camera;
    namespace Rendering {
        class RenderingDelegate
        {
        public:
            virtual void draw(std::shared_ptr<SXNGN::Camera> camera, std::shared_ptr<SXNGN::Tile> tile) = 0;
        };

        class NotVisible : public RenderingDelegate
        {
        public:
            virtual void draw(std::shared_ptr<SXNGN::Camera> camera, std::shared_ptr<SXNGN::Tile> tile) override;
        };

        class Visible : public RenderingDelegate
        {
        public:
            virtual void draw(std::shared_ptr<SXNGN::Camera> camera, std::shared_ptr<SXNGN::Tile> tile) override;
        };
    }
}
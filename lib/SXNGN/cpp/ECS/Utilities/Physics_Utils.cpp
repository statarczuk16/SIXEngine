#include <ECS/Utilities/Physics_Utils.hpp>
namespace SXNGN {
    namespace ECS {
            std::pair<float, float> Physics_Utils::CalculateDistanceTo(SDL_Rect e1, SDL_Rect e2)
            {
                float dx = 0;
                float dy = 0;

                if (e1.x < e2.x)
                {
                    dx = e2.x - (e1.x + e1.w);
                }
                else if (e1.x > e2.x)
                {
                    dx = e1.x - (e2.x + e2.w);
                }

                if (e1.y < e2.y)
                {
                    dy = e2.y - (e1.y + e1.h);
                }
                else if (e1.y > e2.y)
                {
                    dy = e1.y - (e2.y + e2.h);
                }

                return std::make_pair(dx, dy);
            }

            std::pair<float, float> Physics_Utils::CalculateDistanceTo(SDL_FRect e1, SDL_FRect e2)
            {
                float dx = 0.0;
                float dy = 0.0;

                if (e1.x < e2.x)
                {
                    dx = e2.x - (e1.x + e1.w);
                }
                else if (e1.x > e2.x)
                {
                    dx = e1.x - (e2.x + e2.w);
                }

                if (e1.y < e2.y)
                {
                    dy = e2.y - (e1.y + e1.h);
                }
                else if (e1.y > e2.y)
                {
                    dy = e1.y - (e2.y + e2.h);
                }

                return std::make_pair(dx, dy);
            }
        }
    }

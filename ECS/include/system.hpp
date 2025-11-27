/*
** EPITECH PROJECT, 2025
** G-CPP-500-RUN-5-2-bsrtype-5
** File description:
** system.hpp
*/

#ifndef INCLUDED_SYSTEM_HPP
    #define INCLUDED_SYSTEM_HPP
    #include "Component.hpp"
    #include "registry.hpp"

class Systems {
    public:
        Systems() = default;
        ~Systems() = default;

        void movement(Registry &r, float deltaTime) {
            auto &positions = r.get_components<Position>();
            auto &velocities = r.get_components<velocity>();

            for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i) {
                auto &pos = positions[i];
                auto &vel = velocities[i];

                if (pos && vel) {
                    pos->x += vel->vx * deltaTime * 60.0f;
                    pos->y += vel->vy * deltaTime * 60.0f;
                }
            }
        }

        void rendering(Registry &r) {
            auto &positions = r.get_components<Position>();
            auto &comps = r.get_components<Comp>();

            for (size_t i = 0; i < positions.size() && i < comps.size(); ++i) {
                auto &pos = positions[i];
                auto &sqr = comps[i];

                if (pos && sqr) {
                    DrawRectangle(
                        (int)pos->x, 
                        (int)pos->y, 
                        (int)sqr->size,
                        (int)sqr->size,
                        sqr->color
                    );
                }
            }
        }

        void logging(Registry &r) {
            auto const &positions = r.get_components<Position>();
            auto const &velocities = r.get_components<velocity>();

            for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i) {
                auto const &pos = positions[i];
                auto const &vel = velocities[i];

                if (pos && vel) {
                    std::cout << "Entity " << i << ": Pos(" << pos->x << ", " << pos->y 
                              << ") Vel(" << vel->vx << ", " << vel->vy << ")" << std::endl;
                }
            }
        }
};

#endif


/*
** EPITECH PROJECT, 2025
** G-CPP-500-RUN-5-2-bsrtype-5
** File description:
** registry.hpp
*/

#ifndef REGISTRY_HPP
    #define REGISTRY_HPP
    #include <unordered_map>
    #include <typeindex>
    #include <any>
    #include "sparse_array.hpp"
    #include "entity.hpp"
    #include <stdexcept>

    class Registry {
        public:
        Registry() = default;

        
        template <class Component>
        sparse_array<Component> &register_component() {
            
            std::type_index key(typeid(Component));
        
            if (!_components_arrays.contains(key)) {
                _components_arrays[key] = sparse_array<Component>();
            }
            return std::any_cast<sparse_array<Component>&>(_components_arrays[key]);
        }

        template <class Component>
        sparse_array<Component> &get_components() {
            std::type_index key(typeid(Component));

            if (!_components_arrays.contains(key)){
                throw std::runtime_error("Comoponet not registered");
            }
            return std::any_cast<sparse_array<Component>&> (_components_arrays.at(key));
        }

        template <class Component>
        sparse_array<Component>const& get_components() const{
            std::type_index key(typeid(Component));
        
            return std::any_cast<const sparse_array<Component>&> (
                _components_arrays.at(key)
            );
        }
        
        Entity spawn_entity() {
            if (_new_entity >= MAX_ENTITY) {
                throw std::runtime_error("We reach the max of enity");
            }
            _alive[_new_entity] = true;
            return _new_entity++;
        }

        void kill_entity(Entity entity) {
            if (entity < _alive.size()) {
                _alive[entity] = false;
            }
        }
        template <class Component>
        void add_component(Entity entity, Component const& component) {
            sparse_array<Component> &array =  register_component<Component>();

            array.insert_at(entity, component);
        }

        template <class Component>
        void remove_component(Entity entity) {
            sparse_array<Component> &array = get_components<Component>();

            if (entity < array.size()) {
                array.erase(entity);
            }
        }
        private:

        std::unordered_map<std::type_index, std::any> _components_arrays;
        Entity _new_entity = 0;
        std::vector<bool> _alive = std::vector<bool>(MAX_ENTITY, false);
    };
    
#endif


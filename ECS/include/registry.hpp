/*
** EPITECH PROJECT, 2025
** RType-CI-CD
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
    #include <vector>

    class Registry {
        public:
        Registry() : _alive(5000, false) {};

        template <class Component>
        sparse_array<Component> &register_component() {
            std::type_index key(typeid(Component));
            if (_components_arrays.find(key) == _components_arrays.end()) {
                _components_arrays[key] = sparse_array<Component>();
            }
            return std::any_cast<sparse_array<Component>&>(_components_arrays[key]);
        }

        template <class Component>
        sparse_array<Component> &get_components() {
            std::type_index key(typeid(Component));
            if (_components_arrays.find(key) == _components_arrays.end()){
                throw std::runtime_error("Component not registered");
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
            if (_new_entity >= 5000) {
                // Consider reusing dead entity IDs
                throw std::runtime_error("We reach the max of entity");
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
        std::vector<bool> _alive;
    };
    
#endif
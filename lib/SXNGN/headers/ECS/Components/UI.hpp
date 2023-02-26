#pragma once

#include <SDL.h>
#include <Database.h>
#include <ECS/Core/Component.hpp>
#include <ECS/Components/CameraComponent.hpp>
#include <ECS/Core/Types.hpp>
#include <memory>
#include<kiss_sdl.h>
#include <mutex>
#include <ECS/Components/EventComponent.hpp>



namespace SXNGN::ECS::A {

    struct UIContainerComponent 
    {
        UIContainerComponent()
        {

        }

        UIContainerComponent(std::shared_ptr<kiss_window> parent_window, UILayer layer, UIType type, std::string name = "uninit_ui_element")
        {
            parent_window_ = parent_window;
            window_ = nullptr;
            layer_ = layer;
            type_ = type;
            button_ = nullptr;
            selectbutton_ = nullptr;
            vscrollbar_ = nullptr;
            hscrollbar_ = nullptr;
            progressbar_ = nullptr;
            entry_ = nullptr;
            textbox_ = nullptr;
            combobox_ = nullptr;
            label_ = nullptr;
            name_ = name;
        }

        
        void ClearUIContainerComponent()
        {
            if (button_ != nullptr)
            {
                delete button_;
                button_ = nullptr;
            }
            if (selectbutton_ != nullptr)
            {
                delete selectbutton_;
                selectbutton_ = nullptr;
            }
            if (vscrollbar_ != nullptr)
            {
                delete vscrollbar_;
                vscrollbar_ = nullptr;
            }
            if (hscrollbar_ != nullptr)
            {
                delete hscrollbar_;
                hscrollbar_ = nullptr;
            }
            if (progressbar_ != nullptr)
            {
                delete progressbar_;
                progressbar_ = nullptr;
            }
            if (entry_ != nullptr)
            {
                delete entry_;
                entry_ = nullptr;
            }
            if (textbox_ != nullptr)
            {
                delete textbox_;
                textbox_ = nullptr;
            }
            if (combobox_ != nullptr)
            {
                delete combobox_;
                combobox_ = nullptr;
            }
            if (label_ != nullptr)
            {
                delete label_;
                label_ = nullptr;
            }
            for (auto component : child_components_)
            {
                component->ClearUIContainerComponent();
            }
        } 

        UIType type_ = UIType::UNKNOWN;
        std::string name_ = "uninit_ui_element";
        std::shared_ptr<kiss_window> parent_window_ = nullptr;
        std::shared_ptr<kiss_window> window_ = nullptr;
        kiss_button* button_ = nullptr;
        kiss_selectbutton* selectbutton_ = nullptr;
        kiss_vscrollbar* vscrollbar_ = nullptr;
        kiss_hscrollbar* hscrollbar_ = nullptr;
        kiss_progressbar* progressbar_ = nullptr;
        kiss_label* label_ = nullptr;
        kiss_entry* entry_ = nullptr;
        kiss_textbox* textbox_ = nullptr;
        kiss_combobox* combobox_ = nullptr;
        UILayer layer_ = UILayer::UNKNOWN;
        std::vector<Event_Component> triggered_events; //events triggered when this element is activated, if any
        std::vector<std::shared_ptr<UIContainerComponent>> child_components_;
        std::vector<std::function<void()>> callback_functions_;
        std::string property_ = ""; //associated property. eg for progressbar to display.

    };

    struct UICollectionSingleton : ECS_Component
    {
    private:
       
       static std::shared_ptr<UICollectionSingleton> instance_;
       static std::mutex lock_;
       

    public:

        UICollectionSingleton()
        {
            component_type = ComponentTypeEnum::UI_SINGLE;
        };



        static std::shared_ptr<UICollectionSingleton> get_instance()
        {
            std::lock_guard<std::mutex> guard(lock_);//Wait until data is available (no other theadss have checked it out
            if (instance_ == nullptr)
            {
                instance_ = std::make_shared< UICollectionSingleton>();
            }
            return instance_;
        }



       void add_ui_element(ComponentTypeEnum game_state, std::shared_ptr<UIContainerComponent> component_ptr)
        {
          
            auto game_state_ui_elements_it = state_to_ui_map_.find(game_state);
            auto layer = component_ptr->layer_;
            if (component_ptr->type_ == UIType::UNKNOWN)
            {
                printf("UI::AddComponent: Trying to add unknown component type_ enum\n");
                abort();
            }
            if (game_state <= ComponentTypeEnum::GAME_STATE_START_HERE)
            {
                printf("UI:Add_Component:: passed in state (%s) is not a game state.\n", component_type_enum_to_string().at(game_state).c_str());
                abort();
            }
            if (layer == UILayer::UNKNOWN)
            {
                printf("UI:Add_Component:: Layer is UNKNOWN");
                abort();
            }
            if (game_state_ui_elements_it == state_to_ui_map_.end())
            {
                printf("UI:Add_Component::Adding new Game State : %s\n", component_type_enum_to_string().at(game_state).c_str());
                printf("UI:Add_Component::Adding new Layer to Game State: %d\n", layer);
                std::map<UILayer, std::vector<std::shared_ptr<UIContainerComponent>>> layer_to_components;
                std::vector<std::shared_ptr<UIContainerComponent>> components;
                components.push_back(component_ptr);
                layer_to_components[layer] = components;
                state_to_ui_map_[game_state] = layer_to_components;
            }
            else
            {
                bool placed = false;
                for (auto components_per_layer : game_state_ui_elements_it->second)
                {
                    if (components_per_layer.first == layer)
                    {
                        components_per_layer.second.push_back(component_ptr);
                        game_state_ui_elements_it->second[layer] = components_per_layer.second;
                        state_to_ui_map_[game_state] = game_state_ui_elements_it->second;
                        placed = true;
                    }
                }
                if (!placed)
                {
                    printf("UI:Add_Component::Adding new Layer to Game State: %d\n", layer);
                    std::vector<std::shared_ptr<UIContainerComponent>> components;
                    components.push_back(component_ptr);
                    game_state_ui_elements_it->second[layer] = components;
                    state_to_ui_map_[game_state] = game_state_ui_elements_it->second;
                }
                return;
            }
            
        }
         
  



      
       //Map of GameStates to a map of Components by Layer
       //Visualization:
       /**
       * state_to_ui_map
       *                |
       *                 __ MAIN_MENU_STATE 
       *                                    |
       *                                     __ TOP (layer)
       *                                                    |
       *                                                     __ Button 1
       *                                                    |
       *                                                     __ Button 2
       *  
       *                                     __ BOTTOM (layer)
       *                                                    |
       *                                                     __ Window 1
       *                                                    |
       *                                                     __ Button 3 (if Button 1 is overlapping Button 2, Button 2 will be clicked instead of Button 1 because Button 1 on a higher layer (TOP vs BOTTOM)
       * 
       * 
       *                 __ MAIN_GAME_STATE Access a map of UI elements based on index to game state (MAIN MENU has different UI than MAIN GAME ...)
       *                                    |
       *                                     __ TOP (layer)
       *                                                    |
       *                                                     __ Some buttons...
       *  
       *                                     __ BOTTOM (layer)
       *                                                    |
       *                                                     __ Some windows...
       * 
       **/
       
       
       //unordered map first  because order does not matter, std::map for layer because order matters (need to iterate through top, then mid, then bottom, etc. when handling events)
        std::unordered_map<ComponentTypeEnum,std::map<UILayer,std::vector<std::shared_ptr<UIContainerComponent>>>> state_to_ui_map_;
        

       
    };

}


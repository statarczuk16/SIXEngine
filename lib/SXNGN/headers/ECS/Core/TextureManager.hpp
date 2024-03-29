#pragma once

#include "ECS/Core/System.hpp"
#include "ECS/Core/Types.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <SDL.h>
#include <Texture.h>
#include <SDL_FontCache.h>
#include <gameutils.h>


class TextureManager
{
public:

	TextureManager(SDL_Renderer* renderer)
	{
		renderer_ = renderer;
		debug_font_ = FC_CreateFont();
		
		std::string  media_folder = SXNGN::Gameutils::find_folder_in_project_string("media");
		if (media_folder == SXNGN::BAD_STRING_RETURN)
		{
			std::cout << "Fatal: TextureManager: " << " Could not find media folder" << std::endl;
			abort();
		}
		std::string debug_font_path = media_folder + "/fonts/debug_font.ttf";
		FC_LoadFont(debug_font_, renderer_, debug_font_path.c_str(), 20, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);
	}

	std::shared_ptr<SXNGN::Texture> get_texture(std::string texture_name)
	{
		if (m_string_to_texture_map_.count(texture_name) == 1)
		{
			return m_string_to_texture_map_[texture_name];
		}
		return nullptr;
	}

	bool add_texture(std::string texture_name, std::shared_ptr<SXNGN::Texture> texture)
	{
		if (texture)
		{
			m_string_to_texture_map_[texture_name] = texture;
			return true;
		}
		printf("TextureManager:: add_texture:: Cannot store null texture %s\n", texture_name.c_str());
		return false;
	}

	bool load_texture_from_path(std::string texture_name, std::string texture_path)
	{
		if (renderer_ == nullptr)
		{
			printf("TextureManager::load_texture_from_path:: Renderer is null %s\n", texture_path.c_str());
			return false;
		}
		std::shared_ptr<SXNGN::Texture> texture = std::make_shared<SXNGN::Texture>(renderer_);

		//Load tile texture
		if (!texture->loadFromFile(texture_path))
		{
			printf("TextureManager::load_texture_from_path:: failed to load from path: %s\n for texture name:%s\n", texture_path.c_str(), texture_name.c_str());
			return false;
		}

	
		bool success = add_texture(texture_name, texture);
		if (success)
		{
			printf("TextureManager::load_texture_from_path:: Successfully loaded texture %s from path\n%s\n", texture_name.c_str(), texture_path.c_str());
		}
		return success;
					
	}

	SDL_Renderer* get_renderer()
	{
		return renderer_;
	}
	
	FC_Font* get_debug_font()
	{
		return debug_font_;
	}

private:
	//Entities must have ALL components in signature to be added to entities_actable list of system
	std::unordered_map < std::string, std::shared_ptr<SXNGN::Texture> > m_string_to_texture_map_;
	SDL_Renderer* renderer_;
	FC_Font* debug_font_;

};

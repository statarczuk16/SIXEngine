#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <memory>

namespace SXNGN {
	//Texture wrapper class
	class Texture
	{
	public:
		//Initializes variables
		Texture(SDL_Renderer*);

		//Deallocates memory
		~Texture();

		//Loads image at specified path
		bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor(Uint8 red, Uint8 green, Uint8 blue);

		//Set blending
		void setBlendMode(SDL_BlendMode blending);

		//Set alpha modulation
		void setAlpha(Uint8 alpha);

		//Renders texture at given point
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

		void render2(SDL_Rect bounding_box, SDL_Rect clip, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, bool outline = false);
		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture_;

		//Image dimensions
		int mWidth_;
		int mHeight_;
		SDL_Renderer* renderer_;
	};

}
#include <Texture.h>
#include <Constants.h>
#include <Database.h>


SXNGN::Texture::Texture(SDL_Renderer* renderer)
{
	//Initialize
	mTexture_ = NULL;
	mWidth_ = 0;
	mHeight_ = 0;
	renderer_ = renderer;
}

SXNGN::Texture::~Texture()
{
	//Deallocate
	free();
}


bool SXNGN::Texture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer_, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth_ = loadedSurface->w;
			mHeight_ = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture_ = newTexture;
	return mTexture_ != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool SXNGN::Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void SXNGN::Texture::free()
{
	//Free texture if it exists
	if (mTexture_ != NULL)
	{
		SDL_DestroyTexture(mTexture_);
		mTexture_ = NULL;
		mWidth_ = 0;
		mHeight_ = 0;
	}
}

void SXNGN::Texture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture_, red, green, blue);
}

void SXNGN::Texture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture_, blending);
}

void SXNGN::Texture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture_, alpha);
}

void SXNGN::Texture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth_, mHeight_ };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;

		//SDL_RenderSetScale(renderer_, float(SXNGN::TILE_WIDTH_SCALE), float(SXNGN::TILE_HEIGHT_SCALE));
		renderQuad.x *= SXNGN::Database::get_scale();
		renderQuad.y *= SXNGN::Database::get_scale();
		renderQuad.w *= SXNGN::Database::get_scale();
		renderQuad.h *= SXNGN::Database::get_scale();
	}

	//Render to screen
	if (mTexture_)
	{
		SDL_RenderCopyEx(renderer_, mTexture_, clip, &renderQuad, angle, center, flip);
	}
	else
	{
		//Render red filled quad
		SDL_SetRenderDrawColor(renderer_, 0xFF, 0xCC, 0xCC, 0xFF);
		SDL_RenderFillRect(renderer_, &renderQuad);
	}
	
}

void SXNGN::Texture::render2(SDL_Rect bounding_box, SDL_Rect clip, double angle, SDL_Point* center, SDL_RendererFlip flip, bool outline)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = bounding_box;

	//Set clip rendering dimensions
	//renderQuad.w = clip->w;
	//renderQuad.h = clip->h;

	//SDL_RenderSetScale(renderer_, float(SXNGN::TILE_WIDTH_SCALE), float(SXNGN::TILE_HEIGHT_SCALE));
	renderQuad.x *= SXNGN::Database::get_scale();
	renderQuad.y *= SXNGN::Database::get_scale();
	renderQuad.w *= SXNGN::Database::get_scale();
	renderQuad.h *= SXNGN::Database::get_scale();
	//Render to screen
	if (mTexture_)
	{
		SDL_RenderCopyEx(renderer_, mTexture_, &clip, &renderQuad, angle, center, flip);
	}
	else
	{
		//Render red filled quad
		SDL_SetRenderDrawColor(renderer_, 0xFF, 0xCC, 0xCC, 0xFF);
		SDL_RenderFillRect(renderer_, &renderQuad);
	}
	if (outline)
	{
		//Render red filled quad
		SDL_SetRenderDrawColor(renderer_, 0xAD, 0xD8, 0xE6, 0xFF);
		SDL_RenderDrawRect(renderer_, &renderQuad);
	}


}

int SXNGN::Texture::getWidth()
{
	return mWidth_;
}

int SXNGN::Texture::getHeight()
{
	return mHeight_;
}

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
		//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
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

void SXNGN::Texture::render_text(int x, int y, FC_Font* font, std::string string)
{
	x *= SXNGN::Database::get_scale();
	y *= SXNGN::Database::get_scale();
	FC_Draw(font, renderer_, x, y, string.c_str());
}

void SXNGN::Texture::render_circle(int32_t center_x, int32_t center_y, int32_t radius)
{
	center_x *= SXNGN::Database::get_scale();
	center_y *= SXNGN::Database::get_scale();
	radius *= SXNGN::Database::get_scale();
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer_, center_x + x, center_y - y);
		SDL_RenderDrawPoint(renderer_, center_x + x, center_y + y);
		SDL_RenderDrawPoint(renderer_, center_x - x, center_y - y);
		SDL_RenderDrawPoint(renderer_, center_x - x, center_y + y);
		SDL_RenderDrawPoint(renderer_, center_x + y, center_y - x);
		SDL_RenderDrawPoint(renderer_, center_x + y, center_y + x);
		SDL_RenderDrawPoint(renderer_, center_x - y, center_y - x);
		SDL_RenderDrawPoint(renderer_, center_x - y, center_y + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

void SXNGN::Texture::renderf(SDL_FRect bounding_box, SDL_Rect clip, double angle, SDL_FPoint* center, SDL_RendererFlip flip, bool outline)
{
	//Set rendering space and render to screen
	SDL_FRect renderQuad = bounding_box;

	//Set clip rendering dimensions
	//renderQuad.w = clip->w;
	//renderQuad.h = clip->h;
	//clip.x += 1;
	//clip.y += 1;
	//clip.w -= 2;
	//clip.h -= 2;

	double scale = (SXNGN::Database::get_scale());
	
	//renderQuad.x = scale * round(renderQuad.x);
	//renderQuad.y = scale * round(renderQuad.y);
	//renderQuad.w = scale * round(renderQuad.w);
	//renderQuad.h = scale * round(renderQuad.h);
	
	SDL_RenderSetScale(renderer_, scale, scale);

	//SDL_RenderSetScale(renderer_, SXNGN::Database::get_scale(), SXNGN::Database::get_scale());
	//SDL_RenderSetLogicalSize(renderer_, 1920, 1080);
	//SDL_RenderSetIntegerScale(renderer_, SDL_TRUE);
	//Render to screen
	if (mTexture_)
	{
		SDL_RenderCopyExF(renderer_, mTexture_, &clip, &renderQuad, angle, center, flip);
	}
	else
	{
		//Render red filled quad
		SDL_SetRenderDrawColor(renderer_, 0xFF, 0xCC, 0xCC, 0xFF);
		SDL_RenderFillRectF(renderer_, &renderQuad);
	}
	if (outline)
	{
		//Render red filled quad
		SDL_SetRenderDrawColor(renderer_, 0xAD, 0xD8, 0xE6, 0xFF);
		SDL_RenderDrawRectF(renderer_, &renderQuad);
	}
	SDL_RenderSetScale(renderer_, 1, 1);


}

void SXNGN::Texture::render2(SDL_Rect bounding_box, SDL_Rect clip, double angle, SDL_Point* center, SDL_RendererFlip flip, bool outline)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = bounding_box;

	//Set clip rendering dimensions
	//renderQuad.w = clip->w;
	//renderQuad.h = clip->h;
	clip.x += 1;
	clip.y += 1;
	clip.w -= 2;
	clip.h -= 2;

	renderQuad.x *= SXNGN::Database::get_scale();
	renderQuad.y *= SXNGN::Database::get_scale();
	renderQuad.w *= (SXNGN::Database::get_scale());
	renderQuad.h *= (SXNGN::Database::get_scale());

	//SDL_RenderSetScale(renderer_, SXNGN::Database::get_scale(), SXNGN::Database::get_scale());
	//SDL_RenderSetLogicalSize(renderer_, 1920, 1080);
	//SDL_RenderSetIntegerScale(renderer_, SDL_TRUE);
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

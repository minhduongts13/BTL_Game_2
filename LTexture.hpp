#ifndef LTEXTURE_HPP
#define LTEXTURE_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <SDL_ttf.h> // để thêm chữ

// The window renderer
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont; // Biến toàn cục để lưu font

// Texture wrapper class
class LTexture
{
public:
	// Initializes variables
	LTexture();

	// Deallocates memory
	~LTexture();

	// Loads image at specified path
	bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
	// Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
	bool rac1(TTF_Font* font, std::string textureText, SDL_Color textColor);
#endif

	// Deallocates texture
	void free();

	// Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// Set blending
	void setBlendMode(SDL_BlendMode blending);

	// Set alpha modulation
	void setAlpha(Uint8 alpha);

	// Renders texture at given point
	void render(double x, double y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void renderScale(int x, int y, int newWidth, int newHeight, SDL_Rect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	// Gets image dimensions
	int getWidth();
	int getHeight();

private:
	// The actual hardware texture
	SDL_Texture *mTexture;

	// Image dimensions
	double mWidth;
	double mHeight;
};

LTexture::LTexture()
{
	// Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	// Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	// Get rid of preexisting texture
	free();

	// The final texture
	SDL_Texture *newTexture = NULL;

	// Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		// Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	// Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	// Get rid of preexisting texture
	free();

	// Render text surface
	TTF_SetFontStyle(gFont, TTF_STYLE_BOLD);
	SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		// Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}

	// Return success
	return mTexture != NULL;
}

bool LTexture::rac1(TTF_Font* font, std::string textureText, SDL_Color textColor)
{
	// Giải phóng texture cũ
    free();
    
    // Kết xuất văn bản ra Surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
    if (textSurface != NULL)
    {
        // Tạo texture từ surface
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        mWidth = textSurface->w;
        mHeight = textSurface->h;

        // Giải phóng surface
        SDL_FreeSurface(textSurface);
    }
    else
    {
        printf("Không thể render text! SDL_ttf Error: %s\n", TTF_GetError());
    }

    return mTexture != NULL;
}
#endif

void LTexture::free()
{
	// Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	// Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	// Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	// Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(double x, double y, SDL_Rect *clip, double angle, SDL_FPoint *center, SDL_RendererFlip flip)
{
	// Set rendering space and render to screen
	SDL_FRect renderQuad = {x, y, mWidth, mHeight};

	// Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// Render to screen
	SDL_RenderCopyExF(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::renderScale(int x, int y, int newWidth, int newHeight, SDL_Rect *clip, double angle, SDL_FPoint *center, SDL_RendererFlip flip)
{
	if (mTexture != NULL)
	{
		// Xác định vùng vẽ với kích thước tùy chỉnh
		SDL_FRect renderQuad = {x, y, newWidth, newHeight};

		// Nếu có clip, lấy kích thước của clip
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		// Render texture với kích thước mới
		SDL_RenderCopyExF(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	}
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

#endif
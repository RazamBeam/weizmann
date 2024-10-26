#include <math.h>

void load_texture(const char* path) {
    // Load image
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        fprintf(stderr, "Error: Unable to load image %s: %s\n", path, IMG_GetError());
        exit(1);
    }

    // Get image dimensions
    imageWidth = surface->w;
    imageHeight = surface->h;

    // Create texture from surface
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "Error: Unable to create texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        exit(1);
    }

    // Free surface
    SDL_FreeSurface(surface);
}


void draw_image(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    SDL_Rect srcRect = { sx, sy, sw, sh };
    SDL_Rect destRect = { dx, dy, dw, dh };
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}


void draw_rect(int x, int y, int w, int h) {
	SDL_Rect rect;
    rect.x = x; // X position
    rect.y = y; // Y position
    rect.w = w; // Width
    rect.h = h; // Height
	
    // Draw the rectangle
    SDL_RenderFillRect(renderer, &rect);
}


void draw_line(int x1, int y1, int x2, int y2) {
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}


void draw_circle(int x, int y, int radius, int r, int g, int b, int a) {
	filledCircleRGBA(renderer, x, y, radius, r, g, b, a);
}


void set_color(int r, int g, int b, int a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}


void clear_display() {
	SDL_RenderClear(renderer);
}


void update_display() {
	SDL_RenderPresent(renderer);
}


void draw_piece(Board b, int i, int x, int y, int r) {
	unsigned int p = 1U << i;
	
	if (p & b.whites) {
		draw_circle(x, y, r, 240, 240, 240, 255);
		if (p & b.kings) {
			draw_circle(x, y, r/2, 0, 0, 0, 255);
			draw_circle(x, y, r/2-2, 240, 240, 240, 255);
		}
	}
	else if (p & b.blacks) {
		draw_circle(x, y, r, 0, 0, 0, 255);
		if (p & b.kings) {
			draw_circle(x, y, r/2, 240, 240, 240, 255);
			draw_circle(x, y, r/2-2, 0, 0, 0, 255);
		}
	}
}












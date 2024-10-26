#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "checkers.h"
#include "bot.h"
#include <time.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

int imageWidth, imageHeight;


#include "sdl_functions.h"


// Screen dimensions
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;


const int gridLen = (SCREEN_WIDTH < SCREEN_HEIGHT ? SCREEN_WIDTH : SCREEN_HEIGHT) * 0.8;
const int gridX = SCREEN_WIDTH/2 - gridLen/2;
const int gridY = SCREEN_HEIGHT/2 - gridLen/2;
const int squareLen = gridLen/8;

int mouseX=-1, mouseY=-1;

int heldI=-1, heldJ=-1;

Board b;
Board b1;


// Function prototypes
void init();
void load_content();
void render();
void handle_events();
void cleanup();

// Game loop control
bool running = true;

int main(int argc, char *argv[]) {
	
	setup(&b);
	
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
	
    // Create window and renderer
    window = SDL_CreateWindow("CHECKERS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // Set multisampling (antialiasing) quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
    // Game loop
    while (running) {
        handle_events();
        render();
    }

    // Cleanup and exit
    cleanup();
    return 0;
}


void render() {
    set_color(86, 86, 86, 255);
    clear_display();
	
	set_color(0, 186, 0, 255);
	draw_rect(0, (b.turn==0 ? SCREEN_HEIGHT-gridY/2 : 0), SCREEN_WIDTH, gridY/2);
	
    for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if ((i+j) % 2 == 0)
				set_color(230, 190, 130, 255);
			else
				set_color(177, 110, 65, 255);
			draw_rect(gridX + j*squareLen, gridY + i*squareLen, squareLen, squareLen);
			if ((i+j)%2 == 1 && (i != heldI || j != heldJ))
				draw_piece(b, getIndex(i,j), gridX + j*squareLen + squareLen/2, gridY + i*squareLen + squareLen/2, squareLen/3);
		}
    }
	
	if (isInBounds(heldI, heldJ) && (heldI+heldJ)%2 == 1) {
		draw_piece(b, getIndex(heldI,heldJ), mouseX, mouseY, squareLen/3);
	}
		
	
    update_display();
}


void handle_events() {
	
	if (hammingWeight(b.whites | b.blacks) <= 10)
		depth = 8;
	
	while (b.turn==1) {
		time_t t = clock();
		Node* root = createNode(b);
		printf("%d\n", buildTree(root, depth));
		b = choose_best_move(root);
		freeTree(root);
		t = clock() - t;
		double timeTaken = ((double)t) / CLOCKS_PER_SEC;
		printf("time taken - %lf\n", timeTaken);
		printf("depth %d\n", depth);
	}
	
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                    
                    heldI = (mouseY-gridY)/squareLen;
                    heldJ = (mouseX-gridX)/squareLen;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                    
                    int releasedI = (mouseY-gridY)/squareLen;
                    int releasedJ = (mouseX-gridX)/squareLen;
                    
                    int heldIndex = getIndex(heldI, heldJ);
                    int releasedIndex = getIndex(releasedI, releasedJ);
                    
                    if (!isInBounds(heldI, heldJ) || !isInBounds(releasedI, releasedJ)
                    	|| ((heldI+heldJ)%2 == 0) || ((releasedI+releasedJ)%2 == 0)) {
                    	heldI = -1;
                    	heldJ = -1;
                    	break;
                    }
                    
					unsigned int validMoves = getValidMoves(b, heldIndex).moves;
					
					if ((1U << releasedIndex) & validMoves) {
						movePiece(&b, heldIndex, releasedIndex);
					}
                    
                    heldI = -1;
                    heldJ = -1;
                }
            default:
                break;
        }
    }
}


void cleanup() {
    // Free texture
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    // Destroy renderer and window
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
	
    // Quit SDL
    SDL_Quit();
}


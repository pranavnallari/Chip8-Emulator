#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef struct { // SDL Window
    uint32_t window_height;
    uint32_t window_width;
    uint32_t fg_colour;
    uint32_t bg_colour;
    uint32_t scale_factor; // Integral Scaling for a singlular chip8 pixel
} config_t;

typedef enum {
    QUIT,
    RUNNING,
    PAUSED
} emulator_state_t;

typedef struct {
    emulator_state_t state;
} chip8_t;

bool init_sdl(sdl_t *sdl, const config_t config) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Failed to Initialize SDL : %s\n", SDL_GetError());
        return false;
    }
    sdl->window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.window_width * config.scale_factor, config.window_height * config.scale_factor, 0);
    if (!sdl->window) {
        SDL_Log("Could not cerate SDL Window : %s\n", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer) {
        SDL_Log("Failed to initialize Renderer : %s\n",SDL_GetError());
        return false;
    }

    return true;
}

// Set up initial emu config from args

bool set_config_from_args(config_t *config, int argc, char **argv) {
    // Set defaults
    *config = (config_t){
        .window_width = 64,
        .window_height = 32,
        .fg_colour = 0xFFFFFFFF,
        .bg_colour = 0x00000000,
        .scale_factor = 20
    };
    return true;
}

//initialize chip8 machine
bool init_chip8(chip8_t *chip8) {
    chip8->state = RUNNING; // default state
    return true;
}

void final_cleanup(const sdl_t sdl) {
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
}

// Clear Screen / SDl Window to background colour
void clear_screen(sdl_t sdl, config_t config) {
    const uint8_t r = (config.bg_colour >> 24) & 0xFF;
    const uint8_t g = (config.bg_colour >> 16) & 0xFF;
    const uint8_t b = (config.bg_colour >> 8) & 0xFF;
    const uint8_t a = (config.bg_colour >> 0) & 0xFF;

    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
    SDL_RenderClear(sdl.renderer);
}

//update window with any changes
void update_screen(const sdl_t sdl) {
    SDL_RenderPresent(sdl.renderer);
}

// handle user input
void handle_input(chip8_t* chip8) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                chip8->state = QUIT;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: // closes window when user presses esc
                        chip8->state = QUIT;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                break;
            default:
                break;
        }
    }
}

int main(int argc, char **argv) {
    
    // Initialize Emulator config
    config_t config = {0};
    if (!set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);

    // Initialize SDL
    sdl_t sdl = {0};
    if (!init_sdl(&sdl, config)) exit(EXIT_FAILURE);

    // Initialize CHIP8 Machine
    chip8_t chip8 = {0};
    if (!init_chip8(&chip8)) exit(EXIT_FAILURE);
    // Intial Screen Clear
    clear_screen(sdl, config);

    // main emulator loop

    while (chip8.state != QUIT) {
        // handle input
        handle_input(&chip8);
        // Emulate Chip8 instructions

        // delay for 60Hz/60fps
        SDL_Delay(16);
        // update window with changes
        update_screen(sdl);
    }

    // Final Cleanup
    final_cleanup(sdl);

    return 0;
}




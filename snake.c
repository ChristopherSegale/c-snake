/*
  Author: Christopher Segale
  Version: 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600
#define TILE 20

enum direction { UP, DOWN, LEFT, RIGHT };

struct body {
    SDL_Rect *part;
    struct body *rest;
};

void clear_screen(SDL_Renderer *r);
SDL_Rect * make_head(void);
struct body * make_body(int x, int y);
void add_body(struct body **b, int x, int y);
void step_body(struct body **b, int x, int y);
SDL_Rect * make_fruit(void);
int intersect_body(struct body *b, SDL_Rect *r);
void draw_head(SDL_Renderer *r, SDL_Rect *h);
void draw_fruit(SDL_Renderer *r, SDL_Rect *f);
void draw_body(SDL_Renderer *r, struct body *b);
void clean_body(struct body *b);

int main (void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Window *win = SDL_CreateWindow
    (
        "Snake",
        100,
        100,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        goto error1;
    }
    SDL_Renderer *ren = SDL_CreateRenderer
    (
        win,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!ren) {
        printf("SDL_Renderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        goto error1;
    }
    SDL_Event event;
    SDL_Rect *head = make_head();
    SDL_Rect *fruit = make_fruit();
    if (!head || !fruit) {
        puts("Could not allocate memory for SDL_Rect.");
        goto error2;
    }
    struct body *snake_body = NULL;
    enum direction snake_direction = LEFT;
    int previousx, previousy;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                goto end;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    goto end;
                    break;
                case SDLK_UP:
                    snake_direction = UP;
                    break;
                case SDLK_DOWN:
                    snake_direction = DOWN;
                    break;
                case SDLK_LEFT:
                    snake_direction = LEFT;
                    break;
                case SDLK_RIGHT:
                    snake_direction = RIGHT;
                    break;
                }
            }
        }
        previousx = head->x;
        previousy = head->y;
        switch (snake_direction) {
        case UP:
            head->y -= 20;
            break;
        case DOWN:
            head->y += 20;
            break;
        case LEFT:
            head->x -= 20;
            break;
        case RIGHT:
            head->x += 20;
            break;
        }
        if (intersect_body(snake_body, head)) {
            goto end;
        }
        step_body(&snake_body, previousx, previousy);
        if((head->x < 0 || head->x > (WIDTH - TILE)) || (head->y < 0 || head->y > (HEIGHT - TILE))) {
            goto end;
        }
        if (SDL_HasIntersection(head, fruit) == SDL_TRUE) {
            add_body(&snake_body, previousx, previousy);
            do {
                fruit = make_fruit();
            } while (intersect_body(snake_body, fruit));
        }
        clear_screen(ren);
        draw_head(ren, head);
        draw_body(ren, snake_body);
        draw_fruit(ren, fruit);
        SDL_RenderPresent(ren);
        SDL_Delay(130);
    }
end:
    free(head);
    clean_body(snake_body);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
error1:
    SDL_Quit();
    return -1;
error2:
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    goto error1;
}

void clear_screen(SDL_Renderer *r)
{
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderClear(r);
}

SDL_Rect * make_head(void)
{
    SDL_Rect *h = malloc(sizeof(SDL_Rect));
    if (h) {
        h->x = WIDTH / 2;
        h->y = HEIGHT / 2;
        h->w = TILE;
        h->h = TILE;
    }
   return h;
}

struct body * make_body(int x, int y)
{
    struct body *b = malloc(sizeof(struct body));
    if (b) {
        b->part = malloc(sizeof(SDL_Rect));
        if (b->part) {
            b->part->x = x;
            b->part->y = y;
            b->part->w = TILE;
            b->part->h = TILE;
            b->rest = NULL;
        }
        else {
            free(b);
            b = NULL;
        }
    }
    return b;
}

void add_body(struct body **b, int x, int y)
{
    if (!(*b)) {
        (*b) = make_body(x, y);
    }
    else {
        add_body(&((*b)->rest), x, y);
    }
}

void step_body(struct body **b, int x, int y)
{
    if ((*b)) {
        struct body *placeholder = (*b)->rest;
        free((*b)->part);
        free(*b);
        (*b) = placeholder;
        add_body(b, x, y);
    }
}

SDL_Rect * make_fruit(void)
{
    SDL_Rect *f = malloc(sizeof(SDL_Rect));
    if (f) {
        f->w = TILE;
        f->h = TILE;
        do {
            f->x = rand() % ((WIDTH - TILE) + 1);
            f->y = rand() % ((HEIGHT - TILE) + 1);
        } while (f->x % TILE != 0 || f->y % TILE != 0);
    }
    return f;
}

int intersect_body(struct body *b, SDL_Rect *r)
{
    if (b && r) {
        if (SDL_HasIntersection(b->part, r) == SDL_TRUE) {
            return 1;
        }
        else {
            return intersect_body(b->rest, r);
        }
    }
    else {
        return 0;
    }
}

void draw_head(SDL_Renderer *r, SDL_Rect *h)
{
    SDL_SetRenderDrawColor(r, 153, 76, 0, 255);
    SDL_RenderFillRect(r, h);
    SDL_RenderDrawRect(r, h);
}

void draw_fruit(SDL_Renderer *r, SDL_Rect *f)
{
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    SDL_RenderFillRect(r, f);
    SDL_RenderDrawRect(r, f);
}

void draw_body(SDL_Renderer *r, struct body *b)
{
    if (b) {
        struct body *i = NULL;
        SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
        for (i = b; i; i = i->rest) {
            SDL_RenderFillRect(r, i->part);
            SDL_RenderDrawRect(r, i->part);
        }
    }
}

void clean_body(struct body *b)
{
    if (b) {
        struct body *p = NULL;
        struct body *r = NULL;
        for (p = b; p; p = r) {
            r = p->rest;
            free(p->part);
            free(p);
        }
    }
}

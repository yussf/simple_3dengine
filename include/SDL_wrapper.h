#include "structures.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;
class SDL_wrapper
{
public:
    SDL_bool done;
    int initial_time;
    SDL_Renderer* renderer  = NULL;
    SDL_Window* window      = NULL;
    int fg_rate             = 500;
    int window_width        = 800;
    int window_heigth       = 800;
    float coef_reduction    = 0.005;
    color background        = {0,0,0,1};
    int mode                = 0;
    int step                = 0;
    ~SDL_wrapper()
    {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    int init(int mode)
    {
        this->mode = mode;
        on_create();

        if (SDL_Init(SDL_INIT_VIDEO) == 0)
        {
            if (SDL_CreateWindowAndRenderer(window_width, window_heigth, SDL_WINDOW_RESIZABLE, &window, &renderer) == 0)
            {
                done            = SDL_FALSE;
                return 0;
            }else
            {
                cout << "Error : SDL_CreateWindowAndRenderer failed";
                return 1;
            }
        }else
        {
            cout << "Error: SDL_INIT_VIDEO failed";
            return 1;
        }
    }
    void start()
    {
        auto initial_time = system_clock::now();
        while (!done)
        {
            auto current_time   = system_clock::now();
            auto counter        = duration<float>(current_time - initial_time);
            float elapsed       = counter.count();
            int int_elapsed     = elapsed*100000;

            if (int_elapsed % fg_rate == 0)
            {
                SDL_Event event;
                SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                if (mode == 0) on_update(elapsed);
                if (mode == 1) on_update(step,coef_reduction);
                SDL_RenderPresent(renderer);
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT) done = SDL_TRUE;
                    else if(event.type == SDL_KEYDOWN) on_keydown(event.key.keysym.sym);
                    else if((event.type == SDL_WINDOWEVENT)
                            & (event.window.event == SDL_WINDOWEVENT_RESIZED)) on_resize();
                }
            }
        }
    }
    void stepIncrement(int i = 1)
    {
        step += i;
    }
    void set_color(color c)
    {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    }
    void draw_line(pt a, pt b)
    {
        SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y);
    }
    void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
        SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
    }
    void draw_triangle(triangle &T, color c = {255,255,255})
    {
        set_color(c);
        draw_triangle(T.d[0].x, T.d[0].y, T.d[1].x, T.d[1].y, T.d[2].x, T.d[2].y);
    }
    void draw_triangle(triangle &T, color c[3])
    {
        set_color(c[0]);
        draw_line(T.d[0],T.d[2]);
        set_color(c[1]);
        draw_line(T.d[0],T.d[1]);
        set_color(c[2]);
        draw_line(T.d[1],T.d[2]);
    }
    void fill_triangle(triangle T, color c = {255,255,255})
    {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        vector<pt> nodes = {(pt)T.d[0], (pt)T.d[1], (pt)T.d[2]};
        sort(nodes.begin(), nodes.end(), [](pt a, pt b)
        {
                return (b.y > a.y);
        });

        line line1 = nodes[0] + nodes[2];
        line line2 = nodes[0] + nodes[1];
        line line3 = nodes[1] + nodes[2];

        //cout << line1.getString() << "||" << line2.getString() << "||" << line3.getString() << endl;
        //cout << nodes[0].getString() << "||" << nodes[1].getString() << "||" << nodes[2].getString() << endl;

        for (int y = nodes[0].y+1; y<nodes[2].y-1; y++)
        {
            if (y <= nodes[1].y)
            {
                SDL_RenderDrawLine(renderer, line1.getX(y), y, line2.getX(y), y);
            }else
            {
                SDL_RenderDrawLine(renderer, line1.getX(y), y, line3.getX(y), y);
            }
        }
    }
    int get_Wheigth()
    {
        return window_heigth;
    }
    int get_Wwidth()
    {
        return window_width;
    }
    void update_window_size(){
        SDL_GetWindowSize(window, &window_width, &window_heigth);
    }
    int on_resize(){
        update_window_size();
        if (mode == 1) on_update(step,0.2f);
        return 0;
    }
    virtual int on_create()                                         = 0;
    virtual int on_update(float elapsed)                            = 0;
    virtual int on_update(int step, float reduction_coef)           = 0;
    virtual int on_keydown(SDL_Keycode key)                         = 0;
};
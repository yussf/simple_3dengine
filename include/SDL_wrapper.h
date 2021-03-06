/*
	Author 		: Youssef Doubli
	Github 		: github.com/yussf
	Email		: doubli@eurecom.fr
	Last update : 3/01/2020

*/
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
    rgb background          = {0,0,0,255};
    int step                = 0;
    ~SDL_wrapper()
    {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    int init()
    {
        on_create();

        if (SDL_Init(SDL_INIT_VIDEO) == 0)
        {
            if (SDL_CreateWindowAndRenderer(window_width, window_heigth, SDL_WINDOW_RESIZABLE, &window, &renderer) == 0)
            {
                done = SDL_FALSE;
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
                set_rgb(background);
                SDL_RenderClear(renderer);
                on_update(elapsed);                
                SDL_RenderPresent(renderer);
                while (SDL_PollEvent(&event))
                {
                    if     (event.type  == SDL_QUIT) done = SDL_TRUE;
                    else if(event.type  == SDL_KEYDOWN) on_keydown(event.key.keysym.sym);
                    else if(event.type  == SDL_KEYUP) on_keyup(event.key.keysym.sym);
                    else if(event.type  == SDL_MOUSEWHEEL) on_wheel(event.wheel.y);
                    else if(event.type  == SDL_MOUSEMOTION) on_motion();
                    else if (event.type == SDL_MOUSEBUTTONUP
                            | event.type == SDL_MOUSEBUTTONDOWN) on_mouse(event.type);
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
    void set_rgb(rgb c)
    {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    }
    void draw_line(float x1, float y1, float x2, float y2)
    {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
    void draw_line(pt a, pt b)
    {
        draw_line(a.x, a.y, b.x, b.y);
    }
    void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        draw_line(x1, y1, x2, y2);
        draw_line(x2, y2, x3, y3);
        draw_line(x3, y3, x1, y1);
    }
    void draw_triangle(triangle &T, rgb c)
    {
        set_rgb(c);
        draw_triangle(T.d[0].x, T.d[0].y, T.d[1].x, T.d[1].y, T.d[2].x, T.d[2].y);
    }
    void draw_triangle(triangle &T)
    {
        draw_triangle(T,T.color);
    }
    void draw_triangle(triangle &T, rgb c[3])
    {
        set_rgb(c[0]);
        draw_line(T.d[0],T.d[2]);
        set_rgb(c[1]);
        draw_line(T.d[0],T.d[1]);
        set_rgb(c[2]);
        draw_line(T.d[1],T.d[2]);
    }
    void fill_triangle(triangle &T, rgb c)
    {
        set_rgb(c);
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

        for (int y = nodes[0].y+1; y<nodes[2].y; y++)
        {
            if (y <= nodes[1].y)
            {
                draw_line(line1.getX(y), y, line2.getX(y), y);
            }else
            {
                draw_line(line1.getX(y), y, line3.getX(y), y);
            }
        }
    }
    void fill_triangle(triangle &T)
    {
        fill_triangle(T,T.color);
    }
    void fill_triangle(triangle &T, float L)
    {
        fill_triangle(T,T.color*L);
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
        return 0;
    }
    virtual int on_create()                                         = 0;
    virtual int on_update(float elapsed)                            = 0;
    virtual int on_keydown(SDL_Keycode key)                         = 0;
    virtual int on_keyup(SDL_Keycode key)                           = 0;
    virtual int on_wheel(Sint32 wheely)                             = 0;
    virtual int on_mouse(Uint32 type)                               = 0;
    virtual int on_motion()                                         = 0;
};
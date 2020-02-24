#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include <OpenGL/glu.h>
#include "main.h"
#include "libs/maximilian.h"
#include "synth.h"
#include "control.h"
#include "gui_container.h"
#include "utils.h"
#include "defs.h"

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
  /* Extract relevent data from user pointer */
  User_pointers* user_pointers = (User_pointers*) user_data;
  int* sample_num = user_pointers->sample_num;
  Synth* synth = user_pointers->synth;

  Sint16 *buffer = (Sint16*) raw_buffer;
  int length = bytes / 2;

  for(int i = 0; i < length; i++, sample_num++)
  {
    buffer[i] = synth->tick();
  }
}

void init_sdl(User_pointers* user_ptrs) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_Window* window = SDL_CreateWindow("WobWob",SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  SDL_GLContext context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1);

  user_ptrs->window = window;
  user_ptrs->context = context;
}

void init_audio(User_pointers* user_ptrs)
{
  SDL_AudioSpec spec_want;
  SDL_AudioSpec spec_have; //for error checking, l8r

  spec_want.freq = SAMPLE_RATE;
  spec_want.format = AUDIO_S16SYS;
  spec_want.channels = 1;
  spec_want.samples = BUFFER;
  spec_want.callback = audio_callback;
  spec_want.userdata = user_ptrs;

  SDL_OpenAudio(&spec_want, &spec_have);
}

void init_gl()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.f, 0.f, 0.f, 1.f);
}


void render(User_pointers* user_ptrs)
{
  glClear(GL_COLOR_BUFFER_BIT);

  user_ptrs->gui_container->draw();

  //draw cursor (debug)
  glPointSize(10);
  glBegin(GL_POINTS);
  glVertex2f(*user_ptrs->mouse_x, *user_ptrs->mouse_y);
  glEnd();

}

void quit(User_pointers* user_ptrs)
{
  SDL_DestroyWindow(user_ptrs->window);
  SDL_Quit();
}

int main(int argc, char* args[])
{

  bool should_quit = false;

  struct User_pointers user_pointers;
  user_pointers.sample_num = new int(0);
  Synth synth;
  user_pointers.synth = &synth;

  init_sdl(&user_pointers);
  SDL_Event events;
  init_gl();
  init_audio(&user_pointers);
  SDL_PauseAudio(0);

  float mouse_x, mouse_y;
  user_pointers.mouse_x = &mouse_x;
  user_pointers.mouse_y = &mouse_y;
  to_gl_coords(&mouse_x, &mouse_y, WIDTH, HEIGHT);

  Control first, second, third;
  first.assign_control(&synth.carrier_freq, 0, 800);
  second.assign_control(&synth.modulator_freq, 0, 50);
  third.assign_control(&synth.amplitude, 0, 1);

  GUI_Container gui_container;
  user_pointers.gui_container = &gui_container;
  gui_container.add(&first);
  gui_container.add(&second);
  gui_container.add(&third);

  while(!should_quit)
  {
    to_gl_coords(&mouse_x, &mouse_y, WIDTH, HEIGHT);
    while(SDL_PollEvent(&events) != 0)
    {
      if(events.type == SDL_QUIT)
      {
        should_quit = true;
      }
    }

    render(&user_pointers);
    SDL_GL_SwapWindow(user_pointers.window);

    gui_container.intersect(mouse_x, mouse_y);

  }
  quit(&user_pointers);
  return 0;
}

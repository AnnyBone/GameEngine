/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "engine.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

// SDL Window Manager

#define WINDOW_MINIMUM_WIDTH    320
#define WINDOW_MINIMUM_HEIGHT   240

namespace xenon {
    namespace engine {

        class Window {
        public:
            Window(unsigned int width, unsigned int height);

            ~Window();

            void SetSize(unsigned int width, unsigned int height);

            void SetFullscreen(bool fullscreen);

            void SetVerticalSync(unsigned int interval);

            void SetGamma(unsigned short *ramp, int size);

            unsigned int GetWidth() { return width_; }

            unsigned int GetHeight() { return height_; }

            void Swap();

        protected:
        private:
            void Update();

            bool is_unlocked_;
            bool is_active_;
            bool is_fullscreen_;

            unsigned int interval_;
            unsigned int width_, height_;

            SDL_Window *instance_;
            SDL_GLContext glcontext_;
        };

        Window::Window(unsigned int width, unsigned int height) :
                is_unlocked_(false), is_active_(true), is_fullscreen_(false), interval_(0) {
            // todo, window parm checks
#if 0
            if (COM_CheckParm("-window"))
                g_mainwindow.is_unlocked = true;

            if (COM_CheckParm("-width")) {
                g_mainwindow.width = (PLuint) atoi(com_argv[COM_CheckParm("-width") + 1]);
                g_mainwindow.is_unlocked = true;
            } else g_mainwindow.width = WINDOW_MINIMUM_WIDTH;

            if (COM_CheckParm("-height")) {
                g_mainwindow.height = (PLuint) atoi(com_argv[COM_CheckParm("-height") + 1]);
                g_mainwindow.is_unlocked = true;
            } else g_mainwindow.height = WINDOW_MINIMUM_HEIGHT;
#endif

#if 0       // todo, include this
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
#if 0       // todo, try this
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
#endif
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            unsigned int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
            if (is_fullscreen_) {
                flags |= SDL_WINDOW_FULLSCREEN;
            }

            instance_ = SDL_CreateWindow(
                    "Xenon", // todo, pull from game script.
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    width_, height_,
                    flags
            );
            if (!instance_) {
                System_Error("Failed to create window!\n%s\n", SDL_GetError());
            }

            std::string path = current_gamepath + "/icon.bmp";
            SDL_Surface * icon = SDL_LoadBMP(path.c_str());
            if(icon) {
                SDL_SetColorKey(icon, true, SDL_MapRGB(icon->format, 0, 0, 0));
                SDL_SetWindowIcon(instance_, icon);
                SDL_FreeSurface(icon);
            } else {
                // todo, warning that it's missing
            }

            glcontext_ = SDL_GL_CreateContext(instance_);
            if(!glcontext_) {
                System_Error("Failed to create GL context!\n%s\n", SDL_GetError());
            }

            SetVerticalSync(interval_);
        }

        Window::~Window() {
            if (instance_) {
                SDL_DestroyWindow(instance_);
            }
        }

        void Window::SetSize(unsigned int width, unsigned int height) {
            if((width == width_) && (height == height_)) {
                return;
            }

            if(width < WINDOW_MINIMUM_WIDTH || height < WINDOW_MINIMUM_HEIGHT) {
                // todo, warning

                width = WINDOW_MINIMUM_WIDTH; height = WINDOW_MINIMUM_HEIGHT;
            } else if(!is_fullscreen_ && ((width > plGetScreenWidth()) || (height > plGetScreenHeight()))) {
                // todo, warning "attempted to set window size larger than scope of desktop"

                width = plGetScreenWidth(); height = plGetScreenHeight();
            }

            width_ = width; height_ = height;
            SDL_SetWindowSize(instance_, width_, height_);
            SDL_SetWindowPosition(instance_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

            // todo, handle console vars
            // todo, update viewport
        }

        void Window::SetFullscreen(bool fullscreen) {
            if(is_fullscreen_ == fullscreen) {
                return;
            }

            if(SDL_SetWindowFullscreen(instance_, (SDL_bool)fullscreen) != 0) {
                // todo, warning
                return;
            }

            if(!fullscreen) {
                SDL_SetWindowPosition(instance_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            }

            is_fullscreen_ = fullscreen;

            // todo, update console vars
        }

        void Window::SetVerticalSync(unsigned int interval) {
            if(interval_ == interval) {
                return;
            }

            if(SDL_GL_SetSwapInterval(interval) != 0) {
                // todo, warning
                return;
            }

            interval_ = interval;
            // todo, update console vars
        }

        void Window::SetGamma(unsigned short *ramp, int size) {
            if (!SDL_SetWindowGammaRamp(instance_, ramp, ramp + size, ramp + size * 2)) {
                // todo, error
            }
        }

        void Window::Swap() {
            if (!instance_) {
                return;
            }
            SDL_GL_SwapWindow(instance_);
        }
    }
}

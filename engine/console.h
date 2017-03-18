
#pragma once

#include <deque>

namespace xenon {
    namespace engine {

        class Console {
        public:
            Console();

            void Clear();
            void ClearNotify();

            void Print(const char *text);

            void ScrollUp();
            void ScrollDown();
            void ScrollHome();
            void ScrollEnd();

            void SetSize(unsigned int width, unsigned int height);

            void Draw(bool input);
            void DrawNotify();

        protected:
        private:
            struct Line {
                std::string text;
                time_t time;

                Line(const std::string &t) : text(t), time(0) {}
            };

            size_t cursor_x, cursor_y;
            size_t backscroll;

            unsigned int width_, height_;

            std::deque<Line> lines_;

            void LineFeed();

            std::list<std::string> PrepareText(unsigned int cols, unsigned int rows);
            static std::list<std::string> WrapLine(std::string line, unsigned int cols);
        };

    }
}
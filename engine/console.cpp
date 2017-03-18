
#include "engine.h"

#define CONSOLE_CHAR_WIDTH      8
#define CONSOLE_CHAR_HEIGHT     8
#define CONSOLE_CHAR_BACKLOG    200
#define CONSOLE_MAX_NOTIFY      10

namespace xenon {
    namespace engine {

        Console::Console() : cursor_x(0), cursor_y(0), backscroll(0) {
            lines_.emplace_back("");
        }

        void Console::Clear() {
            cursor_x = cursor_y = 0;

            lines_.clear();
            lines_.emplace_back("");
        }

        void Console::ClearNotify() {
            for (auto l = lines_.begin(); l != lines_.end(); ++l) {
                l->time = 0;
            }
        }

        void Console::Print(const char *text) {
            /* A string starting with 0x02 is "coloured" and the characters are
             * ORd with 128 so the rendering code highlights them.
             */
            unsigned char cbit = 0;
            if (*text == 0x02) {
                cbit = 128;
                ++text;
            }

            while (*text) {
                if (*text == '\n') {
                    LineFeed();
                    cursor_x = 0;
                } else if (*text == '\r') {
                    cursor_x = 0;
                } else {
                    if (lines_[cursor_y].text.size() > cursor_x)
                        lines_[cursor_y].text[cursor_x] = (*text | cbit);
                    else
                        lines_[cursor_y].text += (*text | cbit);

                    lines_[cursor_y].time = time(NULL);

                    ++cursor_x;
                }

                ++text;
            }
        }
    }
}

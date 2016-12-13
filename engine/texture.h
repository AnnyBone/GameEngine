
#pragma once

#include "engine.h"

namespace engine {
    namespace video {
        class Texture {
        public:
            Texture();
            ~Texture();

            void SetImage(PLImage *image);

            unsigned int GetWidth() const { return width_; }
            unsigned int GetHeight() const { return height_; }
            unsigned int GetSize() const { return size_; }
            unsigned int GetFlags() const { return flags_; }

            void AddFlags(unsigned int flags);
            void RemoveFlags(unsigned int flags);
            void SetFlags(unsigned int flags) { flags_ = flags; }
            void ClearFlags() { flags_ = 0; }

            void Bind();
            void Unbind();

        protected:
        private:

        };
    }
}
#ifndef RENDERER_HEADER
#define RENDERER_HEADER

#include <sstream>

class Renderer {
public:
   Renderer() : output() {}
   virtual std::string render() = 0;
protected:
   std::ostringstream output;
};

#endif

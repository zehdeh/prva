#ifndef RENDERER_HEADER
#define RENDERER_HEADER

#include <sstream>

class renderer {
public:
   renderer() : output() {}
   virtual std::string render() = 0;
protected:
   std::ostringstream output;
};

#endif

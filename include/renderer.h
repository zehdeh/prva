#ifndef RENDERER_HEADER
#define RENDERER_HEADER

#include <sstream>

#include "relation_properties.h"

class renderer {
public:
   renderer(relation_properties props) : output(), relation_props(props) {}
   virtual std::string render() = 0;
   virtual std::string getLog() = 0;
protected:
   std::ostringstream output;
   std::ostringstream log;
   relation_properties relation_props;
};

#endif

#ifndef HTMLRENDERER_HEADER
#define HTMLRENDERER_HEADER

#include "renderer.h"

class html_renderer : public renderer {
public:
   html_renderer(const char * header_file, const char * footer_file);
   std::string render();
private:
   const char * header_file;
   const char * footer_file;

   void output_file(const char * fileName);
};

#endif

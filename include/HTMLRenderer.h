#ifndef HTMLRENDERER_HEADER
#define HTMLRENDERER_HEADER

#include "Renderer.h"

class HTMLRenderer : public Renderer {
public:
   HTMLRenderer(const char * header_file, const char * footer_file);
   std::string render();
private:
   const char * header_file;
   const char * footer_file;

   void output_file(const char * fileName);
};

#endif

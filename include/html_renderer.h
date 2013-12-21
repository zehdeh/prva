#ifndef HTMLRENDERER_HEADER
#define HTMLRENDERER_HEADER

#include "renderer.h"
#include <set>

class html_renderer : public renderer {
public:
   html_renderer(relation_properties props, const char * header_file, const char * footer_file);
   std::string render();
private:
   const char * header_file;
   const char * footer_file;

   void output_file(const char * fileName);
   void render_page();
   void render_transactions();
};

#endif

#ifndef HTMLRENDERER_HEADER
#define HTMLRENDERER_HEADER

#include "renderer.h"
#include <set>
#include "value_renderer.h"

class html_renderer : public renderer {
public:
   html_renderer(relation_properties props, const char * header_file, const char * footer_file);
   std::string render();
   std::string getLog() { return log.str(); }
private:
   const char * header_file;
   const char * footer_file;
   value_renderer val_renderer;

   void output_file(const char * fileName);
   void render_page();
   void render_transactions();

   void render_valueviews();
   void render_headervalues();
   void render_tuplevalues();
};

#endif

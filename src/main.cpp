#include <iostream>
#include <stdlib.h>

#include "cmdline.h"
#include "html_renderer.h"
#include "database_wrapper.h"

int main(int argc, char** argv) {
   gengetopt_args_info args_info;

   if(cmdline_parser(argc, argv, &args_info) != 0) {
         exit(1);
   }

   // Connection info for pqxx
   const char * conninfo = (std::string("dbname=") + std::string(args_info.database_arg)).c_str();

   // Page-Nr, default is = 0
   unsigned int page_nr = (args_info.page_nr_arg >= 0)?args_info.page_nr_arg:0;
   try {
      database_wrapper setup(conninfo, args_info.relation_arg, page_nr);
      relation_properties props = setup.pack_relation_properties();

      html_renderer renderer(props, "resource/header.html", "resource/footer.html");

      if(args_info.debug_flag) {
         renderer.render();
         std::cout << renderer.getLog();
      } else {
         std::cout << renderer.render();
      }
   } catch(const std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}

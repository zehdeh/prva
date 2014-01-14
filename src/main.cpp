#include <iostream>
#include <stdlib.h>

#include "cmdline.h"
#include "html_renderer.h"
#include "database_wrapper.h"

#define PGDATABASE test

int main(int argc, char** argv) {
   gengetopt_args_info args_info;

   if(cmdline_parser(argc, argv, &args_info) != 0) {
         exit(1);
   }

   const char * conninfo = (std::string("dbname=") + std::string(args_info.database_arg)).c_str();
   relation_properties props;
   try {
      database_wrapper setup(conninfo, args_info.relation_arg);
      props = setup.pack_relation_properties(); 
      
      html_renderer renderer(props, "resource/header.html", "resource/footer.html");
      //renderer.render();
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



   //char * binaryPage = PQgetvalue(res, 0, 0);

   //std::cout << binaryPage << std::endl;
   //PQclear(res);

   //res = PQexec(conn, "CLOSE raw_cursor");
   //checkCommand(res, "Closing raw_cursor", PGRES_COMMAND_OK, conn);
   //PQclear(res);

   //res = PQexec(conn, "FETCH ALL IN items_cursor");
   //checkCommand(res, "Fetching from items_cursor", PGRES_TUPLES_OK, conn);

   /*
   int nTuples = PQntuples(res);
   int * lp_off = new int[nTuples];
   int * lp_len = new int[nTuples];

   for(int i = 0; i < nTuples; i++) {
      lp_off[i] = atoi(PQgetvalue(res,i,0)) * 2;
      lp_len[i] = atoi(PQgetvalue(res,i,1)) * 2;
      //std::cout << lp_off[i] << std::endl;
      //std::cout << lp_len[i] << std::endl;
      //printf("%.*s\n", lp_len[i], binaryPage + lp_off[i]);
   }

   outputFile("header.html");
   renderTable(binaryPage, nTuples, freespaceLower, freespaceUpper, lp_off, lp_len);
   outputFile("footer.html");

   res = PQexec(conn, "CLOSE items_cursor");
   //checkCommand(res, "Closing items_cursor", PGRES_COMMAND_OK, conn);
   PQclear(res);

   res = PQexec(conn, "END");
   //checkCommand(res, "Ending transaction", PGRES_COMMAND_OK, conn);
   PQclear(res);

   PQfinish(conn);

   delete [] lp_off;
   delete [] lp_len;
   */

   return 0;
}

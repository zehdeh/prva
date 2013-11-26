#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#define PGDATABASE test

void exit_nicely(PGconn * conn) {
   PQfinish(conn);
   exit(1);
}

void checkCommand(PGresult * res, const char * identifier, ExecStatusType status, PGconn * conn) {
   if(PQresultStatus(res) != status) {
      std::cout << identifier << " failed: " << PQerrorMessage(conn) << std::endl;
      PQclear(res);
      exit_nicely(conn);
   }
}

void outputFile(const char * fileName) {
   std::ifstream header(fileName, std::ios::in);

   if(header.is_open()) {
      std::string s;
      while(getline(header,s)) {
         std::cout << s << std::endl;
      }

      header.close();
   }
}

void renderTable(char * bin, int nTuples, int freespaceLower, int freespaceUpper, int * lp_off, int * lp_len) {
   int binaryLen = strlen(bin);

   bool tuple_open = false;
   for(int i = 0; i < binaryLen; i++) {
      if(i == freespaceLower) {
         std::cout << "<span class=\"range freespace\">" << std::endl;   
      }

      if(i == freespaceUpper) {
         std::cout << "</span>" << std::endl;   
      }

      for(int j = 0; j < nTuples; j++) {
         if(!tuple_open && (i == lp_off[j])) {
            tuple_open = true;
            std::cout << "<span class=\"range tuple\">" << std::endl;
         }
      }
      std::cout << bin[i] << std::endl;

      for(int j = 0; j < nTuples; j++) {
         if(tuple_open && i == (lp_off[j] + lp_len[j])) {
            tuple_open = false;
            std::cout << "</span>" << std::endl;   
         }
      }
      for(int j = 0; j < nTuples; j++) {
         if(!tuple_open && (i == lp_off[j])) {
            tuple_open = true;
            std::cout << "<span class=\"range tuple\">" << std::endl;
         }
      }
   }
   if(tuple_open) {
      tuple_open = false;
      std::cout << "</span>" << std::endl;   
   }
}

int main(int argc, char** argv) {

   const char * conninfo = "dbname = fabian";
   PGconn * conn;
   PGresult * res;

   conn = PQconnectdb(conninfo);
   if(PQstatus(conn) != CONNECTION_OK) {
      std::cout << "Verbindung fehlgeschlagen: " << PQerrorMessage(conn) << std::endl;
      exit_nicely(conn);
   }

   res = PQexec(conn, "BEGIN");
   checkCommand(res, "Beginning transaction", PGRES_COMMAND_OK, conn);

   res = PQexec(conn, "DECLARE pageheader_cursor CURSOR FOR SELECT lower,upper FROM page_header(get_raw_page('users', 0))");
   checkCommand(res, "Declaring cursor for pageheader", PGRES_COMMAND_OK, conn);

   res = PQexec(conn, "DECLARE raw_cursor CURSOR FOR SELECT encode(get_raw_page::bytea, 'hex') FROM get_raw_page('users', 0)");
   checkCommand(res, "Declaring cursor for get_raw_page", PGRES_COMMAND_OK, conn);

   res = PQexec(conn, "DECLARE items_cursor CURSOR FOR SELECT lp_off, lp_len FROM heap_page_items(get_raw_page('users', 0)) order by lp desc");
   checkCommand(res, "Declaring cursor for heap_page_items", PGRES_COMMAND_OK, conn);

   res = PQexec(conn, "FETCH ALL IN pageheader_cursor");
   checkCommand(res, "Fetching from pageheader_cursor", PGRES_TUPLES_OK, conn);

   int freespaceLower = atoi(PQgetvalue(res,0,0)) * 2;
   int freespaceUpper = atoi(PQgetvalue(res,0,1)) * 2;

   res = PQexec(conn, "CLOSE pageheader_cursor");
   checkCommand(res, "Closing pageheader_cursor", PGRES_COMMAND_OK, conn);
   PQclear(res);

   res = PQexec(conn, "FETCH ALL IN raw_cursor");
   checkCommand(res, "Fetching from raw_cursor", PGRES_TUPLES_OK, conn);

   char * binaryPage = PQgetvalue(res, 0, 0);

   //std::cout << binaryPage << std::endl;
   PQclear(res);

   res = PQexec(conn, "CLOSE raw_cursor");
   checkCommand(res, "Closing raw_cursor", PGRES_COMMAND_OK, conn);
   PQclear(res);

   res = PQexec(conn, "FETCH ALL IN items_cursor");
   checkCommand(res, "Fetching from items_cursor", PGRES_TUPLES_OK, conn);

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
   checkCommand(res, "Closing items_cursor", PGRES_COMMAND_OK, conn);
   PQclear(res);

   res = PQexec(conn, "END");
   checkCommand(res, "Ending transaction", PGRES_COMMAND_OK, conn);
   PQclear(res);

   PQfinish(conn);

   delete [] lp_off;
   delete [] lp_len;

   return 0;
}

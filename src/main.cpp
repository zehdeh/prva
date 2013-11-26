#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#define PGDATABASE test

void exit_nicely(PGconn * conn) {
   PQfinish(conn);
   exit(1);
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

   PQfinish(conn);

   return 0;
}

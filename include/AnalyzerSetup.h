#ifndef ANALYZERSETUP_HEADER
#define ANALYZERSETUP_HEADER

#include <pqxx/pqxx>
#include <vector>

class AnalyzerSetup {
public:
   AnalyzerSetup(const char * conninfo, const char * pRelation);

   unsigned int get_freespace_lower_bound();
   unsigned int get_freespace_upper_bound();
private:
   pqxx::connection conn;
   pqxx::transaction<> work;

   std::string relation;

   unsigned int freespace_lbound;
   unsigned int freespace_ubound;
   bool freespace_queried;

   void get_freespace_bounds();

   pqxx::result do_get_freespace_bounds();
   pqxx::result do_get_raw_page();
   pqxx::result do_get_heap_page_items();
};

#endif

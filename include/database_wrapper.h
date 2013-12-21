#ifndef DATABASE_WRAPPER_HEADER
#define DATABASE_WRAPPER_HEADER

#include <pqxx/pqxx>
#include "relation_properties.h"

class database_wrapper {
public:
   database_wrapper(const char * conninfo, const char * pRelation);
   ~database_wrapper() {};

   unsigned int get_freespace_lower_bound();
   unsigned int get_freespace_upper_bound();
   std::string get_raw_page();
   std::vector<unsigned int> get_lp_off();
   std::vector<unsigned int> get_lp_len();
   std::vector<unsigned int> get_xmin();
   std::vector<unsigned int> get_xmax();
   unsigned int get_cnt_tuples();

   relation_properties pack_relation_properties();
private:
   pqxx::connection conn;
   pqxx::transaction<> work;

   std::string relation;

   unsigned int freespace_lbound;
   unsigned int freespace_ubound;
   bool freespace_queried;

   std::string raw_page;

   std::vector<unsigned int> lp_off;
   std::vector<unsigned int> lp_len;
   std::vector<unsigned int> xmin;
   std::vector<unsigned int> xmax;

   unsigned int cnt_tuples;

   void get_freespace_bounds();
   void get_heap_page_items();

   pqxx::result do_get_freespace_bounds();
   pqxx::result do_get_raw_page();
   pqxx::result do_get_heap_page_items();
};

#endif

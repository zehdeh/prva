#ifndef DATABASE_WRAPPER_HEADER
#define DATABASE_WRAPPER_HEADER

#include <pqxx/pqxx>
#include "relation_properties.h"

// Wrapper for Postgres, hides everything database-related and just output stuff important to us
class database_wrapper {
public:
   database_wrapper(const char * conninfo, const char * pRelation, const unsigned int pPage_nr);
   ~database_wrapper() {};

   // Getter for single attributes
   unsigned int get_freespace_lower_bound();
   unsigned int get_freespace_upper_bound();
   std::string get_raw_page();
   std::vector<unsigned int> get_lp_off();
   std::vector<unsigned int> get_lp_len();
   std::vector<unsigned int> get_xmin();
   std::vector<unsigned int> get_xmax();
   std::vector<unsigned int> get_att_types();
   std::vector<std::string> get_att_names();
   unsigned int get_cnt_tuples();

   // Getter that packs everything important in a struct
   relation_properties pack_relation_properties();
private:
   pqxx::connection conn;
   pqxx::transaction<> work;

   std::string relation;
   const unsigned int page_nr;

   std::string page_nr_str;

   unsigned int freespace_lbound;
   unsigned int freespace_ubound;
   bool freespace_queried;

   std::string raw_page;

   std::vector<unsigned int> lp_off;
   std::vector<unsigned int> lp_len;
   std::vector<unsigned int> xmin;
   std::vector<unsigned int> xmax;

   std::vector<unsigned int> types;
   std::vector<std::string> names;

   unsigned int cnt_tuples;
   unsigned int cnt_types;

   void get_freespace_bounds();
   void get_heap_page_items();
   void get_attributes();

   // Actual pq-related funcs
   pqxx::result do_get_freespace_bounds();
   pqxx::result do_get_raw_page();
   pqxx::result do_get_heap_page_items();
   pqxx::result do_get_attributes();
};

#endif

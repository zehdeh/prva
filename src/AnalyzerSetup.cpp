#include "AnalyzerSetup.h"

AnalyzerSetup::AnalyzerSetup(const char * conninfo, const char * pRelation) : conn(conninfo), work(conn), relation(pRelation), freespace_queried(false) {}

unsigned int AnalyzerSetup::get_freespace_lower_bound() {
   if(!freespace_queried) {
      get_freespace_bounds();
   }

   return freespace_lbound; 
}  

unsigned int AnalyzerSetup::get_freespace_upper_bound() {
   if(!freespace_queried) {
      get_freespace_bounds();
   }

   return freespace_ubound; 
}  

void AnalyzerSetup::get_freespace_bounds() {
   pqxx::result result = do_get_freespace_bounds();

   freespace_lbound = result[0]["lower"].as<unsigned int>();
   freespace_ubound = result[0]["upper"].as<unsigned int>();

   freespace_queried = true;
}

pqxx::result AnalyzerSetup::do_get_freespace_bounds() {
   return work.exec("SELECT lower,upper FROM page_header(get_raw_page('" + relation + "', 0))");
}

pqxx::result AnalyzerSetup::do_get_raw_page() {
   return work.exec("SELECT encode(get_raw_page::bytea, 'hex') FROM get_raw_page('" + relation + "', 0)");
}

pqxx::result AnalyzerSetup::do_get_heap_page_items() {
   return work.exec("SELECT lp_off, lp_len FROM heap_page_items(get_raw_page('" + relation + "', 0)) order by lp desc");
}

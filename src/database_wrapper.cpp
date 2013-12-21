#include "database_wrapper.h"
#include <iostream>

relation_properties database_wrapper::pack_relation_properties() {
   relation_properties props;

   props.freespace_lbound = get_freespace_lower_bound();
   props.freespace_ubound = get_freespace_upper_bound();
   props.raw_page = get_raw_page();
   props.lp_off = get_lp_off();
   props.lp_len = get_lp_len();
   props.xmin = get_xmin();
   props.xmax = get_xmax();
   props.cnt_tuples = get_cnt_tuples();

   return props;
}

database_wrapper::database_wrapper(const char * conninfo, const char * pRelation) : conn(conninfo), work(conn),
   relation(pRelation), freespace_queried(false), lp_off(), lp_len(), xmin(), xmax() {}

unsigned int database_wrapper::get_cnt_tuples() {
   if(cnt_tuples == 0) {
      get_heap_page_items();
   }

   return cnt_tuples;
}

std::vector<unsigned int> database_wrapper::get_lp_off() {
   if(lp_off.empty()) {
      get_heap_page_items();
   }

   return lp_off;
}

std::vector<unsigned int> database_wrapper::get_lp_len() {
   if(lp_len.empty()) {
      get_heap_page_items();
   }

   return lp_len;
}

std::vector<unsigned int> database_wrapper::get_xmin() {
   if(xmin.empty()) {
      get_heap_page_items();
   }

   return xmin;
}

std::vector<unsigned int> database_wrapper::get_xmax() {
   if(xmax.empty()) {
      get_heap_page_items();
   }

   return xmax;
}

void database_wrapper::get_heap_page_items() {
   pqxx::result res = do_get_heap_page_items();

   cnt_tuples = res.size();
   
   for(unsigned int i = 0; i < res.size(); i++) {
      lp_off.push_back(res[i]["lp_off"].as<unsigned int>() * 2);
      lp_len.push_back(res[i]["lp_len"].as<unsigned int>() * 2);
      xmin.push_back(res[i]["t_xmin"].as<unsigned int>());
      xmax.push_back(res[i]["t_xmax"].as<unsigned int>());
      //std::cout << lp_off[i] << std::endl;
      //std::cout << lp_len[i] << std::endl;
   }
}

std::string database_wrapper::get_raw_page() {
   if(raw_page.empty()) {
      pqxx::result res = do_get_raw_page();

      raw_page = res[0][0].as<std::string>();
   }

   return raw_page;
}

unsigned int database_wrapper::get_freespace_lower_bound() {
   if(!freespace_queried) {
      get_freespace_bounds();
   }

   return freespace_lbound; 
}  

unsigned int database_wrapper::get_freespace_upper_bound() {
   if(!freespace_queried) {
      get_freespace_bounds();
   }

   return freespace_ubound; 
}  

void database_wrapper::get_freespace_bounds() {
   pqxx::result result = do_get_freespace_bounds();

   freespace_lbound = result[0]["lower"].as<unsigned int>() * 2;
   freespace_ubound = result[0]["upper"].as<unsigned int>() * 2;

   freespace_queried = true;
}

pqxx::result database_wrapper::do_get_freespace_bounds() {
   return work.exec("SELECT lower,upper FROM page_header(get_raw_page('" + relation + "', 0))");
}

pqxx::result database_wrapper::do_get_raw_page() {
   return work.exec("SELECT encode(get_raw_page::bytea, 'hex') FROM get_raw_page('" + relation + "', 0)");
}

pqxx::result database_wrapper::do_get_heap_page_items() {
   return work.exec("SELECT t_xmin, t_xmax, lp_off, lp_len FROM heap_page_items(get_raw_page('" + relation + "', 0)) order by lp desc");
}

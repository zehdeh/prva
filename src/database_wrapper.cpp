#include "database_wrapper.h"
#include <iostream>

relation_properties database_wrapper::pack_relation_properties() {
   relation_properties props(relation, page_nr, get_raw_page());

   props.freespace_lbound = get_freespace_lower_bound();
   props.freespace_ubound = get_freespace_upper_bound();
   props.lp_off = get_lp_off();
   props.lp_len = get_lp_len();
   props.xmin = get_xmin();
   props.xmax = get_xmax();
   props.cnt_tuples = get_cnt_tuples();
   props.cnt_item_identifiers = ((get_freespace_lower_bound() - 24) / 4);
   props.types = get_att_types();
   props.names = get_att_names();

   return props;
}

database_wrapper::database_wrapper(const char * conninfo, const char * pRelation, const unsigned int pPage_nr) : conn(conninfo), work(conn),
   relation(pRelation), page_nr(pPage_nr), freespace_queried(false), lp_off(), lp_len(), xmin(), xmax(), types(), names(){

      // Convert to string, since pgxx wants so
      std::stringstream sstm;
      sstm << page_nr;
      page_nr_str = sstm.str();
   }

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
   if(types.empty()) {
      get_heap_page_items();
   }

   return lp_len;
}

std::vector<std::string> database_wrapper::get_att_names() {
   if(names.empty()) {
      get_attributes();
   }

   return names;
}

std::vector<unsigned int> database_wrapper::get_att_types() {
   if(types.empty()) {
      get_attributes();
   }

   return types;
}

void database_wrapper::get_attributes() {
   pqxx::result res = do_get_attributes();

   int cnt_types = res.size();
   for(int i = 0; i < cnt_types; i++) {
      types.push_back(res[i]["atttypid"].as<unsigned int>());
      names.push_back(res[i]["attname"].as<std::string>());
   }
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

// Converts from pqxx- to standard-formats
void database_wrapper::get_heap_page_items() {
   pqxx::result res = do_get_heap_page_items();

   cnt_tuples = res.size();

   for(unsigned int i = 0; i < res.size(); i++) {
      lp_off.push_back(res[i]["lp_off"].as<unsigned int>());
      lp_len.push_back(res[i]["lp_len"].as<unsigned int>());
      if(!res[i]["t_xmin"].is_null()) {
         xmin.push_back(res[i]["t_xmin"].as<unsigned int>());
      } else {
         xmin.push_back(0);
      }
      if(!res[i]["t_xmax"].is_null()) {
         xmax.push_back(res[i]["t_xmax"].as<unsigned int>());
      } else {
         xmax.push_back(0);
      }
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

   freespace_lbound = result[0]["lower"].as<unsigned int>();
   freespace_ubound = result[0]["upper"].as<unsigned int>();

   freespace_queried = true;
}

pqxx::result database_wrapper::do_get_freespace_bounds() {
   return work.exec("SELECT lower,upper FROM page_header(get_raw_page('" + relation + "', " + page_nr_str + "))");
}

pqxx::result database_wrapper::do_get_raw_page() {
   return work.exec("SELECT encode(get_raw_page::bytea, 'hex') FROM get_raw_page('" + relation + "', " + page_nr_str + ")");
}

pqxx::result database_wrapper::do_get_heap_page_items() {
   return work.exec("SELECT t_xmin, t_xmax, lp_off, lp_len FROM heap_page_items(get_raw_page('" + relation + "', " + page_nr_str + ")) ORDER BY lp desc");
}

pqxx::result database_wrapper::do_get_attributes() {
   return work.exec("SELECT attname, atttypid FROM pg_attribute WHERE attrelid = (SELECT oid FROM pg_class WHERE relname = '" + relation + "') AND attnum > 0");
}

#ifndef RELATION_PROPERTIES_HEADER
#define RELATION_PROPERTIES_HEADER

#include <vector>

struct relation_properties {
   relation_properties(const std::string pRelation, const unsigned int pPage_nr, const std::string pRaw_page) : relation(pRelation), page_nr(pPage_nr), raw_page(pRaw_page) {}
   const std::string relation;
   const unsigned int page_nr;

   unsigned int freespace_lbound;
   unsigned int freespace_ubound;

   const std::string raw_page;

   std::vector<unsigned int> lp_off;
   std::vector<unsigned int> lp_len;
   std::vector<unsigned int> xmin;
   std::vector<unsigned int> xmax;
   std::vector<unsigned int> types;
   std::vector<std::string> names;
   unsigned int cnt_tuples;
   unsigned int cnt_item_identifiers;
};

#endif

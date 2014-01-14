#ifndef RELATION_PROPERTIES_HEADER
#define RELATION_PROPERTIES_HEADER

#include <vector>

struct relation_properties {
   unsigned int freespace_lbound;
   unsigned int freespace_ubound;

   std::string raw_page;

   std::vector<unsigned int> lp_off;
   std::vector<unsigned int> lp_len;
   std::vector<unsigned int> xmin;
   std::vector<unsigned int> xmax;
   std::vector<unsigned int> types;
   unsigned int cnt_tuples;
};

#endif

#ifndef RELATION_PROPERTIES_HEADER
#define RELATION_PROPERTIES_HEADER

struct relation_properties {
   unsigned int freespace_lbound;
   unsigned int freespace_ubound;

   std::string raw_page;

   unsigned int * lp_off;
   unsigned int * lp_len;
   unsigned int cnt_tuples;
};

#endif

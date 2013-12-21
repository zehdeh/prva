#include "html_renderer.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string.h>

html_renderer::html_renderer(relation_properties props, const char * header_file, const char * footer_file) : renderer(props), header_file(header_file), footer_file(footer_file) {
}

void html_renderer::output_file(const char * fileName) {
   std::ifstream fileStream(fileName, std::ios::in);

   if(fileStream.is_open()) {
      std::string s;
      while(getline(fileStream,s)) {
         output << s << std::endl;
      }

      fileStream.close();
   } else {
      std::cerr << fileName << " could not be opened for reading." << std::endl;
   }
}

std::string html_renderer::render() {
   output_file(header_file); 

   render_page();
   render_transactions();

   output_file(footer_file); 
   return output.str();
}

void html_renderer::render_transactions() {
   output << "<div id=\"controls\">" << std::endl;
   output << "<div id=\"transaction_table\">" << std::endl;
         
   std::vector<unsigned int> inserts;
   std::vector<unsigned int> deletes;
   std::vector<unsigned int> updates;

   std::sort(relation_props.xmin.begin(), relation_props.xmin.end());
   std::sort(relation_props.xmax.begin(), relation_props.xmax.end());

   std::vector<unsigned int>::const_iterator it1;
   for(it1 = relation_props.xmin.begin(); it1 != relation_props.xmin.end(); ++it1) {
      if(std::find(relation_props.xmax.begin(), relation_props.xmax.end(), *it1) == relation_props.xmax.end()) {
         inserts.push_back(*it1);
      } else {
         updates.push_back(*it1);
      }
   }

   std::vector<unsigned int>::const_iterator it2;
   for(it2 = relation_props.xmax.begin(); it2 != relation_props.xmax.end(); ++it2) {
      if(*it2 != 0 && std::find(relation_props.xmin.begin(), relation_props.xmin.end(), *it2) == relation_props.xmin.end()) {
         deletes.push_back(*it2);
      } 
   }

   unsigned int inserts_i = 0;
   unsigned int updates_i = 0;
   unsigned int deletes_i = 0;
   for(unsigned int i = 0; i < (inserts.size() + updates.size() + deletes.size()); i++) {
      unsigned int smallest_tid = 0;
      if(inserts.size() > inserts_i) {
         if(updates.size() > updates_i) {
            if(deletes.size() > deletes_i) {
               smallest_tid = std::min(std::min(inserts[inserts_i], updates[updates_i]), deletes[deletes_i]);
            } else {
               smallest_tid = std::min(inserts[inserts_i], updates[updates_i]);
            }
         } else {
            if(deletes.size() > deletes_i) {
               smallest_tid = std::min(inserts[inserts_i], deletes[deletes_i]);
            } else {
               smallest_tid = inserts[inserts_i];
            }
         }
      } else {
         if(updates.size() > updates_i) {
            if(deletes.size() > deletes_i) {
               smallest_tid = std::min(updates[updates_i], deletes[deletes_i]);
            } else {
               smallest_tid = updates[updates_i];
            }
         } else {
            smallest_tid = deletes[deletes_i];
         }
      }

      if(inserts[inserts_i] == smallest_tid) {
         output << "<div class=\"transaction insert\" onmouseover=\"highlight_rows(" << inserts[inserts_i] << ")\" onmouseout=\"unhighlight_rows(" << inserts[inserts_i]<< ")\">" << inserts[inserts_i] << "</div>" << std::endl;
         inserts_i++;
      } else if(updates[updates_i] == smallest_tid) {
         output << "<div class=\"transaction update\" onmouseover=\"highlight_rows(" << updates[updates_i] << ")\" onmouseout=\"unhighlight_rows(" << updates[updates_i]<< ")\">" << updates[updates_i] << "</div>" << std::endl;
         updates_i++;
      } else {
         output << "<div class=\"transaction delete\" onmouseover=\"highlight_rows(" << deletes[deletes_i] << ")\" onmouseout=\"unhighlight_rows(" << deletes[deletes_i]<< ")\">" << deletes[deletes_i] << "</div>" << std::endl;
         deletes_i++;
      }
   }

   output << "</div>" << std::endl;
}

void html_renderer::render_page() {
   output << "<div id=\"page\">" << std::endl;
   unsigned int binaryLen = relation_props.raw_page.length();

   bool tuple_open = false;
   for(unsigned int i = 0; i < binaryLen; i++) {
      if(i == relation_props.freespace_lbound) {
         output << "<span class=\"range freespace\">" << std::endl;   
      }

      if(i == relation_props.freespace_ubound) {
         output << "</span>" << std::endl;   
      }

      for(unsigned int j = 0; j < relation_props.cnt_tuples; j++) {
         if(!tuple_open && (i == relation_props.lp_off[j])) {
            tuple_open = true;
            output << "<span class=\"range tuple\" data-xmin=\"" << relation_props.xmin[j] << "\" data-xmax=\"" << relation_props.xmax[j] << "\">" << std::endl;
         }
      }
      
      output << relation_props.raw_page[i] << std::endl;

      for(unsigned int j = 0; j < relation_props.cnt_tuples; j++) {
         if(tuple_open && i == (relation_props.lp_off[j] + relation_props.lp_len[j])) {
            tuple_open = false;
            output << "</span>" << std::endl;   
         }
      }
      for(unsigned int j = 0; j < relation_props.cnt_tuples; j++) {
         if(!tuple_open && (i == relation_props.lp_off[j])) {
            tuple_open = true;
            output << "<span class=\"range tuple\" data-xmin=\"" << relation_props.xmin[j] << "\" data-xmax=\"" << relation_props.xmax[j] << "\">" << std::endl;
         }
      }
   }
   if(tuple_open) {
      tuple_open = false;
      output << "</span>" << std::endl;   
   }
   output << "</div>" << std::endl;
}

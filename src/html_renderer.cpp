#include "html_renderer.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string.h>

html_renderer::html_renderer(const relation_properties & props, const char * header_file, const char * footer_file) : renderer(props), header_file(header_file), footer_file(footer_file), val_renderer() {}

// Simply points a file to output stream
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

// Render everything
std::string html_renderer::render() {
   output_file(header_file);

   render_title();

   render_page();
   render_transactions();
   render_valueviews();

   output_file(footer_file);
   return output.str();
}

void html_renderer::render_title() {
   output << "<h1>Page <span id=\"page_nr\">" << relation_props.page_nr << "</span> of Relation <span id=\"relation\">" << relation_props.relation << "</span></h1>" << std::endl;
   output << "<div id=\"creator-info\">analyzed by prva</div>" << std::endl;
}

void html_renderer::render_valueviews() {
   render_headervalues();
   render_tuplevalues();
}

void html_renderer::render_headervalues() {
   output << "<div class=\"valueview\" onclick=\"event.cancelBubble=true\" id=\"headervalues\">" << std::endl;
   output << "<div class=\"headline\">Page Header</div><table>" << std::endl;
   output << val_renderer.render_row(relation_props.raw_page, 0,16, &value_renderer::parse_none, "pd_lsn", "LSN: next byte after last byte of xlog record for last change to this page");
   output << val_renderer.render_row(relation_props.raw_page, 16,4, &value_renderer::parse_uint16, "pd_tli", "TimeLineID of last change (only its lowest 16 bits)");
   output << val_renderer.render_row(relation_props.raw_page, 20,4, &value_renderer::parse_uint16, "pd_flags", "Flag bits");
   output << val_renderer.render_row(relation_props.raw_page, 24,4, &value_renderer::parse_uint16, "pd_lower", "Offset to start of free space");
   output << val_renderer.render_row(relation_props.raw_page, 28,4, &value_renderer::parse_uint16, "pd_upper", "Offset to end of free space");
   output << val_renderer.render_row(relation_props.raw_page, 32,4, &value_renderer::parse_uint16, "pd_special", "Offset to start of special space");
   output << val_renderer.render_row(relation_props.raw_page, 32,4, &value_renderer::parse_uint16, "pd_pagesize", "Page size");
   output << val_renderer.render_row(relation_props.raw_page, 36,4, &value_renderer::parse_version, "version", "layout version number information");
   output << val_renderer.render_row(relation_props.raw_page, 40,4, &value_renderer::parse_uint16, "pd_prune_xid", "Oldest unpruned XMAX on page, or zero if none");

   const unsigned int cnt_item_identifiers = ((relation_props.freespace_lbound - 24) / 4);
   log << cnt_item_identifiers << " item identifiers discovered" << std::endl;
   output << "</table></div>" << std::endl;
}

void html_renderer::render_tuplevalues() {
   unsigned int binaryLen = relation_props.raw_page.length();
   for(unsigned int j = 0; j < relation_props.cnt_tuples; j++) {
      output << "<div class=\"valueview\" onclick=\"event.cancelBubble=true\" id=\"tuple-" << relation_props.lp_off[j] << "\">" << std::endl;
         unsigned int lp_off = relation_props.lp_off[j] * 2;
         unsigned int user_off = lp_off + (val_renderer.parse_num<unsigned char>(relation_props.raw_page.substr(lp_off+44,2)) * 2);

         output << "<div class=\"headline\">Tuple Header</div><table>" << std::endl;
         output << val_renderer.render_row(relation_props.raw_page, lp_off,8, &value_renderer::parse_uint32, "t_xmin", "insert XID stamp");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+8,8, &value_renderer::parse_uint32, "t_xmax", "delete XID stamp");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+16,8, &value_renderer::parse_uint32, "t_cid", "insert and/or delete CID stamp (overlays with t_xvac)");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+24,8, &value_renderer::parse_uint32, "t_xvac", "XID for VACUUM operation moving a row version");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+32,4, &value_renderer::parse_itempointerdata, "t_ctid", "current TID of this or newer row version");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+36,4, &value_renderer::parse_uint16, "t_infomask2", "number of attributes, plus various flag bits");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+40,4, &value_renderer::parse_uint16, "t_infomask", "various flag bits");
         output << val_renderer.render_row(relation_props.raw_page, lp_off+44,2, &value_renderer::parse_uint8, "t_hoff", "offset to user data");
         output << "</table><div class=\"headline\">User Data</div><table class=\"userdata\">" << std::endl;

         unsigned int offset = user_off;
         for(unsigned int i = 0; i < relation_props.types.size(); i++) {
            unsigned int align = val_renderer.get_type_align(relation_props.types[i]);
            unsigned int pad = (align * 2) - ((offset - user_off) % (align * 2));
            if(((offset - user_off) % (align * 2)) > 0) {
               offset += pad;
               log << "Padding with " << pad << " Bytes" << std::endl;
            }

            unsigned int next_byte = 8;
            if(offset + next_byte > binaryLen) {
               next_byte = 2;
            }
            unsigned int len = val_renderer.get_type_len(relation_props.types[i], relation_props.raw_page.substr(offset, next_byte));
            log << "Value with typeid "<< relation_props.types[i] << " starts at "
                << (offset - user_off) << " (with alignment " << align << ") and ends at " << (offset - user_off + len) << std::endl;
            output << val_renderer.render_row(relation_props.raw_page, offset, len, val_renderer.get_parser_func(relation_props.types[i]),relation_props.names[i].c_str(), "");
            offset += len;
         }
      output << "</table></div>" << std::endl;
   }
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
      if(*it1 == 0) continue;
      if(std::find(relation_props.xmax.begin(), relation_props.xmax.end(), *it1) == relation_props.xmax.end()) {
         if(std::find(inserts.begin(), inserts.end(), *it1) == inserts.end()) {
            inserts.push_back(*it1);
         }
      } else {
         if(std::find(updates.begin(), updates.end(), *it1) == updates.end()) {
            updates.push_back(*it1);
         }
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
         output << "<div class=\"transaction insert\" onmouseover=\"highlight_rows(" << inserts[inserts_i] << ")\" onmouseout=\"unhighlight_rows(" << inserts[inserts_i]<< ")\">" << inserts[inserts_i] << ": INSERT</div>" << std::endl;
         inserts_i++;
      } else if(updates[updates_i] == smallest_tid) {
         output << "<div class=\"transaction update\" onmouseover=\"highlight_rows(" << updates[updates_i] << ")\" onmouseout=\"unhighlight_rows(" << updates[updates_i]<< ")\">" << updates[updates_i] << ": UPDATE</div>" << std::endl;
         updates_i++;
      } else {
         output << "<div class=\"transaction delete\" onmouseover=\"highlight_rows(" << deletes[deletes_i] << ")\" onmouseout=\"unhighlight_rows(" << deletes[deletes_i]<< ")\">" << deletes[deletes_i] << ": DELETE</div>" << std::endl;
         deletes_i++;
      }
   }

   output << "</div>" << std::endl;
   output << "<input type=\"checkbox\" name=\"show_freespace\" id=\"show_freespace\" onchange=\"toggleFreespace()\" onclick=\"event.cancelBubble = true\" checked><label for=\"show_freespace\" onclick=\"event.cancelBubble = true\">Show freespace</label></div><br class=\"clear\" />" << std::endl;
}

void html_renderer::render_page() {
   output << "<div id=\"page\">" << std::endl;
   unsigned int binaryLen = relation_props.raw_page.length();

   bool tuple_open = false;

   output << "<span class=\"range header\" onclick=\"return show_valueview('headervalues')\">" << std::endl;

   for(unsigned int i = 0; i < binaryLen; i += 2) {
      unsigned int pos = i/2;
      if(pos == 24) {
         output << "</span>" << std::endl;
      }

      for(unsigned int j = 0; j < relation_props.cnt_item_identifiers; j++) {
         if(pos == (24 + (4 * j))) {
            if(j != 0) {
               output << "</span>" << std::endl;
            }
            output << "<span class=\"range item_identifier\">" << std::endl;
         }
      }

      if(pos == relation_props.freespace_lbound) {
         if(relation_props.cnt_item_identifiers > 0) {
            output << "</span>" << std::endl;
         }
         log << "Freespace starts at " << pos << std::endl;
         output << "<span class=\"range freespace\">" << std::endl;
      }

      if(pos == relation_props.freespace_ubound) {
         log << "Freespace ends at " << pos << std::endl;
         output << "</span>" << std::endl;
      }

      for(unsigned int j = 0; j < relation_props.cnt_tuples; j++) {
         if(pos == relation_props.lp_off[j] && relation_props.lp_len[j] != 0) {
            if(tuple_open) {
               log << "A tuple ends at " << pos << std::endl;
               output << "</span>" << std::endl;
            }
            log << "A tuple starts at " << pos << std::endl;
            tuple_open = true;
            output << "<span class=\"range tuple\" onclick=\"show_valueview('tuple-"
               << relation_props.lp_off[j] << "')\" data-xmin=\""
               << relation_props.xmin[j]
               << "\" data-xmax=\""
               << relation_props.xmax[j]
               << "\">" << std::endl;
         }
      }

      output << relation_props.raw_page[i] << relation_props.raw_page[i + 1]  << std::endl;

   }
   if(tuple_open) {
      log << "A tuple ends at " << binaryLen / 2 << std::endl;
      tuple_open = false;
      output << "</span>" << std::endl;
   }
   output << "</div>" << std::endl;
}

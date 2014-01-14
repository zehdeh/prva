#include "value_renderer.h"

#include <iostream>

unsigned int value_renderer::get_type_align(unsigned int typid) {
   unsigned int align = 0;
   switch(typid) {
      case 23:
         align = 4;
         break;
      case 1043:
         align = 1;
         break;
      default:
         break;
   }

   return align;
}

unsigned int value_renderer::get_type_len(unsigned int typid, std::string input) {
   unsigned int len = 0;
   switch(typid) {
      case 23:
         len = 8;
         break;
      case 1043:
         len = ((parse_num<unsigned char>(input) >> 1) & 0x7F) * 2;
         break;
      default:
         break;
   }

   return len;
}

value_renderer::parser_func value_renderer::get_parser_func(unsigned int typid) {
   parser_func parser = &value_renderer::parse_none;
   switch(typid) {
      case 23:
         parser = &value_renderer::parse_int32;
         break;
      case 1043:
         parser = &value_renderer::parse_varchar;
      default:
         break;
   }
   
   return parser;
}

std::string value_renderer::render_row(std::string raw_input, unsigned int pos, unsigned int len, parser_func parser, const char* description) {
   std::ostringstream output;
   std::string input_part = raw_input.substr(pos, len);

   bool num_value = false;
   if(parser == &value_renderer::parse_uint8 || parser == &value_renderer::parse_uint16 || 
         parser == &value_renderer::parse_uint32 || parser == &value_renderer::parse_itempointerdata || parser == &value_renderer::parse_version) {
      num_value = true;
   }

   output << "<div class=\"row\">" << std::endl;
      output << "<div class=\"raw_data\">" << std::endl;
      output << input_part << std::endl;
      output << "</div>" << std::endl;

      output << "<div class=\"value" << ((num_value)?" number":"") << "\">" << std::endl;
      output << (this->*parser)(input_part) << std::endl;
      output << "</div>" << std::endl;

      if(strlen(description) > 0) {
         output << "<div class=\"description\">" << std::endl;
         output << description << std::endl;
         output << "</div>" << std::endl;
      }
      output << "<br class=\"clear\" />" << std::endl;
   output << "</div>" << std::endl;

   return output.str();
}


std::string value_renderer::parse_uint8(std::string input) {
   std::stringstream ss;
   ss << (int)parse_num<unsigned char>(input);

   return ss.str();
}

std::string value_renderer::parse_uint16(std::string input) {
   std::stringstream ss;
   ss << parse_num<unsigned short>(input);

   return ss.str();
}

std::string value_renderer::parse_uint32(std::string input) {
   std::stringstream ss;
   ss << parse_num<unsigned int>(input);

   return ss.str();
}

std::string value_renderer::parse_int32(std::string input) {
   std::stringstream ss;
   ss << parse_num<int>(input);

   return ss.str();
}

std::string value_renderer::parse_version(std::string input) {
   std::stringstream ss;
   unsigned short val = parse_num<unsigned short>(input);
   ss << (val & 0x00FF);

   return ss.str();
}

std::string value_renderer::parse_itempointerdata(std::string input) {
   std::stringstream ss;
   ss << "(" << (int)parse_num<unsigned char>(input.substr(0,1)) << ",";
   ss << (int)parse_num<unsigned char>(input.substr(1,1)) << ")";

   return ss.str();
}
std::string value_renderer::parse_varchar(std::string input) {
   std::stringstream ss;

   for(unsigned int i = 2; i < input.size(); i += 2) {
      ss << parse_num<char>(input.substr(i,2));
   }

   return ss.str();
}

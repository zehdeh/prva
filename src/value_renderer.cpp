#include "value_renderer.h"

#include <iostream>

#include "type_exception.h"

// We hardcode byte-alignments here, querying pg_type would also be possible
const unsigned int value_renderer::get_type_align(const unsigned int & typid) const {
   unsigned int align = 0;
   switch(typid) {
      case 21:
         align = 2;
         break;
      case 23:
      case 24:
      case 25:
      case 26:
      case 28:
      case 194:
      case 700:
      case 1009:
      case 1034:
         align = 4;
         break;
      case 16:
      case 18:
      case 19:
      case 1043:
         align = 1;
         break;
      default:
         throw bad_type_exception(typid);
         break;
   }

   return align;
}

// Hardcoded length of types, some require some figuring
const unsigned int value_renderer::get_type_len(const unsigned int & typid, const std::string & next_byte) const {
   unsigned int len = 0;
   switch(typid) {
      case 16:
      case 18:
         len = 1;
         break;
      case 19:
         len = 64;
         break;
      case 21:
         len = 2;
         break;
      case 23:
         len = 4;
         break;
      case 25:
         len = ((parse_num<unsigned char>(next_byte) >> 1) & 0x7F);
         break;
      case 26:
         len = 4;
         break;
      case 28:
         len = 4;
         break;
      case 700:
         len = 4;
         break;
      case 1009:
         len = (parse_num<unsigned int>(next_byte) >> 2) & 0x3F;
         break;
      case 1034:
         len = (parse_num<unsigned int>(next_byte) >> 2) & 0x3F;
         break;
      case 1043:
         len = ((parse_num<unsigned char>(next_byte.substr(0,2)) >> 1) & 0x7F);
         break;
      default:
         break;
   }
   len = len * 2;

   return len;
}

// Given type-num X, returns a function pointer to a function that is able to parse the type
const value_renderer::parser_func value_renderer::get_parser_func(const unsigned int & typid) const {
   parser_func parser = &value_renderer::parse_none;
   switch(typid) {
      case 16:
         parser = &value_renderer::parse_bool;
         break;
      case 18:
         parser = &value_renderer::parse_num_str<char>;
      case 19:
         parser = &value_renderer::parse_name;
         break;
      case 21:
         parser = &value_renderer::parse_num_str<short>;
         break;
      case 23:
         parser = &value_renderer::parse_int32;
         break;
      case 25:
         parser = &value_renderer::parse_varchar;
         break;
      case 26:
         parser = &value_renderer::parse_num_str<unsigned int>;
         break;
      case 28:
         parser = &value_renderer::parse_num_str<unsigned int>;
         break;
      case 700:
         //std::cout << (float)parse_num<int>("000043b9") << std::endl;
         parser = &value_renderer::parse_num_str<float>;
         break;
      case 1043:
         parser = &value_renderer::parse_varchar;
         break;
      default:
         break;
   }

   return parser;
}

// Some html-specific stuff here - should be in html-renderer, but more convinient here
const std::string value_renderer::render_row(const std::string & raw_input, const unsigned int & pos, const unsigned int & len, const parser_func parser, const char* name, const char* description) const {
   std::ostringstream output;
   std::string input_part = raw_input.substr(pos, len);

   bool num_value = false;
   // If we have a number, right-align value
   if(parser == &value_renderer::parse_uint8 || parser == &value_renderer::parse_uint16 ||
         parser == &value_renderer::parse_uint32 || parser == &value_renderer::parse_itempointerdata || parser == &value_renderer::parse_version) {
      num_value = true;
   }

   output << "<tr class=\"row\">" << std::endl;
      output << "<td class=\"raw_data\"><div>" << std::endl;
      output << input_part << std::endl;
      output << "</div></td>" << std::endl;

      // Output name, if given. Header fields have a name
      if(strlen(name) > 0) {
         output << "<td class=\"name\">" << name << "</td>" << std::endl;
      }

      output << "<td class=\"value" << ((num_value)?" number":"") << "\"><div>" << std::endl;
      output << (this->*parser)(input_part) << std::endl;
      output << "</div></td>" << std::endl;

      // Output description, if given. Header fields have a description
      if(strlen(description) > 0) {
         output << "<td class=\"description\">" << std::endl;
         output << description << std::endl;
         output << "</td>" << std::endl;
      }
   output << "</tr>" << std::endl;

   return output.str();
}


const std::string value_renderer::parse_uint8(const std::string & input) const {
   std::stringstream ss;
   ss << (int)parse_num<unsigned char>(input);

   return ss.str();
}

const std::string value_renderer::parse_bool(const std::string & input) const {
   unsigned char val = (int)parse_num<unsigned char>(input);
   if(val == 1) {
      return std::string("t");
   } else {
      return std::string("f");
   }
}

const std::string value_renderer::parse_uint16(const std::string & input) const {
   std::stringstream ss;
   ss << parse_num<unsigned short>(input);

   return ss.str();
}

const std::string value_renderer::parse_uint32(const std::string & input) const {
   std::stringstream ss;
   ss << parse_num<unsigned int>(input);

   return ss.str();
}

const std::string value_renderer::parse_int32(const std::string & input) const {
   std::stringstream ss;
   ss << static_cast<int>(parse_num<unsigned int>(input));

   return ss.str();
}

const std::string value_renderer::parse_version(const std::string & input) const {
   std::stringstream ss;
   unsigned short val = parse_num<unsigned short>(input);
   ss << (val & 0x00FF);

   return ss.str();
}

const std::string value_renderer::parse_itempointerdata(const std::string & input) const {
   std::stringstream ss;
   ss << "(" << (int)parse_num<unsigned char>(input.substr(0,1)) << ",";
   ss << (int)parse_num<unsigned char>(input.substr(1,1)) << ")";

   return ss.str();
}

const std::string value_renderer::parse_string(const std::string & input, bool skip_first_byte) const {
   std::stringstream ss;
   const unsigned int start = (skip_first_byte)?2:0;

   for(unsigned int i = start; i < input.size(); i += 2) {
      ss << parse_num<char>(input.substr(i,2));
   }

   return ss.str();
}

const std::string value_renderer::parse_varchar(const std::string & input) const {
   return parse_string(input, true);
}

const std::string value_renderer::parse_name(const std::string & input) const {
   return parse_string(input, false);
}

#ifndef VALUERENDERER_HEADER
#define VALUERENDERER_HEADER

#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string.h>

class value_renderer {
public:
	typedef std::string (value_renderer::*parser_func)(std::string);
   std::string parse_none(std::string input) { return "&nbsp;"; }

   template<typename T> T parse_num(std::string input) {
      T val;
      union U {
         T value;
         unsigned char components[sizeof(T)];
      };

      U u;
      std::stringstream hexstream(input);
      if(sizeof(T) > 1) {
         hexstream >> std::hex >> u.value;

         unsigned char* dst = (unsigned char*)&val;
         for(int i = 0; i < sizeof(T); i++) {
            dst[i] = u.components[sizeof(T) - 1 - i];
         }
      } else {
         int char_val;
         hexstream >> std::hex >> char_val;
         val = char_val;
      }

      return val;
   }
   std::string parse_uint8(std::string input);
   std::string parse_uint16(std::string input);
   std::string parse_uint32(std::string input);
   std::string parse_int32(std::string input);

   std::string parse_itempointerdata(std::string input);
   std::string parse_version(std::string input);

   std::string parse_varchar(std::string input);

   unsigned int get_type_align(unsigned int typid);
   unsigned int get_type_len(unsigned int typid, std::string next_char);
   value_renderer::parser_func get_parser_func(unsigned int typid);
   std::string render_row(std::string raw_input, unsigned int pos, unsigned int len, parser_func parser, const char* description);
};

#endif

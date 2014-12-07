#ifndef VALUERENDERER_HEADER
#define VALUERENDERER_HEADER

#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <stdint.h>
#include <algorithm>

class value_renderer {
public:
   typedef const std::string (value_renderer::*parser_func)(const std::string &) const;
   const std::string parse_none(const std::string & input) const { return "&nbsp;"; }

   // Converts a hex-string into bin, swaps bytes (little endian) and casts to format T
   template<typename T> T parse_num(const std::string & input) const {
      std::stringstream hexstream(input);
      uint32_t bits;

      // Hex to bin
      hexstream >> std::hex >> bits;

      unsigned char* swap = (unsigned char*)&bits;
      // Swap bytes
      std::reverse(swap, swap + sizeof(T));

      // Finally cast to specified format
      T val = reinterpret_cast<T &>(bits);

      return val;
   }

   // Convinience method to immediately get the value as a string
   template<typename T> const std::string parse_num_str(const std::string & input) const {
      std::stringstream ss;
      ss << parse_num<T>(input);

      return ss.str();
   }

   // Parser methods for several internal formats, most just build on parse_num
   const std::string parse_uint8(const std::string & input) const;
   const std::string parse_uint16(const std::string & input) const;
   const std::string parse_uint32(const std::string & input) const;
   const std::string parse_int32(const std::string & input) const;

   const std::string parse_bool(const std::string & input) const;

   const std::string parse_itempointerdata(const std::string & input) const;
   const std::string parse_version(const std::string & input) const;

   const std::string parse_string(const std::string & input, bool skip_first_byte) const;
   const std::string parse_varchar(const std::string & input) const;
   const std::string parse_name(const std::string & input) const;

   const unsigned int get_type_align(const unsigned int & typid) const;
   const unsigned int get_type_len(const unsigned int & typid, const std::string & next_char) const;
   const value_renderer::parser_func get_parser_func(const unsigned int & typid) const;
   const std::string render_row(const std::string & raw_input, const unsigned int & pos, const unsigned int & len, const parser_func parser, const char* name, const char* description) const;
};

#endif

#include "html_renderer.h"

#include <iostream>
#include <fstream>
#include <string.h>

html_renderer::html_renderer(const char * header_file, const char * footer_file) : renderer(), header_file(header_file), footer_file(footer_file) {
}

void html_renderer::output_file(const char * fileName) {
   std::ifstream fileStream(fileName, std::ios::in);
      std::cout << fileName << std::endl;

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

   output_file(footer_file); 
   return output.str();
}

void renderTable(char * bin, int nTuples, int freespaceLower, int freespaceUpper, int * lp_off, int * lp_len) {
   int binaryLen = strlen(bin);

   bool tuple_open = false;
   for(int i = 0; i < binaryLen; i++) {
      if(i == freespaceLower) {
         std::cout << "<span class=\"range freespace\">" << std::endl;   
      }

      if(i == freespaceUpper) {
         std::cout << "</span>" << std::endl;   
      }

      for(int j = 0; j < nTuples; j++) {
         if(!tuple_open && (i == lp_off[j])) {
            tuple_open = true;
            std::cout << "<span class=\"range tuple\">" << std::endl;
         }
      }
      std::cout << bin[i] << std::endl;

      for(int j = 0; j < nTuples; j++) {
         if(tuple_open && i == (lp_off[j] + lp_len[j])) {
            tuple_open = false;
            std::cout << "</span>" << std::endl;   
         }
      }
      for(int j = 0; j < nTuples; j++) {
         if(!tuple_open && (i == lp_off[j])) {
            tuple_open = true;
            std::cout << "<span class=\"range tuple\">" << std::endl;
         }
      }
   }
   if(tuple_open) {
      tuple_open = false;
      std::cout << "</span>" << std::endl;   
   }
}

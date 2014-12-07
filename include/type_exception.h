#ifndef TYPE_EXCEPTION_HEADER
#define TYPE_EXCEPTION_HEADER

#include <exception>
#include <cstdio>

class bad_type_exception : public std::exception {
public:
   bad_type_exception(const unsigned int & typid) : typid(typid) {}
   const char* what() const throw() {
      char * buffer = new char[200];
      sprintf(buffer, "Unknown type discovered: %d", typid);
      return buffer;
   }
private:
   const unsigned int typid;
};

#endif

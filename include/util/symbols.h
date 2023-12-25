#ifndef CNF_CONVERTOR_UTIL_SYMBOLS
#define CNF_CONVERTOR_UTIL_SYMBOLS

#include <iostream>
#include <string>

namespace util {
namespace symbols {

bool checkFirstCharacterOfSymbol(char target);
bool checkLongerSymbol(char target);
bool checkAtom(char target);
bool checkWhitespace(char target);
std::string getLongerSymbol(std::string::const_iterator ptr);
std::string getAtom(std::string::const_iterator ptr);

struct end_of_formula {
  std::string message;
  end_of_formula();
};

}  // namespace symbols
}  // namespace util

#endif  // CNF_CONVERTOR_UTIL_SYMBOLS

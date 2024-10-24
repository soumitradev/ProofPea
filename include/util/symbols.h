#ifndef CNF_CONVERTOR_UTIL_SYMBOLS
#define CNF_CONVERTOR_UTIL_SYMBOLS

#include <logger/logger.h>

#include <iostream>
#include <string>

namespace util {
namespace symbols {

enum SymbolType { IMPL, DISJUNCT, CONJUNCT, NEG, LBRACE, RBRACE, ATOM };

extern std::string SymbolTypeArray[];

bool checkFirstCharacterOfSymbol(char target);
bool checkLongerSymbol(std::string::const_iterator target,
                       std::string::const_iterator end);
bool checkAtom(char target);
bool checkWhitespace(char target);
std::string getLongerSymbol(std::string::const_iterator ptr);
std::string getAtom(std::string::const_iterator ptr,
                    std::string::const_iterator end);

struct end_of_formula {
  std::string message;
  end_of_formula();
};

}  // namespace symbols
}  // namespace util

#endif  // CNF_CONVERTOR_UTIL_SYMBOLS

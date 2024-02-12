#include <util/symbols.h>

namespace util {
namespace symbols {

std::string IMPL_STR = "->";
char DISJUNCT_STR = '+';
char CONJUNCT_STR = '*';
char NEG_STR = '~';
char LBRACE_STR = '(';
char RBRACE_STR = ')';

const char firstCharacterSymbols[] = {LBRACE_STR,   RBRACE_STR,   NEG_STR,
                                      CONJUNCT_STR, DISJUNCT_STR, IMPL_STR[0]};

std::string SymbolTypeArray[]{"IMPL",   "DISJUNCT", "CONJUNCT", "NEG",
                              "LBRACE", "RBRACE",   "ATOM"};

bool checkFirstCharacterOfSymbol(char target) {
  bool found = false;
  for (char c : firstCharacterSymbols) {
    if (c == target) {
      found = true;
      break;
    }
  }
  return found;
}

bool checkLongerSymbol(char target) { return target == IMPL_STR[0]; }

bool checkAtom(char target) {
  return ((target >= 65) && (target <= 90)) ||
         ((target >= 97) && (target <= 122));
}

bool checkWhitespace(char target) {
  return (target == ' ') || (target == '\t') || (target == '\n');
}

std::string getLongerSymbol(std::string::const_iterator ptr) {
  return std::string{*ptr, *(ptr + 1)};
}

std::string getAtom(std::string::const_iterator ptr,
                    std::string::const_iterator end) {
  std::string atom;
  atom.push_back(*ptr);
  ptr++;
  while ((ptr < end) && (*ptr >= (char)48) && (*ptr <= (char)57)) {
    atom.push_back(*ptr);
    ptr++;
  }
  return atom;
}

end_of_formula::end_of_formula() { message = "EndOfFormula"; }

}  // namespace symbols
}  // namespace util

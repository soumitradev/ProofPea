#include <util/symbols.h>

namespace util {
namespace symbols {

const std::string symbolList[] = {"(", ")", "^", "+", "~", "->"};
const char firstCharacterSymbols[] = {'(', ')', '^', '+', '~', '-'};

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

bool checkLongerSymbol(char target) { return target == '-'; }

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

std::string getAtom(std::string::const_iterator ptr) {
  std::string atom;
  atom.push_back(*ptr);
  std::string::const_iterator ptrEnd = ptr + 1;
  while ((*ptrEnd >= 48) && (*ptrEnd <= 57)) {
    atom.push_back(*ptrEnd);
    ptrEnd++;
  }
  return atom;
}

end_of_formula::end_of_formula() { message = "EndOfFormula"; }

}  // namespace symbols
}  // namespace util

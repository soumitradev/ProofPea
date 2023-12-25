#include <tokenizer.h>

std::variant<std::string, util::symbols::end_of_formula,
             error::tokenizer::invalid_symbol>
getNextToken(std::string::const_iterator start, std::string::const_iterator ptr,
             std::string::const_iterator end) {
  if (ptr == end) {
    return util::symbols::end_of_formula();
  }

  if (util::symbols::checkFirstCharacterOfSymbol(*ptr)) {
    if (util::symbols::checkLongerSymbol(*ptr)) {
      return util::symbols::getLongerSymbol(ptr);
    } else {
      return std::string(1, *ptr);
    }
  } else if (util::symbols::checkAtom(*ptr)) {
    return util::symbols::getAtom(ptr);
  } else {
    std::stringstream ss;
    ss << "Invalid symbol at character: " << std::distance(start, ptr)
       << ", found: " << *ptr << std::endl;
    return error::tokenizer::invalid_symbol(ss.str());
  }
}

std::variant<std::vector<std::string>, error::tokenizer::invalid_symbol,
             error::unknown::unknown_error>
tokenize(std::string const formula) {
  std::vector<std::string> tokens;

  auto ptr = formula.begin();
  while (ptr != formula.end()) {
    if (util::symbols::checkWhitespace(*ptr)) {
      while (util::symbols::checkWhitespace(*ptr) && ptr != formula.end()) {
        ptr++;
        logger::Logger::dispatchLog(logger::debugLog{
          log : "Detected whitespace at: " +
          std::to_string(std::distance(formula.begin(), ptr)) + ", skipping..."
        });
      }
    }
    const auto next = getNextToken(formula.begin(), ptr, formula.end());
    if (std::holds_alternative<std::string>(next)) {
      const auto token = std::get<std::string>(next);
      tokens.push_back(token);
      ptr += token.length();
      logger::Logger::dispatchLog(logger::debugLog{
        log : "Detected token: " + token +
        " at: " + std::to_string(std::distance(formula.begin(), ptr))
      });
    } else if (std::holds_alternative<util::symbols::end_of_formula>(next)) {
      logger::Logger::dispatchLog(logger::
                                  infoLog{log : "Reached end of formula"});
      break;
    } else if (std::holds_alternative<error::tokenizer::invalid_symbol>(next)) {
      return std::get<error::tokenizer::invalid_symbol>(next);
    } else {
      return error::unknown::unknown_error(
          "Unknown token alternative returned by getNextToken()");
    }
  }
  logger::Logger::dispatchLog(logger::
                              infoLog{log : "Finished tokenizing formula"});

  return tokens;
}
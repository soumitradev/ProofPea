#include <parser/tokenizer.h>

namespace proofpea {
namespace parser {
namespace tokenizer {

std::variant<Token, util::symbols::end_of_formula,
             error::tokenizer::invalid_symbol>
getNextToken(std::string::const_iterator start, std::string::const_iterator ptr,
             std::string::const_iterator end) {
  if (ptr == end) {
    return util::symbols::end_of_formula();
  }

  const size_t position = std::distance(start, ptr);

  if (util::symbols::checkFirstCharacterOfSymbol(*ptr)) {
    if (util::symbols::checkLongerSymbol(ptr, end)) {
      logger::Logger::dispatchLog(logger::debugLog{
        log : "Classified incoming token as long symbol at position " +
        std::to_string(std::distance(start, ptr))
      });
      return Token{util::symbols::IMPL, util::symbols::getLongerSymbol(ptr),
                   position};
    } else {
      logger::Logger::dispatchLog(logger::debugLog{
        log : "Classified incoming token as short symbol at position " +
        std::to_string(std::distance(start, ptr))
      });
      util::symbols::SymbolType type;
      switch (*ptr) {
        case '~':
          type = util::symbols::NEG;
          break;
        case '*':
          type = util::symbols::CONJUNCT;
          break;
        case '+':
          type = util::symbols::DISJUNCT;
          break;
        case '(':
          type = util::symbols::LBRACE;
          break;
        case ')':
          type = util::symbols::RBRACE;
          break;
        case '0':
          type = util::symbols::ABSOLUTEFALSE;
          break;
        case '1':
          type = util::symbols::ABSOLUTETRUE;
          break;

        default:
          std::stringstream ss;
          ss << "Unexpected symbol at character: " << position
             << ", found: " << *ptr;
          return error::tokenizer::invalid_symbol(ss.str());
          break;
      }
      return Token{type, std::string(1, *ptr), position};
    }
  } else if (util::symbols::checkAtom(*ptr)) {
    logger::Logger::dispatchLog(logger::debugLog{
      log : "Classified incoming token as atom at position " +
      std::to_string(std::distance(start, ptr))
    });
    // TODO: Allow Tokenizer to de-duplicate tokens
    return Token{util::symbols::ATOM, util::symbols::getAtom(ptr, end),
                 position};
  } else {
    std::stringstream ss;
    ss << "Invalid symbol at character: " << position << ", found: " << *ptr;
    return error::tokenizer::invalid_symbol(ss.str());
  }
}

std::variant<bool, error::tokenizer::invalid_symbol,
             error::unknown::unknown_error>
tokenize(std::string const formula, std::vector<tokenizer::Token>& tokens) {
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
    if (std::holds_alternative<Token>(next)) {
      const auto token = std::get<Token>(next);

      tokens.push_back(token);
      ptr += token.lexeme.length();

      logger::Logger::dispatchLog(logger::debugLog{
        log : "Detected token: " + token.lexeme +
        " at: " + std::to_string(token.position)
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
  return true;
}

}  // namespace tokenizer
}  // namespace parser
}  // namespace proofpea

#ifndef CNF_CONVERTOR_TOKENIZER
#define CNF_CONVERTOR_TOKENIZER

#include <error/tokenizer.h>
#include <error/unknown.h>
#include <logger/logger.h>
#include <util/symbols.h>

#include <iostream>
#include <variant>
#include <vector>

namespace tokenizer {

struct Token {
  util::symbols::SymbolType type;
  std::string lexeme;
  size_t position;
};

std::variant<std::vector<Token>, error::tokenizer::invalid_symbol,
             error::unknown::unknown_error>
tokenize(std::string const formula);

}  // namespace tokenizer

#endif  // CNF_CONVERTOR_TOKENIZER

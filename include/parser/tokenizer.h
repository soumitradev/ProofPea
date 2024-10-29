#ifndef CNF_CONVERTOR_TOKENIZER
#define CNF_CONVERTOR_TOKENIZER

#include <error/parser.h>
#include <error/unknown.h>
#include <logger/logger.h>
#include <util/symbols.h>

#include <iostream>
#include <variant>
#include <vector>

namespace parser {
namespace tokenizer {

struct Token {
  util::symbols::SymbolType type;
  std::string lexeme;
  size_t position;
};

std::variant<bool, error::tokenizer::invalid_symbol,
             error::unknown::unknown_error>
tokenize(std::string const formula, std::vector<tokenizer::Token*>& tokens);
}  // namespace tokenizer
}  // namespace parser

#endif  // CNF_CONVERTOR_TOKENIZER

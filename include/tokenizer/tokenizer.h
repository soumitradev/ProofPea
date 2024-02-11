#ifndef CNF_CONVERTOR_TOKENIZER
#define CNF_CONVERTOR_TOKENIZER

#include <error/tokenizer.h>
#include <error/unknown.h>
#include <logger/logger.h>
#include <util/symbols.h>

#include <iostream>
#include <variant>
#include <vector>

std::variant<std::vector<std::string>, error::tokenizer::invalid_symbol,
             error::unknown::unknown_error>
tokenize(std::string const formula);
#endif  // CNF_CONVERTOR_TOKENIZER
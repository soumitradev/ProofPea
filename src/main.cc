#include <main.h>

int main() {
  const logger::Logger* logger =
      logger::Logger::initLogger(logger::Level::INFO);

  logger::Logger::dispatchLog(logger::infoLog{log : "Logger Initialized!"});

  std::string formula;
  std::cout << "Enter your well formed formula" << std::endl;

  std::getline(std::cin, formula);

  logger::Logger::dispatchLog(
      logger::infoLog{log : "User entered formula: " + formula});

  std::vector<tokenizer::Token*> tokens;
  auto tokenizerResult = tokenizer::tokenize(formula, tokens);
  if (std::holds_alternative<error::tokenizer::invalid_symbol>(
          tokenizerResult)) {
    const auto error =
        std::get<error::tokenizer::invalid_symbol>(tokenizerResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (std::holds_alternative<error::unknown::unknown_error>(
                 tokenizerResult)) {
    const auto error = std::get<error::unknown::unknown_error>(tokenizerResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 125;
  } else if (!std::holds_alternative<std::vector<tokenizer::Token*>>(
                 tokenizerResult)) {
    logger::Logger::dispatchLog(logger::errorLog{
      error : error::unknown::unknown_error(
          "Unknown tokens alternative returned by tokenize()")
    });
    return 125;
  }

  for (size_t i = 0; i < tokens.size(); i++) {
    std::cout << tokens[i]->lexeme << "\t";
  }

  std::cout << std::endl;
  logger::Logger::dispatchLog(logger::
                              infoLog{log : "Finished outputting tokens"});
  const auto syntaxTreeResult = parser::parseAST(tokens);
  if (std::holds_alternative<error::parser::unexpected_token>(
          syntaxTreeResult)) {
    const auto error =
        std::get<error::parser::unexpected_token>(syntaxTreeResult);
  }
  const auto syntaxTree = std::get<const parser::Node*>(syntaxTreeResult);

  debug::ast::printAST(syntaxTree);

  parser::deallocAST(syntaxTree);
  tokenizer::deallocTokens(tokens);
  return 0;
}

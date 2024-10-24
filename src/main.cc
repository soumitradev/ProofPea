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

  std::ostringstream lexemes;
  for (size_t i = 0; i < tokens.size(); i++) {
    lexemes << tokens[i]->lexeme << "\t";
  }
  logger::Logger::dispatchLog(
      logger::infoLog{log : "Tokens detected: " + lexemes.str()});

  const auto syntaxTreeResult = parser::parseAST(tokens);
  if (std::holds_alternative<error::parser::unexpected_token>(
          syntaxTreeResult)) {
    const auto error =
        std::get<error::parser::unexpected_token>(syntaxTreeResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }
  const auto syntaxTree = std::get<parser::AST*>(syntaxTreeResult);

  debug::ast::printAST(syntaxTree);

  const auto truthTableResult =
      truth_table::tabulator::printTruthTable(syntaxTree);
  if (std::holds_alternative<error::eval::unexpected_node>(truthTableResult)) {
    const auto error = std::get<error::eval::unexpected_node>(truthTableResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(truthTableResult)) {
    const auto error =
        std::get<error::eval::mismatched_atoms>(truthTableResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }

  parser::deallocAST(syntaxTree);
  tokenizer::deallocTokens(tokens);
  return 0;
}

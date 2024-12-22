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

  std::vector<parser::tokenizer::Token*> tokens;
  auto tokenizerResult = parser::tokenizer::tokenize(formula, tokens);
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
  } else if (!std::holds_alternative<bool>(tokenizerResult)) {
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

  const auto syntaxTreeResult = parser::parser::parseAST(tokens);
  tokens.clear();
  if (std::holds_alternative<error::parser::unexpected_token>(
          syntaxTreeResult)) {
    const auto error =
        std::get<error::parser::unexpected_token>(syntaxTreeResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }
  const auto syntaxTree = std::get<parser::parser::AST*>(syntaxTreeResult);
  const auto copySyntaxTree = parser::parser::AST::copy(syntaxTree);
  const auto copyCopySyntaxTree = parser::parser::AST::copy(copySyntaxTree);
  parser::parser::deallocAST(syntaxTree);

  debug::ast::printAST(copySyntaxTree, false);
  debug::ast::printAST(copyCopySyntaxTree, false);

  const auto truthTableResult =
      truth_table::tabulator::printTruthTable(copySyntaxTree);
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

  const auto copyTruthTableResult =
      truth_table::tabulator::printTruthTable(copyCopySyntaxTree);
  if (std::holds_alternative<error::eval::unexpected_node>(
          copyTruthTableResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(copyTruthTableResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(
          copyTruthTableResult)) {
    const auto error =
        std::get<error::eval::mismatched_atoms>(copyTruthTableResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }

  parser::parser::deallocAST(copySyntaxTree);

  debug::ast::printAST(copyCopySyntaxTree, false);
  const auto cnfTransformResult =
      transformer::cnf::transformToCNF(copyCopySyntaxTree);

  if (std::holds_alternative<error::eval::unexpected_node>(
          nnfTransformResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(nnfTransformResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  } else if (std::holds_alternative<bool>(nnfTransformResult)) {
    if (!std::get<bool>(nnfTransformResult)) {
      logger::Logger::dispatchLog(logger::errorLog{
        error : error::unknown::unknown_error{
            "Encountered unexpected error in transformToIMPLFREE"}
      });
    }
  }

  debug::ast::printAST(copyCopySyntaxTree, false);

  const auto copyTruthTableNNFResult =
      truth_table::tabulator::printTruthTable(copyCopySyntaxTree);
  if (std::holds_alternative<error::eval::unexpected_node>(
          copyTruthTableNNFResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(copyTruthTableNNFResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(
          copyTruthTableNNFResult)) {
    const auto error =
        std::get<error::eval::mismatched_atoms>(copyTruthTableNNFResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }

  parser::parser::deallocAST(copyCopySyntaxTree);
  logger::Logger::freeLogger();
  return 0;
}

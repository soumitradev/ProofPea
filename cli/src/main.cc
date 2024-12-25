#include <iostream>
#include <variant>
#include <vector>

#include "proofpea.h"

using namespace proofpea;

int main() {
  logger::Logger::initLogger(logger::Level::INFO);

  logger::Logger::dispatchLog(logger::infoLog{log : "Logger Initialized!"});

  std::string formula;
  std::cout << "Enter your well formed formula" << std::endl;

  std::getline(std::cin, formula);

  logger::Logger::dispatchLog(
      logger::infoLog{log : "User entered formula: " + formula});

  std::vector<parser::tokenizer::Token> tokens;
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
    lexemes << tokens[i].lexeme << "\t";
  }
  logger::Logger::dispatchLog(
      logger::infoLog{log : "Tokens detected: " + lexemes.str()});

  const auto syntaxTree = std::make_shared<parser::parser::AST>();
  const auto syntaxTreeResult = syntaxTree->parseAST(tokens);
  tokens.clear();
  if (std::holds_alternative<error::parser::unexpected_token>(
          syntaxTreeResult)) {
    const auto error =
        std::get<error::parser::unexpected_token>(syntaxTreeResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }

  const auto copySyntaxTree = parser::parser::AST::copy(syntaxTree.get());
  const auto copyCopySyntaxTree =
      parser::parser::AST::copy(copySyntaxTree.get());

  debug::ast::printAST(copySyntaxTree.get(), false, (char*)"ast.svg");
  debug::ast::printAST(copyCopySyntaxTree.get(), false, (char*)"ast.svg");

  const auto truthTableResult =
      truth_table::tabulator::printTruthTable(copySyntaxTree.get());
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
      truth_table::tabulator::printTruthTable(copyCopySyntaxTree.get());
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

  debug::ast::printAST(copyCopySyntaxTree.get(), false, (char*)"ast.svg");
  const auto cnfTransformResult =
      checker::validity::checkValidity(copyCopySyntaxTree.get());

  if (std::holds_alternative<error::eval::unexpected_node>(
          cnfTransformResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(cnfTransformResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  } else if (std::holds_alternative<bool>(cnfTransformResult)) {
    const auto valid = std::get<bool>(cnfTransformResult);

    std::ostringstream log;
    log << "The given formula is " << (valid ? "" : "NOT ") << "valid";
    logger::Logger::dispatchLog(logger::infoLog{log.str()});
  } else if (std::holds_alternative<error::eval::unexpected_node>(
                 cnfTransformResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(cnfTransformResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
  }

  debug::ast::printAST(copyCopySyntaxTree.get(), false, (char*)"ast.svg");

  const auto copyTruthTableCNFResult =
      truth_table::tabulator::printTruthTable(copyCopySyntaxTree.get());
  if (std::holds_alternative<error::eval::unexpected_node>(
          copyTruthTableCNFResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(copyTruthTableCNFResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(
          copyTruthTableCNFResult)) {
    const auto error =
        std::get<error::eval::mismatched_atoms>(copyTruthTableCNFResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }

  // const auto hornClauseCheck =
  //     checker::horn_satisfiability::checkHornSatisfiability(copyCopySyntaxTree);
  // if (std::holds_alternative<error::horn::invalid_horn_formula>(
  //         hornClauseCheck)) {
  //   const auto error =
  //       std::get<error::horn::invalid_horn_formula>(hornClauseCheck);
  //   logger::Logger::dispatchLog(logger::errorLog{error : error});
  //   return 1;
  // } else if (std::holds_alternative<bool>(hornClauseCheck)) {
  //   const auto satisfiable = std::get<bool>(hornClauseCheck);
  //   std::ostringstream log;
  //   log << "The given formula is " << (satisfiable ? "" : "NOT ")
  //       << "satisfiable";
  //   logger::Logger::dispatchLog(logger::infoLog{log.str()});
  // }

  std::string formula2;
  std::cout << "Enter your other well formed formula" << std::endl;

  std::getline(std::cin, formula2);

  logger::Logger::dispatchLog(
      logger::infoLog{log : "User entered formula: " + formula2});

  std::vector<parser::tokenizer::Token> tokens2;
  auto tokenizerResult2 = parser::tokenizer::tokenize(formula2, tokens2);
  if (std::holds_alternative<error::tokenizer::invalid_symbol>(
          tokenizerResult2)) {
    const auto error =
        std::get<error::tokenizer::invalid_symbol>(tokenizerResult2);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (std::holds_alternative<error::unknown::unknown_error>(
                 tokenizerResult2)) {
    const auto error =
        std::get<error::unknown::unknown_error>(tokenizerResult2);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 125;
  } else if (!std::holds_alternative<bool>(tokenizerResult2)) {
    logger::Logger::dispatchLog(logger::errorLog{
      error : error::unknown::unknown_error(
          "Unknown tokens alternative returned by tokenize()")
    });
    return 125;
  }

  const auto otherTree = std::make_shared<parser::parser::AST>();
  const auto otherTreeResult = otherTree->parseAST(tokens2);
  tokens2.clear();
  if (std::holds_alternative<error::parser::unexpected_token>(
          otherTreeResult)) {
    const auto error =
        std::get<error::parser::unexpected_token>(otherTreeResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 1;
  }

  const auto equivalenceResult = checker::equivalence::checkEquivalence(
      copyCopySyntaxTree.get(), otherTree.get());
  if (std::holds_alternative<error::eval::unexpected_node>(equivalenceResult)) {
    const auto error =
        std::get<error::eval::unexpected_node>(equivalenceResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (std::holds_alternative<error::eval::mismatched_atoms>(
                 equivalenceResult)) {
    const auto error =
        std::get<error::eval::mismatched_atoms>(equivalenceResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (std::holds_alternative<
                 error::equivalence::equivalence_chack_unsupported>(
                 equivalenceResult)) {
    const auto error =
        std::get<error::equivalence::equivalence_chack_unsupported>(
            equivalenceResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (std::holds_alternative<error::equivalence::incomplete_truth_table>(
                 equivalenceResult)) {
    const auto error =
        std::get<error::equivalence::incomplete_truth_table>(equivalenceResult);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (!std::holds_alternative<bool>(equivalenceResult)) {
    logger::Logger::dispatchLog(logger::errorLog{
      error : error::unknown::unknown_error(
          "Unknown tokens alternative returned by tokenize()")
    });
    return 125;
  }
  const auto equivalent = std::get<bool>(equivalenceResult);

  std::ostringstream equivalenceLog;
  equivalenceLog << "The formulae are "
                 << (equivalent ? "equivalent" : "NOT equivalent");
  logger::Logger::dispatchLog(logger::infoLog{log : equivalenceLog.str()});
  return 0;
}

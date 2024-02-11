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

  const std::variant<std::vector<tokenizer::Token>,
                     error::tokenizer::invalid_symbol,
                     error::unknown::unknown_error>
      tokens = tokenizer::tokenize(formula);

  if (std::holds_alternative<std::vector<tokenizer::Token>>(tokens)) {
    const auto tokenVector = std::get<std::vector<tokenizer::Token>>(tokens);
    for (size_t i = 0; i < tokenVector.size(); i++) {
      std::cout << tokenVector[i].lexeme << "\t";
    }
    std::cout << std::endl;
    logger::Logger::dispatchLog(logger::
                                infoLog{log : "Finished outputting tokens"});
  } else if (std::holds_alternative<error::tokenizer::invalid_symbol>(tokens)) {
    const auto error = std::get<error::tokenizer::invalid_symbol>(tokens);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 22;
  } else if (std::holds_alternative<error::unknown::unknown_error>(tokens)) {
    const auto error = std::get<error::unknown::unknown_error>(tokens);
    logger::Logger::dispatchLog(logger::errorLog{error : error});
    return 125;
  } else {
    logger::Logger::dispatchLog(logger::errorLog{
      error : error::unknown::unknown_error(
          "Unknown tokens alternative returned by tokenize()")
    });
    return 125;
  }
  return 0;
}
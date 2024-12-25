#include <error/horn.h>

namespace proofpea {
namespace error {
namespace horn {

invalid_horn_formula::invalid_horn_formula(std::string c) {
  error = "InvalidHornFormula";
  cause = c;
}

}  // namespace horn
}  // namespace error
}  // namespace proofpea

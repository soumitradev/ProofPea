#include <error/eval.h>

namespace proofpea {
namespace error {
namespace eval {

unexpected_node::unexpected_node(std::string c) {
  error = "UnexpectedNode";
  cause = c;
}

mismatched_atoms::mismatched_atoms(std::string c) {
  error = "MismatchedAtoms";
  cause = c;
}

}  // namespace eval
}  // namespace error
}  // namespace proofpea

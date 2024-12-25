#include <error/equivalence.h>

namespace proofpea {
namespace error {
namespace equivalence {

equivalence_chack_unsupported::equivalence_chack_unsupported(std::string c) {
  error = "EquivalenceCheckUnsupported";
  cause = c;
}

incomplete_truth_table::incomplete_truth_table(std::string c) {
  error = "IncompleteTruthTable";
  cause = c;
}

}  // namespace equivalence
}  // namespace error
}  // namespace proofpea

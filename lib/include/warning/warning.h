#ifndef CNF_CONVERTOR_WARNING_WARNING
#define CNF_CONVERTOR_WARNING_WARNING

#include <string>

namespace proofpea {
namespace warning {

struct warning {
  std::string warning;
  std::string cause;
};

}  // namespace warning
}  // namespace proofpea

#endif  // CNF_CONVERTOR_WARNING_WARNING

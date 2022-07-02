#include "builtin.h"

#include <chrono>

namespace draft {

std::size_t ClockFunction::arity()
{
    return 0;
}

object::Object ClockFunction::call(Interpreter *, std::vector<object::Object>)
{
    namespace cr = std::chrono;
    auto now = cr::system_clock::now();
    auto epoch = now.time_since_epoch();
    auto seconds = cr::duration_cast<cr::seconds>(epoch).count();
    return static_cast<object::Number>(seconds);
}

}  // namespace draft

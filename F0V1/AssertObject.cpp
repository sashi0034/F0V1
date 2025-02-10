#include "AssertObject.h"

#include <stdexcept>

namespace ZG
{
    void Assert_HRESULT::operator<<(HRESULT result) const
    {
        if (FAILED(result))
        {
            throw std::runtime_error("HRESULT failed");
        }
    }
}

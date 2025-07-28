#pragma once
#include <type_traits>

namespace SSGE
{
template <class TDerived, class TBase>
concept Derived = std::is_base_of_v<TBase, TDerived>;
}
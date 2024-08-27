#pragma once

namespace sota {

// C++ 20 not available for Android build => can't include <numbers>
// M_PI may not compile for Windows.
// So, as a workaround, declare PI here
constexpr double PI = 3.14159265;

}  // namespace sota

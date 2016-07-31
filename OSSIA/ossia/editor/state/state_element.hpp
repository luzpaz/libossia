#pragma once
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/custom_state.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/state/state_element_fwd.hpp>
#include <ossia_export.h>

namespace ossia
{
OSSIA_EXPORT void launch(const state_element& e);
OSSIA_EXPORT std::ostream& print(std::ostream& os, const state_element& dt);
}

#include <ossia/editor/state/state_element.hpp>

#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <iostream>

namespace ossia
{
std::size_t state::size() const
{
    return children.size();
}

void state::launch() const
{
    state_execution_visitor v;
    for(const auto& state : children)
    {
        if(state)
            eggs::variants::apply(v, state);
    }
}

void state::add(const state_element& e)
{
    if(e)
        children.push_back(e);
}

void state::add(state_element&& e)
{
    if(e)
        children.push_back(std::move(e));
}

void state::remove(const state_element& e)
{
    children.erase(
                std::remove(children.begin(), children.end(), e),
                children.end());
}

void state::reserve(std::size_t s)
{
    children.reserve(s);
}

void state::clear()
{
    children.clear();
}

bool operator==(const state& lhs, const state& rhs)
{
    return lhs.children == rhs.children;
}

bool operator!=(const state& lhs, const state& rhs)
{
    return lhs.children != rhs.children;
}


void flattenAndFilter(state& state, const state_element& element)
{
    if(element)
        eggs::variants::apply(state_flatten_visitor{state}, element);
}

void flattenAndFilter(state& state, state_element&& element)
{
    if(element)
        eggs::variants::apply(state_flatten_visitor{state}, std::move(element));
}
}

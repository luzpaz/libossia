// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <algorithm>
#include <iostream>
namespace ossia
{
ossia::state_element time_interval::tick(time_value date, double ratio)
{
  m_date += std::ceil(date.impl * m_speed / ratio);
  m_position = double(m_date) / double(m_nominal);
  node->requested_tokens.push_back({m_date, m_position, 0_tv});

  auto st = state();
  if (m_callback)
    (m_callback)(m_position, m_date, st);
  return st;
}

ossia::state_element time_interval::tick(time_value date)
{
  return tick(date, 1.0);
}

ossia::state_element time_interval::tick_offset(time_value date, ossia::time_value offset)
{
  m_date += std::ceil(date.impl * m_speed);
  m_position = double(m_date) / double(m_nominal);
  node->requested_tokens.push_back({m_date, m_position, offset});
  m_tick_offset = offset;
  auto st = state();
  if (m_callback)
    (m_callback)(m_position, m_date, st);
  return st;
}

ossia::state_element time_interval::tick_offset(time_value date, double ratio, ossia::time_value offset)
{
  m_date += std::ceil(date.impl * m_speed / ratio);
  m_position = double(m_date) / double(m_nominal);
  node->requested_tokens.push_back({m_date, m_position, offset});

  m_tick_offset = offset;
  auto st = state();
  if (m_callback)
    (m_callback)(m_position, m_date, st);
  return st;
}

std::shared_ptr<time_interval> time_interval::create(
    time_interval::exec_callback callback, time_event& startEvent,
    time_event& endEvent, ossia::time_value nominal, ossia::time_value min, ossia::time_value max)
{
  auto timeInterval = std::make_shared<time_interval>(
      callback, startEvent, endEvent, nominal, min, max);

  startEvent.next_time_intervals().push_back(timeInterval);
  endEvent.previous_time_intervals().push_back(timeInterval);

  return timeInterval;
}

time_interval::time_interval(
    time_interval::exec_callback callback, time_event& startEvent,
    time_event& endEvent, ossia::time_value nominal, ossia::time_value min, ossia::time_value max)
    : m_callback(std::move(callback))
    , m_start(startEvent)
    , m_end(endEvent)
    , m_nominal(nominal)
    , m_min(min)
    , m_max(max)
{
  node = std::make_shared<ossia::interval_node>();
}

time_interval::~time_interval()
{
}

void time_interval::start()
{
  ossia::state st;
  start(st);
  ossia::launch(st);
}

void time_interval::start(ossia::state& pstate)
{
  if (!m_running)
  {
    m_running = true;
    // start all time processes
    for (const auto& timeProcess : get_time_processes())
    {
      if (timeProcess->enabled())
        timeProcess->start(pstate);
    }

    // launch the clock
    if (m_nominal <= m_offset)
      return stop();

    // set clock at a tick
    m_date = m_offset;
    m_position = double(m_date) / double(m_nominal);
/*
    auto st = state();
    if (m_callback)
      (m_callback)(m_position, m_date, st);
      */
  }
}

void time_interval::stop()
{
  if (m_running)
  {
    // stop all time processes
    for (const std::shared_ptr<ossia::time_process>& timeProcess : get_time_processes())
    {
      timeProcess->stop();
    }

    m_date = 0;
    m_position = 0;
    m_running = false;
  }
}

ossia::state_element time_interval::offset(ossia::time_value date)
{
  m_offset = date;
  m_date = date;

  if (m_nominal != Zero)
    m_position = m_date / m_nominal;
  else
    m_position = Zero;

  const auto& processes = get_time_processes();
  const auto N = processes.size();
  if (N > 0)
  {
    ossia::state state;
    state.reserve(N);
    const auto pos = date / get_nominal_duration();

    // get the state of each TimeProcess at current clock position and date
    for (const auto& timeProcess : processes)
    {
      if (timeProcess->enabled())
      {
        auto res = timeProcess->offset(date, pos);
        if (res)
          state.add(std::move(res));
      }
    }
    return state;
  }
  return {};
}

ossia::state_element time_interval::state()
{
  const auto& processes = get_time_processes();
  const auto N = processes.size();
  if (N > 0)
  {
    ossia::state state;
    state.reserve(N);

    const auto date = get_date();
    const auto pos = date / get_nominal_duration();

    // get the state of each TimeProcess at current clock position and date
    for (const std::shared_ptr<ossia::time_process>& timeProcess : processes)
    {
      auto& p = *timeProcess;
      if (p.enabled())
      {
        if (auto res = p.state(date, pos, m_tick_offset))
          state.add(std::move(res));
      }
    }

    return state;
  }
  return {};
}

void time_interval::pause()
{
  // pause all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->pause();
  }
}

void time_interval::resume()
{
  // resume all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->resume();
  }
}

void time_interval::set_callback(time_interval::exec_callback callback)
{
  m_callback = callback;
}

void time_interval::set_stateless_callback(time_interval::exec_callback cb)
{
  m_callback = cb;
}

const time_value& time_interval::get_nominal_duration() const
{
  return m_nominal;
}

time_interval&
time_interval::set_nominal_duration(ossia::time_value durationNominal)
{
  m_nominal = durationNominal;

  if (m_nominal < m_min)
    set_min_duration(m_nominal);

  if (m_nominal > m_max)
    set_max_duration(m_nominal);

  return *this;
}

const time_value& time_interval::get_min_duration() const
{
  return m_min;
}

time_interval&
time_interval::set_min_duration(ossia::time_value durationMin)
{
  m_min = durationMin;

  if (m_min > m_nominal)
    set_nominal_duration(m_min);

  return *this;
}

const time_value& time_interval::get_max_duration() const
{
  return m_max;
}

time_interval&
time_interval::set_max_duration(ossia::time_value durationMax)
{
  m_max = durationMax;

  if (durationMax < m_nominal)
    set_nominal_duration(m_max);

  return *this;
}

time_event& time_interval::get_start_event() const
{
  return m_start;
}

time_event& time_interval::get_end_event() const
{
  return m_end;
}

void time_interval::add_time_process(
    std::shared_ptr<time_process> timeProcess)
{
  if (!timeProcess)
    return;

  // store a TimeProcess if it is not already stored
  if (find(m_processes, timeProcess) == m_processes.end())
  {
    m_processes.push_back(std::move(timeProcess));
  }
}

void time_interval::remove_time_process(time_process* timeProcess)
{
  auto it = find_if(m_processes, [=](const auto& other) {
    return other.get() == timeProcess;
  });
  if (it != m_processes.end())
  {
    m_processes.erase(it);
  }
}

interval_node::interval_node()
{
  m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
  m_inlets.push_back(ossia::make_inlet<ossia::value_port>());
  m_inlets.push_back(ossia::make_inlet<ossia::midi_port>());

  m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
  m_outlets.push_back(ossia::make_outlet<ossia::value_port>());
  m_outlets.push_back(ossia::make_outlet<ossia::midi_port>());
}


void interval_node::run(token_request t, execution_state&)
{
  {
    auto i = m_inlets[0]->data.target<ossia::audio_port>();
    auto o = m_outlets[0]->data.target<ossia::audio_port>();
    o->samples = std::move(i->samples);
  }

  {
    auto i = m_inlets[1]->data.target<ossia::value_port>();
    auto o = m_outlets[1]->data.target<ossia::value_port>();
    o->data = std::move(i->data);
  }

  {
    auto i = m_inlets[2]->data.target<ossia::midi_port>();
    auto o = m_outlets[2]->data.target<ossia::midi_port>();
    o->messages = std::move(i->messages);
  }
}

}
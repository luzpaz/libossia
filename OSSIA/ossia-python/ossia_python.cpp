#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/minuit/minuit.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;
class python_local_device
{
  ossia::net::generic_device m_dev;
  ossia::net::local_protocol& m_local;

public:
  python_local_device(std::string name):
    m_dev{std::make_unique<ossia::net::local_protocol>(), std::move(name)},
    m_local{static_cast<ossia::net::local_protocol&>(m_dev.getProtocol())}
  {

  }

  bool exposeToMinuit(std::string ip, int remoteport, int localport)
  {
    try {
      m_local.exposeTo(
            std::make_unique<ossia::net::minuit_protocol>(
              m_dev.getName(), std::move(ip), localport, remoteport));
      return true;
    } catch(...) { }
    return false;
  }

  bool exposeToOsc(std::string ip, int remoteport, int localport)
  {
    try {
      m_local.exposeTo(std::make_unique<ossia::net::osc_protocol>(
                         std::move(ip), localport, remoteport));
      return true;
    } catch(...) { }
    return false;
  }

  ossia::net::node_base* addNode(const std::string& address)
  {
    return &ossia::net::find_or_create_node(m_dev.getRootNode(), address);
  }

  ossia::net::node_base* findNode(const std::string& address)
  {
    return ossia::net::find_node(m_dev.getRootNode(), address);
  }
};

PYBIND11_PLUGIN(ossia_python) {
    py::module m("ossia_python", "ossia");

    py::class_<python_local_device>(m, "LocalDevice")
        .def(py::init<std::string>())
        .def("exposeToMinuit", &python_local_device::exposeToMinuit)
        .def("exposeToOsc", &python_local_device::exposeToOsc)
        .def("addNode", &python_local_device::addNode, py::return_value_policy::reference)
        .def("findNode", &python_local_device::findNode, py::return_value_policy::reference)
        ;
    py::class_<ossia::net::node_base>(m, "Node")
        .def("getAddress", &ossia::net::node_base::getAddress, py::return_value_policy::reference)
        .def("createAddress", [] (ossia::net::node_base& node, int type) { return node.createAddress((ossia::val_type) type); }, py::return_value_policy::reference)
        ;

    py::class_<ossia::value>(m, "Value")
        .def(py::init<int>())
        .def(py::init<float>())
        .def(py::init<char>())
        .def(py::init<bool>())
        .def(py::init<const std::string&>())
        .def(py::init<const std::vector<ossia::value>&>())
        .def(py::init<std::array<float, 2>>())
        .def(py::init<std::array<float, 3>>())
        .def(py::init<std::array<float, 4>>())
        .def("valid", &ossia::value::valid)
        .def("reset", &ossia::value::reset)
        .def("get_int",    [] (const ossia::value& val) { return val.get<int>(); })
        .def("get_float",  [] (const ossia::value& val) { return val.get<float>(); })
        .def("get_char",   [] (const ossia::value& val) { return val.get<char>(); })
        .def("get_string", [] (const ossia::value& val) { return val.get<std::string>(); })
        .def("get_vec2f",  [] (const ossia::value& val) { return val.get<std::array<float, 2>>(); })
        .def("get_vec3f",  [] (const ossia::value& val) { return val.get<std::array<float, 3>>(); })
        .def("get_vec4f",  [] (const ossia::value& val) { return val.get<std::array<float, 4>>(); })
        ;

    py::class_<ossia::net::address_base>(m, "Address")
        .def("pullValue", &ossia::net::address_base::pullValue)
        .def("cloneValue",  [] (ossia::net::address_base& addr) -> ossia::value { return addr.cloneValue(); })
        .def("pushValue", [] (ossia::net::address_base& addr, const ossia::value& v) { addr.pushValue(v); })
        ;

    return m.ptr();
}

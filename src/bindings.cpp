#ifdef BUILD_PYTHON

#include "environment.hpp"
#include "action_space.hpp"

#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(tetris_environment, handle)
{
    handle.doc() = "Python bindings for the Tetris environment.";
    py::class_<Environment>(handle, "Environment")
				.def(py::init<>())
        .def("executeAction", &Environment::executeAction)
        // .def("getObservationSpace", &Environment::getObservationSpace)
        .def("isActive", &Environment::isActive)
				.def("render", &Environment::render);
    py::enum_<ActionSpace::Action>(handle, "ActionSpace")
        .value("left", ActionSpace::Action::Left)
        .value("right", ActionSpace::Action::Right)
        .value("rotate", ActionSpace::Action::Rotate)
        .value("none", ActionSpace::Action::None)
        .def("sample", &ActionSpace::random);
}

#endif
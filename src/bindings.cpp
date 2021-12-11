#ifdef BUILD_PYTHON

#include "environment.hpp"
#include "action_space.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

PYBIND11_MODULE(tetris_environment, handle)
{
    handle.doc() = "Python bindings for the Tetris environment.";
    py::class_<Environment>(handle, "Environment")
				.def(py::init<>())
        .def("executeAction", &Environment::executeAction)
        .def("getObservationSpace", [](Environment &self) {
          py::array out = py::cast(self.getObservationSpace());
          return out;
        })
        .def("getPieceConfigurations", [](Environment &self) {
          py::list out;
          for (auto const &config : self.getPieceConfigurations()) 
          {
            py::array img = py::cast(std::get<0>(config));
            py::list actions = py::cast(std::get<1>(config));
            py::tuple in = py::make_tuple(img, actions);
            out.append(in);
          }
          return out;
        })
        .def("getScore", &Environment::getScore)
        .def("isActive", &Environment::isActive)
		    .def("render", &Environment::render)
        .def("reset", &Environment::reset);

    py::enum_<ActionSpace::Action>(handle, "ActionSpace")
        .value("none", ActionSpace::Action::None) // Action 0
        .value("left", ActionSpace::Action::Left) // Action 1
        .value("right", ActionSpace::Action::Right) // Action 2
        .value("rotate", ActionSpace::Action::Rotate) // Action 3
        .def("sample", &ActionSpace::random)
        .def("getActionSpace", []() {
          std::vector<std::vector<int>> actions;
          for (int i = 0; i < 4; i++) {
            actions.push_back(std::vector<int>{i});
          }
          py::array out = py::cast(actions);
          return out;
        });

}

#endif

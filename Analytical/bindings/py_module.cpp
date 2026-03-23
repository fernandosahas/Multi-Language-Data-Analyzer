#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pipeline.h"

namespace py = pybind11;

PYBIND11_MODULE(analytics, m) {
    py::class_<Detection>(m, "Detection")
        .def_readonly("x", &Detection::box.x)
        .def_readonly("y", &Detection::box.y)
        .def_readonly("w", &Detection::box.width)
        .def_readonly("h", &Detection::box.height)
        .def_readonly("score", &Detection::score)
        .def_readonly("label", &Detection::label)
        .def_readonly("class_id", &Detection::class_id);

    py::class_<Pipeline>(m, "Pipeline")
        .def(py::init<const std::string&, const std::string&>())
        .def("start", [](Pipeline& self, py::object callback = py::none()) {
            if (!callback.is_none()) {
                // Capture Python callable, execute with GIL on detection events.
                auto cb = [callback](const std::vector<Detection>& dets) {
                    py::gil_scoped_acquire gil;
                    try {
                        callback(py::cast(dets));
                    } catch (const py::error_already_set& e) {
                        py::print("[analytics] callback error:", e.what());
                    }
                };
                return self.start(cb);
            }
            return self.start();
        }, py::arg("callback") = py::none())
        .def("stop", &Pipeline::stop)
        .def("running", &Pipeline::running);
}

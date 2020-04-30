#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>

#include "timing.hpp"
#include "counting.hpp"
#include "profiling.hpp"

using namespace peptalk;
using std::runtime_error;
using std::string;
using std::vector;

void OnErrorOrWarning(const string& peptalk_msg, const string& papi_msg) {
    throw runtime_error(peptalk_msg + ": " + papi_msg);
}

/*
 * Timing
 */
void TimingInit(const string &result_file,
                const vector<string> &parameters,
                unsigned int num_measurements) {
    timing::Init(result_file, parameters, num_measurements, OnErrorOrWarning);
}

void TimingStart(const vector<string> &parameters) {
    timing::Start(parameters, OnErrorOrWarning);
}

void TimingStop() {
    timing::Stop(OnErrorOrWarning);
}

void TimingClose() {
    timing::Close(OnErrorOrWarning);
}

/*
 * Counting
 */
void CountingInit(const string &result_file,
                  const vector<string> &parameters,
                  const vector<string> &performance_events,
                  unsigned int num_measurements) {
    counting::Init(result_file, parameters, performance_events, num_measurements, OnErrorOrWarning);
}

void CountingStart(const vector<string> &parameters) {
    counting::Start(parameters, OnErrorOrWarning);
}

void CountingStop() {
    counting::Stop(OnErrorOrWarning);
}

void CountingClose() {
    counting::Close(OnErrorOrWarning);
}

/*
 * Profiling
 */
void ProfilingInit(const string &result_file,
                   const string &overflow_event,
                   int overflow_threshold,
                   const vector<string> &performance_events,
                   bool include_inst_address) {
    profiling::Init(
            result_file,
            overflow_event,
            overflow_threshold,
            performance_events,
            include_inst_address,
            OnErrorOrWarning);
}

void ProfilingStart(const string &trace_header) {
    profiling::Start(trace_header, OnErrorOrWarning);
}

void ProfilingStop() {
    profiling::Stop(OnErrorOrWarning);
}

void ProfilingClose() {
    profiling::Close(OnErrorOrWarning);
}

namespace py = pybind11;

PYBIND11_MODULE(peptalk, m) {
    m.doc() = R"pbdoc(
        pep-talk library for Python
        -----------------------
        .. currentmodule:: peptalk
        .. autosummary::
            :toctree: _generate
            timing_init
            timing_start
            timing_stop
            timing_close
            counting_init
            counting_start
            counting_stop
            counting_close
            profiling_init
            profiling_start
            profiling_stop
            profiling_close
    )pbdoc";
    /*
     * Timing
     */
    m.def("timing_init", &TimingInit, R"pbdoc(
        Initialise timing module
    )pbdoc");
    m.def("timing_start", &TimingStart, R"pbdoc(
        Start timing
    )pbdoc");
    m.def("timing_stop", &TimingStop, R"pbdoc(
        Stop timing
    )pbdoc");
    m.def("timing_close", &TimingClose, R"pbdoc(
        Close timing module
    )pbdoc");
    /*
     * Counting
     */
    m.def("counting_init", &CountingInit, R"pbdoc(
        Initialise counting module
    )pbdoc");
    m.def("counting_start", &CountingStart, R"pbdoc(
        Start counting
    )pbdoc");
    m.def("counting_stop", &CountingStop, R"pbdoc(
        Stop counting
    )pbdoc");
    m.def("counting_close", &CountingClose, R"pbdoc(
        Close counting module
    )pbdoc");
    /*
     * Profiling
     */
    m.def("profiling_init", &ProfilingInit, R"pbdoc(
        Initialise profiling module
    )pbdoc");
    m.def("profiling_start", &ProfilingStart, R"pbdoc(
        Start profiling
    )pbdoc");
    m.def("profiling_stop", &ProfilingStop, R"pbdoc(
        Stop profiling
    )pbdoc");
    m.def("profiling_close", &ProfilingClose, R"pbdoc(
        Close profiling module
    )pbdoc");
    m.attr("__version__") = "1.0";
}
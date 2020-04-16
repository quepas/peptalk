#include <mex.h>
#include <string>
#include <vector>

#include "profiling.hpp"

using std::string;
using std::vector;

void OnErrorOrWarning(const string &peptalk_msg, const string &papi_msg) {
    mexErrMsgTxt((peptalk_msg + ". " + papi_msg).c_str());
}

vector<string> UnpackPerformanceEvents(const mxArray *cell_performance_events) {
    size_t num_events = mxGetNumberOfElements(cell_performance_events);
    vector<string> performance_events;
    performance_events.reserve(num_events);
    for (size_t idx = 0; idx < num_events; ++idx) {
        mxArray *event_name_ptr = mxGetCell(cell_performance_events, idx);
        if (mxIsChar(event_name_ptr)) {
            performance_events.emplace_back(mxArrayToString(event_name_ptr));
        }
    }
    return performance_events;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (!mexIsLocked()) {
        mexLock();
    }

    peptalk::profiling::Init(
            mxArrayToString(prhs[0]),
            mxArrayToString(prhs[1]),
            (int) mxGetScalar(prhs[2]),
            UnpackPerformanceEvents(prhs[3]),
            mxIsLogicalScalarTrue(prhs[4]),
            OnErrorOrWarning
    );
}

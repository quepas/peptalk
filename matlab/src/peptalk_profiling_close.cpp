#include <mex.h>
#include "profiling.hpp"

using std::string;

void OnErrorOrWarning(const string &peptalk_msg, const string &papi_msg) {
    mexErrMsgTxt((peptalk_msg + ". " + papi_msg).c_str());
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (!mexIsLocked()) {
        mexLock();
    }

    peptalk::profiling::Close(OnErrorOrWarning);
}

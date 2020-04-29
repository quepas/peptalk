#include <napi.h>
#include <timing.hpp>
#include <counting.hpp>
#include <profiling.hpp>

using namespace peptalk;
using std::function;
using std::string;
using std::vector;

function<void(string, string)> OnErrorOrWarning(Napi::Env& env) {
    return [&env](string peptalk_msg, string papi_msg) {
        Napi::Error::New(env, peptalk_msg + " : " + papi_msg).ThrowAsJavaScriptException();
    };
}

vector<string> ExtractArrayOfStrings(const Napi::Array &array) {
    vector<string> result;
    result.reserve(array.Length());
    for (size_t idx = 0; idx < array.Length(); ++idx) {
        // TODO: check if each array element is a string
        result.push_back(array.Get(idx).As<Napi::String>().Utf8Value());
    }
    return result;
}

/*
 * Timing
 */
void TimingInit(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Wrong arguments. Expected: "
                                  "(result_file: string, parameters: string[], num_measurements: number)")
                .ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument must be a string").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsArray()) {
        Napi::TypeError::New(env, "Second argument must be of type string[]").ThrowAsJavaScriptException();
        return;
    }
    if (!info[2].IsNumber()) {
        Napi::TypeError::New(env, "Third argument must be a number").ThrowAsJavaScriptException();
        return;
    }

    auto result_file = info[0].As<Napi::String>().Utf8Value();
    auto parameter_names = info[1].As<Napi::Array>();
    auto num_measurements = info[2].As<Napi::Number>().Uint32Value();
    timing::Init(result_file, ExtractArrayOfStrings(parameter_names), num_measurements, OnErrorOrWarning(env));
}

void TimingStart(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env,
                             "Wrong arguments. Expected: (parameters: string[])").ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsArray()) {
        Napi::TypeError::New(env, "First argument must be of type string[]").ThrowAsJavaScriptException();
        return;
    }
    auto parameter_names = info[0].As<Napi::Array>();
    timing::Start(ExtractArrayOfStrings(parameter_names), OnErrorOrWarning(env));
}

void TimingStop(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    timing::Stop(OnErrorOrWarning(env));
}

void TimingClose(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    timing::Close(OnErrorOrWarning(env));
}

/*
 * Counting
 */
void CountingInit(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 4) {
        Napi::TypeError::New(env, "Wrong arguments. Expected: "
                                  "(result_file: string, parameters: string[],"
                                  " performance_events: string[], num_measurements: number)")
                .ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument must be a string").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsArray()) {
        Napi::TypeError::New(env, "Second argument must be of type string[]").ThrowAsJavaScriptException();
        return;
    }
    if (!info[2].IsArray()) {
        Napi::TypeError::New(env, "Third argument must be of type string[]").ThrowAsJavaScriptException();
        return;
    }
    if (!info[3].IsNumber()) {
        Napi::TypeError::New(env, "Fourth argument must be a number").ThrowAsJavaScriptException();
        return;
    }

    auto result_file = info[0].As<Napi::String>().Utf8Value();
    auto parameter_names = info[1].As<Napi::Array>();
    auto performance_events = info[2].As<Napi::Array>();
    auto num_measurements = info[3].As<Napi::Number>().Uint32Value();
    counting::Init(result_file,
                   ExtractArrayOfStrings(parameter_names),
                   ExtractArrayOfStrings(performance_events),
                   num_measurements,
                   OnErrorOrWarning(env));
}

void CountingStart(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong arguments. Expected: (parameters: string[])")
                .ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsArray()) {
        Napi::TypeError::New(env, "First argument must be of type string[]").ThrowAsJavaScriptException();
        return;
    }
    auto parameter_names = info[0].As<Napi::Array>();
    counting::Start(ExtractArrayOfStrings(parameter_names), OnErrorOrWarning(env));
}

void CountingStop(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    counting::Stop(OnErrorOrWarning(env));
}

void CountingClose(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    counting::Close(OnErrorOrWarning(env));
}

/*
 * Profiling
 */
void ProfilingInit(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 5) {
        Napi::TypeError::New(env, "Wrong arguments. Expected: "
                                  "(result_file: string, overflow_event: string,"
                                  " overflow_threshold: number, performance_events: string[],"
                                  " include_inst_address: bool)")
                .ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument must be a string").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsString()) {
        Napi::TypeError::New(env, "Second argument must be a string").ThrowAsJavaScriptException();
        return;
    }
    if (!info[2].IsNumber()) {
        Napi::TypeError::New(env, "Third argument must be a number").ThrowAsJavaScriptException();
        return;
    }
    if (!info[3].IsArray()) {
        Napi::TypeError::New(env, "Fourth argument must be of type string[]").ThrowAsJavaScriptException();
        return;
    }
    if (!info[4].IsBoolean()) {
        Napi::TypeError::New(env, "Fifth argument must be a boolean").ThrowAsJavaScriptException();
        return;
    }

    auto result_file = info[0].As<Napi::String>().Utf8Value();
    auto overflow_event = info[1].As<Napi::String>().Utf8Value();
    auto overflow_threshold = info[2].As<Napi::Number>().Uint32Value();
    auto performance_events = info[3].As<Napi::Array>();
    bool include_inst_address = info[4].As<Napi::Boolean>();
    profiling::Init(result_file,
                    overflow_event,
                    overflow_threshold,
                    ExtractArrayOfStrings(performance_events),
                    include_inst_address,
                    OnErrorOrWarning(env));
}

void ProfilingStart(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong arguments. Expected: (trace_header: string)")
                .ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument must be a string").ThrowAsJavaScriptException();
        return;
    }
    auto trace_header = info[0].As<Napi::String>().Utf8Value();
    profiling::Start(trace_header, OnErrorOrWarning(env));
}

void ProfilingStop(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    profiling::Stop(OnErrorOrWarning(env));
}

void ProfilingClose(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    profiling::Close(OnErrorOrWarning(env));
}

static Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    /*
     * Timing
     */
    exports.Set(Napi::String::New(env, "timingInit"),
                Napi::Function::New(env, TimingInit));
    exports.Set(Napi::String::New(env, "timingStart"),
                Napi::Function::New(env, TimingStart));
    exports.Set(Napi::String::New(env, "timingStop"),
                Napi::Function::New(env, TimingStop));
    exports.Set(Napi::String::New(env, "timingClose"),
                Napi::Function::New(env, TimingClose));
    /*
     * Counting
     */
    exports.Set(Napi::String::New(env, "countingInit"),
                Napi::Function::New(env, CountingInit));
    exports.Set(Napi::String::New(env, "countingStart"),
                Napi::Function::New(env, CountingStart));
    exports.Set(Napi::String::New(env, "countingStop"),
                Napi::Function::New(env, CountingStop));
    exports.Set(Napi::String::New(env, "countingClose"),
                Napi::Function::New(env, CountingClose));
    /*
     * Profiling
     */
    exports.Set(Napi::String::New(env, "profilingInit"),
                Napi::Function::New(env, ProfilingInit));
    exports.Set(Napi::String::New(env, "profilingStart"),
                Napi::Function::New(env, ProfilingStart));
    exports.Set(Napi::String::New(env, "profilingStop"),
                Napi::Function::New(env, ProfilingStop));
    exports.Set(Napi::String::New(env, "profilingClose"),
                Napi::Function::New(env, ProfilingClose));
    return exports;
}

NODE_API_MODULE(pep_talk_node, InitAll)

//
// Created by quepas on 13/04/2020.
//

#ifndef PEP_TALK_TIMING_HPP
#define PEP_TALK_TIMING_HPP

#include <chrono>
#include <functional>
#include <string>
#include <vector>

typedef const std::function<void(const std::string &, const std::string &)> error_callback_type;

namespace peptalk::timing {

    void Init(const std::string& result_file, const std::vector<std::string>& parameters_names, unsigned int num_measurements, error_callback_type& OnErrorOrWarning);

    void Start(const std::vector<std::string>& parameters, error_callback_type& OnErrorOrWarning);

    void Stop(error_callback_type& OnErrorOrWarning);

    void Close(error_callback_type& OnErrorOrWarning);

    class GenericTimer {
    public:
        virtual ~GenericTimer() = default;
        virtual void reset() = 0;
        virtual double elapsed() const = 0;
    };

    class SystemClockTimer : public GenericTimer
    {
    public:
        SystemClockTimer() : beg_(clock_::now()) {}
        ~SystemClockTimer() override = default;
        void reset() override { beg_ = clock_::now(); }
        double elapsed() const override {
            return std::chrono::duration_cast<second_>
                    (clock_::now() - beg_).count(); }

    private:
        typedef std::chrono::system_clock clock_;
        typedef std::chrono::duration<double, std::ratio<1> > second_;
        std::chrono::time_point<clock_> beg_;
    };

}

#endif //PEP_TALK_TIMING_HPP

//
// Created by quepas on 13/04/2020.
//

#ifndef PEP_TALK_TIMING_HPP
#define PEP_TALK_TIMING_HPP

#include <string>
#include <functional>

typedef const std::function<void(const std::string &, const std::string &)> error_callback_type;

namespace peptalk::timing {

    bool Init(const std::string& result_file, const std::vector<std::string>& parameters_names, unsigned int num_measurements, error_callback_type& OnErrorOrWarning);

    bool Start(const std::vector<std::string>& parameters, error_callback_type& OnErrorOrWarning);

    bool Stop(error_callback_type& OnErrorOrWarning);

    bool Close(error_callback_type& OnErrorOrWarning);

    class GenericTimer {
    public:
        virtual ~GenericTimer() = 0;
        virtual void reset() = 0;
        virtual double elapsed() const = 0;
    };

    class SystemClockTimer : public GenericTimer
    {
    public:
        SystemClockTimer() : beg_(clock_::now()) {}
        virtual ~SystemClockTimer() {}
        void reset() { beg_ = clock_::now(); }
        double elapsed() const {
            return std::chrono::duration_cast<second_>
                    (clock_::now() - beg_).count(); }

    private:
        typedef std::chrono::system_clock clock_;
        typedef std::chrono::duration<double, std::ratio<1> > second_;
        std::chrono::time_point<clock_> beg_;
    };

}

#endif //PEP_TALK_TIMING_HPP

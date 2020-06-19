#ifndef PEP_TALK_PEPREADER_HPP
#define PEP_TALK_PEPREADER_HPP

#include "PEPFormat.hpp"

#include <exception>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace peptalk::io {

class PEPReader {
public:

    bool Open(const std::string &file_name);

    void Close();

    bool ReadProfiles(const std::function<bool(
            const ProfileHeader& header,
            const std::vector<Observation>& observations)>& on_new_profile);

    uint16_t GetNumProfiles() const;

    uint16_t GetPepFormatVersion() const;

    bool GetHasInstructionAddressFlag() const;

private:
    std::ifstream in_file_stream;
    uint16_t num_profiles = 0;
    uint16_t pep_format_version = 0;
    bool has_instruction_address = false;

    void ReadFileHeader();

    void ReadProfileStart();

    void ReadProfileEnd();

    void ReadProfileHeader(ProfileHeader& profile_header);

    uint32_t ReadNumObservations();

    void ReadObservation(Observation& observation, size_t num_performance_events);

};

}

#endif //PEP_TALK_PEPREADER_HPP

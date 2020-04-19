//
// Created by quepas on 18/04/2020.
//

#include "PEPReader.hpp"
#include <iostream>

using namespace std;
using peptalk::io::PEPReader;

const char CSV_DELIMITER = ',';
const string CSV_DEFAULT_HEADER = "environment,benchmark,process";

vector<string> SplitBy(const string& str, char delimiter);
void PrintUsage();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Missing input file (PEP)" << endl;
        PrintUsage();
        return EXIT_FAILURE;
    }
    string input_pep_file = argv[1];
    if (argc < 3) {
        cerr << "Missing output file (CSV)" << endl;
        PrintUsage();
        return EXIT_FAILURE;
    }
    string output_csv_file = argv[2];
    PEPReader pep_reader;
    if (!pep_reader.Open(input_pep_file)) {
        cerr << "Failed to open PEP file: " << input_pep_file << endl;
        return EXIT_FAILURE;
    }

    ofstream csv_file;
    csv_file.open(output_csv_file);
    if (!csv_file.is_open()) {
        cerr << "Failed to open CSV file: " << output_csv_file << endl;
        pep_reader.Close();
        return EXIT_FAILURE;
    }
    cout << "pep2csv: " << input_pep_file << " --> " << output_csv_file << endl;

    string csv_header = CSV_DEFAULT_HEADER;
    if (argc < 4) {
        cout << "Using default header: " << csv_header << endl;
    } else {
        csv_header = argv[3];
    }
    auto csv_column_names = SplitBy(csv_header, CSV_DELIMITER);

    if (!csv_header.empty()) {
        csv_header += CSV_DELIMITER;
    }
    csv_file << csv_header << "metrics,time,value" << endl;

    if (!pep_reader.ReadProfiles([&csv_file, &csv_column_names](auto header, auto perf_events, auto measurements) {
        auto num_perf_events = perf_events.size();
        auto column_values = SplitBy(header, CSV_DELIMITER);
        if (column_values.size() != csv_column_names.size()) {
            cerr << "Profile header has different number of columns than excepted "
                 << " (given=" << column_values.size()
                 << "; expected=" << csv_column_names.size() << ")" << endl;
            return false;
        }
        long long int current_time;
        for (size_t idx = 0; idx < measurements.size(); ++idx) {
            auto pe_idx = idx % num_perf_events;
            if (pe_idx == 0) {
                current_time = measurements[idx];
            } else {
                if (!header.empty()) {
                    csv_file << header << CSV_DELIMITER;
                }
                csv_file << perf_events[pe_idx] << CSV_DELIMITER
                         << current_time << CSV_DELIMITER
                         << measurements[idx] << endl;
            }
        }
        return true;
    })) {
        cerr << "Failed to read profiles" << endl;
        return EXIT_FAILURE;
    }
    pep_reader.Close();
    csv_file.close();
    cout << "Success. Converted " << pep_reader.GetNumProfiles() << " profiles." << endl;
    return EXIT_SUCCESS;
}

vector<string> SplitBy(const string& str, char delimiter) {
    vector<string> chunks;
    string word;
    for (auto chr : str) {
        if (chr == delimiter) {
            chunks.push_back(word);
            word.clear();
        } else {
            word += chr;
        }
    }
    chunks.push_back(word);
    return chunks;
}

void PrintUsage() {
    cout << "Usage: pep2csv path_pep path_csv [header=deafult \"" << CSV_DEFAULT_HEADER << "\"]" << endl;
}

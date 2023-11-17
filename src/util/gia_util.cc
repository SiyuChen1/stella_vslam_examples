#include "gia_util.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

gia_sequence::gia_sequence(const std::string& seq_dir_path, const int& start_id) {
    // load timestamps

    std::vector<std::string> tokens;
    std::istringstream stream(seq_dir_path);
    std::string token;

    char delimiter = '/';
    
    while (getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    std::string dataset_record_date = tokens.at(tokens.size() - 1);

    if(dataset_record_date == ""){
        dataset_record_date = tokens.at(tokens.size() - 2);
    }

    const std::string timestamp_file_path = seq_dir_path + "/Stereocam_" + dataset_record_date + ".log";
    std::ifstream ifs_timestamp;
    ifs_timestamp.open(timestamp_file_path.c_str());
    if (!ifs_timestamp) {
        throw std::runtime_error("Could not load a timestamp file from " + timestamp_file_path);
    }

    real_timestamps_.clear();
    recorded_timestamps_.clear();
    while (!ifs_timestamp.eof()) {
        std::string s;
        getline(ifs_timestamp, s);
        if (!s.empty()) {
            std::stringstream ss;
            ss << s;
            double real_timestamp, recorded_timestamp;
            ss >> real_timestamp >> recorded_timestamp;
            real_timestamps_.push_back(real_timestamp);
            recorded_timestamps_.push_back(recorded_timestamp);
        }
    }

    ifs_timestamp.close();

    real_timestamps_.erase(real_timestamps_.begin(), real_timestamps_.begin() + start_id);

    left_img_file_paths_.clear();
    right_img_file_paths_.clear();
    for (unsigned int i = 0; i < real_timestamps_.size(); ++i) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(6) << i + start_id;
        left_img_file_paths_.push_back(seq_dir_path + ss.str() + "L.jpg");
        right_img_file_paths_.push_back(seq_dir_path + ss.str() + "R.jpg");
    }
}

std::vector<gia_sequence::frame> gia_sequence::get_frames() const {
    std::vector<frame> frames;
    assert(timestamps_.size() == left_img_file_paths_.size());
    assert(timestamps_.size() == right_img_file_paths_.size());
    assert(left_img_file_paths_.size() == right_img_file_paths_.size());
    for (unsigned int i = 0; i < real_timestamps_.size(); ++i) {
        frames.emplace_back(frame{left_img_file_paths_.at(i), right_img_file_paths_.at(i), real_timestamps_.at(i)});
    }
    return frames;
}

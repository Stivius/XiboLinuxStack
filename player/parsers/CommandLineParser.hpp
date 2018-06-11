#pragma once

#include <string>
#include <boost/program_options/options_description.hpp>
#include <spdlog/spdlog.h>

class CommandLineParser
{
public:
    CommandLineParser();
    bool parse(int argc, char** argv);

    bool is_version() const;
    bool is_example_dir() const;
    bool is_testing() const;
    const std::string& xlf_path() const;
    const std::string& example_dir_path() const;
    const boost::program_options::options_description& available_options() const;

private:
    std::pair<bool, std::string> find_xlf_file(const std::string& example_dir_path);
    bool check_example_dir(const std::string& example_dir_path);

private:
    bool m_is_version = false;
    bool m_is_example_dir = false;
    bool m_is_testing = false;
    std::string m_xlf_path;
    std::string m_example_dir_path;

    boost::program_options::options_description m_options;
    std::shared_ptr<spdlog::logger> m_logger;
};

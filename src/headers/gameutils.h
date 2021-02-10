#pragma once

#include <iostream>
#include <boost/filesystem.hpp>
#include <Constants.h>


using namespace boost::filesystem;
class Gameutils
{
public:
	static boost::filesystem::path get_working_dir();
	static std::shared_ptr<boost::filesystem::path> find_folder_in_project(std::string folder_name);
	static std::string find_folder_in_project_string(std::string folder_name);
	static bool file_exists(std::string file_path);
};


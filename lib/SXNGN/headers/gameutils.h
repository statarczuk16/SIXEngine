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
	/**
	* Adjust the absolute value of parameter
	* @value_in: value to be adjusted
	* @adjustment: how much to adjust
	*
	* Eg. adjust -10 by +5 = -15
	*	  adjust 6 by -3 = 3
	*	  adjust 6 by -10 = 0 (no such thing as negative absolute value!)
	*	  adjust -3 by 4 = 0
	**/
	static int adjust_absolute_value(int value_in, int adjustment);
	static int adjust_within_range(int value, int adjustment, int max_val, int min_val);

	static int adjust_abs_value_within_range(int value_in, int adjustment, int max_val, int min_val);

	static double adjust_dbl_within_range(double value, double adjustment, double max_val, double min_val);

	static double adjust_dbl_abs_value_within_range(double value_in, double adjustment, double max_val, double min_val);

};
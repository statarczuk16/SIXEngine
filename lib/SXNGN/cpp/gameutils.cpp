#include <gameutils.h>

namespace SXNGN {
	std::filesystem::path Gameutils::get_working_dir()
	{
		return std::filesystem::current_path();
	}

	std::string Gameutils::get_file_in_folder(std::string folder, std::string file_in_folder)
	{
		std::filesystem::path p(folder);
		if (std::filesystem::exists(p))
		{
			p.replace_filename(file_in_folder);
			if (std::filesystem::exists(p))
			{
				return p.string();
			}
		}
		return SXNGN::BAD_STRING_RETURN;
	}

	std::string Gameutils::find_folder_in_project_string(std::string folder_name)
	{

		std::string ret = SXNGN::BAD_STRING_RETURN;

		auto ptr = find_folder_in_project(folder_name);

		if (ptr != nullptr)
		{
			ret = ptr->string();
		}

		return ret;
	}

	bool Gameutils::file_exists(std::string file_path)
	{
		std::filesystem::path p(file_path);
		if (std::filesystem::is_regular_file(file_path))
		{
			return true;
		}
		return false;
	}

	int Gameutils::adjust_absolute_value(int value_in, int adjustment)
	{

		bool val_was_pos = value_in >= 0;
		int value_out = value_in + adjustment;
		if (value_in == 0)
		{
			return value_out;
		}
		bool val_is_pos = value_out >= 0;
		if (val_was_pos && val_is_pos)
		{
			return value_out;
		}
		else if (val_was_pos && !val_is_pos)
		{
			return 0;
		}
		else if (!val_was_pos && val_is_pos)
		{
			return 0;
		}
		else
		{
			return value_out;
		}

	}


	int Gameutils::adjust_within_range(int value, int adjustment, int max_val, int min_val)
	{
		int ret = value + adjustment;
		if (ret > max_val)
		{
			return max_val;
		}
		if (ret < min_val)
		{
			return min_val;
		}
		return ret;
	}

	int Gameutils::adjust_abs_value_within_range(int value_in, int adjustment, int max_val, int min_val)
	{
		bool val_was_pos = value_in >= 0;
		int value_out = Gameutils::adjust_within_range(value_in, adjustment, max_val, min_val);
		if (value_in == 0)
		{
			return value_out;
		}
		bool val_is_pos = value_out >= 0;
		if (val_was_pos && val_is_pos)
		{
			return value_out;
		}
		else if (val_was_pos && !val_is_pos)
		{
			return 0;
		}
		else if (!val_was_pos && val_is_pos)
		{
			return 0;
		}
		else
		{
			return value_out;
		}
	}

	double Gameutils::adjust_dbl_within_range(double value, double adjustment, double max_val, double min_val)
	{
		double ret = value + adjustment;
		if (ret > max_val)
		{
			return max_val;
		}
		if (ret < min_val)
		{
			return min_val;
		}
		return ret;
	}

	double Gameutils::adjust_dbl_abs_value_within_range(double value_in, double adjustment, double max_val, double min_val)
	{
		bool val_was_pos = value_in >= 0.0;
		double value_out = Gameutils::adjust_dbl_within_range(value_in, adjustment, max_val, min_val);
		if (value_in == 0.0)
		{
			return value_out;
		}
		bool val_is_pos = value_out >= 0.0;
		if (val_was_pos && val_is_pos)
		{
			return value_out;
		}
		else if (val_was_pos && !val_is_pos)
		{
			return 0.0;
		}
		else if (!val_was_pos && val_is_pos)
		{
			return 0.0;
		}
		else
		{
			return value_out;
		}
	}

	std::shared_ptr<std::filesystem::path> Gameutils::find_folder_in_project(std::string folder_name)
	{
		bool found = false;
		std::filesystem::path current = std::filesystem::current_path();
		std::shared_ptr<std::filesystem::path> media_folder = nullptr;
		std::filesystem::directory_iterator end_itr;
		int max_iter = 7;
		int iter = 0;
		while (!found)
		{
			for (std::filesystem::directory_iterator itr(current); itr != end_itr; ++itr)
			{
				// If it's not a directory, list it. If you want to list directories too, just remove this check.
				if (is_regular_file(itr->path()) || !(itr->path().has_stem())) {
					continue;
					// assign current file name to current_file and echo it out to the console.
				}
				std::string current_file = itr->path().stem().string();
				std::cout << current_file << std::endl;

				if (current_file == folder_name)
				{
					media_folder = std::make_shared<std::filesystem::path>(itr->path());
					found = true;
				}

			}
			//step up one directory
			current = current.parent_path();
			iter++;
			if (iter > max_iter)
			{
				break;
			}
		}
		if (!found)
		{
			std::cout << "Debug: " << " Could not find folder: " << folder_name << std::endl;
		}
		return media_folder;
	}
}


#include "gameutils.h"


boost::filesystem::path Gameutils::get_working_dir()
{
	return boost::filesystem::current_path();
}

std::string Gameutils::find_folder_in_project_string(std::string folder_name)
{
	//FIXME make this a constant for all "bad string" returns
	std::string ret = SXNGN::BAD_STRING_RETURN;

	auto ptr = find_folder_in_project("media");

	if (ptr != nullptr)
	{
		ret = ptr->string();
	}

	return ret;
}

bool Gameutils::file_exists(std::string file_path)
{
	boost::filesystem::path p(file_path);
	if (boost::filesystem::is_regular_file(file_path))
	{
		return true;
	}
	return false;
}

std::shared_ptr<boost::filesystem::path> Gameutils::find_folder_in_project(std::string folder_name)
{
	bool found = false;
	boost::filesystem::path current = current_path();
	std::shared_ptr<boost::filesystem::path> media_folder = nullptr;
	directory_iterator end_itr;
	int max_iter = 7;
	int iter = 0;
	while (!found)
	{
		for (directory_iterator itr(current); itr != end_itr; ++itr)
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
				media_folder = std::make_shared<boost::filesystem::path>(itr->path());
				found = true;
			}
			
		}
		current = current.branch_path();
		iter++;
		if (iter > max_iter)
		{
			break;
		}
	}
	if (!found)
	{
		std::cout << "Debug: " << " Could not find media folder" <<  folder_name <<std::endl;
	}
	return media_folder;
}



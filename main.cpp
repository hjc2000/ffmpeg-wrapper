#include <ffmpeg-wrapper/mux/SptsEncodeMux.h>
#include <filesystem>
#include <iostream>

using namespace std;

int main()
{
	try
	{
		std::filesystem::current_path(Predefine_ResourceDir);
		test_SptsEncodeMux();
		return 0;
	}
	catch (std::exception const &e)
	{
		std::cout << e.what() << std::endl;
		throw;
	}
}

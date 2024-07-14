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
	catch (std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
		throw;
	}
}

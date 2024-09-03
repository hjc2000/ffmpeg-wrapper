#include <ffmpeg-wrapper/mux/SptsEncodeMux.h>
#include <ffmpeg-wrapper/signal-source/AudioSampler.h>
#include <filesystem>
#include <iostream>

using namespace std;

int main()
{
    try
    {
        std::filesystem::current_path(Predefine_ResourceDir);
        test_SptsEncodeMux();
        // video::TestAudioSampler();
        return 0;
    }
    catch (std::exception const &e)
    {
        std::cout << e.what() << std::endl;
        throw;
    }
}

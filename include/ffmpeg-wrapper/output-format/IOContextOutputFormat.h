#pragma once
#include <ffmpeg-wrapper/output-format/OutputFormat.h>
#include <ffmpeg-wrapper/wrapper/AVIOContextWrapper.h>
#include <memory>

namespace video
{
    class IOContextOutputFormat :
        public OutputFormat
    {
    private:
        std::shared_ptr<AVIOContextWrapper> _io_context;

    public:
        /// @brief
        /// @param url 这里的 url 不是用来创建文件的，而是让 ffmpeg 根据后缀名分析封装格式的
        /// @param io_context
        IOContextOutputFormat(std::string url, std::shared_ptr<AVIOContextWrapper> io_context);
        ~IOContextOutputFormat();
    };
} // namespace video

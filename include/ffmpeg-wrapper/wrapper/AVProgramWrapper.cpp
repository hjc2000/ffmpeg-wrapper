#include "AVProgramWrapper.h"

video::AVProgramWrapper::AVProgramWrapper(AVProgram *program)
{
    _wrapped_obj = program;

    _dic = std::unique_ptr<AVDictionaryWrapper>{
        new AVDictionaryWrapper{
            _wrapped_obj->metadata,
        },
    };

    // 需要设置为不释放字典，否则会与 avformat_free_context 函数重复释放。
    _dic->DoNotFreeInnerDictionary();
}

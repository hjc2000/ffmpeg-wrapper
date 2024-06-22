#include "AVDictionaryWrapper.h"

using namespace video;

video::AVDictionaryWrapper::AVDictionaryIterator video::AVDictionaryWrapper::begin() const
{
	auto entry = av_dict_get(_wrapped_obj, "", nullptr, AV_DICT_IGNORE_SUFFIX);
	return AVDictionaryIterator(_wrapped_obj, entry);
}

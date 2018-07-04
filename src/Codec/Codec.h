#pragma once

#include "../Debug.h"

#define LogCodec(msg, isErr, args...) isErr ? LogError(LogCategory::CODEC, msg, ##args) : LogVerbose(LogCategory::CODEC, msg, ##args);

namespace Codec {

}

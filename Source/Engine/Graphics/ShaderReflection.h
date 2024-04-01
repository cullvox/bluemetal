#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "PipelineResource.h"

namespace bl
{
class BLUEMETAL_API ShaderReflection
{
public:
    ShaderReflection(const std::vector<uint32_t>& binary);
    ~ShaderReflection();



};

} // namespace bl
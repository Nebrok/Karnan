#include "KarnanLightsUBO.h"

KarnanLightsUBO::KarnanLightsUBO(KarnanDevice& device)
    :_karnanDevice(device)
{
    _buffer = std::make_unique<KarnanBuffer>(
        _karnanDevice,
        sizeof(Karnan::LightingUBO),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );
    _buffer->Map();
}

KarnanLightsUBO::~KarnanLightsUBO()
{
}

void KarnanLightsUBO::UpdateUBO(void* data)
{
    _buffer->WriteToBuffer(data);
    _buffer->Flush();
}

VkDescriptorBufferInfo KarnanLightsUBO::GetDescriptorInfo()
{
    return _buffer->DescriptorInfo();
}


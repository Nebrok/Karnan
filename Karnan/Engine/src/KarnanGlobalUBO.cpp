#include "KarnanGlobalUBO.h"

KarnanGlobalUBO::KarnanGlobalUBO(KarnanDevice& device)
    : _karnanDevice(device)
{
    _buffer = std::make_unique<KarnanBuffer>(
        _karnanDevice,
        sizeof(Karnan::GlobalUBO),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );
    _buffer->Map();
}

KarnanGlobalUBO::~KarnanGlobalUBO()
{
}

void KarnanGlobalUBO::UpdateUBO(void* data)
{
    _buffer->WriteToBuffer(data);
    _buffer->Flush();
}

VkDescriptorBufferInfo KarnanGlobalUBO::GetDescriptorInfo()
{
    return _buffer->DescriptorInfo();
}

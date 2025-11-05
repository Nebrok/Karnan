#pragma once

#include "KarnanDevice.h"

class KarnanBuffer {
public:
    KarnanBuffer(
        KarnanDevice& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment = 1);
    ~KarnanBuffer();

    KarnanBuffer(const KarnanBuffer&) = delete;
    KarnanBuffer& operator=(const KarnanBuffer&) = delete;

    VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void Unmap();

    void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void WriteToIndex(void* data, int index);
    VkResult FlushIndex(int index);
    VkDescriptorBufferInfo DescriptorInfoForIndex(int index);
    VkResult InvalidateIndex(int index);

    VkBuffer GetBuffer() const { return _buffer; }
    void* GetMappedMemory() const { return _mapped; }
    uint32_t GetInstanceCount() const { return _instanceCount; }
    VkDeviceSize GetInstanceSize() const { return _instanceSize; }
    VkDeviceSize GetAlignmentSize() const { return _instanceSize; }
    VkBufferUsageFlags GetUsageFlags() const { return _usageFlags; }
    VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return _memoryPropertyFlags; }
    VkDeviceSize GetBufferSize() const { return _bufferSize; }

private:
    static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    KarnanDevice& _karnanDevice;
    void* _mapped = nullptr;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;

    VkDeviceSize _bufferSize;
    uint32_t _instanceCount;
    VkDeviceSize _instanceSize;
    VkDeviceSize _alignmentSize;
    VkBufferUsageFlags _usageFlags;
    VkMemoryPropertyFlags _memoryPropertyFlags;
};
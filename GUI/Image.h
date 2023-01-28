#pragma once

#include <string>

#include "vulkan/vulkan.h"

namespace GUI {
  enum class ImageFormat
  {
    None = 0,
    RGBA,
    RGBA32F
  };

  class Image
  {
  public:
    Image(std::string_view path);
    Image(std::uint32_t width, std::uint32_t heigth, ImageFormat format, const void* data = nullptr);
    ~Image();

    void SetData(const void* data);

    VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

    void Resize(std::uint32_t width, std::uint32_t height);

    std::uint32_t GetWidth() const { return m_Width; }
    std::uint32_t GetHeigth() const { return m_Heigth; }
  private:
    void AllocateMemory(std::uint64_t size);
    void Release();
  private:
    std::uint32_t m_Width = 0, m_Heigth = 0;

    VkImage m_Image = nullptr;
    VkImageView m_ImageView = nullptr;
    VkDeviceMemory m_Memory = nullptr;
    VkSampler m_Sampler = nullptr;

    ImageFormat m_Format = ImageFormat::None;

    VkBuffer m_StagingBuffer = nullptr;
    VkDeviceMemory m_StagingBufferMemory = nullptr;

    size_t m_AlignedSize = 0;

    VkDescriptorSet m_DescriptorSet = nullptr;

    std::string m_Filepath;
  };
}
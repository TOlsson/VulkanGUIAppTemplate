#include "Image.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"

#include "Application.h"

#define STB_IMAGE_IMPLEMENTAION
#include "stb_image.h"

namespace GUI {
  namespace Utils {

    static std::uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags properties, std::uint32_t type_bits)
    {
      VkPhysicalDeviceMemoryProperties prop;
      vkGetPhysicalDeviceMemoryProperties(Application::GetPhysicalDevice(), &prop);
      for (std::uint32_t i = 0; i < prop.memoryTypeCount; i++)
      {
        if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
          return i;
      }
      return 0xffffffff;
    }

    static std::uint32_t BytesPerPixel(ImageFormat format)
    {
      switch (format)
      {
      case GUI::ImageFormat::RGBA:
        return 4;
      case GUI::ImageFormat::RGBA32F:
        return 16;
      }
      return 0;
    }

    static VkFormat GUIFormatToVulkanFormat(ImageFormat format)
    {
      switch (format)
      {
      case GUI::ImageFormat::RGBA:
        return VK_FORMAT_R8G8B8A8_UNORM;
      case GUI::ImageFormat::RGBA32F:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
      }
      return (VkFormat)0;
    }
  }

  Image::Image(std::string_view path)
    : m_Filepath(path)
  {
    int width, heigth, channels;
    std::uint8_t* data = nullptr;

    if (stbi_is_hdr(m_Filepath.c_str()))
    {
      data = (std::uint8_t*)stbi_loadf(m_Filepath.c_str(), &width, &heigth, &channels, 4);
      m_Format = ImageFormat::RGBA32F;
    }
    else
    {
      data = stbi_load(m_Filepath.c_str(), &width, &heigth, &channels, 4);
      m_Format = ImageFormat::RGBA;
    }

    m_Width = width;
    m_Heigth = heigth;

    AllocateMemory(m_Width * m_Heigth * Utils::BytesPerPixel(m_Format));
    SetData(data);
    stbi_image_free(data);
  }

  Image::Image(std::uint32_t width, std::uint32_t heigth, ImageFormat format, const void* data)
    : m_Width(width), m_Heigth(heigth), m_Format(format)
  {
    AllocateMemory(m_Width * m_Heigth * Utils::BytesPerPixel(m_Format));
    if (data)
      SetData(data);
  }

  Image::~Image()
  {
    Release();
  }

  void Image::AllocateMemory(std::uint64_t size)
  {
    VkDevice device = Application::GetDevice();

    VkResult err;

    VkFormat vulkanFormat = Utils::GUIFormatToVulkanFormat(m_Format);

    {
      VkImageCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      info.imageType = VK_IMAGE_TYPE_2D;
      info.format = vulkanFormat;
      info.extent.width = m_Width;
      info.extent.height = m_Heigth;
      info.extent.depth = 1;
      info.mipLevels = 1;
      info.arrayLayers = 1;
      info.samples = VK_SAMPLE_COUNT_1_BIT;
      info.tiling = VK_IMAGE_TILING_OPTIMAL;
      info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
      info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      err = vkCreateImage(device, &info, nullptr, &m_Image);
      check_vk_result(err);
      VkMemoryRequirements req;
      vkGetImageMemoryRequirements(device, m_Image, &req);
      VkMemoryAllocateInfo alloc_info = {};
      alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      alloc_info.allocationSize = req.size;
      alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
      err = vkAllocateMemory(device, &alloc_info, nullptr, &m_Memory);
      check_vk_result(err);
      err = vkBindImageMemory(device, m_Image, m_Memory, 0);
      check_vk_result(err);
    }

    {
      VkImageViewCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      info.image = m_Image;
      info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      info.format = vulkanFormat;
      info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      info.subresourceRange.layerCount = 1;
      info.subresourceRange.levelCount = 1;
      err = vkCreateImageView(device, &info, nullptr, &m_ImageView);
      check_vk_result(err);
    }

    {
      VkSamplerCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      info.magFilter = VK_FILTER_LINEAR;
      info.minFilter = VK_FILTER_LINEAR;
      info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      info.minLod = -1000;
      info.maxLod = 1000;
      info.maxAnisotropy = 1.0f;
      err = vkCreateSampler(device, &info, nullptr, &m_Sampler);
      check_vk_result(err);
    }

    m_DescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }

  void Image::Release()
  {
    Application::SubmitResourceFree([sampler = m_Sampler, imageView = m_ImageView, image = m_Image,
      memory = m_Memory, stagingBuffer = m_StagingBuffer, stagingBufferMemory = m_StagingBufferMemory]()
      {
        VkDevice device = Application::GetDevice();

        vkDestroySampler(device, sampler, nullptr);
        vkDestroyImageView(device, imageView, nullptr);
        vkDestroyImage(device, image, nullptr);
        vkFreeMemory(device, memory, nullptr);
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
      });

    m_Sampler = nullptr;
    m_ImageView = nullptr;
    m_Image = nullptr;
    m_Memory = nullptr;
    m_StagingBuffer = nullptr;
    m_StagingBufferMemory = nullptr;
  }

  void Image::SetData(const void* data)
  {
    VkDevice device = Application::GetDevice();

    size_t upload_size = m_Width * m_Heigth * Utils::BytesPerPixel(m_Format);

    VkResult err;

    if (!m_StagingBuffer)
    {
      {
        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.size = upload_size;
        info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        err = vkCreateBuffer(device, &info, nullptr, &m_StagingBuffer);
        check_vk_result(err);
        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(device, m_StagingBuffer, &req);
        m_AlignedSize = req.size;
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = req.size;
        alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
        err = vkAllocateMemory(device, &alloc_info, nullptr, &m_StagingBufferMemory);
        check_vk_result(err);
        err = vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0);
        check_vk_result(err);
      }
    }

    {
      char* map = NULL;
      err = vkMapMemory(device, m_StagingBufferMemory, 0, m_AlignedSize, 0, (void**)(&map));
      check_vk_result(err);
      memcpy(map, data, upload_size);
      VkMappedMemoryRange range[1] = {};
      range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      range[0].memory = m_StagingBufferMemory;
      range[0].size = m_AlignedSize;
      err = vkFlushMappedMemoryRanges(device, 1, range);
      check_vk_result(err);
      vkUnmapMemory(device, m_StagingBufferMemory);
    }

    {
      VkCommandBuffer command_buffer = Application::GetCommandBuffer(true);

      VkImageMemoryBarrier copy_barrier = {};
      copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      copy_barrier.image = m_Image;
      copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      copy_barrier.subresourceRange.levelCount = 1;
      copy_barrier.subresourceRange.layerCount = 1;
      vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

      VkBufferImageCopy region = {};
      region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      region.imageSubresource.layerCount = 1;
      region.imageExtent.width = m_Width;
      region.imageExtent.height = m_Heigth;
      region.imageExtent.depth = 1;
      vkCmdCopyBufferToImage(command_buffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

      VkImageMemoryBarrier use_barrier = {};
      use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      use_barrier.image = m_Image;
      use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      use_barrier.subresourceRange.levelCount = 1;
      use_barrier.subresourceRange.layerCount = 1;
      vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);

      Application::FlushCommandBuffer(command_buffer);
    }
  }

  void Image::Resize(std::uint32_t width, std::uint32_t heigth)
  {
    if (m_Image && m_Width == width && m_Heigth == heigth)
      return;

    m_Width = width;
    m_Heigth = heigth;

    Release();

    AllocateMemory(m_Width * m_Heigth * Utils::BytesPerPixel(m_Format));
  }
}
#pragma once

#include "Core/Export.h"
#include "Window/Window.hpp"
#include "Render/Vulkan/vk_mem_alloc.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>
#include <functional>

namespace bl
{

/** \brief Creates the basic vulkan elements required to start rendering.
* 
* Creates the vulkan objects like an instance, and device and chooses a 
* physical device to use. Most info that anything might need to continue 
* building a renderer is available for those objects to use. On the event that
* the render device fails you must check \ref good() post construction.
* This object will not fail after it has been constructed.
* 
* \since BloodLust 1.0.0
* 
* \sa Renderer
* \sa Swapchain
* 
*/
class BLOODLUST_API RenderDevice
{
public:

    /** \brief Default constructor, does nothing.
    * 
    * Use this constructor as a place holder while waiting for another render
    * device to be created else where. If \ref good() was called after this 
    * constructor it would return false. You must use the operator= to move
    * another device that has used the other constructor to this one for 
    * \ref good to return true.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice::RenderDevice
    * 
    */
    RenderDevice() noexcept;
    
    /** \brief Constructs the render device, creates all vulkan objects.
    * 
    * Unlike the default constructor this one creates the vulkan objects.If 
    * there was an error when creating the vulkan objects you can call 
    * \ref good() to check for failure. 
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getInstance()
    * \sa getPhysicalDevice()
    * \sa getDevice()
    *
    */
    RenderDevice(Window& window) noexcept;

    /** \brief Default destructor
    * 
    * Destroys the vulkan objects.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice()
    * 
    */
    ~RenderDevice() noexcept;

    /** \brief Collapses this, moves values from rhs to this.
    * 
    * Preforms a move operation from rhs to this. This is important because
    * without the ability to move constructors would be a terrible experience
    * and the fact that everything is noexcept. It causes issues so a default
    * constructor and move operator is essential to the process.
    * 
    * \return *this
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice()
    * 
    */
    RenderDevice& operator=(RenderDevice&& rhs) noexcept;
    RenderDevice& operator=(RenderDevice&) noexcept = delete;

    /** \brief Gets the vulkan instance.
    * 
    * The instance is created during the second construtors initialization.
    * 
    * \return The instance
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice()
    * 
    */
    [[nodiscard]] VkInstance getInstance() const noexcept;
    
    /** \brief Gets the vulkan physical device.
    * 
    * This is the physical device choosen during construction.
    * 
    * \return The vulkan physical device
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice()
    * \sa getInstance()
    * 
    */
    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const noexcept;
    
    /** \brief Gets the logical devices graphics queue family index.
    * 
    * The graphics queue is created during construction and when the device is
    * created this graphics family index was used.
    * 
    * \return The graphics family index
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice()
    * \sa getDevice()
    * 
    */
    [[nodiscard]] uint32_t getGraphicsFamilyIndex() const noexcept;
    
    /** \brief Gets the logical devices present queue family index.
    * 
    * The preset queue is created during construction and when the device is 
    * created this present family index was used.
    * 
    * \return The present family index
    *
    * \since BloodLust 1.0.0 
    * 
    * \sa RenderDevice()
    * \sa getDevice()
    * 
    */
    [[nodiscard]] uint32_t getPresentFamilyIndex() const noexcept;
    
    /** \brief Gets the vulkan device.
    * 
    * Retrieves the vulkan device that was created during construction.
    * 
    * \since BloodLust 1.0.0
    * 
    * \return The device
    * 
    * \sa RenderDevice()
    * \sa getGraphicsFamilyIndex()
    * \sa getPresentFamilyIndex()
    * \sa getGraphicsQueue()
    * \sa getPresentQueue()
    * 
    */
    [[nodiscard]] VkDevice getDevice() const noexcept;
    
    /** \brief Gets the graphics queue.
    * 
    * Post device creation the graphics queue is retrieved from vulkan and
    * returned this value.
    * 
    * \return The graphics queue
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getDevice()
    * \sa getGraphicsFamilyIndex()
    * 
    */
    [[nodiscard]] VkQueue getGraphicsQueue() const noexcept;

    /** \brief Gets the present queue.
    *
    * Post device creation the present queue is retrieved from vulkan and
    * returned this value.
    *
    * \return The present queue
    *
    * \since BloodLust 1.0.0
    *
    * \sa getDevice()
    * \sa getPresentFamilyIndex()
    *
    */
    [[nodiscard]] VkQueue getPresentQueue() const noexcept;

    /** \brief Retrieves a command pool.
    * 
    * At construction a command pool is created to allow for easy command 
    * buffer creation where ever a render device is present.
    * 
    * \return The command pool
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getDevice()
    *
    */
    [[nodiscard]] VkCommandPool getCommandPool() const noexcept;
    
    /** \brief Returns an VMA allocator.
    * 
    * This library makes use of the Vulkan Memory Allocator by AMD. Here is a
    * handle to the allocator that it creates. Buffers and images can and 
    * should make use of this to ensure memory is properly being allocated.
    * 
    * \return The allocator
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getDevice()
    * 
    */
    [[nodiscard]] VmaAllocator getAllocator() const noexcept;
    
    /** \brief Helper function, determines concurrent or exclusive queues.
    * 
    * When creating an image or buffer you need to select what queue(s) you are
    * going to use. When both the \ref getGraphicsFamilyIndex() and
    * \ref getPresentFamilyIndex() return the same value both graphics and
    * present are on the same queue. 
    * 
    * \return true if \ref getGraphicsFamilyIndex() and 
    *       \ref getPresentFamilyIndex() return the same value.
    * 
    * \return false if \ref getGraphicsFamilyIndex() and
    *       \ref getPresentFamilyIndex() do not return the same value.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getDevice()
    * 
    */
    [[nodiscard]] bool areQueuesSame() const noexcept;
    
    /** \brief Helper function, determines a format for images. 
    *
    * Choosing a format for an image can be a difficult process, this function
    * will help you get down to the format that you want without all the 
    * hassle. Give it some candidates ad other options to get what you want
    * and it'll do all the work. If a format could not be found it will return
    * VK_FORMAT_UNDEFINED.
    * 
    * \return Your found format, may be VK_FORMAT_UNDEFINED.
    * 
    * \since BloodLust 1.0.0
    * 
    */
    [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const noexcept;
    
    /** \brief Creates a command buffer object and immediately submits it after recording.
    * 
    * A command buffer is created using the command pool. Then the recorder is given
    * the new command buffer and it records some cool commands. The commands are then
    * submitted to the graphics queue.
    * 
    * \param recorder is a function which records your command, usually a lambda.
    * 
    * \return true if the submission succeeded.
    * \return false if the command pool failed to allocate or a submission error occurred.
    *
    */
    bool immediateSubmit(std::function<void(VkCommandBuffer)> recorder);

    /** \brief Determines if the device was constructed successfully.
    * 
    * This function determines if it was successfully constructed. If one of
    * the processes required to create the object fails it will return false.
    * On the event that it is created successfully it will return true.
    * 
    * \return true if everything was created successfully.
    * \return false if something wasn't created successfully.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa RenderDevice(Window&)
    */
    [[nodiscard]] bool good() const noexcept;

private:

    /** \brief Gets the vaidation layers.
    * 
    * Retrieves validation layers that are used when debugging.
    * 
    * \param[out] layers The validation layers are added to the provided vector.
    * 
    * \return true if the validation layers were retrieved successfully.
    * \return false if the validation layers were not retrieved successfully.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getInstanceExtensions()
    * \sa getDeviceExtensions()
    * 
    */
    [[nodiscard]] bool getValidationLayers(std::vector<const char*>& layers) const noexcept;
    [[nodiscard]] bool getInstanceExtensions(std::vector<const char*>&) const noexcept;
    [[nodiscard]] bool createInstance() noexcept;
    [[nodiscard]] bool choosePhysicalDevice() noexcept;
    [[nodiscard]] bool getDeviceExtensions(std::vector<const char*>&) const noexcept;
    [[nodiscard]] bool createDevice() noexcept;
    [[nodiscard]] bool createCommandPool() noexcept;
    [[nodiscard]] bool createAllocator() noexcept;
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,VkDebugUtilsMessageTypeFlagsEXT,const VkDebugUtilsMessengerCallbackDataEXT*,void*) noexcept;
    void collapse() noexcept;

    Window*             m_pWindow;
    VkInstance          m_instance;
    VkPhysicalDevice    m_physicalDevice;
    uint32_t            m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice            m_device;
    VkQueue             m_graphicsQueue, m_presentQueue;
    VkCommandPool       m_commandPool;
    VmaAllocator        m_allocator;
};

} /* namespace bl */
#pragma once

#include "Core/Export.h"
#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl 
{

/** 
* \brief Creates and manages a vulkan swapchain instance.
* 
* This class manages the Vulkan swapchain from start of litefime 
* to the end of its lifetime. It will create a surface for the specified 
* window. It will recreate the swapchain when acquiring an image fails to 
* create. Renderers can get the images and do their own processing.
* 
* Requires a window an render device in order to be created.
* 
* You can create multiple swapchains for multiple windows if needed.
* 
* \since BloodLust 1.0.0
* 
* \sa Window
* \sa RenderDevice
*
*/
class BLOODLUST_API Swapchain 
{
public:

    /** \brief The default requested format for the swapchain. */
    static const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    /** \brief The default requested color space for the swapchain. */
    static const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    
    /** \brief The default requested present mode for the swapchain. */
    static const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    /** \brief Default constructor 
    *
    * Doesn't initialize anything just creates a place holder swapchain. 
    * Checking \ref good() will return false.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Swapchain(Window&, RenderDevice&);
    * 
    */
    Swapchain() noexcept;

    /** \brief Constructs a Vulkan swapchain.
    * 
    * Creates a VkSurfaceKHR using the function defined by the window. Then 
    * uses the surface to build the swapchain creation info. Finally creates 
    * the swapchain using \ref recreate.
    * 
    * After this constructor is used you MUST check if the swapchain was
    * created successfully by using \ref good. If you don't the swapchain
    * will fail when using \ref acquireNext. This is purely for safety reasons 
    * and not to be a pain and since this project likes to use noexcept.
    * 
    * 
    * \param window The window the swapchain is built on.
    * \param renderDevice Uses the vulkan device to create the swapchain.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa good
    * \sa recreate
    * \sa Window::createVulkanSurface 
    * 
    */
    Swapchain(Window& window, RenderDevice& renderDevice) noexcept;

    /** \brief Destructor
    * 
    * Destroys the whole swapchain including the surface.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Swapchain::Swapchain
    * 
    */
    ~Swapchain() noexcept;
    
    /** \brief Move operator
    * 
    * Moves the values from rhs to this and clears the values in rhs.
    * 
    * \param rhs A swapchain value from the right side of the equals.
    * 
    * \return *this
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Swapchain()
    * 
    */
    Swapchain& operator=(Swapchain&& rhs) noexcept;

    /** \brief Checks if the swapchain creation was successful.
    * 
    * This function checks if the value of the swapchain for a VK_NULL_HANDLE.
    * If the swapchain was not created properly it should be a null handle and
    * you will need to recreate it before using this swapchain.
    * 
    * 
    * \return true The swapchain was created successfully.
    * \return false The swapchain was not created successfully.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    *
    */
    [[nodiscard]] bool good() const noexcept;

    /** \brief Gets the internal vulkan surface.
    * 
    * Gets the surface created by the class at construction. If the surface
    * creation was not good this function will return VK_NULL_HANDLE.
    * 
    * \return The surface or VK_NULL_HANDLE.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Swapchain()
    *
    */
    [[nodiscard]] VkSurfaceKHR getSurface() const noexcept;

    /** \brief Gets the internal swapchain image color format.
    * 
    * Gets the current color format that the swapchain used to create the 
    * images with. The format retrieved choosen by \ref recreate. If the
    * format could not be found or the swapchain was not created this function
    * will return VK_FORMAT_UNKNOWN.
    * 
    * \return The format or VK_FORMAT_UNKNOWN.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * 
    */
    [[nodiscard]] VkFormat getFormat() const noexcept;

    /** \brief Gets the extent that the swapchain is currently using.
    * 
    * Gets the extent choosen by the swapchain and is currently using. This 
    * value will only change when the swapchain is recreated and isn't
    * directly tied to the windows size.
    * 
    * \return The current swapchain extent.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * 
    */
    [[nodiscard]] Extent2D getExtent() const noexcept;

    /** \brief Gets the amount of images used by the swapchain.
    * 
    * This function returns the image count that the swapchain is currently
    * using. It only changes when \ref recreate is called. Getting the size of
    * the value returned from \ref getImages is functionally the same.
    *
    * \return Returns the image choosen image count by the swapchain when using
    * \ref recreate.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * \sa getImages
    * 
    */
    [[nodiscard]] uint32_t getImageCount() const noexcept;
    
    /** \brief Gets the vulkan images that are being used in the swapchain.
     * 
     * This function retrieves all the images created by the swapchain. When
     * the swapchain was not created properly returns a vector that has no
     * elements. Will be regenerated when \ref recreate is called.
     * 
     * \return The images that the swapchain is using. On not \ref good the
     * a value of 0 is returned.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa recreate
     * \sa getImageCount
     * 
     */
    [[nodiscard]] const std::vector<VkImage>& getImages() const noexcept;

    /** \brief Gets the index of the next image in the swapchain.
     * 
     * Gets the next image index that the swapchain using to present to the
     * window. These images can be retrieved through \ref getSwapchainImages.
     * A renderer can then use the image index to present the specified image.
     * 
     * \param semaphore Semaphore to signal when the swapchain image gets 
     * acquired.
     * \param fence Fence to trigger when the image is done being acquired.
     * \param imageIndex The current image index we are using. 
     * 
     * \return true The image acquired and swapchain successfully recreated.
     * \return false The image was not acquired or swapchain failed to recreate.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa getImages
     * \sa getImageCount
     * 
     */
    [[nodiscard]] bool acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& requiresRecreation) noexcept;

    /** \brief Recreates the swapchain on demand.
     * 
     * Destroys the swapchain if it is currently created. Will regenerate
     * all the required info including the extent, format, and images. After
     * this function is called the renderer may need to recreate its 
     * attachments because they will need to be resized.  
     * 
     * \return true The swapchain was successfully recreated.
     * \return false The swapchain could not be recreated.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa getFormat
     * \sa getExtent
     * \sa getImages
     * \sa getSwapchain
     * 
     */
    [[nodiscard]] bool recreate() noexcept;

    /** \brief Gets the internal vulkan swapchain.
    * 
    * Gets the swapchain that was created by \ref recreate. If the swapchain
    * was improperly created this function will return VK_NULL_HANDLE.
    * 
    * \return The swapchain or VK_NULL_HANDLE.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * 
    */
    [[nodiscard]] VkSwapchainKHR getSwapchain() const noexcept;

private:
    
    /**
     * \brief Checks if the created surface supports the physical device.
     * 
     * Uses the physical device from the render device at construction to
     * check if the surface is supported. If the surface isn't checked vulkan
     * will throw a validation error.
     * 
     * \return true The vulkan surface is supported by the physical device.
     * \return false The vulkan surface is not supported by the physical device.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa recreate
     * 
     */
    [[nodiscard]] bool isSurfaceSupported() noexcept;

    /** \brief Determines the amount of images the swapchain will use.
     * 
     * In order to create a swapchain it must know how many images to create.
     * Vulkan sets boundaries and this function determines a good in between.
     * 
     * \return true The image count was choosen successfully.
     * \return false The image could could not be choosen.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa recreate
     * 
     */
    [[nodiscard]] bool chooseImageCount() noexcept;

    /** \brief Determines the color format used by swapchain images.
     * 
     * The swapchain will automatically create some images determined by
     * \ref chooseImageCount, these images require a format that this function
     * will find.
     * 
     * \return true The image format was chosen successfully.
     * \return false The image format was not chosen successfully.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa recreate
     * 
     */
    [[nodiscard]] bool chooseFormat() noexcept;

    /**
     * \brief Determines the present mode that the swapchain is created with.
     * 
     * A swapchain requires a present mode in order to be created. The present
     * modes ensure that images presented at the right times. The default
     * present mode is checked first, otherwise this function uses the FIFO 
     * mode. 
     * 
     * \return true Successfully chose a present mode usable by the swapchain. 
     * \return false No present mode was found.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa recreate 
     * 
     */
    bool choosePresentMode(VkPresentModeKHR requestedPresentMode = DEFAULT_PRESENT_MODE) noexcept;

    /** \brief Chooses the image extent of the swapchain.
     * 
     * Determines the width and height of the swapchain using information from
     * the window and vulkan swapchain capabilities. The extent may not be 
     * directly tied to the window size because they update at different times.
     * 
     * All information built on top of the swapchain should use this extent if 
     * it needs to use it.
     * 
     * \return true The extent was properly choosen. 
     * \return false The extent could not be properly choosen.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa recreate.
     * 
     */
    bool chooseExtent() noexcept;
    
    /** \brief Gets the images from the swapchain.
     * 
     * After the swapchain is created this function can retrieve them and store
     * them for later. The \ref getImages function will return them.
     * 
     * \return true The images were obtained successfully.
     * \return false The images could not be obtained.
     * 
     * \since BloodLust 1.0.0
     * 
     * \sa getImages
     * \sa recreate
     * 
     */
    bool obtainImages() noexcept;

    /**
     * @brief Destroys the swapchain for \ref recreate.
     * 
     * Destroys only the swapchain.
     * 
     */
    void collapse() noexcept;

    /** \brief Window provided at construction, used to create a surface and 
        its extent. */
    Window* m_pWindow;

    /** \brief Render device provided at construction, used for its VkDevice. */
    RenderDevice* m_pRenderDevice;

    /** \brief Surface created by the constructor to facilitate swapchain 
        creation. */
    VkSurfaceKHR m_surface;

    /** \brief Amount of images the swapchain uses. \sa chooseImageCount */
    uint32_t m_imageCount;

    /** \brief The surface format of the swapchain. \sa chooseFormat */
    VkSurfaceFormatKHR m_surfaceFormat;

    /** \brief Present mode of the swapchain. \sa choosePresentMode */
    VkPresentModeKHR m_presentMode;

    /** \brief Extent of the swapchain. \sa chooseExtent */
    VkExtent2D m_extent;

    /** \brief The swapchain. */
    VkSwapchainKHR m_swapchain;

    /** \brief Images created by the swapchain. \sa obtainImages */
    std::vector<VkImage> m_swapImages;

#ifdef BLOODLUST_DEBUG
    bool m_checkedGood;
#endif

};

} /* namespace bl */
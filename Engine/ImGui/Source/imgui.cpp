#include "ImGui.hpp"

#include "Core/Log.hpp"
#include "Core/Macros.hpp"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"

namespace bl
{

	Window* ImmediateModeGUI::_pWindow;
	InputSystem* ImmediateModeGUI::_pInputSystem;
	RenderDevice* ImmediateModeGUI::_pRenderDevice;
	Renderer* ImmediateModeGUI::_pRenderer;
	VkDescriptorPool ImmediateModeGUI::_descriptorPool;

	bool ImmediateModeGUI::init(Window& window, InputSystem& inputSystem, RenderDevice& renderDevice, Renderer& renderer) noexcept
	{
		_pWindow = &window;
		_pInputSystem = &inputSystem;
		_pRenderDevice = &renderDevice;
		_pRenderer = &renderer;
		
		const VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		const VkDescriptorPoolCreateInfo poolInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = 1000,
			.poolSizeCount = std::size(poolSizes),
			.pPoolSizes = poolSizes,
		};

		if (vkCreateDescriptorPool(_pRenderDevice->getDevice(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
		{
			Logger::Error("Could not create the ImGui descriptor pool!");
			return false;
		}

		::ImGui::CreateContext();

		BL_CHECK(ImGui_ImplSDL2_InitForVulkan(window.getHandle()), "Could not init ImGui Impl SDL2!")

		// Initialize ImGui for Vulkan, pass created objects.
		ImGui_ImplVulkan_InitInfo initInfo{
			.Instance = _pRenderDevice->getInstance(),
			.PhysicalDevice = _pRenderDevice->getPhysicalDevice(),
			.Device = _pRenderDevice->getDevice(),
			.Queue = _pRenderDevice->getGraphicsQueue(),
			.DescriptorPool = _descriptorPool,
			.MinImageCount = 3,
			.ImageCount = 3,
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
		};

		BL_CHECK(ImGui_ImplVulkan_Init(&initInfo, renderer.getRenderPass()), "Could not init ImGui Impl Vulkan!")

		// Upload the Vulkan ImGui font textures.
		renderDevice.immediateSubmit([&](VkCommandBuffer cmd) {
				ImGui_ImplVulkan_CreateFontsTexture(cmd);
			});

		// Clear the font texture data from host memory.
		ImGui_ImplVulkan_DestroyFontUploadObjects();

		inputSystem.addHookCallback([&](const SDL_Event* pEvent) {
				ImGui_ImplSDL2_ProcessEvent(pEvent);
			});

		return true;
	}

	void ImmediateModeGUI::shutdown() noexcept
	{
		vkDeviceWaitIdle(_pRenderDevice->getDevice());
		
		vkDestroyDescriptorPool(_pRenderDevice->getDevice(), _descriptorPool, nullptr);

		ImGui_ImplVulkan_Shutdown();
	}

	void ImmediateModeGUI::beginFrame() noexcept
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();

		ImGui::NewFrame();
	}

	void ImmediateModeGUI::endFrame(VkCommandBuffer cmd) noexcept
	{
		ImGui::Render();

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	}

}
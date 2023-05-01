#include "ImGui/ImGui.hpp"

#include "Core/Log.hpp"
#include "Core/Macros.hpp"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"


std::shared_ptr<const blWindow> blImGui::_window;
std::shared_ptr<blInputSystem> blImGui::_inputSystem;
std::shared_ptr<const blRenderDevice> blImGui::_renderDevice;
std::shared_ptr<const blRenderPass> blImGui::_renderPass;
vk::DescriptorPool blImGui::_descriptorPool;
blInputHookCallback blImGui::_hookCallback;

void blImGui::init(
	std::shared_ptr<const blWindow> window,
	std::shared_ptr<const blRenderDevice> renderDevice,
	std::shared_ptr<const blRenderPass> renderPass)
{
	_window = window;
	_renderDevice = renderDevice;
	_renderPass = renderPass;
	_inputSystem = blInputSystem::getInstance();

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

	_descriptorPool = _renderDevice->getDevice().createDescriptorPool(poolInfo);

	ImGui::CreateContext();
	
	ImGui_ImplSDL2_InitForVulkan(window->getHandle());

	// Initialize ImGui for Vulkan, pass created objects.
	ImGui_ImplVulkan_InitInfo initInfo
	{
		(VkInstance)_renderDevice->getInstance(),
		(VkPhysicalDevice)_renderDevice->getPhysicalDevice(),
		(VkDevice)_renderDevice->getDevice(),
		_renderDevice->getGraphicsFamilyIndex(),
		(VkQueue)_renderDevice->getGraphicsQueue(),
		VK_NULL_HANDLE,
		_descriptorPool,
		0,
		3,
		3,
		VK_SAMPLE_COUNT_1_BIT,
	};

	if (not ImGui_ImplVulkan_Init(&initInfo, _renderPass->getRenderPass()))
	{
		throw std::runtime_error("Could not initialize ImGui Vulkan!");
	}

	// Upload the Vulkan ImGui font textures.
	_renderDevice->immediateSubmit([&](VkCommandBuffer cmd) {
			ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});

	// Clear the font texture data from host memory.
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	_hookCallback = _inputSystem->addHook([&](const SDL_Event* pEvent) {
			ImGui_ImplSDL2_ProcessEvent(pEvent);
		});
}

void blImGui::shutdown() noexcept
{
	_renderDevice->waitForDevice();
	_inputSystem->removeHook(_hookCallback);

	_renderDevice->getDevice()
		.destroyDescriptorPool(_descriptorPool);

	ImGui_ImplVulkan_Shutdown();
}

void blImGui::beginFrame() noexcept
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();
}

void blImGui::endFrame(VkCommandBuffer cmd) noexcept
{
	ImGui::Render();

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}
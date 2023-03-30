#include "Core/Macros.hpp"
#include "Core/Log.hpp"

#include "imgui_impl_bloodlust.h"
#include "imgui_impl_sdl2.h"


static VkDevice device{};
static VkDescriptorPool imguiPool{};

bool ImGui_ImplBloodLust_Init(bl::Window& window, bl::InputSystem& inputSystem, bl::RenderDevice& renderDevice, bl::Renderer& renderer)
{
	device = renderDevice.getDevice();

	VkDescriptorPoolSize poolSizes[] =
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

	VkDescriptorPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 1000,
		.poolSizeCount = std::size(poolSizes),
		.pPoolSizes = poolSizes,
	};

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &imguiPool) != VK_SUCCESS)
	{
		bl::Logger::Error("Could not create the ImGui descriptor pool!");
		return false;
	}

	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForVulkan(window.getHandle());

	// Initialize imgui for vulkan
	ImGui_ImplVulkan_InitInfo initInfo{
		.Instance = renderDevice.getInstance(),
		.PhysicalDevice = renderDevice.getPhysicalDevice(),
		.Device = renderDevice.getDevice(),
		.Queue = renderDevice.getGraphicsQueue(),
		.DescriptorPool = imguiPool,
		.MinImageCount = 3,
		.ImageCount = 3,
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
	};

	ImGui_ImplVulkan_Init(&initInfo, renderer.getRenderPass());

	// Upload the vulkan imgui font textures
	renderDevice.immediateSubmit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});

	// Clear the font texture data from host memory
	ImGui_ImplVulkan_DestroyFontUploadObjects();


	inputSystem.addHookCallback([&](const SDL_Event* pEvent) {
				ImGui_ImplSDL2_ProcessEvent(pEvent);
			});

}

void ImGui_ImplBloodLust_Shutdown()
{
	vkDestroyDescriptorPool(device, imguiPool, nullptr);
	ImGui_ImplVulkan_Shutdown();
}

void ImGui_ImplBloodLust_NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();


}
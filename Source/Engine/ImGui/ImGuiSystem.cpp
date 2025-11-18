///////////////////////////////
// Headers
///////////////////////////////

#include "ImGui/ImGuiSystem.h"

#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanWindow.h"
#include "Graphics/VulkanConversions.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_vulkan.h"

#include "Engine/Engine.h"

namespace bl 
{

ImGuiSystem::ImGuiSystem(Engine& engine, VulkanWindow* window, Renderer* renderer)
    : System(engine)
    , _window(window)
    , _renderer(renderer)
{
    Init();
}

ImGuiSystem::~ImGuiSystem()
{
    Unload();
}

std::shared_ptr<Resource> ImGuiSystem::ConstructResource(ResourceSystem*, std::size_t, const std::filesystem::path&)
{
    throw std::runtime_error("ImGuiSystem does not handle any resources.");
}

void ImGuiSystem::ApplyStyle()
{
    float scale = SDL_GetWindowDisplayScale(_window->Get());

    Print::Debug("SDL Display scale: {}", scale);

    ImFontConfig cfg;
    cfg.SizePixels = 13 * scale;
    ImGui::GetIO().Fonts->AddFontDefault(&cfg);
    // ImGui::GetIO().FontGlobalScale = scale;
    ImGui::GetIO().DisplayFramebufferScale = ImVec2{scale, scale};

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    // style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    // style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 2.3f;
}

void ImGuiSystem::Process(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void ImGuiSystem::Init()
{
    auto graphics = GetEngine().GetGraphics();
    auto device = graphics->GetDevice();
    auto instance = graphics->GetInstance();
    auto physicalDevice = graphics->GetPhysicalDevice();
    auto window = _window;

    auto [pass, subpass] = _renderer->GetRenderPass(RenderPassType::eUI);

    device->WaitForDevice();

    std::array poolSizes = { 
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 }, 
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 }, 
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000;
    poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();

    VK_CHECK(vkCreateDescriptorPool(device->Get(), &poolInfo, nullptr, &_descriptorPool))

    ImGui::CreateContext();

    VkInstance inst = instance->Get();
    ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_3, [](const char *function_name, void *vulkan_instance) {
        return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance *>(vulkan_instance)), function_name);
    }, &inst);

    ImGui_ImplSDL3_InitForVulkan(window->Get());

    // Initialize ImGui for Vulkan, pass created objects.
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = instance->Get();
    initInfo.PhysicalDevice = physicalDevice->Get();
    initInfo.Device = device->Get();
    initInfo.QueueFamily = device->GetGraphicsFamilyIndex();
    initInfo.Queue = device->GetGraphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = _descriptorPool;
    initInfo.RenderPass = pass;
    initInfo.Subpass = subpass;
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = 3;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = nullptr;

    if (!ImGui_ImplVulkan_Init(&initInfo)) {
        throw std::runtime_error("Could not initialize ImGui Vulkan!");
    }

    // Upload the Vulkan ImGui font textures.
    ImFontConfig cfg;
    cfg.OversampleH = 3;

    auto io = ImGui::GetIO();

    float scale = SDL_GetWindowDisplayScale(_window->Get());
    ImFont* pFont = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Regular.ttf", 18.0f * scale);
    io.FontDefault = pFont;

    ImGui_ImplVulkan_CreateFontsTexture();
    ImGui_ImplVulkan_DestroyFontsTexture();

    ApplyStyle();
}

void ImGuiSystem::Unload()
{
    auto graphics = GetEngine().GetGraphics();
    auto device = graphics->GetDevice();

    device->WaitForDevice();
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplVulkan_Shutdown();

    vkDestroyDescriptorPool(device->Get(), _descriptorPool, nullptr);
}

void ImGuiSystem::BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiSystem::EndFrame(VkCommandBuffer cmd)
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void ImGuiSystem::DrawDebug()
{
    auto gs = GetEngine().GetGraphics();
    auto& counter = GetEngine().GetFrameCounter();
    auto window = gs->GetWindow();
    auto as = GetEngine().GetAudio();

    ImGui::Begin("BlueMetal Debug Info");

    if (ImGui::CollapsingHeader("Version"))
    {
        ImGui::Text("Compiled " __DATE__ " " __TIME__);
        ImGui::Text("Compiler ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", bl::compiler.c_str());

        ImGui::Text("Bluemetal");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", bl::to_string(bl::engineVersion).c_str());

        ImGui::Text("SDL");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, BL_STRINGIFY(SDL_MAJOR_VERSION) "." BL_STRINGIFY(SDL_MINOR_VERSION));

        ImGui::Text("Vulkan Header");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%d", VK_HEADER_VERSION);

        ImGui::Text("Vulkan Version");
        ImGui::SameLine();
        auto instanceVersion = volkGetInstanceVersion();
        ImGui::TextColored(ImVec4{0.7f, 0.1f, 0.1f, 1.0f}, "%d.%d.%d", VK_VERSION_MAJOR(instanceVersion), VK_VERSION_MINOR(instanceVersion), VK_VERSION_PATCH(instanceVersion));

        ImGui::Text("ImGui");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", ImGui::GetVersion());
    }

    if (ImGui::CollapsingHeader("Input"))
    {
        ImGui::Text("Window Focused: %s", window->GetFocused() ? "Yes" : "No");
        ImGui::Text("ImGui Wants Mouse: %s", ImGui::GetIO().WantCaptureMouse ? "Yes" : "No");
    }


    if (ImGui::CollapsingHeader("Graphics")) {

        ImGui::Text("Graphics Device: %s", gs->GetPhysicalDevice()->GetDeviceName().c_str()); 
        ImGui::SameLine();
        ImGui::HelpMarker("Your graphics card.");
        ImGui::Text("Graphics Vendor: %s", gs->GetPhysicalDevice()->GetVendorName().c_str()); 
        ImGui::Text("F/S: %d", counter.GetFramesPerSecond()); 
        ImGui::Text("MS/F: %.2f", counter.GetMillisecondsPerFrame()); 
        ImGui::Text("Average F/S (Over 10 Seconds): %.1f", counter.GetAverageFramesPerSecond(10));
        ImGui::Text("Average MS/F (Over 144 Frames): %.2f", counter.GetAverageMillisecondsPerFrame(144)); 
        ImGui::Text("Presenting: (%s | %s, %s)", bl::ToString(window->GetSwapchain()->GetPresentMode()).data(), bl::ToString(window->GetSwapchain()->GetSurfaceFormat().format).data(), bl::ToString(window->GetSwapchain()->GetSurfaceFormat().colorSpace).data()); 
        // ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));

        if (ImGui::TreeNode("Physical Devices")) {
            auto physicalDevices = gs->GetInstance()->GetPhysicalDevices();

            for (size_t i = 0; i < physicalDevices.size(); i++) {
                auto& physicalDevice = physicalDevices[i];

                if (ImGui::TreeNode((void*)(intptr_t)i, "%s", physicalDevice->GetDeviceName().c_str())) {
                    const char* deviceType = "";
                    switch (physicalDevice->GetType()) {
                    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "Integrated"; break;
                    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: deviceType = "Discrete"; break;
                    case VK_PHYSICAL_DEVICE_TYPE_CPU: deviceType = "CPU"; break;
                    default: deviceType = "Unknown"; break;
                    }

                    ImGui::SameLine();
                    ImGui::TextColored({0.2f, 0.8f, 0.4f, 1.0f}, "%s", deviceType);

                    if (physicalDevice == gs->GetPhysicalDevice()) {
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4{0.2f, 0.5f, 0.8f, 1.0f}, "Current");
                    }

                    if (ImGui::TreeNode("Present Modes")) {
                        for (VkPresentModeKHR mode : physicalDevice->GetPresentModes(window))
                            ImGui::Text("%s", bl::ToString(mode).data());

                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Audio")) {
        ImGui::Text("Audio Driver: %s", as->GetDriverName().c_str());
        ImGui::Text("Num Channels: %d", as->GetNumChannelsPlaying());
    }

    ImGui::End();
}


} // namespace bl

namespace ImGui
{

void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

} // namespace ImGui
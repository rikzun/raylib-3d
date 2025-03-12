#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0

#include <memory>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <print>
#include <vulkan/vulkan_raii.hpp>
#include <cmath>
#include <optional>

class SDLException final : public std::runtime_error {
public:
    explicit SDLException(const std::string &message) : std::runtime_error(
        std::format("{}: {}", message, SDL_GetError())) {}
};

constexpr auto VULKAN_VERSION{vk::makeApiVersion(0, 1, 4, 0)};

struct Frame {
    vk::raii::CommandBuffer commandBuffer;
    vk::raii::Semaphore imageAvailableSemaphore;
    vk::raii::Semaphore renderFinishedSemaphore;
    vk::raii::Fence fence;
};

constexpr uint32_t IN_FLIGHT_FRAME_COUNT{2};

class App {
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window{nullptr, SDL_DestroyWindow};
    bool running{true};

    std::optional<vk::raii::Context> context{};
    std::optional<vk::raii::Instance> instance{};
    std::optional<vk::raii::SurfaceKHR> surface{};
    std::optional<vk::raii::PhysicalDevice> physicalDevice{};
    uint32_t graphicsQueueFamilyIndex{};
    std::optional<vk::raii::Device> device{};
    std::optional<vk::raii::Queue> graphicsQueue{};

    std::optional<vk::raii::CommandPool> commandPool{};
    std::array<std::optional<Frame>, IN_FLIGHT_FRAME_COUNT> frames{};
    uint32_t frameIndex{};

    std::optional<vk::raii::SwapchainKHR> swapchain{};
    std::vector<vk::Image> swapchainImages{};
    vk::Extent2D swapchainExtent{};
    vk::Format swapchainImageFormat{vk::Format::eB8G8R8A8Srgb};
    uint32_t currentSwapchainImageIndex{};

public:
    App() {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw SDLException("Failed to initialize SDL");
        if (!SDL_Vulkan_LoadLibrary(nullptr))
            throw SDLException("Failed to load Vulkan library");
        window.reset(SDL_CreateWindow("Codotaku", 800, 600,
                                      SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN));
        if (!window)
            throw SDLException("Failed to create window");
        auto vkGetInstanceProcAddr{reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr())};
        context.emplace(vkGetInstanceProcAddr);
        auto const vulkanVersion{context->enumerateInstanceVersion()};
        // std::println("Vulkan {}.{}", VK_API_VERSION_MAJOR(vulkanVersion), VK_API_VERSION_MINOR(vulkanVersion));
    }

    ~App() {
        device->waitIdle();
        SDL_Quit();
    }

    void Init() {
        InitInstance();
        InitSurface();
        PickPhysicalDevice();
        InitDevice();
        InitCommandPool();
        InitFrames();
        RecreateSwapchain();
    }

    void Run() {
        SDL_ShowWindow(window.get());
        while (running) {
            HandleEvents();
            Render();
        }
    }

private:
    struct ImageLayout {
        vk::ImageLayout imageLayout{};
        vk::PipelineStageFlags2 stageMask{};
        vk::AccessFlags2 accessMask{};
        uint32_t queueFamilyIndex{VK_QUEUE_FAMILY_IGNORED};
    };

    static void TransitionImageLayout(vk::raii::CommandBuffer const &commandBuffer, vk::Image const &image,
                                      ImageLayout const &oldLayout, ImageLayout const &newLayout) {
        vk::ImageMemoryBarrier2 const barrier{
            oldLayout.stageMask,
            oldLayout.accessMask,
            newLayout.stageMask,
            newLayout.accessMask,
            oldLayout.imageLayout,
            newLayout.imageLayout,
            oldLayout.queueFamilyIndex,
            newLayout.queueFamilyIndex,
            image, vk::ImageSubresourceRange{
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1
            }
        };
        vk::DependencyInfo dependencyInfo{};
        dependencyInfo.setImageMemoryBarriers(barrier);
        commandBuffer.pipelineBarrier2(dependencyInfo);
    }

    static void RecordCommandBuffer(vk::raii::CommandBuffer const &commandBuffer, vk::Image const &swapchainImage) {
        commandBuffer.reset();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);

        auto const t{static_cast<double>(SDL_GetTicks()) * 0.001};

        vk::ClearColorValue const color{
            std::array{static_cast<float>(std::sin(t * 5.0) * 0.5 + 0.5), 0.0f, 0.0f, 1.0f}
        };

        TransitionImageLayout(commandBuffer, swapchainImage,
                              ImageLayout{
                                  vk::ImageLayout::eUndefined,
                                  vk::PipelineStageFlagBits2::eTransfer,
                                  vk::AccessFlagBits2KHR::eMemoryRead,
                              },
                              ImageLayout{
                                  vk::ImageLayout::eTransferDstOptimal,
                                  vk::PipelineStageFlagBits2::eTransfer,
                                  vk::AccessFlagBits2KHR::eTransferWrite,
                              });

        commandBuffer.clearColorImage(swapchainImage, vk::ImageLayout::eTransferDstOptimal, color,
                                      vk::ImageSubresourceRange{
                                          vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1
                                      });

        TransitionImageLayout(commandBuffer, swapchainImage,
                              ImageLayout{
                                  vk::ImageLayout::eTransferDstOptimal,
                                  vk::PipelineStageFlagBits2::eTransfer,
                                  vk::AccessFlagBits2KHR::eTransferWrite,
                              },
                              ImageLayout{
                                  vk::ImageLayout::ePresentSrcKHR,
                                  vk::PipelineStageFlagBits2::eTransfer,
                                  vk::AccessFlagBits2KHR::eMemoryRead,
                              });

        commandBuffer.end();
    }

    void BeginFrame(Frame const &frame) {
        auto _ = device->waitForFences(*frame.fence, VK_TRUE, UINT64_MAX);
        device->resetFences(*frame.fence);

        auto [acquireResult, imageIndex] = swapchain->
            acquireNextImage(UINT64_MAX, *frame.imageAvailableSemaphore, nullptr);
        currentSwapchainImageIndex = imageIndex;
    }

    void EndFrame(Frame const &frame) {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setSwapchains(**swapchain);
        presentInfo.setImageIndices(currentSwapchainImageIndex);
        presentInfo.setWaitSemaphores(*frame.renderFinishedSemaphore);
        auto _ = graphicsQueue->presentKHR(presentInfo);

        frameIndex = (frameIndex + 1) % IN_FLIGHT_FRAME_COUNT;
    }

    void SubmitCommandBuffer(Frame const &frame) const {
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(*frame.commandBuffer);
        submitInfo.setWaitSemaphores(*frame.imageAvailableSemaphore);
        submitInfo.setSignalSemaphores(*frame.renderFinishedSemaphore);
        constexpr vk::PipelineStageFlags waitStage{vk::PipelineStageFlagBits::eTransfer};
        submitInfo.setWaitDstStageMask(waitStage);
        graphicsQueue->submit(submitInfo, frame.fence);
    }

    void Render() {
        auto const &frame{*frames[frameIndex]};
        BeginFrame(frame);
        RecordCommandBuffer(frame.commandBuffer, swapchainImages[currentSwapchainImageIndex]);
        SubmitCommandBuffer(frame);
        EndFrame(frame);
    }

    void HandleEvents() {
        for (SDL_Event event; SDL_PollEvent(&event);)
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    RecreateSwapchain();
                    break;
                default: break;
            }
    }

    void RecreateSwapchain() {
        vk::SurfaceCapabilitiesKHR const surfaceCapabilities{physicalDevice->getSurfaceCapabilitiesKHR(*surface)};
        swapchainExtent = surfaceCapabilities.currentExtent;

        vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.surface = *surface;
        swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
        swapchainCreateInfo.imageFormat = swapchainImageFormat;
        swapchainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        swapchainCreateInfo.imageExtent = swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransferDst;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainCreateInfo.presentMode = vk::PresentModeKHR::eMailbox;
        swapchainCreateInfo.clipped = true;

        // todo: figure out why old swapchain handle is invalid
        // if (swapchain.has_value()) swapchainCreateInfo.oldSwapchain = **swapchain;

        swapchain.emplace(*device, swapchainCreateInfo);
        swapchainImages = swapchain->getImages();
    }

    void InitFrames() {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.commandPool = *commandPool;
        commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        commandBufferAllocateInfo.commandBufferCount = IN_FLIGHT_FRAME_COUNT;
        auto commandBuffers{device->allocateCommandBuffers(commandBufferAllocateInfo)};

        for (size_t i = 0; i < IN_FLIGHT_FRAME_COUNT; i++)
            frames[i].emplace(
                std::move(commandBuffers[i]),
                vk::raii::Semaphore{*device, vk::SemaphoreCreateInfo{}},
                vk::raii::Semaphore{*device, vk::SemaphoreCreateInfo{}},
                vk::raii::Fence{*device, vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled}}
            );
    }

    void InitCommandPool() {
        vk::CommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        commandPool.emplace(*device, commandPoolCreateInfo);
    }

    void InitDevice() {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        std::array queuePriorities{1.0f};
        queueCreateInfo.setQueuePriorities(queuePriorities);

        vk::DeviceCreateInfo deviceCreateInfo{};
        std::array queueCreateInfos{queueCreateInfo};
        deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);
        std::array<const char* const, 1> enabledExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        deviceCreateInfo.setPEnabledExtensionNames(enabledExtensions);

        vk::PhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.synchronization2 = true;

        vk::StructureChain chain{
            deviceCreateInfo, vulkan13Features
        };

        device.emplace(*physicalDevice, chain.get<vk::DeviceCreateInfo>());

        graphicsQueue.emplace(*device, graphicsQueueFamilyIndex, 0);
    }

    void PickPhysicalDevice() {
        auto const physicalDevices{instance->enumeratePhysicalDevices()};
        if (physicalDevices.empty())
            throw std::runtime_error("No Vulkan devices found");
        physicalDevice.emplace(*instance, *physicalDevices.front());
        auto const rawDeviceName{physicalDevice->getProperties().deviceName};
        std::string deviceName(rawDeviceName.data(), std::strlen(rawDeviceName));
        // std::println("{}", deviceName);
        graphicsQueueFamilyIndex = 0;
    }

    void InitInstance() {
        vk::ApplicationInfo applicationInfo{};
        applicationInfo.apiVersion = VULKAN_VERSION;

        vk::InstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        uint32_t extensionCount;
        instanceCreateInfo.ppEnabledExtensionNames = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        instanceCreateInfo.enabledExtensionCount = extensionCount;

        instance.emplace(*context, instanceCreateInfo);
    }

    void InitSurface() {
        VkSurfaceKHR raw_surface;
        if (!SDL_Vulkan_CreateSurface(window.get(), **instance, nullptr, &raw_surface))
            throw SDLException("Failed to create Vulkan surface");
        surface.emplace(*instance, raw_surface);
    }
};

int main() {
    try {
        App app{};
        app.Init();
        app.Run();
    }
    catch (const SDLException &e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: %s", e.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
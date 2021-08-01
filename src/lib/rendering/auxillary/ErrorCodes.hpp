/* ERROR CODES.hpp
 *   by Lut99
 *
 * Created:
 *   16/04/2021, 17:51:33
 * Last edited:
 *   8/1/2021, 4:06:03 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Static map of VkResult values to human-readable strings.
**/

#ifndef COMPUTE_ERROR_CODES_HPP
#define COMPUTE_ERROR_CODES_HPP

#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>

namespace Rasterizer::Rendering {
    /* Static map of VkResult error codes to human-readable strings. */
    static std::unordered_map<VkResult, std::string> vk_error_map({
        { VK_SUCCESS, "Command successfully completed." },
        { VK_NOT_READY, "A fence or query has not yet completed." },
        { VK_TIMEOUT, "A wait operation has not completed in the specified time." },
        { VK_EVENT_SET, "An event is signaled." },
        { VK_EVENT_RESET, "An event is unsignaled." },
        { VK_INCOMPLETE, "A return array was too small for the result." },
        { VK_SUBOPTIMAL_KHR, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully." },
        #ifdef VK_THREAD_IDLE_KHR
        { VK_THREAD_IDLE_KHR, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call." },
        #endif
        #ifdef VK_THREAD_DONE_KHR
        { VK_THREAD_DONE_KHR, "A deferred operation is not complete but there is no work remaining to assign to additional threads." },
        #endif
        #ifdef VK_OPERATION_DEFERRED_KHR
        { VK_OPERATION_DEFERRED_KHR, "A deferred operation was requested and at least some of the work was deferred." },
        #endif
        #ifdef VK_OPERATION_NOT_DEFERRED_KHR
        { VK_OPERATION_NOT_DEFERRED_KHR, "A deferred operation was requested and no operations were deferred." },
        #endif
        #ifdef VK_PIPELINE_COMPILE_REQUIRED_EXT
        { VK_PIPELINE_COMPILE_REQUIRED_EXT, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed." },
        #endif
        { VK_ERROR_OUT_OF_HOST_MEMORY, "A host memory allocation has failed." },
        { VK_ERROR_OUT_OF_DEVICE_MEMORY, "A device memory allocation has failed." },
        { VK_ERROR_INITIALIZATION_FAILED, "Initialization of an object could not be completed for implementation-specific reasons." },
        { VK_ERROR_DEVICE_LOST, "The logical or physical device has been lost." },
        { VK_ERROR_MEMORY_MAP_FAILED, "Mapping of a memory object has failed." },
        { VK_ERROR_LAYER_NOT_PRESENT, "A requested layer is not present or could not be loaded." },
        { VK_ERROR_EXTENSION_NOT_PRESENT, "A requested extension is not supported." },
        { VK_ERROR_FEATURE_NOT_PRESENT, "A requested feature is not supported." },
        { VK_ERROR_INCOMPATIBLE_DRIVER, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons." },
        { VK_ERROR_TOO_MANY_OBJECTS, "Too many objects of the type have already been created." },
        { VK_ERROR_FORMAT_NOT_SUPPORTED, "A requested format is not supported on this device." },
        { VK_ERROR_FRAGMENTED_POOL, "A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation." },
        { VK_ERROR_SURFACE_LOST_KHR, "A surface is no longer available." },
        { VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again." },
        { VK_ERROR_OUT_OF_DATE_KHR, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface." },
        { VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image." },
        { VK_ERROR_INVALID_SHADER_NV, "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled." },
        { VK_ERROR_OUT_OF_POOL_MEMORY, "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead." },
        { VK_ERROR_INVALID_EXTERNAL_HANDLE, "An external handle is not a valid handle of the specified type." },
        #ifdef VK_ERROR_FRAGMENTATION
        { VK_ERROR_FRAGMENTATION, "A descriptor pool creation has failed due to fragmentation." },
        #endif
        { VK_ERROR_INVALID_DEVICE_ADDRESS_EXT, "A buffer creation failed because the requested address is not available." },
        #ifdef VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS
        { VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid." },
        #endif
        #ifdef VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT
        { VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control." },
        #endif
        #ifdef VK_ERROR_UNKNOWN
        { VK_ERROR_UNKNOWN, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred." },
        #endif
        { VK_ERROR_VALIDATION_FAILED_EXT, "Failed to validate." },
        { VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, "Invalid DRM format modifier plane layout." },
        { VK_ERROR_NOT_PERMITTED_EXT, "Not permitted." }
    });
}

#endif

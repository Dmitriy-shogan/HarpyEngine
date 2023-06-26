#pragma once
#ifndef HARPY_THREADING_SEMAPHORE
#define HARPY_THREADING_SEMAPHORE
#include <utilities/harpy_little_error.h>

namespace harpy::nest::threading
{
        //TODO: make harpy threading instead of vulkan threading. Fences, semaphores and barriers.
        class semaphore
        {
            VkSemaphore vk_semaphore{nullptr};
            VkDevice device{nullptr};
            bool is_free{true};
        public:
            semaphore() = delete;
            semaphore(VkDevice device) : device(device)
            {
            
                VkSemaphoreCreateInfo semaphoreInfo{};
                semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vk_semaphore) != VK_SUCCESS) {

                    throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create semaphores!");
                }
            }
            void use()
            {
                is_free = false;
            }
            void free()
            {
                is_free = true;
            }
            bool is_is_free() const
            {
                return is_free;
            }
            ~semaphore()
            {
                vkDestroySemaphore(device, vk_semaphore, nullptr);
            }

            operator VkSemaphore&() 
            {
                return vk_semaphore;
            }
        };
}
#endif //HARPY_THREADING_SEMAPHORE
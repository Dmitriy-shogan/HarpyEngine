#pragma once
#ifndef HARPY_THREADING_SEMAPHORE
#define HARPY_THREADING_SEMAPHORE
#include <utilities/harpy_little_error.h>

namespace harpy::nest::threading
{
        class harpy_semaphore
        {
            VkSemaphore semaphore{};
            VkDevice device{nullptr};
            bool free{true};
        public:
            harpy_semaphore() = delete;
            harpy_semaphore(VkDevice device) : device(device)
            {
            
                VkSemaphoreCreateInfo semaphoreInfo{};
                semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {

                    throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create semaphores!");
                }
            }
            void use()
            {
                free = false;
            }
            void make_free()
            {
                free = true;
            }
            bool is_free() const
            {
                return free;
            }
            ~harpy_semaphore()
            {
                vkDestroySemaphore(device, semaphore, nullptr);
            }

            operator VkSemaphore&() 
            {
                return semaphore;
            }
        };
}
#endif //HARPY_THREADING_SEMAPHORE
#pragma once
#ifndef HARPY_UTILITIES_ALLOCATORS_ABSTRACT
#define HARPY_UTILITIES_ALLOCATORS_ABSTRACT
#include <algorithm>
#include <cstring>

#include <logger/harpy_little_error.h>

namespace harpy::utilities
{
    enum class allocator_strategy
    {
        less_memory_blocks = 1,
        default_memory_blocks = 4,
        more_memory_block = 8,
        even_more_memory_blocks = 16,

        he_first = less_memory_blocks,
        he_is_bm = false,
        he_sum = 4
    };
    
    template<typename T, allocator_strategy strategy = allocator_strategy::default_memory_blocks>  
    class allocator
    {
    public:
        // type definitions
        using value_type = T;
        static inline constexpr auto value_size = sizeof(T);
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;
        static inline constexpr size_type block_size{sizeof(T) < 64 ? 16 : 4};
        
    private:
        struct memory_helper
        {
            std::pair<size_type, size_type> block_number{-1, -1};
            size_type size{};
            T* begin{nullptr};
        };
        struct memory_block
        {
            size_type actual_size{};
            T* memory {nullptr};
            bool is_continuous{false};
        };
        
        size_type blocks_size {16};
        size_type helpers_size{};
        memory_block* blocks{nullptr};
        memory_helper* helpers{nullptr};
        
        size_type total_memory_consumption{0};

        void repurpose_blocks_into_multi(size_type starter_block_index, size_type objects_amount)
        {
            blocks[starter_block_index].memory = reinterpret_cast<T*>(new char[objects_amount * value_size]);
            blocks[starter_block_index].actual_size = block_size;
            blocks[starter_block_index].is_continuous = true;
            objects_amount -= block_size;

            auto helper = choose_helper();
            helper->begin =  blocks[starter_block_index].memory;

            
            for(size_type i = starter_block_index + 1; starter_block_index < blocks_size - static_cast<size_type>(strategy); ++i)
            {
                blocks[i].memory = blocks[starter_block_index].memory + block_size * (i - starter_block_index);
                blocks[i].is_continuous = true;

                if(objects_amount > block_size)
                {
                    blocks[i].actual_size = block_size;
                    objects_amount -= block_size;
                }
                else
                {
                    blocks[i].actual_size = objects_amount;
                    helper->block_number = {starter_block_index, i};
                    break;
                }
            };
        }

        memory_helper* choose_helper()
        {
            for(size_type i = 0; i < helpers_size; i++)
            {
                if(helpers[i].block_number.first == -1)
                    return &helpers[i];
            }
            return &helpers[allocate_new_helpers()];
        }
        size_type allocate_new_helpers()
        {
            auto temp_size = helpers_size;
            memory_helper* temp_helpers = new memory_helper[helpers_size *= 1.5];
            std::memcpy(temp_helpers, helpers, helpers_size * sizeof(memory_helper));
            delete[] helpers;
            helpers = temp_helpers;
            
            return temp_size;
        }

        size_type allocate_new_blocks()
        {
            auto temp_size = blocks_size;
            memory_block* temp_blocks = new memory_block[blocks_size *= 1.5];
            std::memcpy(temp_blocks, blocks, helpers_size * sizeof(memory_block));
            delete[] blocks;
            blocks = temp_blocks;
            
            for(size_type i = temp_size; i < blocks_size; ++i)
                blocks[i].memory = reinterpret_cast<T*>(new unsigned char[block_size * value_size]);
            
            return temp_size;
        }

        void sort_helpers(bool by_block = false)
        {
            std::sort(helpers, helpers + helpers_size, [by_block](memory_helper a, memory_helper b)
            {
               return by_block ? a.block_number < b.block_number : a.begin < b.begin; 
            });
        }

        size_type allocate_new_blocks(size_type amount)
        {
            if(amount <= blocks_size)
                return 0;
            
            auto temp_size = blocks_size;
            memory_block* temp_blocks = new memory_block[blocks_size += amount + static_cast<size_type>(strategy)];
            std::memcpy(temp_blocks, blocks, blocks_size * sizeof(memory_block));
            delete[] blocks;
            blocks = temp_blocks;

            for(size_type i = temp_size; i < blocks_size; ++i)
                blocks[i].memory = reinterpret_cast<T*>(new unsigned char[block_size * value_size]);
            
            return temp_size;
        }
        
        T* assign_one_block(size_type amount)
        {
            sort_helpers();
            T* return_value{nullptr};
            
            for(size_type i = 0; i < blocks_size; ++i)
            {
                memory_helper valid_helpers[block_size]{};
                size_type valid_helpers_index = 0;
                
                if(block_size - blocks[i].actual_size <= amount)
                {
                    for(size_type j = 0; j < helpers_size; ++j)
                        if(helpers[j].block_number.first == i)
                            valid_helpers[valid_helpers_index++] = helpers[j];
                    
                    if(valid_helpers_index == 0)
                        continue;
                    
                    memory_block& block{blocks[i]};
                    for(size_type j = 0; j < valid_helpers_index-1; ++j)
                    {
                        memory_helper& helper{valid_helpers[j]};
                        if(static_cast<size_type>(std::distance(helper.begin + helper.size, valid_helpers[j+1].begin)) >= amount)
                        {
                            block.actual_size += amount;
                            auto new_helper = choose_helper();
                            new_helper->begin = helper.begin + helper.size();
                            new_helper->size = amount;
                            new_helper->block_number.first = i;
                            return_value = new_helper->begin;
                            return return_value;
                        }
                    }
                }
            }
            auto helper = choose_helper();
            auto index = allocate_new_blocks();
            helper->begin = blocks[index].memory;
            helper->size = amount;
            helper->block_number.first = index;
            return_value = helper->begin;
            
            return return_value;
        }

        size_type allocate_blocks_in_row(size_type amount)
        {
            auto temp_size = blocks_size;
            memory_block* temp_blocks = new memory_block[blocks_size += amount + static_cast<size_type>(strategy)];
            std::memcpy(temp_blocks, blocks, blocks_size * sizeof(memory_block));
            delete[] blocks;
            blocks = temp_blocks;
            
            repurpose_blocks_into_multi(temp_size, amount);
            return temp_size;
        }
    public:
       
       allocator() noexcept {
           helpers_size = blocks_size = static_cast<size_type>(strategy);
           blocks = new memory_block[blocks_size];
           helpers = new memory_helper[blocks_size];
       }
        
       allocator(const allocator&) throw() {
       }
       template <class U>
         allocator (const allocator<U>&) throw() {
       }
        
       ~allocator() {
           for(size_type i = 0; i < blocks_size; ++i)
           {
               if(blocks[i].memory)
               {
                   if(blocks[i].is_continuous)
                   {
                       delete[] blocks[i].memory;
                       for(size_type j = 0; j < helpers_size; j++)
                       {
                           if(helpers[j].block_number.first == i)
                           {
                               i += helpers[j].block_number.second - helpers[j].block_number.first;
                               break;
                           }
                       }
                       continue;
                   }
                   delete[] blocks[i].memory;
               }
           }
           delete[] blocks;
           delete[] helpers;
       }

       // allocate but don't initialize num elements of type T
       T* allocate (size_type amount) {

           if(amount == 0)
               return nullptr;

           total_memory_consumption += amount * value_size;
           T* return_value{nullptr};

           if (amount <= block_size)
           {
               return_value = assign_one_block(amount);
           } else
           {
               size_type blocks_needed = amount / block_size + (amount % blocks_size) != 0;
               
                size_type unused_blocks{0};
               memory_block* starter{nullptr};
               for(size_type i = 0; i < blocks_size; i++)
               {
                   if(!blocks[i].actual_size)
                   {
                       ++unused_blocks;
                       if(!starter)
                           starter = &blocks[i];
                   }
                   else
                   {
                       unused_blocks = 0;
                       starter = nullptr;
                   }
               }
               
               if(unused_blocks >= blocks_needed)
               {
                   repurpose_blocks_into_multi(starter, blocks_needed);
                   return_value = blocks[starter].memory;
               } else
                   return_value = blocks[allocate_blocks_in_row(blocks_needed)].memory;
               
               
           }
           end_allocate:
           return return_value;
       }

       // deallocate storage p of deleted elements
       void deallocate (T* p, size_type amount) {
           for(size_type i = 0; i < helpers_size; i++)
           {
               if(helpers[i].begin == p)
               {
                   auto& helper = helpers[i];
                   auto& block = blocks[helper.block_number.first];
                   T* second_address {helper.begin + helper.size};
                           
                   if constexpr (std::is_trivial_v<T>)
                   {
                       for(char* j = reinterpret_cast<char*>(helper.begin); j < reinterpret_cast<char*>(second_address); ++j)
                           *j = 0;
                   } else
                   {
                       for(T* j = helper.begin; j < second_address; ++j)
                           j->~T();
                   }
                   
                   block.actual_size -= amount;
                   helper.begin = nullptr;
                   helper.size = 0;
                   helper.block_number = {-1, -1};
                   return;
               }
           }
           throw error_handling::harpy_little_error("Deallocation of pointer, \n"
                                                    "that has not been allocated in this allocator, \n"
                                                    "is prohibited");
       }
        
    };
}

#endif //HARPY_UTILITIES_ALLOCATORS_ABSTRACT
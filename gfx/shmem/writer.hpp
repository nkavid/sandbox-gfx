#pragma once

#include <semaphore.h>

#include <cstddef>
#include <string>

namespace gfx::shmem
{
class Writer
{
  public:
    Writer(const char* name, size_t size);

    ~Writer();

    Writer(const Writer&)            = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&)                 = delete;
    Writer& operator=(Writer&&)      = delete;

    void write(void* data);

  private:
    std::string _name{};
    size_t _size{};
    int _fd{};
    void* _address{nullptr};
    sem_t* _semaphoreMutex{nullptr};
    sem_t* _semaphoreEmpty{nullptr};
    sem_t* _semaphoreFull{nullptr};
};
}

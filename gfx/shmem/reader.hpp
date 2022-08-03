#pragma once

#include <semaphore.h>

#include <cstddef>
#include <string>

namespace gfx::shmem
{
class Reader
{
  public:
    Reader(const char* name, size_t size);
    ~Reader() = default;

    Reader(const Reader&)            = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&)                 = delete;
    Reader& operator=(Reader&&)      = delete;

    [[nodiscard]] bool read(void* data);

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

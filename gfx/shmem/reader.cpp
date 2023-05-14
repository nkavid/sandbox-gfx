#include "reader.hpp"

#include "check_for_error.hpp"

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

#include <cstring>
#include <string>

namespace gfx::shmem
{
Reader::Reader(const char* name, size_t size)
    : _name{name},
      _size{size}
{
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  _fd = shm_open(_name.c_str(), O_CREAT | O_RDONLY, S_IRWXU);
  detail::checkForError(_fd, "Reader::shm_open");

  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  _address = mmap(nullptr, _size, PROT_READ, MAP_SHARED, _fd, 0);
  detail::checkForError(_address, "Reader::mmap");

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  _semaphoreMutex = sem_open((_name + "_mutex").c_str(), O_CREAT | O_RDWR, S_IRWXU, 1);
  detail::checkForError(_semaphoreMutex, "Writer::sem_open", SEM_FAILED);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  _semaphoreEmpty = sem_open((_name + "_empty").c_str(), O_CREAT | O_RDWR, S_IRWXU, 1);
  detail::checkForError(_semaphoreEmpty, "Writer::sem_open", SEM_FAILED);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  _semaphoreFull = sem_open((_name + "_full").c_str(), O_CREAT | O_RDWR, S_IRWXU, 0);
  detail::checkForError(_semaphoreFull, "Writer::sem_open", SEM_FAILED);
}

[[nodiscard]] bool Reader::read(void* data)
{
  if (sem_trywait(_semaphoreFull) == 0)
  {
    sem_wait(_semaphoreMutex);
    memcpy(data, _address, _size);
    sem_post(_semaphoreMutex);
    sem_post(_semaphoreEmpty);
    return true;
  }
  return false;
}
}

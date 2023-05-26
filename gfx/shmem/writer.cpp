#include "writer.hpp"

#include "check_for_error.hpp"

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <string>

namespace gfx::shmem
{
Writer::Writer(const char* name, size_t size)
    : _name{name},
      _size{size}
{
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  _fd = shm_open(_name.c_str(), O_CREAT | O_RDWR, S_IRWXU);
  detail::checkForError(_fd, "Writer::shm_open");
  detail::checkForError(ftruncate(_fd, static_cast<off_t>(_size)), "Writer::ftruncate");

  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  _address = mmap(nullptr, _size, PROT_WRITE, MAP_SHARED, _fd, 0);
  detail::checkForError(_address, "Writer::mmap");

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

Writer::~Writer()
{
  shm_unlink(_name.c_str());
  sem_unlink((_name + "_mutex").c_str());
  sem_unlink((_name + "_empty").c_str());
  sem_unlink((_name + "_full").c_str());
}

void Writer::write(void* data)
{
  sem_wait(_semaphoreMutex);
  memcpy(_address, data, _size);
  sem_post(_semaphoreMutex);
  if (sem_trywait(_semaphoreEmpty) == 0)
  {
    sem_post(_semaphoreFull);
  }
}
} // namespace gfx::shmem

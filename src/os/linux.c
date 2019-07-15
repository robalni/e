#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>


////////////////////////////////////////
// File functions

struct File {
    int fd;
};

public File
os_open_file_r(const char* filename) {
    return (File) {open(filename, O_RDONLY)};
}

public File
os_open_file_w(const char* filename) {
    return (File) {open(filename, O_WRONLY | O_CREAT | O_TRUNC)};
}

public size_t
os_file_size(File f) {
    off_t size = lseek(f.fd, 0, SEEK_END);
    lseek(f.fd, 0, SEEK_SET);
    return size;
}

public void
os_file_read_all(File f, char* buf, usize buf_size) {
    read(f.fd, buf, buf_size);
}

public void
os_file_write(File f, const char* buf, usize buf_size) {
    write(f.fd, buf, buf_size);
}

public void
os_close_file(File f) {
    close(f.fd);
}


////////////////////////////////////////
// Memory functions - used by memory.c

public void*
os_map_memory(usize size) {
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void* addr = mmap(0, size, prot, flags, 0, 0);
    if (addr == MAP_FAILED) {
        return null;
    }
    return addr;
}

public void
os_unmap_memory(void* addr, usize size) {
    munmap(addr, size);
}


////////////////////////////////////////
// Other functions

public void
os_write_err(const char* str) {
    usize len = 0;
    for (usize i = 0; str[i] != '\0'; i++) {
        len++;
    }
    write(STDERR_FILENO, str, len);
}

public void
os_abort() {
    kill(getpid(), SIGABRT);
}

#ifdef ENTRY
public int ENTRY(int, char**);
int
main(int argc, char** argv) {
    return ENTRY(argc, argv);
}
#else
public int editor_main(int, char**);
int
main(int argc, char** argv) {
    return editor_main(argc, argv);
}
#endif

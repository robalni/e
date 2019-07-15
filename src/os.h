#pragma once


////////////////////////////////////////
// File

struct File;
typedef struct File File;

public File
os_open_file_r(const char* filename);

public File
os_open_file_w(const char* filename);

public size_t
os_file_size(File f);

public void
os_file_read_all(File f, char* buf, usize buf_size);

public void
os_file_write(File f, const char* buf, usize buf_size);

public void
os_close_file(File f);


////////////////////////////////////////
// Memory

public void*
os_map_memory(usize size);

public void
os_unmap_memory(void* addr, usize size);


////////////////////////////////////////
// Other

public void
os_abort();

public void
os_write_err(const char* str);

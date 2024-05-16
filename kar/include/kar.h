#ifndef KRISVERS_KAR_H
#define KRISVERS_KAR_H

#ifndef KAR_API
#define KAR_API
#endif

#include <stdio.h>

typedef struct kar_archive_t kar_archive_t;
typedef struct kar_folder_t kar_folder_t;
typedef struct kar_file_t kar_file_t;

#define kar_archive_get_root(archive) kar_archive_find_folder(archive, "/")

/* create empty archive */
KAR_API kar_archive_t* kar_archive_new(void);

/* reading archive */
KAR_API kar_archive_t* kar_archive_open(FILE* file);
KAR_API kar_archive_t* kar_archive_open_from_path(const char* path);
KAR_API kar_archive_t* kar_archive_open_from_buffer(size_t size, void* buffer);

/* closing/destroying archive */
KAR_API void kar_archive_close(kar_archive_t* archive);

/* saving archive */
KAR_API void kar_archive_save(kar_archive_t* archive, FILE* file);
KAR_API void kar_archive_save_to_path(kar_archive_t* archive, const char* path);
KAR_API size_t kar_archive_save_to_buffer(kar_archive_t* archive, size_t size, void* buffer);

/* traversal utilities */
KAR_API kar_folder_t* kar_folder_get_first_folder(kar_archive_t* archive, kar_folder_t* folder);
KAR_API kar_folder_t* kar_folder_get_next(kar_archive_t* archive, kar_folder_t* folder);
KAR_API kar_file_t* kar_folder_get_first_file(kar_archive_t* archive, kar_folder_t* folder);
KAR_API kar_file_t* kar_folder_get_next_file(kar_archive_t* archive, kar_folder_t* folder, kar_file_t* file);

/* folder related utilities */
KAR_API kar_folder_t* kar_archive_find_folder(kar_archive_t* archive, const char* path);
KAR_API kar_folder_t* kar_folder_find_folder(kar_archive_t* archive, kar_folder_t* folder, const char* subpath);
KAR_API kar_folder_t* kar_archive_create_folder(kar_archive_t* archive, kar_folder_t* parent_folder, const char* name);
KAR_API void kar_archive_destroy_folder(kar_archive_t* archive, kar_folder_t* folder);
KAR_API kar_folder_t* kar_folder_get_parent(kar_archive_t* archive, kar_folder_t* folder);
KAR_API int kar_folder_move(kar_archive_t* archive, kar_folder_t* new_parent, kar_folder_t* folder);

/* file related utilities */
KAR_API kar_file_t* kar_archive_find_file(kar_archive_t* archive, const char* filepath);
KAR_API kar_file_t* kar_folder_find_file(kar_archive_t* archive, kar_folder_t* folder, const char* subpath);
KAR_API kar_file_t* kar_archive_create_file(kar_archive_t* archive, kar_folder_t* folder, const char* name);
KAR_API void kar_archive_destroy_file(kar_archive_t* archive, kar_file_t* file);
KAR_API size_t kar_file_write(kar_file_t* file, size_t size, void* buffer);
KAR_API size_t kar_file_read(kar_file_t* file, size_t size, void* buffer);
KAR_API void kar_file_resize(kar_file_t* file, size_t size);
KAR_API kar_folder_t* kar_file_get_parent(kar_archive_t* archive, kar_file_t* file);
KAR_API int kar_file_move(kar_archive_t* archive, kar_folder_t* new_parent, kar_folder_t* folder);

/* file and folder info */
KAR_API size_t kar_file_get_size(kar_file_t* file);
KAR_API const char* kar_file_get_path(kar_file_t* file);
KAR_API const char* kar_file_get_name(kar_file_t* file);
KAR_API int kar_file_is_empty(kar_file_t* file);

KAR_API const char* kar_folder_get_name(kar_folder_t* folder);
KAR_API const char* kar_folder_get_path(kar_folder_t* folder);
KAR_API size_t kar_folder_get_file_count(kar_folder_t* folder);
KAR_API size_t kar_folder_get_folder_count(kar_folder_t* folder);

#endif

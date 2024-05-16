#include <kar.h>
#include <stdlib.h>
#include <string.h>

struct kar_folder_t {
	kar_folder_t* parent;
	size_t hash;
	const char* name;

	kar_folder_t** folders;
	size_t folder_count;

	kar_file_t** files;
	size_t file_count;
};

struct kar_archive_t {
	kar_folder_t root;
};

struct kar_file_t {
	kar_folder_t* parent;
	size_t hash;
	const char* name;

	size_t length;
	void* buffer;
};

static size_t hash_function(const char* string) {
	size_t hash = 5381;

	if (string == NULL) {
		return 0;
	}

	while (*string != '\0') {
		hash += hash * 33 + *string;
		++string;
	}

	return hash;
}

kar_archive_t* kar_archive_new(void) {
	kar_archive_t* archive = malloc(sizeof(kar_archive_t));
	if (archive == NULL) {
		return archive;
	}

	archive->root = (kar_folder_t) {
		.parent = NULL,
		.name = NULL,
		.hash = 0,

		.folders = NULL,
		.folder_count = 0,

		.files = NULL,
		.file_count = 0,
	};

	archive->root.name = malloc(2);
	if (archive->root.name == NULL) {
		free(archive);
		return NULL;
	}

	((char*) archive->root.name)[0] = '/';
	((char*) archive->root.name)[1] = '\0';
	archive->root.hash = hash_function(archive->root.name);
	return archive;
}

void kar_archive_close(kar_archive_t* archive) {
	if (archive == NULL) {
		return;
	}

	free((void*) archive->root.name);
	if (archive->root.folders != NULL) {
		for (size_t i = 0; i < archive->root.folder_count; ++i) {
			kar_archive_destroy_folder(archive, archive->root.folders[i]);
		}
	}

	if (archive->root.files != NULL) {
		for (size_t i = 0; i < archive->root.file_count; ++i) {
			kar_archive_destroy_file(archive, archive->root.files[i]);
		}
	}

	free(archive);
}

kar_folder_t* kar_folder_get_first_folder(kar_archive_t* archive, kar_folder_t* folder) {
	if (archive == NULL || folder == NULL) {
		return NULL;
	}

	if (folder->folders == NULL) {
		return NULL;
	}

	return folder->folders[0];
}

kar_folder_t* kar_folder_get_next(kar_archive_t* archive, kar_folder_t* folder) {
	if (archive == NULL || folder == NULL) {
		return NULL;
	}

	kar_folder_t* parent = folder->parent;
	if (parent == NULL) {
		return NULL;
	}

	size_t index;
	for (index = 0; index < parent->folder_count; ++index) {
		if (parent->folders[index] == folder) {
			break;
		}
	}

	if (index == parent->folder_count) {
		return NULL;
	}

	if (index + 1 == parent->folder_count) {
		return NULL;
	}

	return parent->folders[index + 1];
}

kar_file_t* kar_folder_get_first_file(kar_archive_t* archive, kar_folder_t* folder) {
	if (archive == NULL || folder == NULL) {
		return NULL;
	}

	if (folder->files == NULL) {
		return NULL;
	}

	return folder->files[0];
}

kar_file_t* kar_folder_get_next_file(kar_archive_t* archive, kar_folder_t* folder, kar_file_t* file) {
	if (archive == NULL || folder == NULL || file == NULL) {
		return NULL;
	}

	size_t index;
	for (index = 0; index < folder->file_count; ++index) {
		if (folder->files[index] == file) {
			break;
		}
	}

	if (index == folder->file_count) {
		return NULL;
	}

	if (index + 1 == folder->file_count) {
		return NULL;
	}

	return folder->files[index + 1];
}

kar_folder_t* kar_archive_find_folder(kar_archive_t* archive, const char* path) {
	if (archive == NULL || path == NULL) {
		return NULL;
	}

	if (path[0] != '/') {
		return NULL;
	}

	kar_folder_t* folder = &archive->root;
	if (path[1] == '\0') {
		return folder;
	}

	const char* start = path + 1;
	const char* end = start;
	while (*end != '\0') {
		if (*end == '/') {
			size_t hash = hash_function(start);
			kar_folder_t* found = NULL;
			for (size_t i = 0; i < folder->folder_count; ++i) {
				if (folder->folders[i]->hash == hash) {
					found = folder->folders[i];
					break;
				}
			}

			if (found == NULL) {
				return NULL;
			}

			folder = found;
			start = end + 1;
		}
		++end;
	}

	size_t hash = hash_function(start);
	for (size_t i = 0; i < folder->folder_count; ++i) {
		if (folder->folders[i]->hash == hash) {
			return folder->folders[i];
		}
	}

	return NULL;
}

kar_folder_t* kar_archive_create_folder(kar_archive_t* archive, kar_folder_t* parent_folder, const char* name) {
	if (archive == NULL || parent_folder == NULL || name == NULL) {
		return NULL;
	}

	kar_folder_t* folder = malloc(sizeof(kar_folder_t));
	if (folder == NULL) {
		return NULL;
	}

	*folder = (kar_folder_t) {
		.parent = parent_folder,
		.name = NULL,
		.hash = 0,

		.folders = NULL,
		.folder_count = 0,

		.files = NULL,
		.file_count = 0,
	};

	folder->name = malloc(strlen(name) + 1);
	if (folder->name == NULL) {
		free(folder);
		return NULL;
	}

	strcpy((char*) folder->name, name);
	folder->hash = hash_function(folder->name);
	return folder;
}

void kar_archive_destroy_folder(kar_archive_t* archive, kar_folder_t* folder) {
	if (archive == NULL || folder == NULL) {
		return;
	}

	if (folder->folders != NULL) {
		for (size_t i = 0; i < folder->folder_count; ++i) {
			kar_archive_destroy_folder(archive, folder->folders[i]);
		}
		free(folder->folders);
	}

	if (folder->files != NULL) {
		for (size_t i = 0; i < folder->file_count; ++i) {
			kar_archive_destroy_file(archive, folder->files[i]);
		}
		free(folder->files);
	}

	free((void*) folder->name);

	kar_folder_t* parent = folder->parent;
	if (parent == NULL) {
		free(folder);
		return;
	}

	size_t index;
	for (index = 0; index < parent->folder_count; ++index) {
		if (parent->folders[index] == folder) {
			break;
		}
	}
	free(folder);

	/* wtf case */
	if (index == parent->folder_count) {
		return;
	}

	kar_folder_t** f = realloc(parent->folders, parent->folder_count - 1);
	if (f == NULL) {
		return;
	}

	/* more elements after deleted folder */
	for (size_t i = index; i < parent->folder_count - 1; ++i) {
		parent->folders[i] = parent->folders[i + 1];
	}

	parent->folders = f;
	--parent->folder_count;
}

kar_file_t* kar_archive_find_file(kar_archive_t* archive, const char* filepath) {
	if (archive == NULL || filepath == NULL) {
		return NULL;
	}

	if (filepath[0] != '/') {
		return NULL;
	}

	kar_folder_t* folder = &archive->root;
	if (filepath[1] == '\0') {
		return NULL;
	}

	const char* start = filepath + 1;
	const char* end = start;
	while (*end != '\0') {
		if (*end == '/') {
			size_t hash = hash_function(start);
			kar_folder_t* found = NULL;
			for (size_t i = 0; i < folder->folder_count; ++i) {
				if (folder->folders[i]->hash == hash) {
					found = folder->folders[i];
					break;
				}
			}

			if (found == NULL) {
				return NULL;
			}

			folder = found;
			start = end + 1;
		}
		++end;
	}

	size_t hash = hash_function(start);
	for (size_t i = 0; i < folder->file_count; ++i) {
		if (folder->files[i]->hash == hash) {
			return folder->files[i];
		}
	}

	return NULL;
}

kar_file_t* kar_folder_find_file(kar_archive_t* archive, kar_folder_t* folder, const char* subpath) {
	if (archive == NULL || folder == NULL || subpath == NULL) {
		return NULL;
	}

	if (subpath[0] == '/') {
		return NULL;
	}

	if (subpath[0] == '\0') {
		return NULL;
	}

	const char* start = subpath;
	const char* end = start;
	while (*end != '\0') {
		if (*end == '/') {
			size_t hash = hash_function(start);
			kar_folder_t* found = NULL;
			for (size_t i = 0; i < folder->folder_count; ++i) {
				if (folder->folders[i]->hash == hash) {
					found = folder->folders[i];
					break;
				}
			}

			if (found == NULL) {
				return NULL;
			}

			folder = found;
			start = end + 1;
		}
		++end;
	}

	size_t hash = hash_function(start);
	for (size_t i = 0; i < folder->file_count; ++i) {
		if (folder->files[i]->hash == hash) {
			return folder->files[i];
		}
	}

	return NULL;
}

kar_file_t* kar_archive_create_file(kar_archive_t* archive, kar_folder_t* folder, const char* name) {
	if (archive == NULL || folder == NULL || name == NULL) {
		return NULL;
	}

	kar_file_t* file = malloc(sizeof(kar_file_t));
	if (file == NULL) {
		return NULL;
	}

	*file = (kar_file_t) {
		.parent = folder,
		.name = NULL,
		.hash = 0,

		.length = 0,
		.buffer = NULL,
	};

	file->name = malloc(strlen(name) + 1);
	if (file->name == NULL) {
		free(file);
		return NULL;
	}

	strcpy((char*) file->name, name);
	file->hash = hash_function(file->name);

	kar_file_t** f = realloc(folder->files, folder->file_count + 1);
	if (f == NULL) {
		free((void*) file->name);
		free(file);
		return NULL;
	}

	folder->files = f;
	folder->files[folder->file_count++] = file;
	return file;
}

void kar_archive_destroy_file(kar_archive_t* archive, kar_file_t* file) {
	if (archive == NULL || file == NULL) {
		return;
	}

	free((void*) file->name);
	free(file->buffer);

	kar_folder_t* parent = file->parent;
	if (parent == NULL) {
		free(file);
		return;
	}

	size_t index;
	for (index = 0; index < parent->file_count; ++index) {
		if (parent->files[index] == file) {
			break;
		}
	}
	free(file);

	/* wtf case */
	if (index == parent->file_count) {
		return;
	}

	kar_file_t** f = realloc(parent->files, parent->file_count - 1);
	if (f == NULL) {
		return;
	}

	/* more elements after deleted file */
	for (size_t i = index; i < parent->file_count - 1; ++i) {
		parent->files[i] = parent->files[i + 1];
	}

	parent->files = f;
	--parent->file_count;
}

static char path_buffer[512];

const char* kar_file_get_path(kar_file_t* file) {
	if (file == NULL) {
		return NULL;
	}

	size_t size = 0;
	kar_folder_t* parent = file->parent;
	while (parent != NULL) {
		size_t length = strlen(parent->name);
		if (size + length + 1 >= sizeof(path_buffer)) {
			return NULL;
		}

		strncpy(path_buffer + sizeof(path_buffer) - 1 - size - length, parent->name, length);
		size += length;
		parent = parent->parent;
	}

	path_buffer[sizeof(path_buffer)] = '\0';
	return &path_buffer[sizeof(path_buffer) - size - 1];
}

const char* kar_file_get_name(kar_file_t* file) {
	if (file == NULL) {
		return NULL;
	}

	return file->name;
}

const char* kar_folder_get_path(kar_folder_t* folder) {
	if (folder == NULL) {
		return NULL;
	}

	size_t size = 0;
	kar_folder_t* parent = folder->parent;
	while (parent != NULL) {
		size_t length = strlen(parent->name);
		if (size + length + 1 >= sizeof(path_buffer)) {
			return NULL;
		}

		strncpy(path_buffer + sizeof(path_buffer) - 1 - size - length, parent->name, length);
		size += length;
		parent = parent->parent;
	}

	path_buffer[sizeof(path_buffer)] = '\0';
	return &path_buffer[sizeof(path_buffer) - size - 1];
}

const char* kar_folder_get_name(kar_folder_t* folder) {
	if (folder == NULL) {
		return NULL;
	}

	return folder->name;
}

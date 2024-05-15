#include <kar.h>
#include <stdlib.h>

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

	free(archive->root.name);
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

kar_folder_t* kar_archive_create_folder(kar_archive_t* archive, kar_folder_t* parent_folder, const char* name) {
	if (archive == NULL || name == NULL) {
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

	strcpy(folder->name, name);
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

	free(folder->name);

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

	/* more elements after deleted folder */
	for (size_t i = index; i < parent->folder_count - 1; ++i) {
		parent->folders[i] = parent->folders[i + 1];
	}

	kar_folder_t* f = realloc(parent->folders, parent->folder_count - 1);
	if (f == NULL) {
		return;
	}

	parent->folders = f;
}

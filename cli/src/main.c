#include <kar.h>

/*
create this folder structure:
/
	Makefile
	src/
		main.c
	include/
		common.h
*/

void print_file(kar_file_t* file, int depth) {

}

void print_folder(kar_folder_t* folder, int depth);
void print_folder(kar_folder_t* folder, int depth) {

}

int main(int argc, char** argv) {
	kar_archive_t* archive = kar_archive_new();
	if (archive == NULL) {
		return 1;
	}

	kar_folder_t* root = kar_archive_find_folder(archive, "/");
	if (root == NULL) {
		return 1;
	}

	kar_folder_t* folder = kar_archive_create_folder(archive, root, "src");
	if (folder == NULL) {
		return 1;
	}

	kar_file_t* file = kar_archive_create_file(archive, folder, "main.c");
	if (file == NULL) {
		return 1;
	}

	folder = kar_archive_create_folder(archive, root, "include");
	if (folder == NULL) {
		return 1;
	}

	file = kar_archive_create_file(archive, folder, "common.h");
	if (file == NULL) {
		return 1;
	}

	file = kar_archive_create_file(archive, root, "Makefile");
	if (file == NULL) {
		return 1;
	}

	print_folder(root, 0);

	kar_archive_destroy_file(archive, file);

	file = kar_archive_find_file(archive, "/test.txt");
	if (file != NULL) {
		return 1;
	}

	file = kar_folder_find_file(archive, root, "test.txt");
	if (file != NULL) {
		return 1;
	}

	kar_archive_close(archive);
	return 0;
}

#define _CRT_SECURE_NO_WARNINGS

// Define constants
#define AUTO_LAYER_COUNT 32
#define BUFFER_SIZE 32
#define ALPHABET_CHARS 26
#define LAYER_HEIGHT 26
#define LAYER_WIDTH 52

// Include libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

// Define structures
typedef struct settings {
	bool autoLayers;
	bool debugMode;
};
typedef struct dictionary {
	long long **storage;
	int countLayers;
};

// Define functions
void set_bit(long long *number, int position);
void clear_bit(long long *number, int position);
bool get_bit(long long number, int position);
bool verify_word(const char *buffer);
void init_settings(dictionary *database, settings *config, bool autoLayers, bool debugMode);
void init_layers(dictionary *database);
void insert_word(dictionary *database, settings config, const char *buffer);
void convert_file(const char *in, const char *out, settings config);
void insert_from_file(dictionary *database, settings config, const char *filename);
bool search_word(dictionary *database, settings config, const char *buffer);
void delete_word(dictionary *database, settings config, const char *buffer);
void free_dictionary(dictionary *database);

void main() {

	settings config;
	dictionary database;
	int countLayers = 0;

	init_settings(&database, &config, true, false);
	insert_from_file(&database, config, "1000.txt");
	if (search_word(&database, config, "trainistration")) {
		printf("OK");
	}
	else {
		printf("Not OK");
	}

	// End
	_getch();

}

/*
Declare SET_BIT()
Usage: set bit n of number
Parameters: number(number to be modified), position(position of the bit)
*/
void set_bit(long long *number, int position){
	*number = *number | (1 << position);
}

/*
Declare CLEAR_BIT()
Usage: clear bit n of number
Parameters: number(number to be modified), position(position of the bit)
*/
void clear_bit(long long *number, int position) {
	*number = *number & (~(1 << position));
}

/*
Declare GET_BIT()
Usage: get bit n of number
Return: the requested bit
Parameters: number(number in which the bit is), position(position of the bit)
*/
bool get_bit(long long number, int position) {
	return number & (1 << position);
}

/* 
Declare VERIFY_WORD()
Usage: verify is the string contains only alphabet characters
Return: true if valid
Parameters: buffer(string to verify)
*/
bool verify_word(const char *buffer) {
	
	int i, len;
	
	len = strlen(buffer);
	for (i = 0; i < len; i++) {
		if ((buffer[i] < 'a') || (buffer[i] > 'z')) {
			return false;
		}
	}
	return true;

}

/*
Declare GET_SETTIMGS()
Usage: set globally the settings
Parameters: database(array of layers), config(settings), autoLayers(true if auto init layers on running program), debugMode(true if print any bit opperation)
*/
void init_settings(dictionary *database, settings *config, bool autoLayers, bool debugMode){

	config->autoLayers = autoLayers;
	config->debugMode = debugMode;
	if (autoLayers) {
		init_layers(database);
	}

}

/*
Declare INIT_LAYERS()
Usage: initialize layers if the mode is on
Parameters: database(array of layers)
*/
void init_layers(dictionary *database) {

	int i, j;

	database->storage = (long long **)calloc(AUTO_LAYER_COUNT, sizeof(long long **));
	for (i = 0; i < AUTO_LAYER_COUNT; i++) {
		(database->storage)[i] = (long long *)calloc(LAYER_HEIGHT, sizeof(long));		
	}
	database->countLayers = AUTO_LAYER_COUNT;

}

/*
Declare INSERT_WORD()
Usage: insert word in dictionary
Parameters: database(array of layers), config(settings), buffer(string to be inserted)
*/
void insert_word(dictionary *database, settings config, const char *buffer){

	int i, len;

	if (!verify_word(buffer)) {
		if (config.debugMode) {
			printf("Cuvantul '%s' nu este valid!", buffer);
		}
		return;
	}
	len = strlen(buffer);
	if ((!config.autoLayers) && (len - 1 > database->countLayers)) {
		database->storage = (long long **)realloc(database->storage, (len - database->countLayers) * sizeof(long long *));
		while (len - 1 > database->countLayers) {
			(database->storage)[database->countLayers] = (long long *)calloc(LAYER_HEIGHT, sizeof(long));
			(database->countLayers)++;
		}
	}
	for (i = 0; i < len - 1; i++) {
		if (config.debugMode) {
			printf("Set: database[%d][%c][%c]\n", i, buffer[i], buffer[i + 1]);
		}
		set_bit(&((database->storage)[i][buffer[i] - 'a']), buffer[i + 1] - 'a');
		if (i == len - 2) {
			if (config.debugMode) {
				printf("Set: database[%d][%c][26 + %c]\n", i, buffer[i], buffer[i + 1]);
			}
			set_bit(&((database->storage)[i][buffer[i] - 'a']), LAYER_WIDTH / 2 + buffer[i + 1] - 'a');
		}
	}
	if (config.debugMode) {
		printf("Cuvantul '%s' a fost introdus in dictionar!\n", buffer);
	}

}

/*
Declare INSERT_FROM_FILE()
Usage: insert words in dictionary from external file
Parameters: database(array of layers), config(settings), filename(name of external file)
*/
void insert_from_file(dictionary *database, settings config, const char *filename) {

	FILE *file;
	char buffer[BUFFER_SIZE];

	file = fopen(filename, "r");
	while (fgets(buffer, BUFFER_SIZE, file) != NULL){
		buffer[strlen(buffer) - 1] = '\0';
		insert_word(database, config, buffer);
	}
	fclose(file);

}

/*
Declare SEARCH_WORD()
Usage: search word in dictionary
Parameters: database(array of layers), config(settings), buffer(string to be searched)
*/
bool search_word(dictionary *database, settings config, const char *buffer) {
	
	bool flag = 0;
	int i, len;
	
	if (!verify_word(buffer)) {
		if (config.debugMode) {
			printf("Cuvantul nu este valid!");
		}
		return false;
	}
	len = strlen(buffer);
	if (len - 1 > database->countLayers) {
		return false;
	}
	for (i = 0; i < len - 1; i++) {
		if (i == len - 2) {
			if (config.debugMode) {
				printf("Get: database[%d][%c][26 + %c]\n", i, buffer[i], buffer[i + 1]);
			}
			if (get_bit((database->storage)[i][buffer[i] - 'a'], LAYER_WIDTH / 2 + buffer[i + 1] - 'a')) {
				flag = 1;
			}
		}
		if (config.debugMode) {
			printf("Get: database[%d][%c][%c]\n", i, buffer[i], buffer[i + 1]);
		}
		if (!get_bit((database->storage)[i][buffer[i] - 'a'], buffer[i + 1] - 'a')) {
			break;
		}
	}
	return flag;

}

/*
Declare DELETE_WORD()
Usage: delete word from dictionary
Parameters: database(array of layers), config(settings), buffer(string to be deleted)
*/
void delete_word(dictionary *database, settings config, const char *buffer) {

	bool flag;
	int i, j, lenght;

	lenght = strlen(buffer);
	for (i = lenght - 2; i >= 0; i--) {
		flag = false;
		if (i == lenght - 2) {
			if (config.debugMode) {
				printf("Clear: database[%d][%c][26 + %c]\n", i, buffer[i], buffer[i + 1]);
			}
			clear_bit(&((database->storage)[i][buffer[i] - 'a']), LAYER_WIDTH / 2 + buffer[i + 1] - 'a');
		}
		for (j = 0; j < ALPHABET_CHARS; j++) {
			if (config.debugMode) {
				printf("Get to unset: database[%d][%c][%c]\n", i + 1, buffer[i + 1], 'a' + j);
			}
			if (get_bit((database->storage)[i + 1][buffer[i + 1] - 'a'], j)) {
				flag = true;
				break;
			}
		}
		if (flag == false) {
			if (config.debugMode) {
				printf("Clear: database[%d][%c][%c]\n", i, buffer[i], buffer[i + 1]);
			}
			clear_bit(&((database->storage)[i][buffer[i] - 'a']), buffer[i + 1] - 'a');
		}
	}
	if (config.debugMode) {
		printf("Cuvantul %s a fost sters!\n", buffer);
	}

}

/*
Declare FREE_DICTIONARY()
Usage: free the memory allocated for dictionary
Parameters: database(array of layers)
*/
void free_dictionary(dictionary *database) {

	int i;

	for (i = 0; i < database->countLayers; i++) {
		free((database->storage)[i]);
	}
	free(database->storage);
	database->storage = NULL;
	database->countLayers = 0;

}

/*
Declare CONVERT_FILE
Usage: covert full language dictionary into a compatible one
Parameters: in(name of input file), out(name of output file), config(settings)
*/
void convert_file(const char *in, const char *out, settings config) {

	FILE *input, *output;
	char buffer[BUFFER_SIZE], maxBuffer[BUFFER_SIZE];
	int max = 0;
	int lenght;

	input = fopen(in, "r");
	output = fopen(out, "w");
	while (fgets(buffer, BUFFER_SIZE, input) != NULL) {
		buffer[strlen(buffer) - 1] = '\0';
		if (verify_word(buffer)) {
			lenght = strlen(buffer);
			if (lenght > max) {
				max = lenght;
				strcpy(maxBuffer, buffer);
			}
			fprintf(output, "%s\n", buffer);
		}
	}
	if (config.debugMode) {
		printf("Numarul maxim de litere este de %d la cuvantul '%s'", max, maxBuffer);
	}
	fclose(input);
	fclose(output);

}
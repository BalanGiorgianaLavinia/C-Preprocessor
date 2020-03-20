#include "tema1.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define ERR_STRCPY	2
#define DIM_SYMBOL	50
#define DIM_DIR		150
#define DIM_SYM_MAP	(DIM_SYMBOL * 2)
#define NR_DIRS		10
#define INVALID_ARGS	3
#define ERR_FD		4
#define DIM_LINE	256
#define ERR_FREAD	5
#define DELIM		" \t[]{}<>=+-*/%!&|^.,:;()\\"

int parseArgs(char **input, char **output,
		struct dirsList *dirs, int argc,
		char const *argv[],
		struct hashmap *myHashMap)
{
	char symAndMapping[DIM_SYM_MAP];
	char symbol[DIM_SYMBOL];
	char mapping[DIM_SYMBOL];
	char *parts[2];
	int i = 1;
	int error = 0;

	while (i < argc) {
		/* check for -D parameter */
		if (strstr(argv[i], "-D") != NULL) {
			if (strlen(argv[i]) > 2) {
				/* -D<SYMBOL>[=<MAPPING>] */
				if (strcpy(symAndMapping,
						argv[i] + 2)
						== NULL) {
					error = ERR_STRCPY;
					return error;
				}

			} else {
				/* -D <SYMBOL>[=<MAPPING>] */
				i++;
				if (strcpy(symAndMapping, argv[i]) == NULL) {
					error = ERR_STRCPY;
					return error;
				}
			}

			/* parts[0] represents symbol */
			/* parts[1] represents mapping if existing */
			parts[0] = strtok(symAndMapping, (const char *)"=");
			strcpy(symbol, parts[0]);

			parts[1] = strtok(NULL, (const char *)"=");
			if (parts[1] == NULL) {
				/* we have only symbol, without mapping */
				error = addKeyValue(myHashMap, symbol, "\0");
				if (error != 0)
					return error;
			} else {
				strcpy(mapping, parts[1]);
				error = addKeyValue(myHashMap, symbol, mapping);
				if (error != 0)
					return error;
			}
		} else {
			/* check for -I parameter */
			if (strstr(argv[i], "-I") != NULL) {
				char *dir = (char *)
					malloc(DIM_DIR * sizeof(char));
				int size = 0;

				if (dir == NULL)
					return -ENOMEM;

				if (strlen(argv[i]) == 2) {
					/* case -I <DIR> */
					i++;
					if (strcpy(dir, argv[i]) == NULL)
						return ERR_STRCPY;
				} else {
					/* case -I<DIR> */
					if (strcpy(dir, argv[i] + 2) == NULL)
						return ERR_STRCPY;
				}

				/* check for / at the end of path */
				size = strlen(dir) - 1;
				if (dir[size] != '/')
					strcat(dir, "/");

				error = addDir(dirs, dir);
				if (error != 0)
					return error;
			} else if (strstr(argv[i], "-o") != NULL) {
				/* check for -o parameter */

				if (strlen(argv[i]) == 2) {
					/* case -o <OUTFILE> */
					i++;
					*output = malloc(sizeof(char *)
								* DIM_DIR);

					if (*output == NULL)
						return -ENOMEM;

					if (strcpy(*output, argv[i])
								== NULL)
						return ERR_STRCPY;
				} else {
					/* case -o<OUTFILE> */
					*output = malloc(sizeof(char *)
								* DIM_DIR);
					if (*output == NULL)
						return -ENOMEM;

					if (strcpy(*output,
						argv[i] + 2) == NULL)
						return ERR_STRCPY;
				}
			} else if (*input == NULL) {
				/* we don't have any -I, -o, -D */
				/* check for an existing input file */

				char tmp[DIM_DIR];
				char *path = NULL;

				*input = malloc(sizeof(char *) * DIM_DIR);
				if (*input == NULL)
					return -ENOMEM;

				if (strcpy(*input, argv[i]) == NULL)
					return ERR_STRCPY;

				if (strcpy(tmp, *input) == NULL)
					return ERR_STRCPY;

				if (strchr(tmp, '/') != NULL) {
					char *tok = NULL;
					char prevDir[DIM_DIR];

					/* save the path where */
					/* the input file is located */
					path = calloc(strlen(*input),
							sizeof(char));
					if (path == NULL)
						return -ENOMEM;

					tok = strtok(tmp, "/");
					strcpy(prevDir, tok);

					tok = strtok(NULL, "/");
					while (tok) {
						strcat(path, prevDir);

						strcat(path, "/");

						strcpy(prevDir, tok);

						tok = strtok(NULL, "/");
					}

					dirs->primaryDir = path;
				}


			} else if (*output == NULL) {
				/* check for an existing output file */
				*output = malloc(sizeof(char *) * DIM_DIR);
				if (*output == NULL)
					return -ENOMEM;

				if (strcpy(*output, argv[i]) == NULL)
					return ERR_STRCPY;
			} else
				return INVALID_ARGS;
		}

		i++;
	}
	return error;
}


int parseFile(struct hashmap *map,
		struct dirsList *dirs,
		FILE *input,
		FILE *output)
{
	char *line = NULL;
	char *copy = NULL;
	int error = 0;
	int ok_to_execute = 1;

	if (input == NULL || output == NULL)
		return -ERR_FD;


	line = malloc(DIM_LINE * sizeof(char));
	if (line == NULL)
		return -ENOMEM;

	/* copy of line */
	copy = malloc(4 * DIM_LINE * sizeof(char));
	if (copy == NULL) {
		free(line);
		return -ENOMEM;
	}

	/* take line by line from input file */
	while (fgets(line, DIM_LINE, input) != NULL) {

		if (strcpy(copy, line) == NULL) {
			error = -ERR_STRCPY;
			goto parseFile_exit;
		}

		/* check if the line begins with # */
		if (line[0] == '#') {
			/* preprocessor items */

			/* define directive */
			if (strncmp(line, "#define", strlen("#define")) == 0) {
				/* if define macro is multi-line, */
				/* then make single line from them */
				while (strchr(line, '\\')) {
					if (fgets(line,	DIM_LINE, input)
							== NULL) {

						error = -ERR_FREAD;
						goto parseFile_exit;
					}

					strcat(copy, line);
				}

				/* add key in hashmap */
				error = f_define(map, copy);
				if (error != 0)
					goto parseFile_exit;

			} else if (strncmp(line, "#undef",
						strlen("#undef")) == 0) {

				error = f_undef(map, line);
				if (error != 0)
					goto parseFile_exit;

			} else if (strncmp(line, "#ifdef",
						strlen("#ifdef")) == 0) {

				error = f_ifdef(map, line, &ok_to_execute);
				if (error != 0)
					goto parseFile_exit;

			} else if (strncmp(line, "#else",
						strlen("#else")) == 0) {
				/* change value of flag */
				/* ok_to_execute say if the current code */
				/* should be executed or not */
				ok_to_execute = ok_to_execute + 1;
				ok_to_execute = ok_to_execute % 2;

			} else if (strncmp(line, "#elif",
						strlen("#elif")) == 0) {
				error = f_if(map, line, &ok_to_execute);
				if (error != 0)
					goto parseFile_exit;

			} else if (strncmp(line, "#ifndef",
						strlen("#ifndef")) == 0) {
				error = f_ifdef(map, line, &ok_to_execute);
				if (error != 0)
					goto parseFile_exit;

				ok_to_execute = ok_to_execute + 1;
				ok_to_execute = ok_to_execute % 2;

			} else if (strncmp(line, "#if", strlen("#if")) == 0) {
				error = f_if(map, line, &ok_to_execute);
				if (error != 0)
					goto parseFile_exit;


			} else if (strncmp(line, "#endif",
						strlen("#endif")) == 0) {
				ok_to_execute = 1;

			} else if (strncmp(line, "#include",
						strlen("#include")) == 0) {
				error = f_include(dirs, line, output, map);
				if (error != 0)
					goto parseFile_exit;
			}
		} else {
			/* normal line */
			/* check if the line should be executed */
			/* and the line is not empty (new line only) */
			if (ok_to_execute == 1 && strlen(line) > 1) {
				error = f_replace(map, line, copy, output);
				if (error != 0)
					goto parseFile_exit;
			}
		}
	}

parseFile_exit:

	if (copy != NULL)
		free(copy);

	if (line != NULL)
		free(line);

	return error;
}



int f_include(struct dirsList *dirs,
		char *line,
		FILE *output,
		struct hashmap *map)
{
	int error = 0;
	FILE *include = NULL;
	char complete_path[DIM_DIR];
	char *tok = NULL;

	tok = strtok(line, " \"");
	tok = strtok(NULL, " \"");

	/* try to open from current directory*/
	include = fopen(tok, "rb");
	if (include == NULL) {
		/* file is not in current dir */
		/* try in input path */
		strcpy(complete_path, dirs->primaryDir);
		strcat(complete_path, tok);

		include = fopen(complete_path, "rb");
		if (include == NULL) {
			/* file is not in input directory */
			int i = 0;

			/* search for it in dirs given as parameter */
			for (; i < dirs->size; i++) {
				if (dirs->paths[i] == NULL)
					break;

				strcpy(complete_path, dirs->paths[i]);
				strcat(complete_path, tok);

				include = fopen(complete_path, "rb");
				if (include != NULL)
					break;
			}
		}

		if (include == NULL)
			return ERR_FD;

		error = parseFile(map, dirs, include, output);
		if (error != 0) {
			fclose(include);
			return error;
		}

		fclose(include);

	}



	return error;
}


int f_replace(struct hashmap *map, char *line, char *copy, FILE *output)
{
	int error = 0;
	char *tok = NULL;
	char buffer[DIM_LINE];
	char *value = NULL;
	char *cursor = NULL;

	memset(buffer, '\0', DIM_LINE);

	tok = strtok(copy, DELIM);
	cursor = strstr(line, tok);

	/* split the line by all delimiters */
	tok = strtok(NULL, DELIM);
	while (tok != NULL) {
		cursor = strstr(line, tok);
		strncat(buffer, line, cursor - line);
		line = cursor;

		value = (char *)getValue(map, tok);
		if (value != NULL) {
			/* if the token has a value in map, replace it*/
			strcat(buffer, value);
			cursor += strlen(tok);
			line = cursor;
		} else {
			strcat(buffer, tok);
			cursor += strlen(tok);
			line = cursor;
		}
		tok = strtok(NULL, DELIM);
	}


	strncat(buffer, line, strlen(line));

	/* write preprocessed line at output*/
	fputs(buffer, output);

	return error;
}

int f_ifdef(struct hashmap *map, char *line, int *ok_to_execute)
{
	int error = 0;
	char *tok = NULL;
	char *value = NULL;

	/* get the condition and check in hashmap its value */
	tok = strtok(line, "\n ");
	tok = strtok(NULL, "\n ");

	value = getValue(map, tok);
	if (value != NULL)
		*ok_to_execute = 1;
	else
		*ok_to_execute = 0;

	return error;
}

int f_if(struct hashmap *map, char *line, int *ok_to_execute)
{
	int error = 0;
	char *tok = NULL;
	char *value = NULL;

	/* get the condition */
	tok = strtok(line, "\n ");
	tok = strtok(NULL, "\n ");

	/* check if condition is a variable or a null value */
	if (atoi(tok) == 0) {
		*ok_to_execute = 0;

		value = (char *)getValue(map, tok);
		if (value != NULL) {
			if (atoi(value) != 0)
				*ok_to_execute = 1;
		}
	}

	return error;
}


int f_undef(struct hashmap *map, char *line)
{
	int error = 0;
	char key[DIM_SYMBOL];
	char *tok = NULL;

	/* get key from macro */
	tok = strtok(line, "\n ");
	tok = strtok(NULL, "\n ");
	if (strcpy(key, tok) == NULL)
		return -ERR_STRCPY;

	removeKey(map, key);

	return error;
}


int f_define(struct hashmap *map, char *line)
{
	int error = 0;
	char *tok = NULL;
	char key[DIM_SYMBOL];
	char value[DIM_SYMBOL];
	char *existing = NULL;

	memset(value, '\0', DIM_SYMBOL);

	/* get the symbol(key) from macro */
	tok = strtok(line, "\n\\ ");
	tok = strtok(NULL, "\n\\ ");
	if (strcpy(key, tok) == NULL)
		return -ERR_STRCPY;

	/* build the value from macro */
	tok = strtok(NULL, "\n\\ ");
	while (tok) {
		if (strlen(value) > 0)
			strcat(value, " ");

		/* get the value from hashmap if existing */
		/* and replace in macro */
		existing = getValue(map, tok);
		if (existing != NULL)
			strcat(value, existing);
		else
			strcat(value, tok);

		tok = strtok(NULL, "\n\\ ");
	}

	/* update hashmap */
	error = addKeyValue(map, key, value);

	return error;
}

int main(int argc, char const *argv[])
{
	FILE *input = NULL;
	FILE *output = NULL;
	char *inputName = NULL;
	char *outputName = NULL;
	int error = 0;
	struct hashmap *myHashMap = NULL;
	struct dirsList *dirs = NULL;


	myHashMap = initMap();
	if (myHashMap == NULL)
		goto exit;

	dirs = initDirsList();
	if (dirs == NULL)
		goto exit;

	/* parse program parameters */
	error = parseArgs(&inputName, &outputName, dirs, argc, argv, myHashMap);
	if (error != 0)
		goto exit;

	/* showDirs(dirs); */
	/* showMap(myHashMap); */
	/* open files for input and output if necessary */
	if (inputName != NULL) {
		input = fopen((const char *)inputName, "rb");
		if (input == NULL) {
			error = -INVALID_ARGS;
			goto exit;
		}
	} else
		input = stdin;

	if (outputName != NULL) {
		output = fopen((const char *)outputName, "wb");
		if (output == NULL) {
			error = -INVALID_ARGS;
			goto exit;
		}
	} else {
		output = stdout;
	}


	/* parse input file (preprocess) */
	error = parseFile(myHashMap, dirs, input, output);
	if (error != 0)
		goto exit;


exit:
	if (inputName != NULL)
		free(inputName);

	if (outputName != NULL)
		free(outputName);

	if (myHashMap != NULL)
		destroy(myHashMap);

	if (dirs != NULL)
		destroyDirs(dirs);

	if (input)
		fclose(input);
	if (output)
		fclose(output);

	return error;
}

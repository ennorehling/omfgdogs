#pragma once

#if defined(__linux__)
#define MAX_FILEPATH_LENGTH     4096        // Maximum length for filepaths (Linux PATH_MAX default value)
#else
#define MAX_FILEPATH_LENGTH      512        // Maximum length supported for filepaths
#endif

char *TextJoinEx(const char * textList[], int count, const char *delimiter);
const char* GetApplicationDirectory(void);

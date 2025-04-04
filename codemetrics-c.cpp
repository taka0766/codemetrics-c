#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

typedef struct {
    int totalLines;
    int commentLines;
    int blankLines;
} LineCounts;

// ファイルの行数をカウントする関数
LineCounts countLines(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    LineCounts counts = {0, 0, 0};
    if (file == NULL) {
        perror("ファイルを開けませんでした");
        return counts;
    }

    char line[1024];
    int inCommentBlock = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        counts.totalLines++;

        // 空白行の判定
        if (strspn(line, " \t\n\r") == strlen(line)) {
            counts.blankLines++;
            continue;
        }

        // コメント行の判定
        char *ptr = line;
        while (*ptr != '\0') {
            if (inCommentBlock) {
                if (*ptr == '*' && *(ptr + 1) == '/') {
                    inCommentBlock = 0;
                    ptr += 2;
                    continue;
                }
                counts.commentLines++;
            } else {
                if (*ptr == '/' && *(ptr + 1) == '/') {
                    counts.commentLines++;
                    break;
                } else if (*ptr == '/' && *(ptr + 1) == '*') {
                    inCommentBlock = 1;
                    counts.commentLines++;
                    ptr += 2;
                    continue;
                }
            }
            ptr++;
        }
    }

    fclose(file);
    return counts;
}

// ディレクトリを再帰的に探索する関数
void processDirectory(const char *dirPath, FILE *outputFile) {
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char searchPath[1024];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", dirPath);

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("ディレクトリを開けませんでした");
        return;
    }

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s\\%s", dirPath, findData.cFileName);

        struct stat statbuf;
        if (stat(filePath, &statbuf) == -1) {
            perror("ファイル情報を取得できませんでした");
            continue;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            processDirectory(filePath, outputFile); // 再帰的にディレクトリを探索
        } else {
            if (strstr(filePath, ".c") != NULL || strstr(filePath, ".cpp") != NULL) {
                LineCounts counts = countLines(filePath);
                if (outputFile == NULL) {
                    printf("%s,%d,%d,%d\n", filePath, counts.totalLines, counts.commentLines, counts.blankLines);
                } else {
                    fprintf(outputFile, "%s,%d,%d,%d\n", filePath, counts.totalLines, counts.commentLines, counts.blankLines);
                }
            }
        }
    } while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);
#else
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        perror("ディレクトリを開けませんでした");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        struct stat statbuf;
        if (stat(filePath, &statbuf) == -1) {
            perror("ファイル情報を取得できませんでした");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            processDirectory(filePath, outputFile); // 再帰的にディレクトリを探索
        } else if (S_ISREG(statbuf.st_mode)) {
            if (strstr(filePath, ".c") != NULL || strstr(filePath, ".cpp") != NULL) {
                LineCounts counts = countLines(filePath);
                if (outputFile == NULL) {
                    printf("%s,%d,%d,%d\n", filePath, counts.totalLines, counts.commentLines, counts.blankLines);
                } else {
                    fprintf(outputFile, "%s,%d,%d,%d\n", filePath, counts.totalLines, counts.commentLines, counts.blankLines);
                }
            }
        }
    }

    closedir(dir);
#endif
}

int main(int argc, char *argv[]) {
    char *dirPath = "."; // デフォルトの探索パス
    char *outputFilePath = NULL;
    FILE *outputFile = NULL;

    // 引数の処理
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            if (i + 1 < argc) {
                dirPath = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Usage: %s [-d directory] [-f output_file]\n", argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                outputFilePath = argv[i + 1];
                outputFile = fopen(outputFilePath, "w");
                if (outputFile == NULL) {
                    perror("出力ファイルを開けませんでした");
                    return 1;
                }
                i++;
            } else {
                fprintf(stderr, "Usage: %s [-d directory] [-f output_file]\n", argv[0]);
                return 1;
            }
        } else {
            fprintf(stderr, "Usage: %s [-d directory] [-f output_file]\n", argv[0]);
            return 1;
        }
    }

    // ディレクトリの探索と結果の出力
    processDirectory(dirPath, outputFile);

    if (outputFile != NULL) {
        fclose(outputFile);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define CMD_SIZE 1024

#define MAX 256

// Function declarations
// Function declarations
void showMenu();
void convertMenu();
void convertTXTtoCSV();
void convertCSVtoTXT();
void convertPDFtoTXT();
void convertTXTtoPDF();
void convertTXTtoHTML();     
void convertHTMLtoTXT();     
void convertJSONtoTXT();    
void convertTXTtoJSON();     

void viewLogs();
void writeLog(const char *message);

void createFile(const char *filename);
void deleteFile(const char *filename);
void readFile(const char *filename);
void writeFile(const char *filename);
void modifyFile(const char *filename);
void searchInFile(const char *filename, const char *word);

int main() {
    int choice;
    char filename[MAX], word[MAX];

    while (1) {
        showMenu();
        scanf("%d", &choice);
        getchar();  // consume newline

        switch (choice) {
            case 1:
                convertMenu();
                break;
            case 2:
                viewLogs();
                break;
            case 3:
                printf("Exiting...\n");
                writeLog("Program exited.");
                return 0;
            case 4:
                printf("Enter filename to create: ");
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;
                createFile(filename);
                break;
            case 5:
                printf("Enter filename to delete: ");
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;
                deleteFile(filename);
                break;
            case 6:
                printf("Enter filename to read: ");
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;
                readFile(filename);
                break;
            case 7:
                printf("Enter filename to write to: ");
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;
                writeFile(filename);
                break;
            case 8:
                printf("Enter filename to modify: ");
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;
                modifyFile(filename);
                break;
            case 9:
                printf("Enter filename to search in: ");
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;
                printf("Enter word to search: ");
                fgets(word, MAX, stdin);
                word[strcspn(word, "\n")] = 0;
                searchInFile(filename, word);
                break;
            default:
                printf("Invalid option. Try again.\n");
        }
    }
}

// Main menu
void showMenu() {
    printf("\n==== File Format Conversion System ====\n");
    printf("1. Convert File Format\n");
    printf("2. View Logs\n");
    printf("3. Exit\n");
    printf("4. Create File\n");
    printf("5. Delete File\n");
    printf("6. Read File\n");
    printf("7. Write to File\n");
    printf("8. Modify (Append to) File\n");
    printf("9. Search in File\n");
    printf("Enter your choice: ");
}

// Conversion sub-menu
void convertMenu() {
    int opt;
    printf("\n-- File Conversion Options --\n");
    printf("1. TXT to CSV\n");
    printf("2. CSV to TXT\n");
    printf("3. PDF to TXT (requires `pdftotext`)\n");
    printf("4. TXT to PDF (requires `txt2pdf`)\n");
    printf("5. TXT to HTML\n");
    printf("6. HTML to TXT\n");
    printf("7. JSON to TXT\n");
    printf("8. TXT to JSON\n");
    printf("Enter your choice: ");
    scanf("%d", &opt);
    getchar();

    switch (opt) {
        case 1: convertTXTtoCSV(); break;
        case 2: convertCSVtoTXT(); break;
        case 3: convertPDFtoTXT(); break;
        case 4: convertTXTtoPDF(); break;
        case 5: convertTXTtoHTML(); break;
        case 6: convertHTMLtoTXT(); break;
        case 7: convertJSONtoTXT(); break;
        case 8: convertTXTtoJSON(); break;
        default: printf("Invalid conversion choice.\n");
    }
}


// 1. TXT to CSV
void convertTXTtoCSV() {
    char inputFile[MAX], outputFile[MAX], buffer[MAX];
    FILE *in, *out;

    printf("Enter input TXT file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output CSV file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    in = fopen(inputFile, "r");
    out = fopen(outputFile, "w");

    if (!in || !out) {
        printf("File error. Check paths.\n");
        writeLog("Error in TXT to CSV conversion.");
        return;
    }

    while (fgets(buffer, MAX, in)) {
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] == ' ') buffer[i] = ',';
        }
        fprintf(out, "%s", buffer);
    }

    fclose(in);
    fclose(out);
    printf("TXT to CSV conversion complete.\n");
    writeLog("TXT to CSV conversion successful.");
}

// 2. CSV to TXT
void convertCSVtoTXT() {
    char inputFile[MAX], outputFile[MAX], buffer[MAX];
    FILE *in, *out;

    printf("Enter input CSV file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output TXT file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    in = fopen(inputFile, "r");
    out = fopen(outputFile, "w");

    if (!in || !out) {
        printf("File error. Check paths.\n");
        writeLog("Error in CSV to TXT conversion.");
        return;
    }

    while (fgets(buffer, MAX, in)) {
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] == ',') buffer[i] = ' ';
        }
        fprintf(out, "%s", buffer);
    }

    fclose(in);
    fclose(out);
    printf("CSV to TXT conversion complete.\n");
    writeLog("CSV to TXT conversion successful.");
}

// 3. PDF to TXT (requires `pdftotext`)
void convertPDFtoTXT() {
    char inputFile[MAX], outputFile[MAX], command[CMD_SIZE];

    printf("Enter input PDF file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output TXT file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    snprintf(command, sizeof(command), "pdftotext \"%s\" \"%s\"", inputFile, outputFile);

    if (system(command) == 0) {
        printf("PDF to TXT conversion successful.\n");
        writeLog("PDF to TXT conversion successful.");
    } else {
        printf("Conversion failed. Make sure `pdftotext` is installed.\n");
        writeLog("PDF to TXT conversion failed.");
    }
}

// 4. TXT to PDF (requires `txt2pdf`)
void convertTXTtoPDF() {
    char inputFile[MAX], outputFile[MAX], command[CMD_SIZE];

    printf("Enter input TXT file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output PDF file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    snprintf(command, sizeof(command), "txt2pdf \"%s\" -o \"%s\"", inputFile, outputFile);

    if (system(command) == 0) {
        printf("TXT to PDF conversion successful.\n");
        writeLog("TXT to PDF conversion successful.");
    } else {
        printf("Conversion failed. Ensure `txt2pdf` is installed.\n");
        writeLog("TXT to PDF conversion failed.");
    }
}

void convertTXTtoHTML() {
    char inputFile[MAX], outputFile[MAX], buffer[MAX];
    FILE *in, *out;

    printf("Enter input TXT file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output HTML file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    in = fopen(inputFile, "r");
    out = fopen(outputFile, "w");

    if (!in || !out) {
        printf("File error. Check paths.\n");
        writeLog("Error in TXT to HTML conversion.");
        return;
    }

    fprintf(out, "<html><body><pre>\n");
    while (fgets(buffer, MAX, in)) {
        fprintf(out, "%s", buffer);
    }
    fprintf(out, "</pre></body></html>\n");

    fclose(in);
    fclose(out);
    printf("TXT to HTML conversion complete.\n");
    writeLog("TXT to HTML conversion successful.");
}

void convertHTMLtoTXT() {
    char inputFile[MAX], outputFile[MAX], ch;
    FILE *in, *out;
    int insideTag = 0;

    printf("Enter input HTML file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output TXT file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    in = fopen(inputFile, "r");
    out = fopen(outputFile, "w");

    if (!in || !out) {
        printf("File error. Check paths.\n");
        writeLog("Error in HTML to TXT conversion.");
        return;
    }

    while ((ch = fgetc(in)) != EOF) {
        if (ch == '<') {
            insideTag = 1;
        } else if (ch == '>') {
            insideTag = 0;
        } else if (!insideTag) {
            fputc(ch, out);
        }
    }

    fclose(in);
    fclose(out);
    printf("HTML to TXT conversion complete.\n");
    writeLog("HTML to TXT conversion successful.");
}

void convertJSONtoTXT() {
    char inputFile[MAX], outputFile[MAX], buffer[MAX];
    FILE *in, *out;

    printf("Enter input JSON file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output TXT file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    in = fopen(inputFile, "r");
    out = fopen(outputFile, "w");

    if (!in || !out) {
        printf("File error. Check paths.\n");
        writeLog("Error in JSON to TXT conversion.");
        return;
    }

    while (fgets(buffer, MAX, in)) {
        fprintf(out, "%s", buffer);
    }

    fclose(in);
    fclose(out);
    printf("JSON to TXT conversion complete.\n");
    writeLog("JSON to TXT conversion successful.");
}

void convertTXTtoJSON() {
    char inputFile[MAX], outputFile[MAX], buffer[MAX];
    FILE *in, *out;

    printf("Enter input TXT file: ");
    fgets(inputFile, MAX, stdin);
    inputFile[strcspn(inputFile, "\n")] = 0;

    printf("Enter output JSON file: ");
    fgets(outputFile, MAX, stdin);
    outputFile[strcspn(outputFile, "\n")] = 0;

    in = fopen(inputFile, "r");
    out = fopen(outputFile, "w");

    if (!in || !out) {
        printf("File error. Check paths.\n");
        writeLog("Error in TXT to JSON conversion.");
        return;
    }

    fprintf(out, "[\n");
    int first = 1;
    while (fgets(buffer, MAX, in)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (!first) fprintf(out, ",\n");
        fprintf(out, "  \"%s\"", buffer);
        first = 0;
    }
    fprintf(out, "\n]\n");

    fclose(in);
    fclose(out);
    printf("TXT to JSON conversion complete.\n");
    writeLog("TXT to JSON conversion successful.");
}



// Logs
void viewLogs() {
    FILE *log = fopen("logs.txt", "r");
    char line[MAX];
    if (!log) {
        printf("No logs found.\n");
        return;
    }
    printf("\n==== Logs ====\n");
    while (fgets(line, sizeof(line), log)) {
        printf("%s", line);
    }
    fclose(log);
}

void writeLog(const char *message) {
    FILE *log = fopen("logs.txt", "a");
    if (log) {
        fprintf(log, "%s\n", message);
        fclose(log);
    }
}

// File Operations
void createFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file) {
        printf("File '%s' created.\n", filename);
        writeLog("File created.");
        fclose(file);
    } else {
        printf("Failed to create file.\n");
    }
}

void deleteFile(const char *filename) {
    if (remove(filename) == 0) {
        printf("File '%s' deleted.\n", filename);
        writeLog("File deleted.");
    } else {
        printf("Could not delete file.\n");
    }
}

void readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    char line[MAX];
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }
    printf("\n-- Content of %s --\n", filename);
    while (fgets(line, MAX, file)) {
        printf("%s", line);
    }
    fclose(file);
}

void writeFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    char content[MAX];
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }
    printf("Enter content (end with '#'): \n");
    while (fgets(content, MAX, stdin)) {
        if (content[0] == '#') break;
        fputs(content, file);
    }
    fclose(file);
    printf("Content written.\n");
    writeLog("Data written to file.");
}

void modifyFile(const char *filename) {
    FILE *file = fopen(filename, "a");
    char content[MAX];
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }
    printf("Enter content to append (end with '#'): \n");
    while (fgets(content, MAX, stdin)) {
        if (content[0] == '#') break;
        fputs(content, file);
    }
    fclose(file);
    printf("Content appended.\n");
    writeLog("Data appended to file.");
}

void searchInFile(const char *filename, const char *word) {
    FILE *file = fopen(filename, "r");
    char line[MAX];
    int found = 0, lineNumber = 1;

    if (!file) {
        printf("Cannot open file.\n");
        return;
    }

    while (fgets(line, MAX, file)) {
        if (strstr(line, word)) {
            printf("Line %d: %s", lineNumber, line);
            found = 1;
        }
        lineNumber++;
    }

    fclose(file);
    if (!found) {
        printf("'%s' not found in the file.\n", word);
    } else {
        writeLog("Search term found in file.");
    }
}
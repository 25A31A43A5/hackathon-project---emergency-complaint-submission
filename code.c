#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>

#define MAX 200
#define LINE_SIZE 1024

typedef enum {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3
} Severity;

typedef enum {
    MEDICAL,
    FIRE,
    ACCIDENT,
    WATER_LEAKAGE,
    OTHER
} EmergencyType;
typedef struct {
    int id;
    EmergencyType type;
    Severity severity;
    char description[300];   
    char location[100];
    char contact[50];
} Complaint;

Complaint complaints[MAX];
int complaintCount = 0;

const char *CSV_URL =
"https://docs.google.com/spreadsheets/d/1HBOE7aSUTO5lfFWlZNX48R-10BRaS3jkIDmGG-SFsZ8/export?format=csv";

EmergencyType parseType(const char *str) {
    if (!str) return OTHER;
    if (strcmp(str, "Medical") == 0) return MEDICAL;
    if (strcmp(str, "Fire") == 0) return FIRE;
    if (strcmp(str, "Road accident") == 0) return ACCIDENT;
    if (strcmp(str, "Water Leakage") == 0) return WATER_LEAKAGE;
    return OTHER;
}

Severity parseSeverity(const char *str) {
    if (!str) return LOW;
    if (strcmp(str, "Low") == 0) return LOW;
    if (strcmp(str, "Medium") == 0) return MEDIUM;
    if (strcmp(str, "High") == 0) return HIGH;
    return LOW;
}

const char* typeToString(EmergencyType t) {
    switch (t) {
        case MEDICAL: return "Medical";
        case FIRE: return "Fire";
        case ACCIDENT: return "Road Accident";
        case WATER_LEAKAGE: return "Water Leakage";
        default: return "Other";
    }
}

const char* severityToString(Severity s) {
    switch (s) {
        case LOW: return "Low";
        case MEDIUM: return "Medium";
        case HIGH: return "High";
        default: return "Unknown";
    }
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void downloadCSV() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("Curl init failed\n");
        return;
    }

    FILE *fp = fopen("complaints.csv", "w");
    if (!fp) {
        printf("Failed to create CSV file\n");
        curl_easy_cleanup(curl);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, CSV_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        printf("Download error: %s\n", curl_easy_strerror(res));
    else
        printf("CSV downloaded successfully\n");

    fclose(fp);
    curl_easy_cleanup(curl);
}
void stripQuotes(char *str) {
    if (!str) return;
    if (str[0] == '"') memmove(str, str + 1, strlen(str));
    char *end = strrchr(str, '"');
    if (end) *end = '\0';
}

void loadCSV() {
    FILE *fp = fopen("complaints.csv", "r");
    if (!fp) {
        printf("CSV not found\n");
        return;
    }

    char line[LINE_SIZE];
    complaintCount = 0;

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (complaintCount >= MAX) break;

        char *fields[10];
        int fieldCount = 0;

        char *ptr = strtok(line, ",");
        while (ptr && fieldCount < 10) {
            if (ptr[0] == '"') {
                char buffer[LINE_SIZE] = "";
                strcat(buffer, ptr + 1); 
                char *next;
                while ((next = strtok(NULL, ",")) != NULL) {
                    strcat(buffer, ",");
                    strcat(buffer, next);
                    if (next[strlen(next)-1] == '"') { 
                        buffer[strlen(buffer)-1] = '\0';
                        break;
                    }
                }
                fields[fieldCount++] = strdup(buffer); 
            } else {
                fields[fieldCount++] = ptr;
            }
            ptr = strtok(NULL, ",");
        }

        if (fieldCount < 6) continue;

        complaints[complaintCount].id = complaintCount + 1;
        complaints[complaintCount].type = parseType(fields[1]);
        complaints[complaintCount].severity = parseSeverity(fields[2]);

        snprintf(complaints[complaintCount].description,
                 sizeof(complaints[complaintCount].description), "%s", fields[3]);

        snprintf(complaints[complaintCount].location,
                 sizeof(complaints[complaintCount].location), "%s", fields[4]);

        snprintf(complaints[complaintCount].contact,
                 sizeof(complaints[complaintCount].contact), "%s", fields[5]);

        complaintCount++;
        for (int i = 0; i < fieldCount; i++) {
            if (fields[i] != ptr && fields[i][0] == '"') free(fields[i]);
        }
    }

    fclose(fp);
    printf("Loaded %d complaints successfully\n", complaintCount);
}


void sortBySeverity() {
    for (int i = 0; i < complaintCount - 1; i++) {
        for (int j = i + 1; j < complaintCount; j++) {
            if (complaints[i].severity < complaints[j].severity) {
                Complaint temp = complaints[i];
                complaints[i] = complaints[j];
                complaints[j] = temp;
            }
        }
    }
}

void displayComplaints() {
    for (int i = 0; i < complaintCount; i++) {
        printf("\nID: %d\nType: %s\nSeverity: %s\nDescription: %s\nLocation: %s\nContact: %s\n",
            complaints[i].id,
            typeToString(complaints[i].type),
            severityToString(complaints[i].severity),
            complaints[i].description,
            complaints[i].location,
            complaints[i].contact);
    }
}

void generateLocationSummary(FILE *fp) {
    char locations[MAX][100];
    int counts[MAX];
    int locCount = 0;

    for (int i = 0; i < complaintCount; i++) {
        int found = -1;

        for (int j = 0; j < locCount; j++) {
            if (strcmp(locations[j], complaints[i].location) == 0) {
                found = j;
                break;
            }
        }

        if (found == -1) {
            strncpy(locations[locCount], complaints[i].location, 99);
            locations[locCount][99] = '\0';
            counts[locCount] = 1;
            locCount++;
        } else {
            counts[found]++;
        }
    }

    fprintf(fp, "\nCOMPLAINTS BY LOCATION\n");
    for (int i = 0; i < locCount; i++) {
        fprintf(fp, "%s : %d\n", locations[i], counts[i]);
    }
}

void generateReport() {
    if (complaintCount == 0) {
        printf("No data loaded\n");
        return;
    }

    FILE *fp = fopen("complaint_report.txt", "w");
    if (!fp) {
        printf("Report creation failed\n");
        return;
    }
    time_t now = time(NULL);
    fprintf(fp, "EMERGENCY REPORT\nGenerated: %s\n\n", ctime(&now));
    generateLocationSummary(fp);
    int high = 0, med = 0, low = 0;
    for (int i = 0; i < complaintCount; i++) {
        switch (complaints[i].severity) {
            case HIGH: high++; break;
            case MEDIUM: med++; break;
            case LOW: low++; break;
        }
    }
    fprintf(fp, "\nSUMMARY:\nTotal complaints: %d\nHigh: %d\nMedium: %d\nLow: %d\n\n",
            complaintCount, high, med, low);

    fprintf(fp, "DETAILS:\n");
    for (int i = 0; i < complaintCount; i++) {
        fprintf(fp,
            "ID: %d\nType: %s\nSeverity: %s\nDescription: %s\nLocation: %s\nContact: %s\n----\n",
            complaints[i].id,
            typeToString(complaints[i].type),
            severityToString(complaints[i].severity),
            complaints[i].description,
            complaints[i].location,
            complaints[i].contact);
    }

    fclose(fp);

    printf("Report generated successfully: complaint_report.txt\n");

    system("code complaint_report.txt");
}


int main() {
    int choice;
    int initialized = 0;

    while (1) {
        printf("\n1.Initialize & Load data\n");
        printf("2.Generate report\n");
        printf("3.Refresh Data\n");
        printf("4.Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            continue;
        }

        switch (choice) {

            case 1:
                downloadCSV();
                loadCSV();
                sortBySeverity();
                displayComplaints();
                initialized = 1;
                break;

            case 2:
                if (!initialized) {
                    printf("Please initialize first.\n");
                } else {
                    generateReport();
                }
                break;

            case 3:
                if (!initialized) {
                    printf("Please initialize first.\n");
                } else {
                    downloadCSV();
                    loadCSV();
                    sortBySeverity();
                    displayComplaints();
                }
                break;

            case 4:
                exit(0);

            default:
                printf("Invalid option\n");
        }
    }
}
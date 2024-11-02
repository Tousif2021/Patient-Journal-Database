#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PATIENT 1000
#define MAX_BILD 10
#define MAX_NAME_LENGTH 41
#define MAX_LINE_LENGTH 100
#define MAX_FILENAME_LENGTH 41
struct journal
{
    char personnummer[13];
    char Namn[MAX_NAME_LENGTH];
    int bildreferenser[MAX_BILD];
} patient[MAX_PATIENT];
void count_patients_in_file(const char *filename, int *initialpatient);
void update_patient_file(const char *filename, int initialpatient);
int find_patient_personnummer(const char *personnummer, int initialpatient);
int find_patient_namn(const char *namn, int initialpatient);
void find_patient_bildreferens(int bildreferens, int initialpatient);
void register_patient(int *initialpatient, const char *filename);
void print_patientlist(int initialpatient);
void search(int initialpatient);
void print_single_patient(int index);
int compareByName(const void *a, const void *b);
int compareByPersonnummer(const void *a, const void *b);
void sort_patients(int initialpatient);
void trim_whitespace(char *str);
void deregister_patient(int *initialpatient, const char *filename);
void add_picture_to_patient(int initialpatient, const char *filename);
void search_by_name(const char *namn, int initalpatient);
void to_lowercase(char *str);
bool is_unique_bildreferens_for_current_patient(int *bildreferenser, int count, int referens);
bool has_bildreferens(int patient_index, int bildreferens);
int main(void)
{
    int initialpatient = 0;
    int choice;
    char filename[MAX_FILENAME_LENGTH];
    printf("Patientjournalsystem\n");
    while (1)
    {
        printf("Vilken fil vill du anvanda: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = '\0';
        if (strcmp(filename, "patientdatabase.txt") == 0)
        {
            count_patients_in_file(filename, &initialpatient);
            break;
        }
        else
        {
            printf("Fel filnamn/fil skanas. Försök igen.\n");
        }
    }
    while (1)
    {
        // printf("Patientdabase.txt är i bruk.\n");
        // count_patients_in_file();
        printf("Huvudmeny\n");
        printf("\t1) Registrera nya patienter\n");
        printf("\t2) Skriva ut alla patienter\n");
        printf("\t3) Soka efter patienter\n");
        printf("\t4) Lagg till bilder\n");
        printf("\t5) Sortera patienter\n");
        printf("\t6) Avregistrera patienter\n");
        printf("\t7) Avsluta programmet\n");
        printf("Ange alternativ: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            register_patient(&initialpatient, filename);
            break;
        case 2:
            print_patientlist(initialpatient);
            break;
        case 3:
            search(initialpatient);
            break;
        case 4:
            add_picture_to_patient(initialpatient, filename);
            break;
        case 5:
            sort_patients(initialpatient);
            break;
        case 6:
            deregister_patient(&initialpatient, filename);
            break;
        case 7:
            printf("Sparar patientdata i patientdatabase.txt\nAvslutar\n");
            return 0;
        default:
            printf("Ogiltigt alternativ. Försök igen.\n");
        }
    }
}
int find_patient_personnummer(const char *personnummer, int initialpatient) {
    for (int i = 0; i < initialpatient; i++) {
        if (strcmp(patient[i].personnummer, personnummer) == 0) {
            return i;
        }
    }
    return -1;
}
int find_patient_namn(const char *namn, int initialpatient) {
    char trimmed_namn[MAX_NAME_LENGTH];
    strncpy(trimmed_namn, namn, MAX_NAME_LENGTH);
    trimmed_namn[MAX_NAME_LENGTH - 1] = '\0';
    trim_whitespace(trimmed_namn);

    for (int i = 0; i < initialpatient; i++) {
        if (strstr(patient[i].Namn, trimmed_namn) != NULL) {
            return i;
        }
    }
    return -1;
}

void find_patient_bildreferens(int bildreferens, int initialpatient) {
    bool found = false;
    for (int i = 0; i < initialpatient; i++) {
        if (has_bildreferens(i, bildreferens)) {
            print_single_patient(i);
            found = true;
        }
    }
    if (!found) {
        printf("Ingen patient med bildreferens %d hittades.\n", bildreferens);
    }
}

bool is_unique_bildreferens_for_current_patient(int *bildreferenser, int count, int referens) {
    for (int i = 0; i < count; i++) {
        if (bildreferenser[i] == referens) {
            return false;
        }
    }
    return true;
}
void register_patient(int *initialpatient, const char *filename) {
    if (*initialpatient >= MAX_PATIENT) {
        printf("Maximalt antal patienter uppnått.\n");
        return;
    }

    struct journal new_patient;
    bool unique_id = false;

    while (!unique_id) {
        printf("Ange personnummer (format: YYMMDD-XXXX, eller tryck 'q' för att avsluta): ");
        scanf("%s", new_patient.personnummer);
        if (strcmp(new_patient.personnummer, "q") == 0 || strcmp(new_patient.personnummer, "Q") == 0) {
            printf("Registrering avbruten.\n");
            return;
        }
        if (find_patient_personnummer(new_patient.personnummer, *initialpatient) == -1) {
            unique_id = true;
        } else {
            printf("Personnummer redan registrerat. Försök igen.\n");
        }
    }

    printf("Ange namn: ");
    getchar();
    fgets(new_patient.Namn, MAX_NAME_LENGTH, stdin);
    trim_whitespace(new_patient.Namn);

    for (int i = 0; i < MAX_BILD; i++) {
        new_patient.bildreferenser[i] = 0;
    }

    int bild_count = 0;
    int new_bildreferens;
    printf("Ange bildreferens (eller 0 för att avsluta): ");
    while (bild_count < MAX_BILD) {
        scanf("%d", &new_bildreferens);
        getchar();

        if (new_bildreferens == 0) {
            break;
        }
        if (is_unique_bildreferens_for_current_patient(new_patient.bildreferenser, bild_count, new_bildreferens)) {
            new_patient.bildreferenser[bild_count++] = new_bildreferens;
            printf("Bildreferens tillagd.\n");
        } else {
            printf("Bildreferens redan registrerad för denna patient. Försök igen.\n");
        }

        if (bild_count < MAX_BILD) {
            printf("Ange nästa bildreferens (eller 0 för att avsluta): ");
        } else {
            printf("Maximalt antal bildreferenser uppnått.\n");
        }
    }

    patient[(*initialpatient)++] = new_patient;
    printf("Patient registrerad.\n");

    update_patient_file(filename, *initialpatient);
}
void print_patientlist(int initialpatient) {
    printf("%-15s %-25s %-20s\n", "Personnummer", "Namn", "Bildreferenser");
    printf("____________________________________________________________\n");

    for (int i = 0; i < initialpatient; i++) {
        printf("%-15s %-25s [", patient[i].personnummer, patient[i].Namn);

        bool first = true;
        for (int j = 0; j < MAX_BILD; j++) {
            if (patient[i].bildreferenser[j] != 0) {
                if (!first) {
                    printf(", ");
                }
                printf("%d", patient[i].bildreferenser[j]);
                first = false;
            }
        }
        printf("]\n");
    }

    if (initialpatient == 0) {
        printf("Inga patienter registrerade.\n");
    }
}
void search(int initialpatient)
{
    int search_operation, bildreferens_search;
    char personnumer_search[20], namn_search[50];

    while (1)
    {
        printf("Sok pa personnumer(1), namn(2), bildreferens(3), avsluta(4): ");
        scanf("%d", &search_operation);
        getchar();

        int i;
        switch (search_operation)
        {
        case 1:
            printf("Ange Personnummer: ");
            fgets(personnumer_search, sizeof(personnumer_search), stdin);
            trim_whitespace(personnumer_search);
            i = find_patient_personnummer(personnumer_search, initialpatient);

            if (i >= 0)
            {
                print_single_patient(i);
            }
            else
            {
                printf("Personnumret saknas i databasen\n");
            }
            break;

        case 2:
            printf("Ange Namn: ");
            fgets(namn_search, sizeof(namn_search), stdin);
            trim_whitespace(namn_search);
            search_by_name(namn_search, initialpatient);
            break;

        case 3:
            printf("Ange bildreferens: ");
            scanf("%d", &bildreferens_search);

            bool found = false;
            for (int i = 0; i < initialpatient; i++)
            {
                if (has_bildreferens(i, bildreferens_search))
                {
                    print_single_patient(i);
                    found = true;
                }
            }
            if (!found)
            {
                printf("Ingen patient med bildreferens %d hittades.\n", bildreferens_search);
            }
            break;

        case 4:
            printf("Avslutar sökning.\n");
            return;

        default:
            printf("Ogiltigt alternativ. Försök igen.\n");
        }
    }
}

void print_single_patient(int index)
{
    printf("%-15s %-25s [", patient[index].personnummer, patient[index].Namn);
    bool first = true;
    for (int j = 0; j < MAX_BILD; j++)
    {
        if (patient[index].bildreferenser[j] != 0)
        {
            if (!first)
            {
                printf(", ");
            }
            printf("%d", patient[index].bildreferenser[j]);
            first = false;
        }
    }
    printf("]\n");
}
void count_patients_in_file(const char *filename, int *initialpatient)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Kunde inte öppna filen.\n");
        *initialpatient = 0;
        return;
    }
    char line[MAX_LINE_LENGTH];
    *initialpatient = 0;
    while (fgets(line, sizeof(line), file) != NULL && *initialpatient < MAX_PATIENT)
    {
        char *token = strtok(line, "\t");
        if (token != NULL)
        {
            strncpy(patient[*initialpatient].personnummer, token, sizeof(patient[*initialpatient].personnummer) - 1);
            patient[*initialpatient].personnummer[sizeof(patient[*initialpatient].personnummer) - 1] = '\0';
        }

        token = strtok(NULL, "\t");
        if (token != NULL)
        {
            strncpy(patient[*initialpatient].Namn, token, MAX_NAME_LENGTH - 1);
            patient[*initialpatient].Namn[MAX_NAME_LENGTH - 1] = '\0';
            trim_whitespace(patient[*initialpatient].Namn);
        }
        token = strtok(NULL, "\t");
        if (token != NULL)
        {
            char *ref_token = strtok(token, "[], ");
            int ref_count = 0;
            while (ref_token != NULL && ref_count < MAX_BILD)
            {
                patient[*initialpatient].bildreferenser[ref_count++] = atoi(ref_token);
                ref_token = strtok(NULL, "[], ");
            }
        }
        (*initialpatient)++;
    }
    fclose(file);
    printf("Antal patienter: %d\n", *initialpatient);
}
int compareByName(const void *a, const void *b)
{
    return strcmp(((struct journal *)a)->Namn, ((struct journal *)b)->Namn);
}
int compareByPersonnummer(const void *a, const void *b)
{
    return strcmp(((struct journal *)a)->personnummer, ((struct journal *)b)->personnummer);
}
void sort_patients(int initialpatient)
{
    int option;
    printf("1) Sortera efter namn\n");
    printf("2) Sortera efter personnummer\n");
    printf("Val: ");
    scanf("%d", &option);

    if (option == 1)
    {
        qsort(patient, initialpatient, sizeof(struct journal), compareByName);
        printf("Patienter sorterade efter namn.\n");
        print_patientlist(initialpatient);
    }
    else if (option == 2)
    {
        qsort(patient, initialpatient, sizeof(struct journal), compareByPersonnummer);
        printf("Patienter sorterade efter personnummer.\n");
        print_patientlist(initialpatient);
    }
    else
    {
        printf("Ogiltigt alternativ.\n");
    }
}

void trim_whitespace(char *str)
{
    int index = 0;
    int i;

    while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    i = 0;
    while (str[index] != '\0')
    {
        str[i++] = str[index++];
    }
    str[i] = '\0';

    i--;
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
    {
        i--;
    }
    str[i + 1] = '\0';
}
void deregister_patient(int *initialpatient, const char *filename)
{
    char personnumer_search[20];
    printf("Ange personnumer på patient som ska avregistreras: ");
    fgets(personnumer_search, sizeof(personnumer_search), stdin);
    trim_whitespace(personnumer_search);

    int index = find_patient_personnummer(personnumer_search, *initialpatient);
    if (index >= 0)
    {
        printf("Vill du avregistrera patienten(j/n): ");
        char answer;
        scanf(" %c", &answer); 
        if (answer == 'j')
        {
            for (int i = index; i < *initialpatient - 1; i++)
            {
                patient[i] = patient[i + 1];
            }
            (*initialpatient)--;
            update_patient_file(filename, *initialpatient);
            printf("Patient avregistrerad.\n");
        }
        else
        {
            printf("Avregistrering avbruten.\n");
        }
    }
    else
    {
        printf("Patienten finns inte i databasen.\n");
    }
}

void update_patient_file(const char *filename, int initialpatient)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Kunde inte öppna filen för att spara uppdaterad data.\n");
        return;
    }
    for (int i = 0; i < initialpatient; i++)
    {
        fprintf(file, "%s\t%s\t[", patient[i].personnummer, patient[i].Namn);

        bool first = true;
        for (int j = 0; j < MAX_BILD; j++)
        {
            if (patient[i].bildreferenser[j] != 0)
            {
                if (!first)
                {
                    fprintf(file, ", ");
                }
                fprintf(file, "%d", patient[i].bildreferenser[j]);
                first = false;
            }
        }
        fprintf(file, "]\n");
    }
    fclose(file);
    printf("Patientdatabasen har uppdaterats.\n");
}
void add_picture_to_patient(int initialpatient, const char *filename)
{
    char personnumer_search[20];
    printf("Ange personnumer på patient för att lägga till bild: ");
    fgets(personnumer_search, sizeof(personnumer_search), stdin);
    trim_whitespace(personnumer_search);

    int index = find_patient_personnummer(personnumer_search, initialpatient);
    if (index < 0)
    {
        printf("Patienten finns inte i databasen.\n");
        return;
    }

    int new_bildreferens;
    int added_count = 0;

    for (int i = 0; i < MAX_BILD; i++)
    {
        if (patient[index].bildreferenser[i] != 0)
        {
            added_count++;
        }
    }
    
    if (added_count >= MAX_BILD)
    {
        printf("Maximalt antal bilder tilldelade för denna patient.\n");
        return;
    }

    printf("Ange ny bildreferens (eller 0 för att avsluta): ");
    while (added_count < MAX_BILD)
    {
        scanf("%d", &new_bildreferens);
        getchar();

        if (new_bildreferens == 0)
        {
            printf("Avslutar lagga till bilder\n");
            break;
        }

        if (is_unique_bildreferens_for_current_patient(patient[index].bildreferenser, added_count, new_bildreferens))
        {
            patient[index].bildreferenser[added_count++] = new_bildreferens;
            printf("Bildreferens tillagd.\n");
        }
        else
        {
            printf("Referensen finns redan!. Försök igen.\n");
        }

        if (added_count < MAX_BILD)
        {
            printf("Ange bildreferens (eller 0 för att avsluta): ");
        }
        else
        {
            printf("Maximalt antal bildreferenser uppnått.\n");
            break;
        }
    }

    update_patient_file(filename, initialpatient);
}

void search_by_name(const char *namn, int initialpatient)
{
    char trimmed_namn[MAX_NAME_LENGTH];
    strncpy(trimmed_namn, namn, MAX_NAME_LENGTH);
    trimmed_namn[MAX_NAME_LENGTH - 1] = '\0';
    trim_whitespace(trimmed_namn);

    to_lowercase(trimmed_namn);

    bool found = false;
    for (int i = 0; i < initialpatient; i++)
    {
        char patient_name_lower[MAX_NAME_LENGTH];
        strncpy(patient_name_lower, patient[i].Namn, MAX_NAME_LENGTH);
        patient_name_lower[MAX_NAME_LENGTH - 1] = '\0';
        to_lowercase(patient_name_lower);

        if (strstr(patient_name_lower, trimmed_namn) != NULL)
        {
            print_single_patient(i);
            found = true;
        }
    }
    if (!found)
    {
        printf("Namn saknas i databasen\n");
    }
}

void to_lowercase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}
bool has_bildreferens(int patient_index, int bildreferens)
{
    for (int j = 0; j < MAX_BILD; j++)
    {
        if (patient[patient_index].bildreferenser[j] == bildreferens)
        {
            return true;
        }
    }
    return false;
}
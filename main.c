#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef struct data_line
{
    char first_name[100];
    char second_name[50];
    int birth_y;
    char phone_num[12];
    char answer[5];
    char vaccinated[6];
} patient;

void list_data(char *fname)
{
    FILE *f;
    f = fopen(fname, "r");
    if (f == NULL)
    {
        perror("Nem sikerült megnyitni a file-t!\n");
        exit(1);
    }
    char line[1000];
    while (!feof(f))
    {
        fgets(line, sizeof(line), f);
        printf("%s", line);
    }
    printf("\n");
    fclose(f);
}

void add_data(char *fname)
{
    struct data_line line;
    FILE *f;
    f = fopen(fname, "a");
    if (f == NULL)
    {
        perror("Nem sikerült megnyitni a file-t!\n");
        exit(1);
    }
    printf("\nAdja meg a nevet: ");
    scanf(" %[^\n]", &line.first_name);

    printf("\nAdja meg a születési évet: ");
    scanf("%d", &line.birth_y);

    printf("\nAdja meg a telefonszámot:");
    scanf(" %[^\n]", &line.phone_num);

    printf("\nVállalja a fizetős felárat (igen\nem): ");
    scanf(" %[^\n]", &line.answer);

    fprintf(f, "\n%s %d %s %s NOPE", line.first_name, line.birth_y, line.phone_num, line.answer);
    fclose(f);
}

void modify_data(char *fname)
{
    struct data_line modified_line;
    int line_num = 0;
    int counter = 0;
    FILE *f1, *f2;
    char line[1000];
    char temp_file[] = "temp.txt";

    f1 = fopen(fname, "r");
    if (f1 == NULL)
    {
        perror("Nem sikerült megnyitni a data.txt-t!\n");
        exit(1);
    }

    f2 = fopen(temp_file, "w");
    if (f2 == NULL)
    {
        perror("Nem sikerült megnyitni a temp.txt-t!\n");
        fclose(f1);
        exit(1);
    }
    list_data(fname);
    printf("\nAdja meg a módosítani kívánt sor sorszámát: ");
    scanf("%d", &line_num);
    while (!feof(f1))
    {
        strcpy(line, "\0");
        fgets(line, sizeof(line), f1);
        counter++;
        if (counter != line_num)
        {
            fprintf(f2, "%s", line);
        }
    }
    printf("\nAdja meg a nevet: ");
    scanf(" %[^\n]", &modified_line.first_name);

    printf("\nAdja meg a születési évet: ");
    scanf("%d", &modified_line.birth_y);

    printf("\nAdja meg a telefonszámot:");
    scanf(" %[^\n]", &modified_line.phone_num);

    printf("\nVállalja a fizetős felárat (igen\nem): ");
    scanf(" %[^\n]", &modified_line.answer);

    fprintf(f2, "\n%s %d %s %s NOPE", modified_line.first_name, modified_line.birth_y, modified_line.phone_num, modified_line.answer, modified_line.vaccinated);

    fclose(f1);
    fclose(f2);
    remove(fname);
    rename(temp_file, fname);
}

void delete_data(char *fname)
{
    int line_num = 0;
    int counter = 0;
    FILE *f1, *f2;
    char line[1000];
    char temp_file[] = "temp.txt";

    f1 = fopen(fname, "r");
    if (f1 == NULL)
    {
        perror("Nem sikerült megnyitni a data.txt-t!\n");
        exit(1);
    }

    f2 = fopen(temp_file, "w");
    if (f2 == NULL)
    {
        perror("Nem sikerült megnyitni a temp.txt-t!\n");
        fclose(f1);
        exit(1);
    }
    list_data(fname);
    printf("Adja meg a törölni kívánt sor sorszámát: ");
    scanf("%d", &line_num);
    while (!feof(f1))
    {
        strcpy(line, "\0");
        fgets(line, sizeof(line), f1);
        counter++;
        if (counter != line_num)
        {
            fprintf(f2, "%s", line);
        }
    }
    fclose(f1);
    fclose(f2);
    remove(fname);
    rename(temp_file, fname);
}

int random_generator()
{
    srand(time(NULL));
    int r = rand() % 10 + 1;
    return r;
}
int needed_busses(char *fname)
{
    FILE *f;
    int num = 0;
    char temp[512];
    char *str = "NOPE";
    int bus_num = 0;
    f = fopen(fname, "r");

    if (f == NULL)
    {
        perror("Nem sikerült megnyitni a data.txt-t!\n");
        exit(1);
    }
    while (fgets(temp, 512, f) != NULL)
    {
        if ((strstr(temp, str)) != NULL)
        {
            num++;
        }
    }
    if (num > 4 && num < 10)
    {
        bus_num = 1;
    }
    if (num > 9)
    {
        bus_num = 2;
    }
    return bus_num;
}

void handler(int signumber)
{
    printf("Egy oltóbusz elindult\n", signumber);
}
void one_bus(char *fname)
{
    FILE *f;
    FILE *f2;
    char line[1000];
    char temp_file[] = "temp.txt";
    patient array[100];
    patient updated_d[100];
    int i = 0;
    int pipefd[2];
    int pipefd2[2];

    if (pipe(pipefd) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }

    f = fopen(fname, "r");
    if (f == NULL)
    {
        perror("Nem sikerült megnyitni a data.txt-t!\n");
        exit(1);
    }

    f2 = fopen(temp_file, "w");
    if (f2 == NULL)
    {
        perror("Nem sikerült megnyitni a temp.txt-t!\n");
        fclose(f);
        exit(1);
    }
    signal(SIGUSR1, handler);
    pid_t child = fork();
    if (child > 0)
    {
        pause();
        close(pipefd[0]);
        printf("Az 1-es oltóbuszhoz várjuk az alábbi embereket:\n");
        while (i < 5)
        {
            fscanf(f, "%s %s %d %s %s %s", &array[i].first_name, &array[i].second_name, &array[i].birth_y, &array[i].phone_num, &array[i].answer, &array[i].vaccinated);
            if (strcmp(array[i].vaccinated, "NOPE") == 0)
            {
                printf("%s %s\n", array[i].first_name, array[i].second_name);
                i++;
            }
        }
        for (int j = 0; j < 5; j++)
        {
            write(pipefd[1], &array[j], sizeof(patient));
        }
        close(pipefd[1]);
        close(pipefd2[1]);
        int num;
        patient p;
        read(pipefd2[0], &num, sizeof(int));
        printf("Az alábbi személyek lettek beoltva:\n");
        for (int i = 0; i < num; i++)
        {
            read(pipefd2[0], &p, sizeof(p));
            printf("%s %s\n", p.first_name, p.second_name);
            updated_d[i] = p;
        }
        close(pipefd2[0]);
        bool not_in = true;
        while (!feof(f))
        {
            not_in = true;
            strcpy(line, "\0");
            fgets(line, sizeof(line), f);
            int len = strlen(line);
            if (len > 0 && line[len - 1] == '\n')
            {
                line[len - 1] = '\0';
            }
            for (int i = 0; i < num; i++)
            {
                if ((strstr(line, updated_d[i].phone_num)) != NULL)
                {
                    not_in = false;
                }
            }
            if (not_in)
            {
                fprintf(f2, "%s", line);
            }
        }
        for (int i = 0; i < num; i++)
        {
            fprintf(f2, "\n%s %s %d %s %s OLTVA", updated_d[i].first_name, updated_d[i].second_name, updated_d[i].birth_y, updated_d[i].phone_num, updated_d[i].answer);
        }
        fclose(f2);
        remove(fname);
        rename(temp_file, fname);

        int status;
        wait(&status);
    }
    else
    {
        kill(getppid(), SIGUSR1);
        sleep(1);
        close(pipefd[1]);
        patient p;
        patient data_back[10];
        int arrived = 0;
        printf("Az oltóbusz által megkapott adatok:\n");
        for (int j = 0; j < 5; j++)
        {
            read(pipefd[0], &p, sizeof(p));
            printf("%s %s %d %s %s %s\n", p.first_name, p.second_name,
                   p.birth_y, p.phone_num, p.answer, p.vaccinated);
            if (random_generator() != 10)
            {
                data_back[arrived] = p;
                arrived++;
            }
        }
        close(pipefd[0]);
        write(pipefd2[1], &arrived, sizeof(int));
        sleep(1);
        for (int j = 0; j < arrived; j++)
        {
            write(pipefd2[1], &data_back[j], sizeof(patient));
        }
        sleep(1);
        close(pipefd2[1]);

        exit(EXIT_SUCCESS);
    }
    fclose(f);
}

void two_bus(char *fname)
{
    FILE *f;
    FILE *f2;
    char line[1000];
    char temp_file[] = "temp.txt";
    patient array[100];
    patient updated_d[100];
    int i = 0;
    int pipefd[2];
    int pipefd2[2];
    int pipefd3[2];
    int pipefd4[2];

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    pid_t child1, child2;

    if (pipe(pipefd) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd3) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd4) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    f = fopen(fname, "r");
    if (f == NULL)
    {
        perror("Nem sikerült megnyitni a data.txt-t!\n");
        exit(1);
    }
    f2 = fopen(temp_file, "w");
    if (f2 == NULL)
    {
        perror("Nem sikerült megnyitni a temp.txt-t!\n");
        fclose(f);
        exit(1);
    }
    int status;
    child1 = fork();
    if (child1 < 0)
    {
        perror("Nem sikerült fork-olni\n");
        exit(1);
    }
    if (child1 > 0) //szülő
    {
        child2 = fork();
        if (child2 < 0)
        {
            perror("Nem sikerült fork-olni\n");
            exit(1);
        }
        if (child2 > 0) //szülő
        {
            pause();
            pause();
            close(pipefd[0]);
            close(pipefd2[0]);
            while (i < 10)
            {
                fscanf(f, "%s %s %d %s %s %s", &array[i].first_name, &array[i].second_name, &array[i].birth_y, &array[i].phone_num, &array[i].answer, &array[i].vaccinated);
                if (strcmp(array[i].vaccinated, "NOPE") == 0)
                {
                    i++;
                }
            }
            printf("Az 1-es oltóbuszhoz várjuk az alábbi embereket:\n");
            for (int i = 0; i < 5; i++)
            {
                printf("%s %s\n", array[i].first_name, array[i].second_name);
            }
            for (int j = 0; j < 5; j++)
            {
                write(pipefd[1], &array[j], sizeof(patient));
            }

            printf("A 2-es oltóbuszhoz várjuk az alábbi embereket:\n");
            for (int i = 5; i < 10; i++)
            {
                printf("%s %s\n", array[i].first_name, array[i].second_name);
            }
            for (int j = 5; j < 10; j++)
            {
                write(pipefd2[1], &array[j], sizeof(patient));
            }
            close(pipefd[1]);
            close(pipefd2[1]);
            int num1;
            int num2;
            patient p;
            read(pipefd4[0], &num1, sizeof(int));
            read(pipefd3[0], &num2, sizeof(int));

            printf("Az alábbi személyek lettek beoltva az 1-es busz által:\n");
            for (int i = 0; i < num1; i++)
            {
                read(pipefd4[0], &p, sizeof(p));
                printf("%s %s\n", p.first_name, p.second_name);
                updated_d[i] = p;
            }

            printf("Az alábbi személyek lettek beoltva a 2-es busz által:\n");
            for (int i = num1; i < (num2 + num1); i++)
            {
                read(pipefd3[0], &p, sizeof(p));
                printf("%s %s\n", p.first_name, p.second_name);
                updated_d[i] = p;
            }
            close(pipefd3[0]);
            close(pipefd4[0]);
            bool not_in = true;
            while (!feof(f))
            {
                not_in = true;
                strcpy(line, "\0");
                fgets(line, sizeof(line), f);
                int len = strlen(line);
                if (len > 0 && line[len - 1] == '\n')
                {
                    line[len - 1] = '\0';
                }
                for (int i = 0; i < (num2 + num1); i++)
                {
                    if ((strstr(line, updated_d[i].phone_num)) != NULL)
                    {
                        not_in = false;
                    }
                }
                if (not_in)
                {
                    fprintf(f2, "%s", line);
                }
            }
            for (int i = 0; i < (num2 + num1); i++)
            {
                fprintf(f2, "\n%s %s %d %s %s OLTVA", updated_d[i].first_name, updated_d[i].second_name, updated_d[i].birth_y, updated_d[i].phone_num, updated_d[i].answer);
            }
            fclose(f2);
            remove(fname);
            rename(temp_file, fname);

            waitpid(child1, &status, 0);
            waitpid(child2, &status, 0);
        }
        else //child2
        {
            kill(getppid(), SIGUSR2);
            sleep(1);
            patient p;
            patient data_back[10];
            int arrived = 0;
            printf("A 2-es oltóbusz által megkapott adatok:\n");
            for (int j = 0; j < 5; j++)
            {
                read(pipefd2[0], &p, sizeof(p));
                printf("%s %s %d %s %s %s\n", p.first_name, p.second_name,
                       p.birth_y, p.phone_num, p.answer, p.vaccinated);
                if (random_generator() != 10)
                {
                    data_back[arrived] = p;
                    arrived++;
                }
            }
            write(pipefd3[1], &arrived, sizeof(int));
            sleep(1);
            for (int j = 0; j < arrived; j++)
            {
                write(pipefd3[1], &data_back[j], sizeof(patient));
            }
            close(pipefd3[1]);
            close(pipefd2[1]);
            close(pipefd2[0]);
            exit(EXIT_SUCCESS);
        }
    }
    else //child1
    {
        kill(getppid(), SIGUSR1);
        sleep(1);
        patient p;
        patient data_back[10];
        int arrived = 0;
        printf("Az 1-es oltóbusz által megkapott adatok:\n");
        for (int j = 0; j < 5; j++)
        {
            read(pipefd[0], &p, sizeof(p));
            printf("%s %s %d %s %s %s\n", p.first_name, p.second_name,
                   p.birth_y, p.phone_num, p.answer, p.vaccinated);
            if (random_generator() != 10)
            {
                data_back[arrived] = p;
                arrived++;
            }
        }
        write(pipefd4[1], &arrived, sizeof(int));
        sleep(1);
        for (int j = 0; j < arrived; j++)
        {
            write(pipefd4[1], &data_back[j], sizeof(patient));
        }

        close(pipefd4[1]);
        close(pipefd[1]);
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
    fclose(f);
}

void new_day(char *fname)
{
    int amount = needed_busses(fname);

    if (amount == 2)
    {
        two_bus(fname);
    }
    else if (amount == 1)
    {
        one_bus(fname);
    }
    else
    {
        printf("Nincs elég oltásra váró ember!");
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Megkell adnia az adatokat tároló txt állományt is!\n");
        exit(1);
    }
    int choice;
    do
    {
        printf("\n******************Felhasználói menü******************\n");
        printf("Állomány kilistázása - 1\n");
        printf("Felvétel az állományba - 2\n");
        printf("Állományban található adat módosítása - 3\n");
        printf("Állományban található adat törlése - 4\n");
        printf("Új nap kezdése - 5\n");
        printf("Kilépés - 6\n");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            needed_busses(argv[1]);
            list_data(argv[1]);
            break;
        }
        case 2:
        {
            add_data(argv[1]);
            break;
        }
        case 3:
        {
            modify_data(argv[1]);
            break;
        }
        case 4:
        {
            delete_data(argv[1]);
            break;
        }
        case 5:
        {
            new_day(argv[1]);
            break;
        }
        case 6:
        {
            exit(1);
            break;
        }
        default:
            printf("Nem megfelelő input, adja meg újra!\n");
            break;
        }
    } while (1 == 1);
}
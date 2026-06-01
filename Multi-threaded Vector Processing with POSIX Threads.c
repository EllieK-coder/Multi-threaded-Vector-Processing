#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <locale.h> // 1. Απαραίτητη βιβλιοθήκη για την εμφάνιση Ελληνικών

// Δομή για το μοίρασμα των δεδομένων σε κάθε thread 
struct ThreadData
{
    int ID;
    int *Mat;
    int From;
    int To;
};

// Ολικές μεταβλητές για το τελικό άθροισμα και το κλείδωμα
double total_sum = 0.0;
pthread_mutex_t mutex_sum;

// Δήλωση της συνάρτησης του thread
void *calc_squares(void *p);

int main(int argc, char **argv)
{
    // 2. Ενεργοποίηση ελληνικών χαρακτήρων για την κονσόλα των Windows
    setlocale(LC_ALL, "Greek");

    int N, t, i, choice;
    clock_t T1, T2;
    long total_time;
    
    // Αρχικοποίηση του Mutex
    pthread_mutex_init(&mutex_sum, NULL);
    srand(time(NULL));

    printf("=== ΘΕΜΑ Α: ΥΠΟΛΟΓΙΣΜΟΣ ΜΕΣΗΣ ΤΙΜΗΣ ΤΕΤΡΑΓΩΝΩΝ ===\n");
    printf("1. Χειροκίνητη εισαγωγή δεδομένων (για μικρά N)\n");
    printf("2. Αυτόματη παραγωγή τυχαίων αριθμών (για μεγάλα N / δοκιμές χρόνου)\n");
    printf("Επιλογή (1 ή 2): ");
    scanf("%d", &choice);

    printf("Δώστε το συνολικό πλήθος στοιχείων n: ");
    scanf("%d", &N);
    printf("Δώστε τον αριθμό των νημάτων (threads) t: ");
    scanf("%d", &t);

    // Έλεγχος αν το n είναι ακέραιο πολλαπλάσιο του t (απαίτηση εκφώνησης)
    if (N % t != 0) {
        printf("Σφάλμα: Το n πρέπει να είναι ακέραιο πολλαπλάσιο του t!\n");
        pthread_mutex_destroy(&mutex_sum);
        return 1;
    }

    // Δυναμική δέσμευση μνήμης για τον πίνακα
    int *Matrix = (int *) malloc(N * sizeof(int));

    if (choice == 1) {
        // (α) Χειροκίνητη εισαγωγή xi από τον χρήστη
        for (i = 0; i < N; i++) {
            printf("Δώστε το στοιχείο x[%d]: ", i + 1);
            scanf("%d", &Matrix[i]);
        }
    } else {
        // (β) Αυτόματο γέμισμα με τυχαίους αριθμούς (1 έως 10) για μεγάλα N
        for (i = 0; i < N; i++) {
            Matrix[i] = (rand() % 10) + 1;
        }
        printf("\nΟ πίνακας %d στοιχείων αρχικοποιήθηκε με τυχαίες τιμές.\n", N);
    }

    // Πίνακες για τα threads και τα δεδομένα τους
    pthread_t *threads = (pthread_t *) malloc(t * sizeof(pthread_t));
    struct ThreadData *dat = (struct ThreadData *) malloc(t * sizeof(struct ThreadData));
    
    // Υπολογισμός του μεγέθους του κομματιού n/t που θα πάρει κάθε thread
    int chunk_size = N / t;
    
    // Καταγραφή αρχικού χρόνου εκτέλεσης
    T1 = clock();
    
    // Δημιουργία και μοίρασμα εργασίας στα threads 
    for (i = 0; i < t; i++) {
        dat[i].ID = i + 1;
        dat[i].Mat = Matrix;
        dat[i].From = i * chunk_size;
        dat[i].To = (i + 1) * chunk_size - 1;
        
        pthread_create(&threads[i], NULL, calc_squares, &dat[i]);
    }
    
    // Αναμονή τερματισμού όλων των threads
    for (i = 0; i < t; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Καταγραφή τελικού χρόνου
    T2 = clock();
    total_time = (long)(T2 - T1);
    
    // Υπολογισμός της τελικής μέσης τιμής E = total_sum / n
    double mean_value = total_sum / N;
    
    printf("\n================ ΑΠΟΤΕΛΕΣΜΑΤΑ ================\n");
    printf("Συνολικό ’θροισμα Τετραγώνων: %.2f\n", total_sum);
    printf("Μέση Τιμή Τετραγώνων (Ε): %f\n", mean_value);
    printf("Χρόνος Εκτέλεσης (Clock Cycles): %ld\n", total_time);
    printf("==============================================\n");
    
    // Αποδέσμευση πόρων
    pthread_mutex_destroy(&mutex_sum);
    free(Matrix);
    free(threads);
    free(dat);
    
    // Αν τρεξει στο Linux, η παρακάτω εντολή system("pause") απλά αγνοείται χωρίς πρόβλημα
    system("pause"); 
    return 0;
}

// Η συνάρτηση εκτέλεσης του thread
void *calc_squares(void *p)
{
    struct ThreadData *t = (struct ThreadData *) p;
    double local_sum = 0.0;
    int i;
    
    // (α) Κάθε thread υπολογίζει τοπικά το δικό του άθροισμα σε τοπική μεταβλητή
    for (i = t->From; i <= t->To; i++) {
        local_sum += (double)t->Mat[i] * (double)t->Mat[i];
    }
    
    // (β)  Χρήση Mutex για την ασφαλή ενημέρωση της κοινής μεταβλητής total_sum 
    pthread_mutex_lock(&mutex_sum);
    total_sum += local_sum;
    pthread_mutex_unlock(&mutex_sum);
    
    return NULL;
}

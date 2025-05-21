#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <semaphore.h>

#define KAT_SAYISI 10
#define DAIRE_SAYISI 4

// Mutex: Aynı katta elektrik/su döşeme çakışmasın diye
pthread_mutex_t kaynak_mutex;

// Semafor: Asansör gibi ortak kaynak erişimi (örnek)
sem_t asansor;

void* daire_insa_et(void* arg) {
    int daire_no = *((int*)arg);

    // Asansöre sırayla binsinler
    sem_wait(&asansor);
    printf("\t[Daire %d] Asansör ile malzeme taşınıyor...\n", daire_no);
    sleep(1);
    sem_post(&asansor);

    // Ortak kaynak kullanımı (örneğin elektrik tesisatı)
    pthread_mutex_lock(&kaynak_mutex);
    printf("\t[Daire %d] Elektrik/su tesisatı döşeniyor...\n", daire_no);
    sleep(1);
    pthread_mutex_unlock(&kaynak_mutex);

    printf("\t[Daire %d] İç tasarım yapılıyor...\n", daire_no);
    sleep(1);

    pthread_exit(NULL);
}

void kat_insa_et(int kat_no) {
    printf("[Kat %d] İnşaata başlandı.\n", kat_no);

    pthread_t daireler[DAIRE_SAYISI];
    int daire_nolar[DAIRE_SAYISI];

    for (int i = 0; i < DAIRE_SAYISI; i++) {
        daire_nolar[i] = kat_no * 10 + i + 1; // Örn: 11, 12, 13, 14
        pthread_create(&daireler[i], NULL, daire_insa_et, &daire_nolar[i]);
    }

    for (int i = 0; i < DAIRE_SAYISI; i++) {
        pthread_join(daireler[i], NULL);
    }

    printf("[Kat %d] İnşaat tamamlandı.\n", kat_no);
}

int main() {
    pthread_mutex_init(&kaynak_mutex, NULL);
    sem_init(&asansor, 0, 1); // 1 kişilik asansör

    printf("[Temel] Temel kazılıyor...\n");
    sleep(2);
    printf("[Temel] Temel tamamlandı.\n\n");

    for (int kat = 1; kat <= KAT_SAYISI; kat++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork hatası");
            exit(1);
        } else if (pid == 0) {
            // Çocuk process: kat inşaatı
            kat_insa_et(kat);
            exit(0);
        } else {
            // Ana process: sıradaki kata geçmeden önce bekler
            wait(NULL);
        }
    }

    pthread_mutex_destroy(&kaynak_mutex);
    sem_destroy(&asansor);

    printf("\n[Tamamlandı] Bina inşaatı başarıyla bitti.\n");
    return 0;
}

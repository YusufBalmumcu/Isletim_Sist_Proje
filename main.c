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

    // İnşaat öncesi hazırlıklar
    sem_wait(&asansor);
    printf("\t[Daire %d] Asansör ile malzeme taşınıyor...\n", daire_no);
    usleep(100000);
    sem_post(&asansor);

    printf("\t[Daire %d] İskele kuruluyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Malzemeler yerleştiriliyor...\n", daire_no);
    usleep(100000);

    // Yapısal işlemler (paralel)
    printf("\t[Daire %d] Duvarlar örülüyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Kapı/pencere kasaları yerleştiriliyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] İç bölme duvarları yapılıyor...\n", daire_no);
    usleep(100000);

    // Altyapı sistemleri (mutex ile sıralı)
    pthread_mutex_lock(&kaynak_mutex);
    printf("\t[Daire %d] Su boruları döşeniyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Elektrik kabloları çekiliyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Doğalgaz boruları döşeniyor...\n", daire_no);
    usleep(100000);
    pthread_mutex_unlock(&kaynak_mutex);

    // Kaplama işlemleri (paralel)
    printf("\t[Daire %d] Sıva işlemleri yapılıyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Fayans/seramik döşeniyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Boyama işlemleri yapılıyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Parke döşeniyor...\n", daire_no);
    usleep(100000);

    // Donanım kurulumu
    sem_wait(&asansor); // Cam montajında vinç/asansör kullanımı varsayalım
    printf("\t[Daire %d] Camlar takılıyor...\n", daire_no);
    usleep(100000);
    sem_post(&asansor);

    printf("\t[Daire %d] Kapı montajı yapılıyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Mutfak dolapları yerleştiriliyor...\n", daire_no);
    usleep(100000);

    // Son rötuşlar
    sem_wait(&asansor); // Mobilya da asansörle taşınıyor
    printf("\t[Daire %d] Mobilyalar taşınıyor...\n", daire_no);
    usleep(100000);
    sem_post(&asansor);

    printf("\t[Daire %d] Aydınlatma montajı yapılıyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Priz ve anahtarlar monte ediliyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Temizlik ve son kontroller yapılıyor...\n", daire_no);
    usleep(100000);

    printf("\t[Daire %d] Daire inşası tamamlandı.\n", daire_no);

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

    for (int kat = 1; kat <= KAT_SAYISI; kat++) {
        // Temel sadece ilk kat için kazılır
        if (kat == 1) {
            printf("[Temel] Temel kazılıyor...\n");
            sleep(2);
            printf("[Temel] Temel tamamlandı.\n\n");
        }

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


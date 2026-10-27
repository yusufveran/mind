/*basit zeka test bölümü*/

#include <stdio.h>
#include <string.h> 

#define GIRDI_BOYUT 100   //main'in klavyeden okuduğu satırın boyutu
#define CEVAP_BOYUT 300   //cevap dizisinin boyutu (dosyadan gelen cevap + önündeki yazı sığsın diye daha büyük)

/*durumlar: konuşmanın neresindeyiz, yani bir sonraki satırın ne anlama geleceği*/
#define BOSTA           0   //yeni bir komut ya da soru bekleniyor
#define AD_BEKLE        1   //admin girişi: kullanıcı adı bekleniyor
#define SIFRE_BEKLE     2   //admin girişi: şifre bekleniyor
#define UNUT_SORU       3   //unut: silinecek soru bekleniyor
#define GUNCELLE_SORU   4   //güncelle: cevabı değişecek soru bekleniyor
#define GUNCELLE_CEVAP  5   //güncelle: yeni cevap bekleniyor
#define EVET_HAYIR      6   //öğrenme: "biliyor musun?" sorusunun cevabı bekleniyor
#define OGRET_CEVAP     7   //öğrenme: öğretilecek cevap bekleniyor

/*isle'nin çağrılar arasında unutmaması gereken her şey*/
struct oturum {
    int durum;                  //yukarıdaki durumlardan biri
    int onay;                   //admin girişi onayı
    char bekleyen[GIRDI_BOYUT]; //bir sonraki adıma kadar saklanan şey
};

/*fonksiyonlar*/
    int isle(struct oturum *o, char girdi[], char cevap[]);
    void temizle(char s[]);
    void normallestir(char s[], int boyut);
    int bul(char soru[], char cevap[]);
    int giris_kontrol(char ad[], char sifre[]);
    void kaydet(char soru[], char cevap[]);
    int hesapla(char soru[], char cevap[]);
    void oneri(char soru[], char cevap[]);
    int unut(char soru[]);
    int guncelle(char soru[], char yeni_cevap[]);

/*bilgi kayıt dosyaları*/

    FILE *kayit_dosyasi;
    FILE *admin;
    FILE *bekleme;

/*frontend: satırı okur, isle'ye verir, cevabı ekrana yazar. hiçbir karar vermez*/
/*10.4'te fgets yerine "ağdan oku", printf yerine "ağa yaz" gelecek, isle değişmeyecek*/
int main(){

    struct oturum o = {0};   //durum = BOSTA, onay = 0
    char girdi[GIRDI_BOYUT];
    char cevap[CEVAP_BOYUT];

    printf("bugün ne öğrenmek istersiniz?\n");

    while (1) //sonsuz döngü
    {
        printf("> ");

        if (fgets(girdi,sizeof(girdi),stdin) == NULL)
        {
            break; //NULL değerlerde program güvenlik için kendini durdurur
        }

        if (isle(&o,girdi,cevap) == 0)   //0 = kullanıcı çıkmak istedi
        {
            break;
        }

        if (cevap[0] != '\0')   //boş girdide söylenecek bir şey yok
        {
            printf("%s\n",cevap);
        }
    }
    return 0;
}

/*backend: bir satır alır, ekranda görünecek cevabı cevap dizisine yazar*/
/*ekrana yazmaz, klavyeden okumaz. program bitmeliyse 0, devam etmeliyse 1 döndürür*/
int isle(struct oturum *o, char girdi[], char cevap[]){

    temizle(girdi);
    cevap[0] = '\0';   //hiçbir şey yazmazsak cevap boş kalsın

    /*önce: bir şey bekliyor muyuz?*/

    if (o->durum == AD_BEKLE)   //girdi = kullanıcı adı
    {
        strcpy(o->bekleyen,girdi);   //girdi bir sonraki turda silineceği için şifre gelene kadar saklıyoruz
        strcpy(cevap,"şifreniz:");
        o->durum = SIFRE_BEKLE;
    }
    else if (o->durum == SIFRE_BEKLE)   //girdi = şifre, ad bekleyen'de
    {
        o->onay = 0;   //yeni giriş denemesi, önceki girişi sayma

        int sonuc = giris_kontrol(o->bekleyen,girdi);
        if (sonuc == 1)
        {
            o->onay = 1;
            strcpy(cevap,"admin olarak giriş yapıldı");
        }
        else if (sonuc == -1)
        {
            strcpy(cevap,"yetkililer dosyası bulunamadı");
        }
        else
        {
            strcpy(cevap,"giriş başarısız");
        }
        o->durum = BOSTA;
    }
    else if (o->durum == UNUT_SORU)   //girdi = silinecek soru
    {
        normallestir(girdi,GIRDI_BOYUT);   //kayıttakine uygun girdi

        if (strcmp(girdi,"\n") == 0)
        {
            strcpy(cevap,"soru boş olamaz");
        }
        else if (unut(girdi) == 1)
        {
            strcpy(cevap,"soru unutuldu");
        }
        else
        {
            strcpy(cevap,"bu soru kayıtlı değil");
        }
        o->durum = BOSTA;
    }
    else if (o->durum == GUNCELLE_SORU)   //girdi = cevabı değişecek soru
    {
        normallestir(girdi,GIRDI_BOYUT);   //kayıtlar normalleştirilmiş halde duruyor
        char eski_cevap[GIRDI_BOYUT];

        if (strcmp(girdi,"\n") == 0)
        {
            strcpy(cevap,"soru boş olamaz");
            o->durum = BOSTA;
        }
        else if (bul(girdi,eski_cevap) == 0)   //kayıtlı değilse yeni cevabı boşuna sormuyoruz
        {
            strcpy(cevap,"bu soru kayıtlı değil, güncellenemedi");
            o->durum = BOSTA;
        }
        else
        {
            strcpy(o->bekleyen,girdi);   //yeni cevap gelene kadar soruyu saklıyoruz
            snprintf(cevap,CEVAP_BOYUT,"şu anki cevabı: %s\nyeni cevap nedir?",eski_cevap);
            o->durum = GUNCELLE_CEVAP;
        }
    }
    else if (o->durum == GUNCELLE_CEVAP)   //girdi = yeni cevap, soru bekleyen'de
    {
        if (strcmp(girdi,"\n") == 0)
        {
            strcpy(cevap,"cevap boş olamaz");
        }
        else if (guncelle(o->bekleyen,girdi) == 1)
        {
            strcpy(cevap,"cevap güncellendi");
        }
        else
        {
            strcpy(cevap,"bu soru kayıtlı değil, güncellenemedi");
        }
        o->durum = BOSTA;
    }
    else if (o->durum == EVET_HAYIR)   //girdi = "biliyor musun?" sorusunun cevabı
    {
        if (strcmp(girdi,"evet\n") == 0)
        {
            strcpy(cevap,"cevap nedir?");
            o->durum = OGRET_CEVAP;
        }
        else if (strcmp(girdi,"hayır\n") == 0)
        {
            strcpy(cevap,"tamam");
            o->durum = BOSTA;
        }
        else
        {
            /*durumu değiştirmiyoruz: bir sonraki satır yine evet/hayır sayılacak*/
            /*ogren'deki while(1) döngüsünün yaptığı işi artık bu yapıyor*/
            strcpy(cevap,"yanlış girdi tekrar deneyiniz (evet veya hayır):");
        }
    }
    else if (o->durum == OGRET_CEVAP)   //girdi = öğretilen cevap, soru bekleyen'de
    {
        if (strcmp(girdi,"\n") == 0)   //boş cevap kaydetmiyoruz, cevap beklemeye devam
        {
            strcpy(cevap,"cevap boş olamaz, tekrar yazınız:");
        }
        else
        {
            kaydet(o->bekleyen,girdi);
            strcpy(cevap,"öğrendim, teşekkürler");
            o->durum = BOSTA;
        }
    }

    /*hiçbir şey beklemiyoruz (BOSTA): yeni bir komut ya da soru geldi*/

    else
    {
        normallestir(girdi,GIRDI_BOYUT);

        if (strcmp(girdi,"\n") == 0)   //boş girdi, cevap boş kalıyor
        {
            return 1;
        }

        if (strcmp(girdi,"çık\n") == 0)   //kullanıcının çıkış kapısı
        {
            return 0;   //break'i main yapacak
        }

        if (strcmp(girdi,"admin\n") == 0 || strcmp(girdi,"admın\n") == 0)   //admin giriş portalı
        {
            strcpy(cevap,"kullanıcı adınız:");
            o->durum = AD_BEKLE;
        }
        else if (strcmp(girdi,"unut\n") == 0)   //admin only
        {
            if (o->onay != 1)
            {
                strcpy(cevap,"bu komut için admin girişi gerekli");
            }
            else
            {
                strcpy(cevap,"hangi soruyu unutayım?:");
                o->durum = UNUT_SORU;
            }
        }
        else if (strcmp(girdi,"güncelle\n") == 0)   //admin only
        {
            if (o->onay != 1)
            {
                strcpy(cevap,"bu komut için admin girişi gerekli");
            }
            else
            {
                strcpy(cevap,"hangi sorunun cevabını güncelleyelim?:");
                o->durum = GUNCELLE_SORU;
            }
        }
        else if (hesapla(girdi,cevap) == 1)   //matematik sorusu: cevap hemen hazır, durum değişmez
        {
        }
        else
        {
            char bulunan[GIRDI_BOYUT];

            if (bul(girdi,bulunan) == 1)   //kayıtlı soru: cevap hemen hazır, durum değişmez
            {
                snprintf(cevap,CEVAP_BOYUT,"sorunuzun cevabı =:%s",bulunan);
            }
            else if (o->onay == 1)   //admin only: öğrenmeye başla
            {
                strcpy(o->bekleyen,girdi);   //cevap iki satır sonra gelecek, soruyu o zamana kadar saklıyoruz
                strcpy(cevap,"bu sorunun cevabını bilmiyorum ama sen söylersen öğrenebilirim;\n"
                             "bu sorunun cevabını biliyor musun? (evet veya hayır):");
                o->durum = EVET_HAYIR;
            }
            else   //users
            {
                oneri(girdi,cevap);
            }
        }
    }

    return 1;
}

/*fazla boşlukları kaldırma*/
void temizle(char s[]){

    int ilk_uzunluk = strlen(s);
    int uzunluk = ilk_uzunluk;

    
    if (uzunluk > 0 && s[uzunluk-1] == '\n')
    {
        uzunluk--;
    }

    
    while (uzunluk > 0 && s[uzunluk-1] == ' ')
    {
        uzunluk--;
    }

    
    if (uzunluk < ilk_uzunluk)
    {
        s[uzunluk] = '\n';
        s[uzunluk+1] = '\0';
    }

}

/*büyük harf kelimeler arası fazla boşluk ve türkçe karakter içeren soruları tek tip soruya döüştürür*/
/*made by claude code */

void normallestir(char s[], int boyut){  

    char yeni[200];
    int y = 0;   //yeni dizide sıradaki boş yer

    for (int i = 0; s[i] != '\0' && s[i] != '\n'; i++)
    {
        unsigned char c = s[i];
        unsigned char sonraki = s[i+1];
        char harf[2];   //bu turda yeni diziye eklenecek byte'lar
        int n = 1;      //kaç byte eklenecek

        if (c == ' ' || c == '\t')
        {
            if (y == 0 || yeni[y-1] == ' ')   //baştaki boşluk ya da üst üste ikinci boşluk: atla
            {
                continue;
            }
            harf[0] = ' ';
        }
        else if (c == 'I')   //Türkçede I'nın küçüğü ı (0xC4 0xB1)
        {
            harf[0] = (char)0xC4; harf[1] = (char)0xB1; n = 2;
        }
        else if (c >= 'A' && c <= 'Z')   //diğer İngilizce büyük harfler
        {
            harf[0] = c + ('a' - 'A');
        }
        else if (c == 0xC4 && sonraki == 0xB0)   //İ -> i
        {
            harf[0] = 'i';
            i++;   //ikinci byte'ı da okuduk
        }
        else if (c == 0xC3 && (sonraki == 0x87 || sonraki == 0x96 || sonraki == 0x9C))   //Ç Ö Ü -> ç ö ü
        {
            harf[0] = c; harf[1] = sonraki + 0x20; n = 2;
            i++;
        }
        else if ((c == 0xC4 || c == 0xC5) && sonraki == 0x9E)   //Ğ Ş -> ğ ş
        {
            harf[0] = c; harf[1] = 0x9F; n = 2;
            i++;
        }
        else if (c >= 0xC0 && (sonraki & 0xC0) == 0x80)   //diğer 2 byte'lık harfler (ç, ş, é...) olduğu gibi
        {
            harf[0] = c; harf[1] = sonraki; n = 2;
            i++;
        }
        else
        {
            harf[0] = c;
        }

        if (y + n > boyut - 2)   //\n ve \0 için yer kalmadı; harfi yarım bırakmamak için burada duruyoruz
        {
            break;
        }
        for (int k = 0; k < n; k++)
        {
            yeni[y] = harf[k];
            y++;
        }
    }

    /*sondaki boşlukları ve ? ! . işaretlerini atıyoruz ("ankara kaç harfli ?!" gibi)*/
    while (y > 0 && (yeni[y-1] == ' ' || yeni[y-1] == '?' || yeni[y-1] == '!' || yeni[y-1] == '.'))
    {
        y--;
    }

    yeni[y] = '\n';
    yeni[y+1] = '\0';
    strcpy(s,yeni);   //y en fazla boyut-2 olduğu için s'ye sığıyor
}

/*soru kayıt dosyasında varsa cevabını (sonundaki \n olmadan) cevap'a yazıp 1 döndürür, yoksa 0 döndürür*/
/*eskiden main'in içindeki arama döngüsüydü; unut'taki gibi her turda iki satır okuyoruz: soru ve altındaki cevap*/
int bul(char soru[], char cevap[])
{
    FILE *dosya = fopen("kayıt dosyası","r");
    if (dosya == NULL)   //bu daha önce hiç kaydın yapılmadığı anlamına geliyor
    {
        return 0;
    }

    char satır[100];
    char alt[100];
    int bulundu = 0;

    while (fgets(satır,sizeof(satır),dosya) != NULL)   //bu satır soru
    {
        if (fgets(alt,sizeof(alt),dosya) == NULL) {break;}   //altındaki satır cevap; cevabı olmayan son satırda dur

        if (strcmp(satır,soru) == 0)
        {
            strcpy(cevap,alt);
            int n = strlen(cevap);
            if (n > 0 && cevap[n-1] == '\n') {cevap[n-1] = '\0';}   //\n'yi main ekrana yazarken kendisi koyuyor
            bulundu = 1;
            break;
        }
    }

    fclose(dosya);
    return bulundu;
}

/*ad ve şifre yetkililer dosyasında eşleşiyorsa 1, eşleşmiyorsa 0, dosya yoksa -1 döndürür*/
/*eskiden main'deki admin bloğunun içindeydi*/
int giris_kontrol(char ad[], char sifre[])
{
    admin = fopen("yetkililer","r");
    if (admin == NULL)
    {
        return -1;
    }

    char okuyucu[100];
    char kayitli_sifre[100];
    int sonuc = 0;

    while (fgets(okuyucu,sizeof(okuyucu),admin) != NULL)   //kullanıcı ad
    {
        if (fgets(kayitli_sifre,sizeof(kayitli_sifre),admin) == NULL) {break;}   //sifre

        if (strcmp(ad,okuyucu) == 0)
        {
            if (strcmp(sifre,kayitli_sifre) == 0)
            {
                sonuc = 1;
            }
            break;
        }
    }
    fclose(admin);

    return sonuc;
}

/*soru ve cevabı kayıt dosyasının sonuna ekler (eskiden ogren'in "evet" kolundaydı)*/
void kaydet(char soru[], char cevap[])
{
    /*dosyada her soru ve cevap ayrı satırda olmalı, \n yoksa ekliyoruz (yoksa sonraki kayıtla aynı satıra yapışır)*/
    kayit_dosyasi = fopen("kayıt dosyası","a");
    if (kayit_dosyasi == NULL)
    {
        return;
    }
    fputs(soru,kayit_dosyasi);
    if (soru[strlen(soru)-1] != '\n') {fputs("\n",kayit_dosyasi);}
    fputs(cevap,kayit_dosyasi);
    if (cevap[strlen(cevap)-1] != '\n') {fputs("\n",kayit_dosyasi);}
    fclose(kayit_dosyasi);
}

/*matematik sorusuysa sonucu cevap'a yazıp 1 döndürür, değilse 0 döndürür*/
/*snprintf printf gibi çalışır ama ekrana değil cevap dizisine yazar; CEVAP_BOYUT da fgets'teki sizeof gibi sınır*/
int hesapla(char soru[], char cevap[]){

    int s1,s2;
    char i;
    int son;   //sscanf'in soruda nereye kadar okuduğu

    /*sscanf kaç parça okuyabildiğini döndürür: "42+9" için 3, "ankara kaç harfli" için 0*/
    /*" %c" deki boşluk "42 + 9" gibi boşluklu yazımda işaretten önceki boşluğu atlar*/
    /*%n ise sona kadar okuyup okumadığımızı anlamak için: "3-5 yaş arası..." matematik sorusu değil*/
    int okunan = sscanf(soru,"%d %c%d %n",&s1,&i,&s2,&son);

    if (okunan != 3 || soru[son] != '\0')
    {
        return 0;
    }

    if (i == '+')
    {
        snprintf(cevap,CEVAP_BOYUT,"sorunuzun cevabı =:%d",s1+s2);
    }
    else if (i == '-')
    {
        snprintf(cevap,CEVAP_BOYUT,"sorunuzun cevabı =:%d",s1-s2);
    }
    else if (i == '*')
    {
        snprintf(cevap,CEVAP_BOYUT,"sorunuzun cevabı =:%d",s1*s2);
    }
    else if (i == '/')
    {
        if (s2 == 0)
        {
            strcpy(cevap,"sıfıra bölme tanımsızdır");
        }
        else
        {
            snprintf(cevap,CEVAP_BOYUT,"sorunuzun cevabı =:%d",s1/s2);
        }
    }
    else
    {
        return 0;   //tanımadığımız bir işaret ("5x3" gibi), normal soru gibi davran
    }

    return 1;
}

void oneri(char soru[], char cevap[])
{
    /*aynı soru daha önce önerildiyse listeye tekrar eklemiyoruz*/
    bekleme = fopen("öneriler","r");
    if (bekleme != NULL)
    {
        char satır[100];
        while (fgets(satır,sizeof(satır),bekleme) != NULL)
        {
            if (strcmp(satır,soru) == 0)
            {
                fclose(bekleme);
                strcpy(cevap,"sorunuzun cevabı sistemde yok, bu soru zaten beklenenler listesinde");
                return;
            }
        }
        fclose(bekleme);
    }

    strcpy(cevap,"sorunuzun cevabı sistemde yok ama sorunuz beklenenler listesine alındı");
    bekleme = fopen("öneriler","a");
    if (bekleme == NULL)
    {
        return;
    }
    fputs(soru,bekleme);
    if (soru[strlen(soru)-1] != '\n') {fputs("\n",bekleme);}
    fclose(bekleme);
}

/*soru kayıtlıysa soruyu ve cevabını silip 1 döndürür, kayıtlı değilse 0 döndürür*/
/*dosyanın ortasından satır silinemediği için silinecek çift hariç her şeyi geçici bir dosyaya kopyalıyoruz,*/
/*sonra geçici dosyayı asıl dosyanın yerine koyuyoruz*/
int unut(char soru[])
{
    FILE *eski = fopen("kayıt dosyası","r");
    if (eski == NULL)   //hiç kayıt yoksa silinecek bir şey de yok
    {
        return 0;
    }

    FILE *yeni = fopen("kayıt dosyası.tmp","w");
    if (yeni == NULL)
    {
        fclose(eski);
        return 0;
    }

    char okuma[100];
    char cevap[100];
    int bulundu = 0;

    while (fgets(okuma,sizeof(okuma),eski) != NULL)   //bu satır soru
    {
        if (fgets(cevap,sizeof(cevap),eski) == NULL)   //cevabı olmayan son satır: olduğu gibi bırakıp duruyoruz
        {
            fputs(okuma,yeni);
            break;
        }

        if (bulundu == 0 && strcmp(okuma,soru) == 0)
        {
            bulundu = 1;   //bu çifti yeni dosyaya hiç yazmıyoruz, silinmiş oluyor
        }
        else
        {
            fputs(okuma,yeni);   //diğer kayıtlar aynen kopyalanıyor
            fputs(cevap,yeni);
        }
    }

    fclose(eski);
    fclose(yeni);

    if (bulundu == 1)
    {
        rename("kayıt dosyası.tmp","kayıt dosyası");   //guncelle'deki gibi: remove yerine doğrudan rename
    }
    else
    {
        remove("kayıt dosyası.tmp");   //değişiklik yok, geçici dosyayı siliyoruz
    }

    return bulundu;
}

/*soru kayıtlıysa cevabını yeni_cevap ile değiştirip 1 döndürür, kayıtlı değilse 0 döndürür*/
/*dosyanın ortasındaki bir satır doğrudan değiştirilemediği için her şeyi geçici bir dosyaya kopyalıyoruz,*/
/*eşleşen sorunun altına eski cevap yerine yenisini yazıyoruz, sonra geçici dosyayı asıl dosyanın yerine koyuyoruz*/
int guncelle(char soru[], char yeni_cevap[])
{
    FILE *eski = fopen("kayıt dosyası","r");
    if (eski == NULL)   //hiç kayıt yoksa güncellenecek bir şey de yok
    {
        return 0;
    }

    FILE *yeni = fopen("kayıt dosyası.tmp","w");
    if (yeni == NULL)
    {
        fclose(eski);
        return 0;
    }

    char satır[100];
    char cevap[100];
    int bulundu = 0;

    while (fgets(satır,sizeof(satır),eski) != NULL)   //bu satır soru
    {
        fputs(satır,yeni);   //soru her durumda aynen kalıyor

        if (fgets(cevap,sizeof(cevap),eski) == NULL) {break;}   //altındaki satır cevap; cevabı olmayan son satırda dur

        if (bulundu == 0 && strcmp(satır,soru) == 0)
        {
            fputs(yeni_cevap,yeni);   //eski cevabı atlayıp yenisini yazıyoruz
            if (yeni_cevap[strlen(yeni_cevap)-1] != '\n') {fputs("\n",yeni);}
            bulundu = 1;
        }
        else
        {
            fputs(cevap,yeni);   //diğer kayıtların cevabı aynen kopyalanıyor
        }
    }

    fclose(eski);
    fclose(yeni);

    if (bulundu == 1)
    {
        /*rename var olan dosyanın üzerine yazar; önce remove yapsaydık, ikisinin arasında program*/
        /*kapanırsa elimizde hiç kayıt dosyası kalmazdı. rename ile ya eski ya yeni dosya her zaman duruyor*/
        rename("kayıt dosyası.tmp","kayıt dosyası");
    }
    else
    {
        remove("kayıt dosyası.tmp");   //değişiklik yok, geçici dosyayı siliyoruz
    }

    return bulundu;
}

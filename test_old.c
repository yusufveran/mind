/* basit zeka test bölümü*/

#include <stdio.h>
#include <string.h> //string fonksiyonlar

/*kullanılan fonksiyonlarımız açıklamaları alt satırda*/
void temizle(char s[]);

/*0. bilgilerimizi kaydettiğimiz bir yapı oluşturduk*/

/*0.1 bilgileri kaydedeceğimiz dosyaların işaretçileri*/

FILE *sorular;
FILE *cevaplar;

//eski struct

struct depo
{
    char soru[200]; //soruların  tutulduğu kısım
    char bilgi[200]; //cevapların tutulduğu kısım
    

}kayitlar[300]; // tek tek değişken girmek yerine bu yapıyı kullanacağız

int kayitsayisi = 0;
int c;
char s[200];


int main(){

    while (1) //sonsuz döngü
    {
        
    
    /*1. önce kullanıcıdan ne istediğini soralım*/
    
    printf("\n\nbugün ne öğrenmek istersiniz: ");
    

    /*2. şimdi ise kullanıcının sorduğu soruyu okuyalım*/

    /*2.1 soruları sorular adlı kayıt dosyamızdan okuyalım*/

    scanf("%[^\n]",s);
    if (strcmp(s,"çık") == 0)
    {
        break;
    }

    int sayac = 0;

    fopen("sorular","r");

    int z; for (z = 0; z < /*kayitlarin sayısı*/; z++)
    {
        if (strcmp(s,/*kayıtlı soruları satır satır karşılaştırmamız lazım*/) == 0)
        {
            sayac = 1;
            break; // bu sayede z yi yani satır sayısını kaydedeceğiz
        }        
    }
    
    fclose(sorular);

   


    //eski okuyucu

    scanf("%[^\n]",kayitlar[kayitsayisi].soru); 
     while ((c = getchar()) != '\n' && c != EOF);
    temizle(kayitlar[kayitsayisi].soru);
    if (strcmp(kayitlar[kayitsayisi].soru, "çık") == 0)
        {
            break;   // döngüden çık, programı bitir
        }
        

    
    /*3. şimdi ise bu bilginin sistemde kayıtlı olup olmadığına bakmamız lazım*/
   
    int i;

    /*kayıtın bulunup bulunmadığını kontrol eden bir şeye ihtiyacımız var*/

    /*3.1 kayıt dosyamızdan sorunun kayıtlı olup olmadığına bakacağız*/

    if (sayac == 1)
    {
        fopen("cevaplar","r");
        /*burda satır sayısına göre (yani z değişkeni) bir cevap seçmemiz lazım ama bunu nasıl yapacağım bilmiyorum*/
        fclose(cevaplar);
    }

    if (sayac == 0)
    {
        printf("bu sorunun cevabını bilmiyorum ama sen biliyorsan öğrenebilirim;\n");
        printf("sorunun cevabını biliyor musun(evet veya hayır): \n");
        char kullanıcı_cevap[10];
        scanf("%[^\n]",kullanıcı_cevap);

        if (kullanıcı_cevap == "evet")
        {
            char cvp[200];
            while ((c = getchar()) != '\n' && c != EOF);
            cevaplar = fopen("cevaplar","w");
            printf("cevabı giriniz: ");
            scanf("%s",cvp);
            fputs(cvp,cevaplar);
            fclose(cevaplar);

            /*burdada bir hata ve yanlış olduğunu düşünüyorum ama tam açıklayamam*/

        }
        
    }
    

    //eski kayit kontrolcüsü

    int bulundumu = 0;  //başlangıçta birşey bulmadı daha o yüzden 0

    for (i = 0; i < kayitsayisi; i++)
    {
        if ((strcmp(kayitlar[kayitsayisi].soru,kayitlar[i].soru)) == 0)
    {
        printf("cevap: ");
        printf("%s",kayitlar[i].bilgi);
        bulundumu = 1 ;
        
    } 
    }

    if (bulundumu == 0) //4. bulunup bulunmadığına göre sistem soruyu ve onun cevabını isteyecektir
    {
    
      printf("bu sorunun cevabını bilmiyorum ama sen biliyorsan öğrenebilirim;\n");
      printf("sorunun cevabını biliyor musun(evet veya hayır): \n");
      char kullanıcı_cevap[10];
      scanf("%[^\n]",kullanıcı_cevap);
      if ((strcmp(kullanıcı_cevap,"evet")) == 0)
      {
         while ((c = getchar()) != '\n' && c != EOF);
        printf("cevabı giriniz: ");
        scanf("%[^\n]",kayitlar[kayitsayisi].bilgi);
        kayitsayisi++;
        while ((c = getchar()) != '\n' && c != EOF);

      }
       else if ((strcmp(kullanıcı_cevap,"hayır")) == 0)
       {
         while ((c = getchar()) != '\n' && c != EOF);
        continue;      
       }
       else{
         while ((c = getchar()) != '\n' && c != EOF);
        printf("yanlış girdi");
       }
       
    }
    
}
    return 0;

}

void temizle(char s[]){

    /*satır sonu boşluk temizleme aracımız*/
    
    int uzunluk = strlen(s);
    --uzunluk;

    while (s[uzunluk] == ' ' && uzunluk > 0)
    {
        s[uzunluk] = '\0';
        uzunluk--;
    }    

}
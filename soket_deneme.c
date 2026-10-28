#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int sayac = 0;

int main(void)
{
    struct addrinfo ipucu, *liste, *p;
    /*adres bilgilerimiz bu struct'ta kayıtlı*/

    memset(&ipucu, 0,sizeof(ipucu));
    /*stuct içini temizliyoruz*/

    ipucu.ai_family = AF_UNSPEC;
    /*IPV4 ve IPV6 farketmez*/

    ipucu.ai_socktype = SOCK_STREAM;
    /*TCP protokolü*/

    ipucu.ai_flags = AI_PASSIVE;
    /*sokete dinleme rolünü(server) veriyoruz*/

    int r = getaddrinfo(NULL,"3490",&ipucu,&liste);
    /*3490 portuna localhost olarak bağlıyoruz*/
    /*aynı zamanda addrinfo ya adres bilgisini veriyoruz*/

    if (r != 0)
    {
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(r));
        /*hata mesajı*/
        return 1;
    }

    int fd;

    for (p = liste;p != NULL; p = p->ai_next)
    {
        fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        /*socketi(uç) yerleştiriyoruz*/
        
        if (fd == -1)
        {
            perror("socket");
            continue;
        }
        
        int evet = 1;
        
        setsockopt(fd, SOL_SOCKET,SO_REUSEADDR, &evet,sizeof(evet));
        /*soketin ayarlarını düzeltiyoruz*/

        if (bind(fd,p->ai_addr,p->ai_addrlen) == -1)
        {
            /*soketi porta bağlıyoruz şuan temelde sunucumuz oluştu*/

            perror("bind");
            close(fd);
            continue;
        }
        

        printf("soket acildi, fd=%d\n",fd);
        break;
    }

    freeaddrinfo(liste);

    if (p == NULL)
    {
    fprintf(stderr,"hicbirine baglanamadım\n");
    /*hata mesajı*/
    return 1;
    }

    if (listen(fd,10) == -1)
    {
        perror("listen");
        close(fd);
        return 1;
    }
   
    while (1)
    {
    
    printf("dinliyorum (fd=%d)\n",fd);
    int baglanan = accept(fd,NULL,NULL);
    if (baglanan == -1)
    {
        perror("accept");
        close(fd);
        return 1;
    }

    printf("biri baglandi (fd=%d)\n",baglanan);
    sayac++;
    printf("%d. bağlantı : ",sayac);
    send(baglanan,"hoş geldin\n",strlen("hoş geldin\n"),0);
    char tampon[256];
    
    while (1)
    {
        int n = recv(baglanan,tampon,sizeof(tampon)-1,0);
        if (n == -1)
        {
            perror("recv");
            break;
        }
        if (n == 0)
        {
            printf("karşı taraf bağlantıyı kesti\n");
            break;
        }
        tampon[n] = '\0' ;
        printf("%s",tampon);
        send(baglanan,tampon,n,0);
        fflush(stdout);
    }
    close(baglanan);
    }

    close(fd);

    return 0;
    
}
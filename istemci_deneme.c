#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


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

    int r = getaddrinfo("127.0.0.1","3490",&ipucu,&liste);
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
                

        if (connect(fd,p->ai_addr,p->ai_addrlen) == -1)
        {
            /*soketi porta bağlıyoruz şuan temelde sunucumuz oluştu*/

            perror("connect");
            close(fd);
            continue;
        }
        

        printf("bağlandı, fd=%d\n",fd);
        

        break;
    }

    freeaddrinfo(liste);

    if (p == NULL)
    {
    fprintf(stderr,"hicbirine baglanamadım\n");
    /*hata mesajı*/
    return 1;
    }

    char tampon[256];

    int x = recv(fd,tampon,sizeof(tampon)-1,0);
    if (x == 0)
    {
        printf("%s","sunucu bağlantıyı kapattı\n");
        close(fd);
        return 1;
    }
    if (x == -1)
    {
        perror("recv");
        close(fd);
        return 1;
    }
    tampon[x] = '\0';
    printf("%s",tampon);


    while (1)
    {
    if (fgets(tampon,sizeof(tampon),stdin) == NULL)
    {
        break;
    }
    send(fd,tampon,strlen(tampon),0);

    int n = recv(fd,tampon,sizeof(tampon)-1,0);
    if (n == -1)
    {
        perror("recv");
        break;
    }
    if (n == 0)
    {
        printf("sunucu bağlantıyı kapattı\n");
        break;
    }
    tampon[n] = '\0';
    printf("%s",tampon);
    }

    close(fd);
    return 0;
}
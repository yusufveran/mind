# Soket çalışma notu (10.2 ve 10.3 için)

Kaynak: Beej's Guide to Network Programming, güncel sürüm (https://beej.us/guide/bgnet/). Bu not rehberin çevirisi değil, işine yarayan kısımların kısa özetidir. Ayrıntı için orijinaline bak.

## 1. Büyük resim

- Soket, ağ bağlantısına verilen bir **tamsayı tanımlayıcıdır** (`int`). `fopen`'ın verdiği `FILE *` gibi düşün: bir kez açarsın, üzerinden okur/yazarsın, sonunda kapatırsın.
- Başarısızlık kontrolü de aynı mantık: `fopen == NULL` neyse, `socket == -1` odur. Her çağrının dönüş değerini kontrol et.
- Bu projede **TCP** kullanıyoruz (`SOCK_STREAM`): sıralı, güvenilir bir bayt akışı.
- IP adresi makineyi, **port** o makinedeki programı seçer. Kendi denemelerinde 1024'ün üstünde bir port al (altı yetki ister).

## 2. Çağrı sırası

**Sunucu:** `getaddrinfo` → `socket` → `setsockopt(SO_REUSEADDR)` → `bind` → `listen` → `accept` → `recv`/`send` → `close`

**İstemci:** `getaddrinfo` → `socket` → `connect` → `send`/`recv` → `close`

| Çağrı | Ne yapar | Hata |
|---|---|---|
| `getaddrinfo` | Adres ve portu çekirdeğin anlayacağı biçime çevirir (bağlantı listesi döner) | 0 dışı döner, mesaj için `gai_strerror` |
| `socket` | Soket tanımlayıcısı üretir | -1 |
| `bind` | Soketi bu makinedeki bir porta bağlar (sunucu) | -1 |
| `listen` | Soketi "bağlantı kabul eder" konumuna alır, kuyruk uzunluğu verilir | -1 |
| `accept` | Kuyruktan bir istemciyi alır ve **yeni bir tanımlayıcı** döndürür | -1 |
| `connect` | İstemci olarak uzak sunucuya bağlanır | -1 |
| `send` / `recv` | Veri yollar / alır | -1 |
| `close` | Soketi kapatır | -1 |

Önemli ayrıntılar:
- `getaddrinfo` bir **liste** döndürür; ilk sonuç her zaman çalışmayabilir. Döngüyle sırayla dene, işin bitince `freeaddrinfo` ile listeyi serbest bırak.
- Sunucuda `getaddrinfo` çağrısına `AI_PASSIVE` bayrağı verilir (kendi adresimi kullan demek). İstemcide verilmez.
- `accept` sonrası **iki ayrı tanımlayıcın** olur: dinleyen (yeni bağlantıları beklemeye devam eder) ve bağlanan (o istemciyle konuşmak için). Konuşmayı hep ikincisiyle yaparsın.
- `SO_REUSEADDR` olmazsa programı kapatıp hemen açtığında `bind` "Address already in use" der.

## 3. recv ve send ile ilgili kritik gerçekler

- `recv`, dönüş değerinde **gerçekten kaç bayt aldığını** söyler. İstediğin kadar gelmeyebilir.
- `recv` **0** döndürürse karşı taraf bağlantıyı kapatmıştır. Bu hata değil, normal bir bitiş sinyalidir (dosyada EOF gibi; `fgets == NULL` kontrolünü hatırla).
- `recv` **sonuna `'\0'` koymaz.** Alınan baytı dizge gibi kullanacaksan sonuna kendin koy (tampon boyutundan 1 küçük oku ki yer kalsın).
- `send` de tek seferde hepsini göndermeyebilir; dönüş değeri gönderilen bayt sayısıdır. Gönderilmeyenleri kalan kısımdan tekrar yollamak sana kalır. Bunu yapan küçük bir yardımcı fonksiyon yazmak iyi bir alıştırmadır.
- Karşı taraf kapandıktan sonra `send` yaparsan Linux sana **SIGPIPE** sinyali gönderir ve program sessizce ölür. `send`'e `MSG_NOSIGNAL` bayrağı vererek bunu engelleyebilirsin.

## 4. TCP bayt akışıdır (10.3'ün asıl konusu)

İstemci "merhaba\n" ve "nasılsın\n" yolladı diye sunucu bunları iki ayrı `recv` ile almaz. Gelebilecek durumlar:

- Tek `recv` yalnızca `"merha"` getirir.
- Tek `recv` `"merhaba\nnasıl"` getirir (bir satır tam, diğeri yarım).
- Tek `recv` ikisini birden getirir.

Bu yüzden protokolümüz "her mesaj bir satır, `\n` ile biter" olduğunda şu mantık gerekir:
1. Gelen baytları bir **tampona biriktir**.
2. Tamponda `\n` var mı bak.
3. Varsa o satırı işle (`isle()`'ye ver), tampondan çıkar, kalanı başa taşı.
4. Yoksa yeniden `recv` yap.

Bunu zaten bildiğin parçalarla kurabilirsin: `strchr` ile `\n` arama, `memmove` ile kalanı başa kaydırma ve tampon doluluk sayacı.

## 5. Sık görülen hatalar

| Belirti | Neden |
|---|---|
| `Connection refused` | Sunucu çalışmıyor ya da port farklı |
| `Address already in use` | `SO_REUSEADDR` yok |
| Sunucu sessizce ölüyor | `send` sonrası SIGPIPE |
| İstemci sonsuza dek bekliyor | `recv` blokluyor; gönderen taraf satırı `\n` ile bitirmemiş |
| Çıktıda çöp karakterler | `recv` sonuna `'\0'` konmamış |

## 6. Test ipuçları

- İstemci yazmadan sunucuyu elle dene: `nc localhost 3490` (yazdığın her satır sunucuya gider).
- Önce her şeyi `127.0.0.1` üzerinde dene; ağ sorunlarını sonraya bırak.
- Derle: `gcc -Wall -Wextra dosya.c -o dosya`
- Bağlantıları görmek için: `ss -tlnp` (dinleyen portlar).

## 7. 10.2 için alıştırma sırası

1. Sunucu: `getaddrinfo` → `socket` → `bind` → `listen` → `accept`; bağlanan olunca bir mesaj yazdırıp kapat. `nc` ile dene.
2. Gelen veriyi `recv` ile al, aynen `send` ile geri yolla (yankı). Tek bağlantı, tek `recv`.
3. Karşı taraf kapatınca (`recv == 0`) döngüden çık, sonraki bağlantıyı `accept` et.
4. İstemciyi yaz: `connect`, klavyeden `fgets` ile satır oku, `send`, cevabı `recv` ile al, yazdır.

Sırası önemli: 3. adım bitmeden istemciye geçme.

## 8. Bu notta olmayanlar (şimdilik gerek yok)

Windows'a özgü Winsock kurulumu, UDP/datagram soketleri, `select`/`poll` (çoklu istemci, 10.6), `fork` ve sinyal işleyicileri.

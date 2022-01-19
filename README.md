# Preddiplomski projekt, [Džapini anđeli](https://www.youtube.com/watch?v=dQw4w9WgXcQ) - STM32
Ovo je repozitorij za dio projekta koji se tiče STM32. <br>
Link na repozitorij za web stranicu: <i> Ovdje se unosi link </i>

## Upute za koristenje BME280 drivera

### Spajanje senzora
Pinove je potrebno spojiti na sljedeći način:
| BME280 | STM32F429IDISCOVERY |
|:------:|:-------------------:|
| Vcc    | 3V                  |
| GND    | GND                 |
| SCL    | PA8                 |
| SDA    | PC9                 |
| CSB    | -                   |
| SDO    | GND                 |

### Korištenje Drivera
1. Deklarirati strukturu tipa `BME280`. U njoj će se nalaziti svi podatci sa senzora.
2. Inicijalizirati senzor pomoću funkcije `BME280_Initialise(BME280 *dev, I2C_HandleTypeDef *i2c_handle)`. Prvi argument je pokazivač na strukturu senzora, a drugi argument je adresa I<sup>2</sup>C handlera. U našem slučaju drugi argument mora biti `&hi2c3`, jer koristimo I2C3 periferiju.
3. Senzor će na početku odraditi mjerenje. Kada se mjerenje želi ponovno obaviti potrebno je prvo pozvati funkciju `BME280_PerformMeasurements(BME280 *dev)`.

Vrijednosti mjerenja su dostupne u strukturi senzora i nalaze se u varijablama:
* temp (temperatura zrak)
* press (tlak zraka)
* hum (vlažnost zraka)

<i> Napomena: funkcije `BME280_Initialise` i `BME280_PerformMeasurements` vraćaju broj pogrešaka koje su se dogodile tijekom komunikacije sa senzorom u formatu `uint8_t`. </i>

## Upute za korištenje senzora za vlagu tla

### Spajanje senzora
Senzor je spojen na LM393 komparatora. Pločicu na kojoj je komparator treba spojiti na sljedeći način:
| LM393 breakout | STM32F429IDISCOVERY |
|:-:|:-:|
| GND | GND |
| A0 | PF6 |
| D0 | - |
| +5V | 5V |

### Korištenje drivera
1. Deklarirati strukturu tipa `SH`. U njoj će se nalaziti svi podatci za senzor.
2. Inicijalizirati senzor pomoću funkcije `SH_init(SH *dev, ADC_HandleTypeDef *ADC_handle)`. Prvi argument je pokazivač na strukturu senzora, a drugi argument je adresa ADC handlera. U našem slučaju drugi argument mora biti `&hadc3`, jer koristimo ADC3 periferiju.
3. Kada se želi napraviti mjerenje potrebno je pozvati funkciju `SH_ReadData(SH *dev)`, u koju predajemo pokazivač na strukturu senzora. Kao argument predajemo pokazivač na strukturu senzora.

Vrijednosti mjerenja su dostupne u strukturi senzora pod varijablom `soilHumidity`.

<i> Napomena: funkcija `SH_ReadData` vraća broj grešaka koje su se dogodile tokom ADC pretvorbe u formatu `uint8_t`. </i>

## Upute za korištenje ESP8266 WiFi drivera

### Spajanje ESP-01 modula
Slika rasporeda pinova na ESP-01 pločici može se naći [ovdje](https://www.makerlab-electronics.com/?attachment_id=11461). <br>
Pinove je potrebno spojiti na sljedeći način:
| ESP-01 | STM32F429IDISCOVERY |
|:-:|:-:|
| RXD | PB6 (TX) |
| VCC | 3V |
| CHPD | 3V |
| GND | GND |
| TXD | PB7 (RX) |

### Korištenje drivera
1. Prije početka rada s WiFi-jem pozvati funkciju: `int8_t WIFI_Init(char *ssid, char *pwd)` Parametri funkcije su ime i lozinka mreže na koju se treba spojiti. Povratna vrijednost je 0 ako nije bilo grešaka i -1 ako je došlo do greške. <br>
<i><b>Napomena:</b> ESP nema mogućnost spajanja na eduroam mrežu s trenutnim firmware-om. Ako radite na faksu, najbolje je spojiti se na AP od mobitela.</i>

2. Za slanje podataka na server koristiti funkciju: `int8_t WIFI_SendRequestWithParams(char *hostname, char *path, double temp, double moisture, double humidity, double waterLevel)` Hostname servera je ekantica.herokuapp.com, bez sheme na početku (http:// ili https://). Path za slanje podataka je /data. Ostali parametri su vrijednosti očitanja senzora koje se žele poslati, a prilikom slanja zahtjeva odrežu im se decimale (trebalo bi biti zaokruživanje na 2 decimale ali to još nije implementirano na serveru). Funkcija na temelju odgovora servera vraća 0 ako nije potrebno uključiti pumpu, a 1 ako jest. Ako je došlo do pogreške, povratna vrijednost je -1.
  
## Korisni linkovi
I2C komunikacija na STM32: https://www.youtube.com/watch?v=cvmQNTVJrzs <br>
Pisanje drivera: https://www.youtube.com/watch?v=_JQAve05o_0 <br>
Općenito dosta dobar kanal za STM32, PCB, embedded i ostalo: https://www.youtube.com/c/PhilS94/featured

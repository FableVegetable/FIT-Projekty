# IPK 1.projekt - Klient-Server aplikácia
### Obmedzenia



### IPK Klient spustenie
```sh
$ ./ipk-client -h merlin.fit.vutbr.cz -p 5333 [-n|-f|-l] login
```
**POVINNÉ OPTIONS:**    
**-h     host** -  IP adresa alebo hostname
**-p     port** -  Číslo portu 
**login**  - login alebo podreťazec loginu  

**NEPOVINNÉ OPTIONS:** (povinný je práve jeden z troch)
**-n     name** - meno a informácie 
**-f     file** - priečinok
**-l     list** - zoznam uživateľov začinajucich prefixom list 

### Príklad spustenia
```sh
./ipk-client -h eva.fit.vutbr.cz -p 55555 -n rysavy
./ipk-client -h host -p port -l
```

### IPK-Server
```sh
$ ./ipk-server -p <port>
```
**POVINNÉ OPTIONS:**  
**-p     port** - port

### Príklad spustenia
```sh
$ ./ipk-server -p 11111 
```
### Kompilácia  
**make**

### Príklad spustenia
```sh
$ ./ipk-server -p 11111 
$ ./ipk-client -h localhost -p 11111 -n xzubri00
```
### Author  
**Zubrik Tomáš**


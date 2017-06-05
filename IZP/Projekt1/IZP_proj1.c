/**
  * Názov programu:	1. projekt z IZP
  * Názov projektu: 	Práce s textem
  * Autor:		Tomáš Zubrik (c) 2016
  * Cieľ projektu:	Cílem projektu je vytvořit program, který buď binární data formátuje
  *			do textové podoby nebo textovou podobu dat převádí do binární podoby.
  *			V případě převodu binárních dat na text bude výstupní formát obsahovat
  *			adresy vstupních bajtů, hexadecimální kódování a textovou reprezentaci obsahu.
  *			V případě převodu textu do binární podoby je na vstupu očekáváné hexadecimální kódování bajtů.
  */


/**
  * Systémové hlavičkové súbory
  */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define N 200 		// definovanie makra


/**
  * --> Funkcia, zisťujúca rovnosť dvoch reťazcov
  * --> @param *str1 - 1. string na porovnanie
  * --> @param *str2 - 2. string na porovnanie
  */
int compareStrings(char *str1, char *str2)
{
    for(int i = 0; str1[i] == str2[i]; i++)
    {
        if (str1[i] == '\0')
            return 0;
    }
    return 1;
}


/** Funkcia, ktorá vytlačí nápovedu */
void printHelp()
{
    printf(	"------------------   NÁPOVEDA ---------------------------\n"
            "Názov programu: 1. projekt z IZP\n"
            "Autor: Tomáš Zubrik (c) 2016\n"
            "Pre argument: 	- r program prevedie vstupný hexadecimálny kód na text\n"
            "		- x program prevedie vstupný text do hexadecimálne podoby\n"
            "		- [-s M] [-n N] program vypíše hexdump od pozície M, s dlzkou N\n"
            "		- [-s N] program vypíše len reťazce s väčším alebo rovným počtom ako N\n"
            "Program spustený bez argumentov vypíše hexadecimálnu hodnotu prvého bajtu v riadku,\n"
            "hexadecimálnu podobu textu 16 bajtov a zvyšku bajtov v nasledujúcom riadku a reprezentujuci text\n"
            "----------------------------------------------------------\n");
}


/**
  *  Enumerácia chybových kódov
  */

enum programErrors
{
    ERRHEX, 	//- Nebol zadaný hexadecimálny znak
    ERRSN,		//- Záporné argumenty
    ERRSTRINGS, 	//- Nápoveda
    ERRUNKNOWN, 	//- Neznáma chyba
};

/**
  *  Zoznam chybových hlásení, ktorých poradie odpovedá |programErrors|
  */
const char *ERRMSG[] =
{
    "Nevalidný vstup!",
    "Aspoň jeden z argumentov nie je kladné číslo!",
    "Zadaný číselný argument musí byť v rozsahu 0 < S < 200!",
    "Neznáma chyba v poli!",
};


/**
  * Funkcia, ktorá vypíše danú chyby na štandartný chybový výstup
  */
void printErrors(int err)
{
    if (err < ERRHEX || err > ERRUNKNOWN)
        err = ERRUNKNOWN;
    fprintf(stderr, "V programe je chyba: %s\n ", ERRMSG[err]);
}


/**
  * --> Funkcia, ktorá kontroluje vstupné dáta a prepočítava dáta z hexadecimálnej hodnoty na desiatkovú
  *  @param c - kontorolovaný znak
  */

int charToInt(char c)
{
    if (isxdigit(c))
    {
        if(c >='0' && c <= '9')
        {
            return c- '0';
        }
        else
            return tolower(c) - 'a' + 10;
    }
    return 0;
}


/**
  * --> Funkcia na prevod hexadecimálneho čísla na celočís
  */

int intOfHexDigit(char* pole)
{
    return charToInt(pole[0]) * 16 + charToInt(pole[1]);		// prepočíta číslo z hexadecimálnej do desiatkovej sústavy
}


/**
  * --> Funkcia, ktorá načítava znaky hexadecim. podoby do dvojprvkového poľa
  *     a tlačí dané znaky z hexadecim. podoby do znakovej formy
  */


int hexDigitToChar()
{
    int c;
    char pole[2];
    int i = 0;

    while ((c = getchar()) != EOF)
    {
        if(isspace(c))
            continue;
        if(isxdigit(c))
        {
            pole[i++] = c;

            if (i == 2)
            {
                printf("%c", intOfHexDigit(pole));		// tlač znaku z desiatkovej sustavy
                i = 0;
            }
        }
        else
        {
            printErrors(ERRHEX);
            printf("  -->  časť reťazca preložená bez chyby \n");
            return EXIT_FAILURE;
        }

    }
    printf("\n");
    return EXIT_SUCCESS;

}


/**
  * -->  Funkcia, ktorá vstupné dáta spracuje a reťazce znakov oddelenými netlačiteľnými znakmi
  *	a vypíše do riadkov v závislosti od argumentu (S)
  *  @param S - hodnota ktorá určuje ako dlhé reťazce sa majú vypísať do riadkov resp. ktoré sa majú ignorovať
  */

int stringsToRows(int S)
{
    int c;
    char pole[N];
    int counter = 0;
    if ((S > N) || (S < 0))
    {
        printErrors(ERRSTRINGS);
        return 0;
    }

    while((c=getchar())!=EOF)
    {
        if (isprint(c) || isblank(c))
            pole[counter++] = c;
        else
        {
            if (counter >= S)
            {
                pole[counter] = '\0';
                printf("%s\n", pole);
            }
            counter = 0;
        }
    }
    return 0;
}



/** --> Funkcia, ktorá prevádza všetky načítané znaky na hexadecimálnu podobu
  *
  */
void strToHex()
{
    int c;
    while((c = getchar())!=EOF)
        printf("%02x", c);
    printf("\n");
}


/**
  * --> Funkcia pracujúca na princípe Hexdumpu s parametrami (s) a (n)
  * --> Funkcia vypíše: - adresu daného bajtu v závislosti od parametru (s)
  * -->                 - hexadecimálnu podobu postupnosti znakov s dĺžkou (n)
  * -->                 - príslušné znaky
  */


void hexDumpFunctionSN(int s, int n)
{
    int charCounter = 0;
    int totalChars = s;
    char line[17];
    char ch;
    line[16]='\0';

    while( ( ch = getchar() ) != EOF )
    {
        s--;
        if (s > 0)
            continue;
        if (n == 0)
            break;
        n--;

        if(charCounter == 0)
            printf("%08x  ", totalChars);			// výpis adresy prvého bajtu

        printf("%02x ", (unsigned char)ch);
        if ( ch>=32 && ch<=126 )
            line[charCounter] = ch;
        else
            line[charCounter] = '.';
        totalChars++;
        charCounter++;
        if (charCounter ==16)
        {
            charCounter = 0;
            printf(" |%s|\n", line);
        }
        else
        {
            if (charCounter==8)
                printf(" ");
        }
    }

    if (charCounter > 0)
    {
        for(int i = charCounter; i < 16; i++)
            printf("   ");

        printf(" ");

        if(charCounter < 8)
            printf(" ");

        line[charCounter] = '\0';
        printf("|%s", line);

        for(int i = charCounter; i < 16; i++)
            printf(" ");
        printf("|");
    }
    printf("\n");
}




/**
  * --> Funkcia pracujúca na princípe Hexdumpu
  * --> Funkcia vypíše: - adresu prvého bajtu zo 16 načítaných znakov
  * --> 		- 16 alebo daný zvyšok znakov do hexadecimálnej podoby
  * -->			- dané znaky v riadku
  */

void hexDump()
{
    int charCounter = 0;
    int lineCounter = 0;
    char line[17];
    char ch;
    line[16]='\0';

    while( ( ch = getchar() ) != EOF )
    {
        if(charCounter == 0)
            printf("%08x  ", lineCounter * 16);
        printf("%02x ",(unsigned char)ch);
        if ( ch>=32 && ch<=126 )
            line[charCounter] = ch;
        else
            line[charCounter] = '.';

        charCounter++;
        if (charCounter ==16)
        {
            charCounter = 0;
            lineCounter++;
            printf(" |%s|\n", line);
        }
        else
        {
            if (charCounter==8)
                printf(" ");
        }
    }

    if (charCounter > 0)
    {
        for(int i = charCounter; i < 16; i++)
            printf("   ");

        printf(" ");

        if(charCounter < 8)
            printf(" ");

        line[charCounter] = '\0';
        printf("|%s", line);

        for(int i = charCounter; i < 16; i++)
            printf(" ");

        printf("|\n");

    }
    if (charCounter == 16)
        printf("\n");
}




/**
  * Funkcia, ktorá kontroluje zadané argumenty na vstupe, a v ich závislosti zavolá danú funkciu
		* @param argc - počet argumentov zadaných na vstupe
		* @param argv - pole textových reťazcov
  */
void doParams(int argc, char **argv)
{
    if ( argc == 1 )
        hexDump();
    else if ( argc == 2 )
    {
        if (compareStrings("-x", argv[1]) == 0)
            strToHex();
        else if (compareStrings("-r", argv[1]) == 0)
            hexDigitToChar();
        else
            printHelp();
    }

    else if(argc == 3)
    {
        char *end;
        int num = strtol(argv[2], &end, 10);
        if ((compareStrings("-S", argv[1]) == 0) && (*end == '\0'))
            stringsToRows(num);

        else if ((compareStrings("-s", argv[1]) == 0) && (*end == '\0'))
            hexDumpFunctionSN(0, num);
        else
            printHelp();
    }

    else if(argc == 5)
    {
        char *end1, *end2;
        int s = strtol(argv[2], &end1, 10);
        int n = strtol(argv[4], &end2, 10);

        if( (compareStrings("-s", argv[1]) == 0) && (compareStrings("-n", argv[3]) == 0) && (*end1 == '\0') && (*end2 == '\0'))
            if(s >= 0 && n >= 0 )
                hexDumpFunctionSN(s, n);
            else
                printErrors(ERRSN);
        else
            printHelp();
    }
    else
        printHelp();
}



/**
  * Hlavná funkcia, ktorá vykoná príkazy za použitia funkcií vo funkcii doParams();
  * @param argc		- počet argumentov zadaných na vstupe
  * @param argv 	- pole textových reťazcov (jednotlivé argumenty)
  */

int main(int argc, char **argv)
{
    doParams(argc, argv);
    return 0;
}

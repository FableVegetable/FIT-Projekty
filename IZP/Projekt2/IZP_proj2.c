/**
  * Názov programu:     2. projekt z IZP
  * Názov projektu:     Iterační výpočty
  * Autor:              Tomáš Zubrik (c) 2016
  * Cieľ projektu:     	Implementovat výpočet přirozeného logaritmu a exponenciální funkce
  *			s obecným základem pouze pomocí matematických operací +,-,*,/.
  */


/**
  * Systémové hlavičkové súbory
  */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


/**
  *  Enumerácia chybových kódov
  */
enum errorCodes
{
    ERRN,						//- Počet iterací nie je kladné číslo
    ERPOW,					//- Zadané argumenty nie sú odpovedajúce čísla
    ERLOG,					//- Zadané argumenty nie sú odpovedajúce čísla
    ERMYPOW,			//- Chyba v mypow
    ERMYLOG,			//- Chyba v mylog
    ERPARAMS,		//- Zle zadané argumenty
    ERRUNKNOWN,//- Neznáma chyba

};


/**
  *  Ukazovateľ na pole chybových reťazcov
  */
const char *ERRMSG[] =
{
    "Počet iterací n musí byť kladné číslo!",
    "Zadané argumenty vo funkcii pre výpočet mocniny  nie sú očakávané čísla!",
    "Zadané argumenty vo funkcii pre výpočet logaritmu nie sú očakávané čísla!",
    "Chyba vo funkcii mypow!",
    "Chyba vo funkcii mylog!",
    "Boli zadané nesprávne argumenty pre funkciu!",
    "V programe je neznáma chyba",
};


/**
  * Funkcia, ktorá vypíše danú chyby na štandartný chybový výstup
  * @param err	- index v poli chybových reťazcov
  */

void printErrors(int err)
{
    if (err < ERRN || err > ERRUNKNOWN)
        err = ERRUNKNOWN;
    fprintf(stderr, "V programe je chyba: %s\n ", ERRMSG[err]);
}

/**
  *  Funkcia, ktorá vypočíta mocninu čísla typu double s celočíselným mocniteľom
  *  @param x	- mocnenec typu double
  *  @param n	- celočíselný mocniteľ
  */
double simplePow(double x, int n)
{
    double result = 1.0;
    for(int i = 1; i <= n; i++)
    {
        result *= x;
    }
    return result;
}


/**
  *  Funkcia, ktorá vypočíta logaritmus z čísla (1-x) s (n) počtom iterací
  *  @param x	- číslo typu, z ktorého sa počíta logaritmus
  *  @param n	- počet iterácií
  */

double taylor_log(double x, unsigned int n)
{
    double tay_log = 0.0;

    if (x < 0 || isnan(x))
        return NAN;

    if (x == 0)
        return -INFINITY;
    else if (isinf(x))
        return INFINITY;

    for (unsigned i = 1; i <= n; i++)
    {
        if (fabs(x) < 1)
            tay_log += -(simplePow((1-x), i)/i);
        else if ( x > 0.5 )
            tay_log += simplePow(((x-1)/x), i)/i;
        else
            return NAN;
    }

    return tay_log;
}

/**
  *  Funkcia, ktorá vypočíta prirodzený logaritmus z čísla x pomocou zreťazených zlomkov
  *  @param x	- číslo typu double, z ktorého sa počíta logaritmus
  *  @param n 	- počet iterací
  */

double cfrac_log(double x, unsigned int n)
{
    double z =(x-1)/(x+1);
    unsigned i;
    double cf = 1.0;
    double powz = z*z;

    if (x < 0 || isnan(x))
        return NAN;

    if (x == 0)
        return -INFINITY;
    else if (isinf(x))
        return INFINITY;

    for (i = n+1; i >= 1; i--)
    {
        cf = (2 * i -1) - (powz * i * i)/cf;
    }
    return cf = 2 * z / cf;
}

/**
  *  Funkcia, ktorá vypočíta y-tú mocninu z čísla x pomocou Taylorovho polynomu
  *  @param x	- základ z ktorého sa počíta mocnina, mocnenec
  *  @param y 	- mocniteľ
  *  @param n	- počet iterací
  */

double taylor_pow(double x, double y, unsigned int n)
{
    double polynomElement = 1.0;
    double sum = polynomElement;

    if (n <= 0 || isnan(x))
        return NAN;

    if (x == 0 && y != 0)
        return 0;
    if (x == 0 && y == 0)
        return NAN;

    double tay_log = taylor_log(x, n);
    for(unsigned int i = 1; i<=n; i++)
    {
        polynomElement = (polynomElement * y * tay_log)/i;
        sum += polynomElement;
    }
    return sum ;
}

/**
  *  Funkcia, ktorá vypočíta y-tú mocninu z čísla x pomocou zreťazených zlomkov
  *  @param x   - základ z ktorého sa počíta mocnina, mocnenec
  *  @param y   - mocniteľ
  *  @param n   - počet iterací
  */

double taylorcf_pow(double x, double y, unsigned int n)
{
    double polynomElement = 1.0;
    double sum = polynomElement;

    if (n <= 0 || isnan(x))
        return NAN;

    if (x == 0 && y != 0)
        return 0;
    if (x == 0 && y == 0)
        return NAN;

    double cf_log = cfrac_log(x, n);

    for(unsigned int i = 1; i <= n; i++)
    {
        polynomElement = (polynomElement * y * cf_log)/i;
        sum += polynomElement;
    }
    return sum;

}

/**
  *  Funkcia, ktorá vypočíta logaritmus z čísla x, za určitý počet iterací s presnosťou .7e
  *  @param x	- číslo z ktorého sa vypočíta prirodzený logaritmus
  */

double mylog(double x)
{
    double e = INT_MAX;
    if (x <= 0 || isnan(x))
        return NAN;

    int i = 1;
    double element1;
    double element2 = floor(cfrac_log(x, i)*1e+9)/1e+9;


    while(i < e)
    {
        element1 = element2;
        element2 = floor(cfrac_log(x, ++i)*1e+9)/1e+9;


        if (element1 == element2)
        {
            return element2;
        }

        i++;
    }

    return 1;
}

/**
  *  Funkcia, ktorá vypočíta y-tú mocninu z čísla x
  *  @param x	- mocnenec
  *  @param y	- mocniteľ
  */

double mypow(double x, double y)
{
    if (x == 0 && y > 0)
        return 0;
    if (x == 0 && y <= 0)
        return NAN;
    if (x <= 0 || isnan(x))
        return NAN;
    if (isinf(x))
        return INFINITY;

    double e = 1e-8;
    double n = 1;
    int i = 1;
    double polynomElement = 1.0;
    double sum = polynomElement;

    while (fabs(polynomElement) >= e)
    {
        n += 1000;
        polynomElement = (polynomElement * y * cfrac_log(x, n))/i;
        sum += polynomElement;
        i++;
    }
    return ceil(sum * 1e+8)/1e+8;

}

/**
  * Funkcia, ktorá kontroluje zadané argumenty na vstupe, a v ich závislosti zavolá danú funkciu
  * @param argc - počet argumentov zadaných na vstupe
  * @param argv - pole textových reťazcov
  */

int doParams(int argc, char **argv)
{

    if (argc == 3 && strcmp(argv[1], "--mylog") == 0)
    {
        char *end;
        double x = strtod(argv[2], &end);
        if (*end == '\0')
        {
            printf("  log: %.7e\n", log(x));
            printf("mylog: %.7e\n", mylog(x));
            return 0;

        }
        else
        {
            printErrors(ERMYLOG);
            return 1;
        }

    }

    else if ((argc == 4) && strcmp(argv[1], "--mypow")== 0)
    {
        char *end, *end1;
        double x = strtod(argv[2], &end);
        double y = strtod(argv[3], &end1);

        if (*end == '\0' && *end1 == '\0')
        {
            printf("  pow: %.7e\n", pow(x, y));
            printf("mypow: %.7e\n", mypow(x, y));
            return 0;
        }
        else
        {
            printErrors(ERMYPOW);
            return 1;
        }
    }

    else if ((argc == 4) && strcmp(argv[1],"--log") == 0)
    {
        char *end, *end1;
        signed int n = strtoul(argv[3], &end, 10);
        double x = strtod(argv[2], &end1);

        if (n <= 0)
        {
            printErrors(ERRN);
            return 1;
        }

        if(*end =='\0' && *end1=='\0')
        {

            printf("       log(%.5g) = %.12g\n", x, log(x));
            printf(" cfrac_log(%.5g) = %.12g \n", x, cfrac_log(x, n));
            printf("taylor_log(%.5g) = %.12g\n", x, taylor_log(x, n));
            return 0;
        }
        else
        {
            printErrors(ERLOG);
            return 1;
        }
    }
    else if ((argc == 5) && strcmp(argv[1],"--pow") == 0)
    {
        char *end, *end1, *end2;
        signed int n = strtoul(argv[4], &end, 10);
        double y = strtod(argv[3], &end2);
        double x = strtod(argv[2], &end1);

        if (n <= 0)
        {
            printErrors(ERRN);
            return 1;
        }

        if(*end =='\0' && *end1=='\0' && *end2 == '\0')
        {
            printf("         pow(%g,%g) = %.12g\n", x, y, pow(x, y));
            printf("  taylor_pow(%g,%g) = %.12g\n", x, y, taylor_pow(x, y, n));
            printf("taylorcf_pow(%g,%g) = %.12g\n", x, y, taylorcf_pow(x, y, n));
        }
        else
        {
            printErrors(ERPOW);
            return 1;
        }
    }

    else
    {
        printErrors(ERPARAMS);
        return 1;
    }
    return 0;

}


/**
  * Hlavná funkcia, ktorá vykoná príkazy za použitia funkcií vo funkcii doParams();
  * @param argc         - počet argumentov zadaných na vstupe
  * @param argv         - pole textových reťazcov (jednotlivé argumenty)
  */

int main(int argc, char **argv)
{
    doParams(argc, argv);
    return 0;

}

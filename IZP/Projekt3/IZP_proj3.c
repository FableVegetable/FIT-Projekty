/**
  * Názov programu: 3. projekt z IZP
  * Názov projektu: Jednoduchá shluková analýza
  * Autor: Tomáš Zubrik (c) 2016
  * Cieľ projektu: Vytvořte program, který implementuje jednoduchou shlukovou analýzu,
  * 			metodu nejvzdálenějšího souseda (angl. complete linkage).
  */

/**
  *  Systémové hlavičkové súbory
  */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t
{
    int id;
    float x;
    float y;
};

struct cluster_t
{
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/**
  *  Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
  *  Ukazatel NULL u pole objektu znamena kapacitu 0.
  *  @param c 	- ukazovatel na štruktúru cluster_t
  *  @param cap	- kapacita objektu
  */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    c->capacity = cap;
    c->obj = malloc(sizeof(struct obj_t)*cap);

}

/**
  *  Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
  *  @param c - ukazovateľ na štruktúru cluster_t
  */
void clear_cluster(struct cluster_t *c)
{

    free(c->obj);
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/**
  *  Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
  *  @param c	- ukazovatel na strukturu cluster_t
  *  @param cap	- nova kapacita
  */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/**
  *	Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
  *	nevejde.
  *	@param c	- ukazovatel na strukturu cluster_t
  *	@param obj	- urcity objekt
  */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->size >= c->capacity )
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);

    c->obj[c->size].id = obj.id;
    c->obj[c->size].x = obj.x;
    c->obj[c->size].y = obj.y;
    c->size += 1;
}

/**
  *  Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
  *  @param c	- dany cluster
  */
void sort_cluster(struct cluster_t *c);

/**
  *  Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
  *  Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
  *  Shluk 'c2' bude nezmenen.
  *  @param c1	- cluster, do ktoreho sa priradia objekty clustra c2
  *  @param c2	- cluster, z ktoreho sa skopiruju objekty do clustra c1
  */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i = 0; i < c2->size; i++)
    {
        if (c1->capacity < c2->capacity + c1->capacity)
            c1 = resize_cluster(c1, c1->capacity + CLUSTER_CHUNK);

        int size = c1->size;
        c1->obj[size].id = c2->obj[i].id;
        c1->obj[size].x = c2->obj[i].x;
        c1->obj[size].y = c2->obj[i].y;

        c1->size= (c1->size) + 1;
    }
    sort_cluster(c1);

}

/**********************************************************************/
/* Prace s polem shluku */

/**
  *  Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
  *  (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
  *  pocet shluku v poli.
  *  @param carr	- pole shluku
  *  @param narr	- pocet shlukov v poli shluku
  *  @param idx		- index ktory chcem odstranit
  */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    int i;

    for(i = idx; i < narr-1 ; i++)
    {
        if(carr[i].capacity < carr[i+1].capacity)
            resize_cluster(&carr[i], carr[i+1].capacity);
        if(carr[i].size < carr[i+1].size)
            carr[i].size = carr[i+1].size;

        for(int j = 0; j < carr[i+1].size; j++)
        {
            carr[i].obj[j].id = carr[i+1].obj[j].id;
            carr[i].obj[j].x = carr[i+1].obj[j].x;
            carr[i].obj[j].y = carr[i+1].obj[j].y;
        }
    }

    return narr - 1;
}

/**
  *	Pocita Euklidovskou vzdalenost mezi dvema objekty.
  *	@param o1	- objekt cislo 1
  *	@param o2	- objekt cislo 2
  */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float result =  sqrt(pow((o1->x - o2->x), 2)+pow((o1->y - o2->y), 2));
    return result;
}

/**
  *	Pocita vzdalenost dvou shluku.
  *	@param c1	- shluk cislo 1
  * 	@param c2	- shluk cislo 2
  */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float maxDistance = 0;
    float distance;

    for(int i = 0; i < c1->size; i++)
    {
        for(int j = 0; j < c2->size; j++)
        {
            distance = obj_distance(&c1->obj[i], &c2->obj[j]);
            if(distance > maxDistance)
                maxDistance = distance;
        }
    }

    return maxDistance;
}

/**
  *	Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
  *	hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
  *	'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
  *	adresu 'c1' resp. 'c2'.
  *
  *	@param carr	- pole zhlukov
  *	@param narr	- veľkosť poľa zhlukov
  *	@param c1	- adresa indexu zhluku c1
  *	@param c2	- adresa indexu zhluku c2
  */

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float closestClusters;
    float minCluster = INT_MAX;
    for(int i = 0; i < narr - 1; i++ )
    {
        for(int j = i+1; j < narr; j++)
        {
            closestClusters = cluster_distance(&carr[i], &carr[j]);
            if(minCluster > closestClusters)
            {
                minCluster = closestClusters;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

/**
  *  Pomocna funkce pro razeni shluku
  *  @param a - konstatny ukazovatel a
  *  @param b - konstatny ukazovatel b
  */
static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/**
  *	Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
  *	@param c	- zhluk, ktorého objekty budú zoradené podľa ID objektov
  */
void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
  *	Tisk shluku 'c' na stdout.
  *	@param c	- zhluk, ktorý bude vytlačený na stdout
  */
void print_cluster(struct cluster_t *c)
{
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
  *	Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
  *	jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
  *	polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
  *	kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
  *	V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
  *
  *	@param filename	- ukazovateľ na char, teda názov súboru, z ktorého čítame dané objekty
  *	@param arr	- pole zhlukov, do ktorého načítavame dané zhluk
  */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    struct obj_t obj;

    FILE *file;
    file = fopen (filename, "r");
    if (file == NULL)
        return -1;

    int count;
    fscanf(file,"count=%d", &count);

    *arr = malloc(sizeof(struct cluster_t) * count);

    if(arr == NULL)
        return -2;

    for(int i = 0; i < count; i++)
    {
        init_cluster(&(*arr)[i], CLUSTER_CHUNK);
        if(fscanf(file, "%d %g %g", &obj.id, &obj.x, &obj.y) != 3)
        {
            for(int j = 0; j <= i; j++)
                clear_cluster(&(*arr)[j]);
            if(fclose(file) != 0)
                return -4;
            return -3;

        }

        if(obj.x > 1000 || obj.x < 0 || obj.y> 1000 || obj.y<0 || ceilf(obj.x) != obj.x || ceilf(obj.y) != obj.y || ceilf(obj.id) != obj.id  || obj.id < 0 || obj.id > INT_MAX)
        {
            for(int j = 0; j <= i; j++)
                clear_cluster(&(*arr)[j]);
            if(fclose(file) != 0)
                return -4;

            return -3;
        }
        append_cluster(&(*arr)[i], obj);
    }
    if(fclose(file) != 0)
        return -4;


    return count;

}

/**
  *	Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
  *	Tiskne se prvnich 'narr' shluku.
  *	@param carr	- ukazateľ na prvú položku v poli zhlukov
  *	@param narr	- počet tlačených zhlukov
  */
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }

    return;
}

/**
  *	Hlavná funkcia, ktorá kontroluje vstupné argumenty a volá dané funckie v logickom poradí
  *	pre správnu funkcionalitu zhlukovania
  *	@param argc	- počet zadaných argumentov
  *	@param argv	- ukazovaťel na pole zadaných argumentov
  */
int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    if(argc == 3)
    {
        char *end;
        int n = strtoul(argv[2], &end, 10);
        char *fileptr = argv[1];
        if(*end == '\0' && n > 0 && n < INT_MAX)
        {
            int c1;
            int c2;
            int count = load_clusters(fileptr, &clusters);
            int count2 = count;

            if (count == -1)
            {
                fprintf(stderr,"Súbor sa nepodarilo otvoriť!\n");
                return 1;
            }

            else if (count == -2)
            {
                printf("Nepodarilo sa alokovat pamat pre pole shlukov!\n");
                free(clusters);
                return 1;
            }

            else if (count == -3)
            {
                fprintf(stderr,"Nespravne parametre nacitane zo suboru!\n");
                free(clusters);
                return 1;
            }
            else if (count == -4)
            {
                fprintf(stderr,"Subor sa nepodarilo zatvorit!\n");
                free(clusters);
                return 1;

            }

            if (n > count)
            {
                printf("Chyba v programe: n > count of objects\n");
                for(int j = 0; j < count; j++)
                    clear_cluster(&clusters[j]);
                free(clusters);
                return 1;

            }

            for(int i = count; i > n; i--)
            {
                find_neighbours(clusters, count, &c1, &c2);
                merge_clusters(&clusters[c1], &clusters[c2]);
                count = remove_cluster(clusters, count, c2);
            }

            print_clusters(clusters, count);

            for(int j = 0; j < count2; j++)
                clear_cluster(&clusters[j]);

            fileptr = NULL;
            end = NULL;
            free(clusters);
        }
        else
        {
            fprintf(stderr,"Zadaný ˇargument [n] nie je číslo alebo nesplňuje podmienku (n > 0 && n < INT_MAX)\n");
            return 1;
        }
    }

    else if(argc==2)
    {
        char *fileptr = argv[1];
        int c1;
        int c2;
        int count = load_clusters(fileptr, &clusters);
        int count2 = count;

        if (count == -1)
        {
            fprintf(stderr,"Súbor sa nepodarilo otvoriť!\n");
            return 1;
        }

        else if (count == -2)
        {
            printf("Nepodarilo sa alokovat pamat pre pole shlukov!\n");
            free(clusters);
            return 1;
        }

        else if (count == -3)
        {
            fprintf(stderr,"Nespravne parametre nacitane zo suboru!\n");
            free(clusters);
            return 1;
        }

        else if (count == -4)
        {
            fprintf(stderr,"Subor sa nepodarilo zatvorit!\n");
            free(clusters);
            return 1;

        }
        for(int i = count; i > 1; i--)
        {
            find_neighbours(clusters, count, &c1, &c2);
            merge_clusters(&clusters[c1], &clusters[c2]);
            count = remove_cluster(clusters, count, c2);
        }


        print_clusters(clusters, count);

        for(int j = 0; j < count2; j++)
            clear_cluster(&clusters[j]);

        free(clusters);
        fileptr = NULL;
    }

    else
    {
        fprintf(stderr,"Zadaný nesprávny počet argumentov!\n");
        return 1;
    }

    return 0;

}

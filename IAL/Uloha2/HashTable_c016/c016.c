
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for(int i=0; i<HTSIZE; i++)
		(*ptrht)[i] = NULL;	//inicializacia vsetkych poloziek na NULL
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if(!ptrht) return NULL;//ak beexistuje hashtable
	for(tHTItem *tmp = (*ptrht)[hashCode(key)]; tmp != NULL; tmp = tmp->ptrnext)
		if(!strcmp(tmp->key, key))	//ak sa polozka nachadza v zozname na danom indexe vrat ju
			return tmp;
	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {

 	if(!ptrht)	return;//ak neexistuje hastable
 	tHTItem *tmp = htSearch(ptrht, key);
 	if(tmp) //ak je kluc uz v tabulke, zmenime iba data
 		tmp->data = data;
 	else
 	{
 		tHTItem *tmp = malloc(sizeof(tHTItem)); //alokovanie pamate pre polozku
 		if(!tmp)	return; //ak sa nepodarila alokacia
 		else
 		{
 			int keylen = strlen(key)+1; //dlzka kluca + '\0'
 			int index = hashCode(key);
 			tmp->key = malloc(keylen);
 			if(tmp->key)	
 			{
 				strcpy(tmp->key, key);
 				tmp->data = data;
 				tmp->ptrnext = (*ptrht)[index];
 				(*ptrht)[index] = tmp;
 			}
 			else	 
 				free(tmp);
 			}
 		}
 }


/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	if(!ptrht)	return NULL; //ak neexistuje
	tHTItem *tmp = htSearch(ptrht, key);
	return (!tmp) ? NULL : &(tmp->data); //ak je v tabulke vrat data
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if(!ptrht)	return; //ak neexistuje tabulka
	int index = hashCode(key);
	tHTItem *tmp2,*tmp = (*ptrht)[index];
	if(!tmp)	return;		// ak nie je v tabulke
	if(!strcmp(tmp->key,key))//ak je prva v poradi vymazeme
	{
		(*ptrht)[index] = tmp->ptrnext;
		free(tmp->key);
		free(tmp);
		return;
	}
	while(tmp->ptrnext)//prehladavame na danom indexe, ak najdeme vymazeme
	{
		tmp2 = tmp->ptrnext;
		if(!strcmp(tmp2->key, key))
		{
			tmp->ptrnext = tmp2->ptrnext;
			free(tmp2->key);
			free(tmp2);
			return;
		}
		tmp = tmp->ptrnext; 
	}
}


/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	if(!ptrht)	return;
	tHTItem *tmp, *tmp2;
	unsigned int index = 0;
	while(index < HTSIZE)
	{
		tmp = (*ptrht)[index];
		while(tmp)
		{
			tmp2 = tmp->ptrnext;
			free(tmp->key);
			free(tmp);
			tmp = tmp2;
		}
		(*ptrht)[index] = NULL;
		index++;
	}
}

	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    
L->First = NULL; //inicializacia vsetko na NULL
L->Act = NULL;
L->Last = NULL;

}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
while(L->First != L->Last) //kym sa prvy nerovna posledny vymazuj prvky
{
	tDLElemPtr tmp = L->First->rptr;
	free(L->First);
	L->First = tmp;
}
free(L->First); //vymaz posledny ktory ostal
L->First = NULL; //daj ukazatele do povodneho stavu po inicializacii
L->Act = NULL;
L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

tDLElemPtr element = (tDLElemPtr) malloc (sizeof(struct tDLElem));
	if (element == NULL){
		DLError();
		return; 
	}

element->data = val;
if(L->First == NULL)
{
	L->First = element;
	L->Last = element;
	element->lptr = NULL;
	element->rptr = NULL;
}
else
{
	L->First->lptr = element;
	element->rptr = L->First;
	element->lptr = NULL;
	L->First = element;
}

}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
tDLElemPtr element = (tDLElemPtr) malloc (sizeof(struct tDLElem));
	if (element == NULL){
		DLError();
		return;
	}

element->data = val;
if(L->First == NULL)
{
	L->First = element;
	L->Last = element;
	element->lptr = NULL;
	element->rptr = NULL;
}
else
{
	element->lptr = L->Last;
	element->rptr = NULL;
	L->Last->rptr = element;
	L->Last = element;
}	

}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
L->Act = L->First; //aktivita na prvy prvok

}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
	
 L->Act = L->Last; //aktviita na posledny prvok
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

if(L->First == NULL) //ak je prazdny
	DLError();
else
	*val = L->First->data; //na adresu premennej da hodnotu posledneho prvku seznamu

}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

if(L->Last == NULL)
	DLError();
else
	*val = L->Last->data;	//na daresu da hodnotu posledneho prvku seznamu
	
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
if(L->First != NULL) //ak nie je prazdny
{
	if(L->First == L->Act)
		L->Act = NULL;
	if(L->First->rptr == NULL)
	{
		free(L->First);
		L->First = NULL;
		L->Last = NULL;
	}
	else
	{	
	tDLElemPtr tmp = L->First->rptr;
	free(L->First);
	L->First = tmp;
	L->First->lptr = NULL;
	}
}	

}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
if(L->Last != NULL)//ak nieje seznam prazdny
{
	if(L->Last == L->Act)
		L->Act = NULL;
	
	if(L->Last == L->First)
	{
		L->First = NULL;
		free(L->Last);
	}
	else
	{
		tDLElemPtr tmp = L->Last;
		L->Last = L->Last->lptr;
		L->Last->rptr = NULL;
		free(tmp); //uvolneni polozky
	}

}	
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
if(L->Act != NULL && L->Act != L->Last)	//ak existuje aktivny a nie je posledny
{
	if(L->Act->rptr == L->Last)
	{
		tDLElemPtr tmp = L->Last;
		L->Last = L->Act;
		L->Act->rptr = NULL;
		free(tmp);

	}
	else
	{
		tDLElemPtr tmp = L->Act->rptr;
		L->Act->rptr = tmp->rptr;
		tmp->rptr->lptr = L->Act;
		free(tmp);
	}
}	

}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
if(L->Act != NULL && L->Act != L->First) //ak existuje aktviny a nie je prvy
{
	if(L->Act->lptr == L->First)
	{
		tDLElemPtr tmp = L->First;
		L->Act->lptr = NULL;
		L->First = L->Act;
		free(tmp);
	}
	else
	{
		tDLElemPtr tmp = L->Act->lptr;
		L->Act->lptr = tmp->lptr;
		tmp->lptr->rptr = L->Act;
		free(tmp);
	}
}
				
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
if(L->Act != NULL) //ak neni aktviny prvek nerob nic
{
	tDLElemPtr element = (tDLElemPtr) malloc (sizeof(struct tDLElem));
	if (element == NULL){ //ak sa nepodarila alokacia pamate skonc s chybou
		DLError();
		return;
	}
	element->data = val;

	if(L->Act == L->Last)//ak je aktviny poslendny novy pridaj na koniec seznamu
	{
		L->Act->rptr = element;
		element->lptr = L->Act;
		element->rptr = NULL;
		L->Last = element;
	}
	else
	{
		element->rptr = L->Act->rptr;
		element->lptr = L->Act;
		L->Act->rptr->lptr = element;
		L->Act->rptr = element;
	}
}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
if(L->Act != NULL) //ak neni aktivny prvek nerob nic
{
	tDLElemPtr element = (tDLElemPtr) malloc (sizeof(struct tDLElem));
	if (element == NULL){
		DLError();
		return;
	}
	element->data = val;
	if(L->Act == L->First) //ak je aktivny prvy prvok, vloz ho na prve miesto
	{
		L->Act->lptr = element;
		L->First = element;
		element->lptr = NULL;
		element->rptr = L->Act;
	}
	else 
	{
		element->rptr = L->Act;
		element->lptr = L->Act->lptr;
		L->Act->lptr->rptr = element;
		L->Act->lptr = element;
	}
}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

if(L->Act != NULL)
	*val = L->Act->data; //ak je akivny na adresu val daj hodnotu aktivneho prvku
else
	DLError();
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
if(L->Act != NULL)
	L->Act->data = val; //hodnota aktiv.elementu bude val

}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
if(L->Act != NULL)
	L->Act = L->Act->rptr; //posun aktivity doprava
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	
if(L->Act != NULL)
	L->Act = L->Act->lptr; //posun aktivity dolava
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
return (L->Act == NULL) ? 0 : 1; //vrati 1 ak je aktivny, inak 0
}

/* Konec c206.c*/

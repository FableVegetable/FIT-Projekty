/**
@file Proj3.h
*/

/**
 * @defgroup Structures Structures
 * @{
 */

/**
  * @struct obj_t
  * @brief Structure of object. 
  
 * @defgroup VAR1 Struct obj_t
   @{
  * @var obj_t::id 
  * Member <b><i>id</i></b> is ID of object.
  * @var obj_t::x 
  * Member <b><i>x</i></b> is first position data.	
  * @var obj_t::y 
  * Member <b><i>y</i></b> is second position data.
  *@}
  */
struct obj_t {
    int id;
    float x;
    float y;
};

/**
  * @struct cluster_t
  * @brief Structure of clusters.
 *@defgroup VAR2 Struct cluster_t 
  @{
  * @var cluster_t::size
  *  Member <b><i>size</i></b> is number of objects, that cluster contains.
  * @var cluster_t::capacity
  *  Member <b><i>capacity</i></b> is number, that describes alloc'd number of objects.
  * @var cluster_t::obj
  *  Member <b><i>obj</i></b> is pointer to structure of objects. 
  *@}
  */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/**@}*/


/**
 * @defgroup Variables Global variables
 * @{*/

 /**     @brief  Constant. Chunk of cluster objects. Value recommended for reallocation.
   */
extern const int CLUSTER_CHUNK;

/**
  * @} */


/**
 * @defgroup FunctionsAll Functions
 * @{
 */

/**
 * @defgroup Functions Functions >>> Init, Clear, Remove, Merge, Append & Resize  
 * @{
 */

/**
  *  
  * Function, that initializes cluster, what means allocation of memory and setting cluster's capacity and size.
  * It allocates memory for number of objects, which represents parameter capacity.
  * @returns	It returns no value(void).
  * @param c
  * Cluster. Pointer to structure of clusters. Certain cluster that is supposed to be initialized.
  * @param cap
  * Capacity. Number that represents allocation of memory for number of objects.
  * @pre	
     -# <b>c</b> pointer to cluster can not point to NULL.
     -# <b>cap</b> contains a positive integer.
  * @post
     - There should be place in memory for objects allocation. 
  */
void init_cluster(struct cluster_t *c, int cap);

/**
  * 	Function, that frees alloc'd memory of cluster. Set value of capacity and size to 0.
  *	@returns		It returns no value(void). 
  *	@param c		Cluster, that will be freed.
  * @pre
     - <b>c</b> cluster must be alloc'd.
  * @post
     - There should be place in memory for objects allocation.

  */
void clear_cluster(struct cluster_t *c);


/**
  *  Function, that changes capacity of cluster 'c' to new capacity 'new_cap'.
  *  @returns 		Pointer to cluster which capacity was changed.
  *  @param c
  *  Cluster. Function changes cluster's capacity.	
  *  @param new_cap		New capacity of cluster.
  * @pre
     -# <b>c</b> pointer to cluster can not point to NULL.
     -# <b>c->capacity</b> capacity of cluster c contains positive integer.			
     -# <b>new_cap</b> contains a positive integer.
  * @post
     -# There should be place in memory for objects reallocation.
  */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);




/**
  *	Function, that adds object 'obj' to end of cluster. It expands cluster, if object has no place in it - low capacity.
  	@returns 	 It returns no value(void).
  *	@param c		Cluster. Cluster to which we are putting new object.
  *	@param obj		Certain object with its id, and x, y coordinates.
  * @pre
     - <b>c</b> size of cluster must be larger or equal to capacity of cluster to resize it.

  */
void append_cluster(struct cluster_t *c, struct obj_t obj);




/**
  *  	Function, that adds objects to cluster 'c1' from cluster 'c2'. If it is necessary cluster 'c1' will be expanded.
  *					Objects in cluster 'c1' will be sorted from lowest to highest according to their IDs.
  *					Cluster 'c2' will not be changed.		  
  *     @returns			It returns no value(void).
  *  	@param c1			Cluster no.1 to which we add objects from cluster no.2.
  *  	@param c2			Cluster no.2. We copy objects from it to cluster no.1.
 * @pre
     -# <b>c1</b> first pointer to cluster can not point to NULL.
     -# <b>c2</b> second point to cluster can not point to NULL.
  * @post
     -# Capacity of <b>c1 cluster</b> is increased by @sa CLUSTER_CHUNK

  */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);




/**
  *	Function that deletes cluster from filed of clusters 'carr'. Field of clusters contains 'narr' clusters.
  *	Deleted cluster is on 'idx' index.
  *  	@param carr		Field of clusters.
  *  	@param narr		Number of clusters in field of clusters.
  *  	@param idx		Index, that represents cluster we want to delete.
  *	@returns		It returns <b>narr</b>
  *     @pre
     	  -# <b>narr</b> contains a positive integer.
     	  -# <b>idx</b> contains a positive integer and its lower than <b>narr</b>

  */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
  *@} */



/**
 * @defgroup DistanceFunctions Distance functions
 * @{
 */


/**
  *	Function, that calculates Eucleadian distance between two objects.
  *	@param o1		Object no.1.
  *	@param o2		Object no.2.
  *	@returns		It returns distance between two objects of float type.
  * @pre
     -# <b>o1</b> pointer to object1 can not point to NULL.
     -# <b>o2</b> pointer to object2 can not point to NULL.
  * @post  
     - included header file <b><math.h></b> because of functions <b>sqrt, pow</b>
  */

float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
  *	Function that calculates distance between two clusters.
  *	@returns		It returns distance of float type.
  *	@param c1		Cluster no.1.
  * 	@param c2		Cluster no.2.
 * @pre
     -# <b>c1</b> pointer to cluster1 can not point to NULL.
     -# <b>c2</b> pointer to cluster2 can not point to NULL.
     -# <b>c1->size</b> contains a positive integer.
     -# <b>c2->size</b> contains a positive integer.
  */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
  *	Function searches two closest clusters. In the field of clusters 'carr'. Identifies clusters that were found by their indexes in field of clusters 'carr'. 
	Function saves to memory found clusters to address 'c1' resp. 'c2'. 
  *     @returns                It return no value(void).
  *	@param carr	Field of clusters.
  *	@param narr	Number of clusters in the field of clusters.
  *	@param c1	Address of index of cluster no.1.
  *	@param c2	Address of index of cluster no.2.
  * @pre
     - <b>narr</b> contains a positive integer.
  */

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);


/**
  *@}*/



/**
 * @defgroup Functions2 Functions >>> Load, Sort & Print
 * @{
 */

/**
  *  	Function, that sorts objects in cluster 'c' according to their ID. From lowest to higest.
  *     @returns                It return no value(void).
  *  	@param c		Cluster. Certain cluster, that we work with.
  */
void sort_cluster(struct cluster_t *c);





/**
  *	Function, that prints cluster 'c' on stdout.
  *	@returns		It return no value(void).
  *	@param c		Cluster, that will be printed. Every object's id, x, y  of every cluster will be printed.
  */
void print_cluster(struct cluster_t *c);





/**
  *	From file 'filename' function loads objects. For every object makes cluster and cluster adds to field of clusters.
  *				Function allocates memory for field of every cluster and pointer to first cluster saves to memory, where parameter 'arr' points.
				In case of error where 'arr' points, saves value NULL.
  *     @returns                It return no value(void).
  *	@param filename		Name of file, that we read informations about certain objects.
  *	@param arr		Field of clusters, that we work with.
  *@pre
     -# <b>arr</b> pointer to field of clusters can not point to NULL and we should have memory for its allocati$
     -# <b>file</b> has to be declared and opened.
     -# coordinates <b>x</b> and <b>y</b> have to be in interval <0, 1000>
  *@post
     - <b>file</b> has to be closed
  */
int load_clusters(char *filename, struct cluster_t **arr);





/**
  *	Function that prints field of clusters. Parameter 'carr' is pointer to first cluster.
				Function prints first 'narr' clusters.
  *     @returns                It return no value(void).
  *	@param carr		Pointer to first cluster of field of clusters.
  *	@param narr		Number of printed clusters.
  */
void print_clusters(struct cluster_t *carr, int narr);

/** 
  *@} */

/** 
  *@} */

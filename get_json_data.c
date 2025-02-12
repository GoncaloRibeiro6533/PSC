#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "get_json_data.h"

typedef struct{
	char *memory;
	size_t size;
} Buffer;

static size_t my_write(char *ptr, size_t size, size_t nmemb, void *fd){
	Buffer *b = (Buffer *)fd; //cast
	size_t aux = size*nmemb;
	/*increase the buffer in order to hold more aux bytes */
	b->memory = realloc(b->memory, b->size + aux + 1);
	if(b->memory == NULL){
		/*out of memory*/
		printf("not enogh memory\n");
		return 0;
		}
		/*copy the new data block*/
	memcpy(b->memory + b->size, ptr, aux);
	/*update size and close string*/
	b->size += aux;
	b->memory[b->size] = '\0';
	return aux;
	}

json_t *http_get_json_data(const char *url){
	Buffer buffer;
	buffer.memory = NULL;
	buffer.size = 0;
	char *result = NULL;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	/* initialize a CURL handle in order to perform an HTTP GET request */
	CURL *curl = curl_easy_init();  //creating curl object
	
	if (curl != NULL) {
		/* configure the handler with the URL */
		curl_easy_setopt(curl, CURLOPT_URL, url);
		
		/* set WRITEFUNCTION and WRITEDATA to save body and data in a block of memory */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
	
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		
		/* set the VERBOSE option */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		
		/*perform the HTTP GET request*/
		CURLcode res = curl_easy_perform(curl);
		
		/* cleanup the curl handler */
		curl_easy_cleanup(curl);
		
		if (CURLE_OK != res) {
				fprintf(stderr, "curl told us %d\n", res);
				return NULL;
			}
		}		
		result = buffer.memory;
		json_t *root;
		json_error_t error;
		root = json_loads(result, 0, &error);
		if(root == NULL) printf("Error %d: %s\n", error.line, error.text);
		curl_global_cleanup();
		return root;
}



void allocateString(char **dest, const char *string){
	*dest = malloc(sizeof(char) * strlen(string) + 1);
	strcpy(*dest, string);
}


//p é uma pointer to pointer to Product
void makeProduct(int id, float price, const char *description, const char *category, Product **p){
	*p = malloc(sizeof(Product));
	if(*p == NULL) printf("Error not enough memory");
	(*p)->id = id;
	(*p)->price = price;
	allocateString((char**)&(*p)->description, description);
	allocateString((char**)&(*p)->category, category);
	return;
}

Products *makeProducts(const json_t *json){
	Products *p = malloc(sizeof(Products));
	json_t *array, *products,  *description, *category;
		array = json_object_get(json, "products");
		int arrayLen = json_array_size(array);
		p->size = arrayLen;
		p->products = malloc(sizeof(Product) * p->size);
		if(p->products == NULL) printf("Error not enough memory");
		for (int i = 0; i < arrayLen; i++){
			products = json_array_get(array, i);
			int id = (int)json_number_value(json_object_get(products, "id"));
			float price = (float)json_number_value(json_object_get(products, "price"));
			description = json_object_get(products, "description");
			category  = json_object_get(products, "category");
			makeProduct(id, price, json_string_value(description),
			json_string_value(category), &p->products[i]);
	}
	return p;
} 

Products *products_get(){
	char *url = "https://dummyjson.com/products";
	const json_t *data = http_get_json_data(url);
	return makeProducts(data);
}

void printProduct(const Product *p){
	printf("\nid = %d \n", p->id);
	printf("price = %f \n", p->price);
	printf("description = %s \n", p->description);
	printf("category = %s \n", p->category);
}

void printProducts(const Products *prs){
	printf("size: %d\n", prs->size);
	
	for(int i = 0; i< prs->size; i++){
		printProduct(prs->products[i]);
	}
}




void makeUser(int id, const char *name, User **u){
	*u = malloc(sizeof(User));
	if(*u == NULL) printf("Error not enough memory");
	(*u)->id = id;
	allocateString((char**)&(*u)->name, name);
	return;
}

Users *makeUsers(const json_t *json){
	Users *u = malloc(sizeof(Users));
	json_t *array, *users, *firstName, *lastName;
		array = json_object_get(json, "users");
		int arrayLen = json_array_size(array);
		u->size = arrayLen;
		u->users = malloc(sizeof(User) * u->size);
		if(u->users == NULL) printf("Error not enough memory");
		for (int i = 0; i < arrayLen; i++){
			users = json_array_get(array, i);
			firstName = json_object_get(users, "firstName");
			lastName = json_object_get(users, "lastName");
			char * aString = strcat((char*)json_string_value(firstName), " ");
			strcat(aString, json_string_value(lastName));
			int id = (int)json_number_value(json_object_get(users, "id"));
			makeUser(id, aString, &u->users[i]);
	}
	return u;
} 

Users *users_get(){
	char *url = "https://dummyjson.com/users";
	const json_t *data = http_get_json_data(url);
	return makeUsers(data);
}



void printUser(const User *p){
	printf("id = %d \n", p->id);
	printf("name = %s \n", p->name);
}

void printUsers(const Users *prs){
	printf("size: %d\n", prs->size);
	
	for(int i = 0; i< prs->size; i++){
		printUser(prs->users[i]);
	}
}


void makeCart(int userId, size_t nProducts, Cart **c, json_t *arrayProducts){
	size_t s = sizeof((*c)->products[0])*nProducts + sizeof(Cart);
	*c = malloc( s);
	if(*c == NULL) printf("Error not enough memory");
	(*c)->user_id = userId;
	(*c)->n_products = nProducts;
	for (int i = 0; i < nProducts; i++){			
			json_t *products = json_array_get(arrayProducts, i);
			int productId = (int)json_number_value(json_object_get(products, "id"));
			size_t quantity = (size_t)json_number_value(json_object_get(products, "quantity"));
		(*c)->products[i].id = productId;
		(*c)->products[i].quantity = quantity;
	}	
	return;
}

Carts *makeCarts(const json_t *json){
	Carts *c = malloc(sizeof(Carts));
	if(c == NULL) printf("Error not enough memory");
	json_t *array, *carts, *arrayProducts;
		array = json_object_get(json, "carts");
		int arrayLen = json_array_size(array);
		c->size = arrayLen;
		c->carts = malloc(sizeof(Cart) * c->size);
		if(c->carts == NULL) printf("Error not enough memory");
		for (int i = 0; i < arrayLen; i++){
			carts = json_array_get(array, i);
			int userId = (int)json_number_value(json_object_get(carts, "userId"));
			size_t nProducts = (size_t)json_number_value(json_object_get(carts, "totalProducts"));
			arrayProducts = json_object_get(carts, "products");
			makeCart(userId, nProducts, &c->carts[i], arrayProducts);
		}
	return c;
} 

Carts *carts_get(){
	char *url = "https://dummyjson.com/carts";
	const json_t *data = http_get_json_data(url);
	return makeCarts(data);
}



void printCart(const Cart *p){
	printf("user_id = %d \n", p->user_id);
	printf("n_products = %ld \n", p->n_products);
	for(int i = 0; i< p->n_products; i++){
		printf("id = %d\n quantity = %ld\n", p->products[i].id, p->products[i].quantity);
	}
}

void printCarts(const Carts *prs){
	printf("size: %d\n", prs->size);
	for(int i = 0; i< prs->size; i++){
		printf("----------------------------------");
		printCart(prs->carts[i]);
	}
}


void writeFileProducts( Products *p,void *fd) {
	fprintf(fd, "ID, Price, Description, Category\n");
	for(int i = 0; i< p->size; i++){
		fprintf(fd,"%d, %f, %s, %s\n",p->products[i]->id, 
		p->products[i]->price, p->products[i]->description, p->products[i]->category );
	}
}

void writeFileUsers( Users *p,void *fd) {
	fprintf(fd, "---------------------------------------------------------\n");
	fprintf(fd, "ID, Name\n");
	for(int i = 0; i< p->size; i++){
		fprintf(fd,"%d, %s\n", 
		p->users[i]->id, p->users[i]->name);
	}
}

void writeFileCarts( Carts *p,void *fd) {
	fprintf(fd, "---------------------------------------------------------\n");
	for(int i = 0; i< p->size; i++){
		fprintf(fd, "UserId, n_products\n");
		 fprintf(fd, "%d, %ld\n",p->carts[i]->user_id,
		 p->carts[i]->n_products);
 		fprintf(fd, "products_id, quantity\n");
		 for(int j = 0; j< p->carts[i]->n_products; j++){
			fprintf(fd,"%d, %ld\n", 
			p->carts[i]->products[j].id, p->carts[i]->products[j].quantity);
			}
	}
}

/*
gcc get_json_data.c -o get_json -l jansson -l curl -Wall

*//*
int main(){
		Products *p = products_get();
		printProducts(p);
		Users *u = users_get();
		printUsers(u);
		Carts *c = carts_get();
		printCarts(c);
		FILE *fpt;
		fpt= fopen("Info.csv","w+");;
		writeFileProducts(p,fpt);
		writeFileUsers(u,fpt);
		writeFileCarts(c,fpt);
		fclose(fpt);
}*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "get_json_data.h"

#define BUFFER_CHUNK 50

//gcc 4.c -l get_json_data -l jansson -l curl -Wall-L .
//Lista duplamente ligada intrusiva, com sentinela
typedef struct List_node{
	struct List_node *next, *prev;
	int user_id;
	const char *name;
	float totalPrice;
	int listed;
}	List_node;



List_node queue = {.next = &queue, .prev= &queue};


void free_users(Users *u){
	for(int i =0; i< u->size; i++){
			free((char*)u->users[i]->name);
			free(u->users[i]);
		}
	free(u);
}

void free_carts(Carts *c){
	free(c);
}

void free_products(Products *p){
	for(int i =0; i< p->size; i++){
				free((char*)p->products[i]->description);
				free((char*)p->products[i]->category);
				free(p->products[i]);
				}
				free(p->products);
				free(p);
	}



void users_insert(Users *u){
	if(queue.next != &queue) return;
	List_node *first = queue.next;
	for(int i = 0; i < u->size; i++){
		List_node *node = malloc(sizeof(List_node));
		if(i ==0){
			node->user_id = u->users[i]->id;
			node->name = u->users[i]->name;
			node->next= &queue;
			node->prev =queue.prev;
			queue.prev->next= node;
			queue.prev = node;
			first = node;
		} else {
				first = queue.next;
				
				while(first != &queue){
					if(strcmp((char*)first->name,u->users[i]->name)>0) {
						break;
					}
					first = first->next;
				}
				
				node->name = u->users[i]->name;
				node->user_id = u->users[i]->id;
				node->totalPrice = 0.0f;
				node->listed =0;
				node->next = first;
				node->prev=first->prev;
				first->prev->next = node;
				first->prev=node;
		}
		
	}
	
}

void list_users() {
	if(&queue == queue.next){
		printf("Lista de users vazia\n");
		return;
	}
	for(List_node *node = queue.next; node!=&queue; node=node->next)
		printf("Name: %s Id: %d\n", node->name, node->user_id);
}


void carts_insert(Carts *c, Products *p){
	if(queue.next->listed !=0) return;
	for(int i = 0; i < c->size; i++){
		float totalPrice = 0;
		for(int j = 0; j< c->carts[i]->n_products; j++){ 
			  for(int a = 0; a < p->size; a++ ){
				if(p->products[a]->id == c->carts[i]->products[j].id){ 
					totalPrice += p->products[a]->price;
					//printf("Product id = %d\n", p->products[a]->id);
					//printf("Product on cart id = %d\n", c->carts[i]->products[j].id);
					//printf("User id = %d\n", c->carts[i]->user_id);

					} else totalPrice+=0.0f;
				}
			}
		List_node *first = queue.next;
		while(first != &queue){
			if(first->user_id == c->carts[i]->user_id) {
					break;
			}
			first=first->next;
		}
		if(first->user_id == c->carts[i]->user_id) first->totalPrice = totalPrice;
	}	
}



void list_carts(Users *users) {
	if(&queue == queue.next){
		printf("Lista de carts vazia\n");
		return;
	}
	float minValue=queue.next->totalPrice;
	float lastValue =0.0f;
	List_node *node;
	int size = users->size; 
	
	while(size!=0){
		for(List_node *node = queue.next; node!=&queue; node=node->next){
		if(node != queue.next && node->totalPrice <= minValue && node->listed==0) {
			minValue =node->totalPrice;
			}
		if(node != queue.next && node->totalPrice>minValue && node->listed==0) {
			lastValue =node->totalPrice;
			}		
		if(node != queue.next && node->totalPrice>minValue && node->totalPrice< lastValue && node->listed==0) {
			lastValue =node->totalPrice;
			}		
		}	
		
	for(node = queue.next; node!=&queue; node=node->next){
		if(node->totalPrice == minValue && node->listed ==0){
			printf("Name: %s Id: %d totalPrice: %f\n", node->name, node->user_id ,node->totalPrice);
			node->listed =1;
			size = size-1;
		}

	}
	minValue = lastValue;
	}
	for(node = queue.next; node!=&queue; node=node->next){
			node->listed =0;
		}
	return;
}


char *readline(){
	static char *buffer = NULL;
	static size_t buffer_size = 0;
	size_t index = 0;
	int c = getchar();
	while(c != '\n'){
		if(index + 1 >= buffer_size ){
			buffer_size += BUFFER_CHUNK;
			buffer = realloc(buffer, buffer_size);
			if(buffer == NULL) {
				printf("Out of memory\n");
				break;
			}
		}
		buffer[index++] = c;
		c = getchar();
	}
	buffer[index] = '\0';
	return buffer;
}


void free_list(){
	for (List_node *next, *node = queue.next; node != &queue; node = next) {
		next = node->next;
		free(node);
	}
}


int main(){
	do{
	printf("Option? \n");
	char *line = readline();
	if(line == NULL) {
		printf("Invalid option\n");
		break;
		}
	Users *u;
	Products *p;
	Carts *c;
	char *option = strtok(line, " \n");
	switch(tolower(option[0])){
			case 's':
				printf("Finishing the program\n");
				if(c !=NULL)free_carts(c);
				if(u !=NULL)free_users(u);
				if(p !=NULL)free_products(p);
				if(queue.next!= &queue)free_list();
				free(line);
				exit(0);
			case 'u':
				if(u==NULL) u =users_get();
				users_insert(u);
				list_users();
				break;
			case 'c':
				if(c==NULL) c = carts_get();
				if(p==NULL) p = products_get();
				carts_insert(c,p);
				list_carts(u);
				break;
			default:
				printf("Invalid option\n");
				break;
		}
	
	}
	while(1);
	return 0;
}

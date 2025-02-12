#include <stdio.h>
#include <stdlib.h>
#include "get_json_data.h"

int main(){
		Products *p = products_get();
		//printProducts(p);
		Users *u = users_get();
		//printUsers(u);
		Carts *c = carts_get();
		//printCarts(c);
		FILE *fpt;
		fpt= fopen("Info.csv","w+");;
		writeFileProducts(p,fpt);
		writeFileUsers(u,fpt);
		writeFileCarts(c,fpt);
		fclose(fpt);
		for(int i =0; i< p->size; i++){
			//free(p->products[i]->id);
			//free(p->products[i]->price);
			free((char*)p->products[i]->description);
			free((char*)p->products[i]->category);
			free(p->products[i]);
		}
		free(p->products);
		free(p);
		
		for(int i =0; i< u->size; i++){
			//free(u->users[i]->id);
			free((char*)u->users[i]->name);
			free(u->users[i]);
		}
		free(u->users);
		free(u);
		for(int i =0; i< c->size; i++){
			free(c->carts[i]);
		}
		free(c->carts);
		free(c);
		return 0;
}

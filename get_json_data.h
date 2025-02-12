#ifndef GET_JSON_DATA_H
#define GET_JSON_DATA_H


typedef struct{
	int id;
	float price;
	const char *description;
	const char *category;
} Product;

typedef struct{
	int size;
	Product **products;
}	Products;




typedef struct{
	int id;
	const char *name;
} User;

typedef struct{
	int size;
	User **users;
}	Users;


typedef struct {
	int user_id;
	size_t n_products;
	struct {
		int id;
		size_t quantity;
		} products [];
} Cart;
	
typedef struct {
	int size;
	Cart **carts;
}	Carts;	


Products *products_get();

Users *users_get();

Carts *carts_get();

void writeFileProducts( Products *p,void *fd);


void writeFileUsers( Users *p,void *fd);

void writeFileCarts( Carts *p,void *fd);


#endif



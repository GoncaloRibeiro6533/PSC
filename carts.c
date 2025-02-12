

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




void makeCart(int userId, size_t nProducts, Cart **c, json_t *arrayProducts){
	size_t s = sizeof((*c)->products[0])*nProducts + sizeof(Cart);
	printf("%ld\n", s);
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
	json_t *array, *carts, *products, *arrayProducts;
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

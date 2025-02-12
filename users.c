

typedef struct{
	int id;
	const char *name;
} User;

typedef struct{
	int size;
	User **users;
}	Users;




void makeUser(int id, const char *name, User **u){
	*u = malloc(sizeof(User));
	if(*u == NULL) printf("Error not enough memory");
	(*u)->id = id;
	allocateString(&(*u)->name, name);
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
			char space = ' ';
			char *s = &space;
			char * aString = strcat(json_string_value(firstName), s);
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

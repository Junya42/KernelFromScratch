#ifndef CHAINED_LIST_H
# define CHAINED_LIST_H

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
} t_list;

t_list *create_list(void *content);
int listsize(t_list *lst);
t_list *listlast(t_list *lst);
void listclear(t_list *lst);
void listadd_back(t_list *alst, void *content);
t_list *create_elem(char *content);
void free_elem(t_list *elem);
void free_list(t_list *lst);
void listadd_and_replace(t_list *alst, char *content);

#endif
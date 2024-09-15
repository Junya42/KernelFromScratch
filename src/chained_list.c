#include "../includes/chained_list.h"
#include "../includes/memory.h"
#include "../includes/stddef.h"
#include "../includes/common.h"

t_list *create_list(void *content)
{
	t_list *list = malloc(sizeof(t_list));
	if (!list)
		return NULL;

	list->content = content;
	list->next = NULL;

	return list;
}

int listsize(t_list *lst)
{
	int size = 0;
	while (lst)
	{
		size++;
		lst = lst->next;
	}
	return size;
}

t_list *listlast(t_list *lst)
{
	t_list *last = lst;

	if (!last)
		return NULL;
	while (last->next)
		last = last->next;
	return last;
}

void listclear(t_list *lst)
{
	t_list *tmp;

	while (lst)
	{
		tmp = lst->next;
		free(lst);
		lst = tmp;
	}
}

void listadd_back(t_list *alst, void *content)
{
	t_list *new = create_list(content);
	if (!new)
		return;
	t_list *last = listlast(alst);

	if (!last)
	{
		alst = new;
		return;
	}
	last->next = new;
}

t_list *create_elem(char *content) {
	t_list *elem = malloc(sizeof(t_list));
	if (!elem)
		return NULL;

	elem->content = (void *)ft_strdup(content);
	elem->next = NULL;

	return elem;
}

void free_elem(t_list *elem) {
	free(elem->content);
	free(elem);
}

void free_list(t_list *lst) {
	t_list *tmp;

	while (lst)
	{
		tmp = lst->next;
		free_elem(lst);
		lst = tmp;
	}
}

void listadd_and_replace(t_list *alst, char *content) {

	t_list *new = create_list((void *)content);
	if (!new)
		return;

	if (!alst)
	{
		alst = new;
		return;
	}

	if (listsize(alst) == 100) {
		t_list *tmp = alst;
		alst = alst->next;
		free_elem(tmp);
	}

	t_list *last = listlast(alst);

	last->next = new;
}
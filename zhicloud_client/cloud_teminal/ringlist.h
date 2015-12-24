#ifndef DULIST_H
#define DULIST_H
/* ���Ա��˫������洢�ṹ */
typedef void * elemtype;
typedef struct dulnode
{ 
  elemtype data;
  struct dulnode *prior,*next;
}dulnode,*dulinklist;
/*��ͷ����˫��ѭ������Ļ�������(14��) */
void    list_init(dulinklist *l);
void    list_destory(dulinklist *l);
void    list_clear(dulinklist l); 
int     list_empty(dulinklist l); 
int     list_length(dulinklist l); 
int     list_getelem(dulinklist l,int i,elemtype *e);
int     list_locateelem(dulinklist l,elemtype e,int(*compare)(elemtype,elemtype));
struct dulnode* list_priorelem(dulinklist l, elemtype cur_e, elemtype *pre_e);
struct dulnode* list_nextelem(dulinklist l, elemtype cur_e, elemtype *next_e);
dulinklist list_get_elemp(dulinklist l,int i); /* ��� */
int     list_insert(dulinklist l,int i,elemtype e); 
int     list_delete(dulinklist l,elemtype e); 
void    list_traverse(dulinklist l,void(*visit)(elemtype));
void    list_traverse_back(dulinklist l,void(*visit)(elemtype));
  
#endif
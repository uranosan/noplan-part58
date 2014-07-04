#ifndef __LIST_H_INCLUDED__
#define __LIST_H_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------------------------
  List: Global Typedef
  ---------------------------------------------------------------------------*/
typedef struct tagListEntry	ListEntry;

/*-----------------------------------------------------------------------------
  List: Static Typedef
  ---------------------------------------------------------------------------*/
typedef struct tagListNode	ListNode;

/*-----------------------------------------------------------------------------
  List: Global Structures
  ---------------------------------------------------------------------------*/
struct tagListEntry {
	int			datasize;	// ノードに格納するdataのbyte数。ユーザ定義。
	int			__nodesum;	// リストが持つノードの数
	int			__isinited;	// リストが初期化されているかどうか。値が０なら初期化済みと見なす。
	ListNode	*headnode;	// 先頭ノード
	ListNode	*tailnode;	// 末尾ノード
};

/*-----------------------------------------------------------------------------
  List: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern void  List_PushBack	(ListEntry *list, void* data);
extern void  List_PushFront	(ListEntry *list, void* data);
extern void* List_Back		(ListEntry *list);
extern void* List_Front		(ListEntry *list);
extern void  List_PopBack	(ListEntry* list);
extern void  List_PopFront	(ListEntry* list);
extern void  List_Clear		(ListEntry *list);
extern int   List_Size		(ListEntry *list);
extern int   List_Empty		(ListEntry *list);
extern void* List_Search	(ListEntry *list, const void* key, int (*compar)(const void* val1, const void* val2));

#endif/*__LIST_H_INCLUDED__*/


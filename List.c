#include "List.h"

/*------------------------------------------------------------------------
  静的なデータ宣言
  ------------------------------------------------------------------------*/
struct tagListNode {
	struct tagListNode	*prev;
	struct tagListNode	*next;
	void				*data;
};

/*------------------------------------------------------------------------
  静的な広域変数の実体
  ------------------------------------------------------------------------*/
static const int debug = 0;// 0でデバッグ表示をオフ

/*------------------------------------------------------------------------
  静的な広域関数
  ------------------------------------------------------------------------*/
static void Error(ListEntry* list, const char *errmsg)
{
	fprintf(stderr, "%s : List address: %d", errmsg, (unsigned int)list);
	exit(2);
}

/**
 * リストが有効かを調べる。
 * @ret 有効でなければ０。有効ならそれ以外。
 */
static int IsValid(ListEntry* list)
{
	if (list == NULL) {
		return 0;
	} else if (list->__isinited != 0) {
		Error(list, "error: List: エントリーを初期化してください。");
		return 0;
	} else if (list->datasize <= 0) {
		Error(list, "error: List: データサイズを設定してください。");
		return 0;
	}
	
	return 1;
}

/**
 * ノードとノード内データのメモリを確保して、そのノードを返す
 */
static ListNode* GetNode(ListEntry* list, void* data)
{
	ListNode *addnode;
	
	addnode = (ListNode*)malloc(sizeof(ListNode));
	if (addnode == NULL) {
		Error(list, "List: PushBack: addnode is null.");
	}
	if (debug)
		printf("\tin List: malloc data size: addnode: %d byte: address: %d\n",
				sizeof(*addnode), (unsigned int)addnode);
	
	addnode->data = malloc(list->datasize);
	if (addnode->data == NULL) {
		Error(list, "List: PushBack: addnode->data is null.");
	}
	if (debug)
		printf("\tin List: malloc data size: addnode->data: %d byte: address: %d\n",
				list->datasize, (unsigned int)&(addnode->data));
	
	memmove(addnode->data, data, list->datasize);
	addnode->prev = NULL;
	addnode->next = NULL;
	
	list->__nodesum++;// ノードの数を更新
	
	return addnode;
}

/**
 * リストからノードを削除
 */
static void __Remove(ListEntry* list, ListNode *targetnode)
{
	ListNode *delnode, *prevnode, *nextnode;
	
	if (targetnode == NULL)
		return;
	
	list->__nodesum--;// ノードの数を更新
	
	delnode  = targetnode;
	prevnode = targetnode->prev;
	nextnode = targetnode->next;
	
	// リスト内にノードが一つしかない場合(孤立してる場合)
	if (prevnode == NULL && nextnode == NULL)
	{
		free(delnode->data);
		delnode->data = NULL;
		free(delnode);
		delnode = NULL;
		list->headnode = list->tailnode = NULL;
		return;
	}
	// リスト先頭のノードの場合
	else if (prevnode == NULL) {
		nextnode->prev = NULL;
		list->headnode = nextnode;// 先頭ノードの更新
	}
	// リスト中間にあるノードの場合
	else if (prevnode != NULL && nextnode != NULL) {
		prevnode->next = nextnode;
		nextnode->prev = prevnode;
	}
	// リスト末尾のノードの場合
	else if (nextnode == NULL) {
		prevnode->next = NULL;
		list->tailnode = prevnode;// 末尾ノードの更新
	} else {
		Error(list, "error: List: __Remove: illegal exception error.\n");
	}
	free(delnode->data);
	delnode->data = NULL;
	free(delnode);
	delnode = NULL;
}

/*------------------------------------------------------------------------
  外部に公開する広域関数
  ------------------------------------------------------------------------*/

/**
 * リストが持つノード（要素）数を返す
 */
extern int List_Size(ListEntry* list)
{
	if (! IsValid(list))
		return -1;
	
	return list->__nodesum;
}

/**
 * リストから目的のデータを探す。
 * @ret		(void *)data	見つけたデータへのアドレス。見つからなければNULL。
 * @param	*list			ListEntry
 * @param	*key			目的のデータ
 * @param	*compar			比較用の関数ポインタ（ユーザ定義）
 * compar関数のルール
 *	val1 が val2 よりも小さい場合 : 0 より小さい値を返す
 *	val1 が val2 と一致する場合   : 0 を返す
 *	val1 が val2 よりも大きい場合 : 0 より大きい値を返す
 */
extern void* List_Search(ListEntry *list, const void* key, int (*compar)(const void* val1, const void* val2))
{
	if (! IsValid(list))
		return NULL;
	
	ListNode *thisnode;
	
	for (thisnode = list->headnode; thisnode != NULL; thisnode = thisnode->next)
	{
		if ( (*compar)(key, thisnode->data) == 0 )
		{
			return thisnode->data;
		}
	}
	return NULL;
}

/**
 * リストが空なら真を返す。
 */
extern int List_Empty(ListEntry *list)
{
	return (list->headnode == NULL && list->tailnode == NULL);
}

/**
 * リストの末尾にデータを格納したノードを追加する。
 */
extern void List_PushBack(ListEntry *list, void* data)
{
	if (! IsValid(list))
		return;
	
	ListNode *addnode;
	
	addnode = GetNode(list, data);
	
	if (list->tailnode == NULL) {
		list->headnode = list->tailnode = addnode;
	} else {
		addnode->prev		 = list->tailnode;
		list->tailnode->next = addnode;
		list->tailnode		 = addnode;
	}
	
	if (debug)
		printf("\tin List: PushBack: addnode: address: %d\n", (unsigned int)addnode);//debug
}

/**
 * リストの先頭にデータを格納したノードを追加する。
 */
extern void List_PushFront(ListEntry *list, void* data)
{
	if (! IsValid(list))
		return;
	
	ListNode *addnode;
	
	addnode = GetNode(list, data);
	
	if (list->tailnode == NULL) {
		list->headnode = list->tailnode = addnode;
	} else {
		addnode->next		 = list->headnode;
		list->headnode->prev = addnode;
		list->headnode		 = addnode;
	}
}


/**
 * リスト末尾のノードを削除する
 */
extern void List_PopBack(ListEntry* list)
{
	if (! IsValid(list))
		return;
	__Remove(list, list->tailnode);
}

/**
 * リスト先頭のノードを削除する
 */
extern void List_PopFront(ListEntry* list)
{
	if (! IsValid(list))
		return;
	__Remove(list, list->headnode);
}

/**
 * リストの末尾のノードからデータを返す
 */
extern void* List_Back(ListEntry *list)
{
	if (! IsValid(list))
		return NULL;
	if (List_Empty(list))
		return NULL;
	return list->tailnode->data;
}

/**
 * リストの先頭のノードからデータを返す
 */
extern void* List_Front(ListEntry *list)
{
	if (! IsValid(list))
		return NULL;
	if (List_Empty(list))
		return NULL;
	return list->headnode->data;
}

/**
 * リストをクリア。リスト内のノードをすべて開放。
 */
extern void List_Clear(ListEntry *list)
{
	if (! IsValid(list))
		return;
	
	ListNode *thisnode, *remnode;
	
	for (thisnode = list->headnode; thisnode != NULL; )
	{
		remnode = thisnode;
		if (debug) printf("\tin List: remnode: %d\n", (unsigned int)remnode);//debug
		
		thisnode = thisnode->next;
		free(remnode->data);
		remnode->data = NULL;
		free(remnode);
		remnode = NULL;
	}
	list->headnode = list->tailnode = NULL;	// ポインタをクリア
	list->__nodesum = 0;					// ノードの数を更新
}


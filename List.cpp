
#include "list.h"

///////////////////////////////////// Implementation of ListNode (done here since template) //////////////////////////
ListNode::ListNode(void * newObj, ListNode * newNext)
{
	pObject = newObj;			//make us point to passed in object and next
	next = newNext;
}


void * ListNode::GetObj()		//returns pointer to object
{
	return pObject;
}

unsigned long ListNode::getObjSize()
{
	return objectSize;
}



void ListNode::SetObj(void * newObj, unsigned long size)
{
	objectSize = size;
	pObject = newObj;
}


ListNode * ListNode::getNext()
{
	return next;
}


void ListNode::SetNext(ListNode * newNext)
{
	next = newNext;
}

void ListNode::SetObjSize(unsigned long size)
{
	objectSize = size;
}




///////////////////////////////////// Implementation of List (done here since template) //////////////////////////
List::List() : head(NULL), curGetNode(NULL), end(NULL), size(0)
{
}


List::~List()						//Deconstructor - cleans up memory
{
	Clear();
}

ListNode * List::RemoveHead()			//remove node from head and return it
{
	if(head != NULL)	//if a node exists, return head but remove form list
	{
		ListNode * temp = head;
		head = head->getNext();
		if(head == NULL)
			end = NULL;
		temp->SetNext(NULL);
		size--;
		return temp;
	}
	else
		return NULL;	//no nodes
}


bool List::AddItemToStart(void * item)		//adds item to start of list 
{
	if(item != NULL)
	{
		ListNode * newNode = new ListNode(item, head);

		if(head == NULL)	//if no nodes before make end also be first node
			end = newNode;

		head = newNode;
		size++;
		return true;
	}
	return false;
}

bool List::AddItemToEnd(void * item, unsigned long size)			//adds item to end of list
{
	if(item != NULL)
	{
		size++;
		if(head == NULL)	//if no nodes before
		{
			ListNode * newNode = new ListNode(item, head);

			head = newNode;
			end = newNode;

			newNode->SetObjSize(size);
			return true;
		}
		else	//else there is nodes
		{
			ListNode * newNode = new ListNode(item, NULL);

			end->SetNext(newNode);
			end = newNode;
			newNode->SetObjSize(size);
			return true;
		}
	}
	return false;
}



void List::Clear()					//deletes all nodes (called by deconstructor)
{
	ListNode * cur = head;
	while(cur != NULL)
	{
		head = cur->getNext();
		delete cur;
		cur = head;
	}
	
	size = 0;
	head = NULL;
	end = NULL;
}


void List::DeepClear()				//deletes all nodes and objects they point to
{
	ListNode * cur = head;
	while(cur != NULL)
	{
		head = cur->getNext();
		delete cur->GetObj();
		delete cur;
		cur = head;
	}
	
	size = 0;
	head = NULL;
	end = NULL;
}

int List::GetSize()
{
	return size;
}

ListNode * List::GetFirstNode()		//returns first node, used for getnextnode
{
	curGetNode = head;
	return curGetNode;
}


ListNode * List::GetNextNode()
{
	if(curGetNode != NULL)
		curGetNode = curGetNode->getNext();

	return curGetNode;
}


ListNode * List::GetNodeAt(int pos)	//gets the listnode at position pos, (start = 0), if null returns 0
{
	int i = 0;
	ListNode * cur = head;
	while(cur != NULL)
		if(i++ == pos)	//compare and then increment i
			return cur;
		else
			cur = cur->getNext();

	return NULL;		//didnt find anything
}

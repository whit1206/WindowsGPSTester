//List.h - class to hold generic items in a single linked list
//         Note: when list is destryoed it will only destroy nodes
//               not the objects the nodes point to

#ifndef LIST_H
#define LIST_H

#include <windows.h>

class ListNode
{
	public:
		ListNode(void * newObj = NULL, ListNode * newNext = NULL);
		
		void * GetObj();		//returns pointer to object
		void SetObj(void * newObj, unsigned long size = 0);
		unsigned long getObjSize();
		void SetObjSize(unsigned long size);

		ListNode * getNext();
		void SetNext(ListNode * newNext);

		
	private:
		void * pObject;			//pointer to object pointed to by node
		ListNode * next;
		int objectSize;
};




class List				//List to hold object of type Type
{
	public:
		List();							//Constructor - inits all vars to empty/not used
		~List();						//Deconstructor - cleans up memory

		bool AddItemToStart(void * item);		//adds item to start of list 
		bool AddItemToEnd(void * item, unsigned long size = 0);			//adds item to end of list

		void Clear();					//deletes all nodes (called by deconstructor)
		void DeepClear();				//deletes all nodes and objects they point to

		ListNode * GetFirstNode();		//returns first node, used for getnextnode
		ListNode * GetNextNode();		//called after GetFirstNode to get all nodes in list, returns NULL
										//when at end of list

		int GetSize();					//return the size of the list

		ListNode * RemoveHead();			//remove node from head and return it

		ListNode * GetNodeAt(int pos);	//gets the listnode at position pos

	private:
		int size;
		ListNode * head;				//start of the list 
		ListNode * end;
		ListNode * curGetNode;			//pointer of where GetNode is in list

};


#endif //SENSOR_H

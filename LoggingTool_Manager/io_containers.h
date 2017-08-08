#ifndef IO_CONTAINERS_H
#define IO_CONTAINERS_H

#include <stdlib.h>


template <class Item>
class STACK
{
public:
    STACK()
    {
        head = 0;
        cnt = 0;
    }

    ~STACK()
    {
        while (!empty()) pop();
    }

    bool empty() const
    {
        return head == 0;
    }

    void push(Item x)
    {
        head = new Node(x, head);
        cnt++;
    }

    Item pop()
    {
        Item v = head->item;
        Node *t = head->next;

        delete head;
        head = t;
        --cnt;
        return v;
    }

    void add(Item *a, int size)
    {
        for (int i = 0; i < size; i++)
        {
            Item t = a[i];
            push(t);
        }
    }

    int count() const
    {
        return cnt;
    }

private:
    struct Node
    {
        Item item;
        Node *next;

        Node(Item x, Node* t)
        {
            item = x;
            next = t;
        }
    };

    Node *head;
    int cnt;
};


template <class Item>
class QUEUE
{
public:
    QUEUE()
    {
        head = 0;
        cnt = 0;
        max = 0;
    }

    QUEUE(int size)
    {
        head = 0;
        cnt = 0;
        max = size;
    }

    ~QUEUE()
    {
        remove(cnt);
    }

    int empty() const
    {
        return head == 0;
    }

    void put(Item x)
    {
        Node *t = tail;
        tail = new Node(x);
        //tail = (Node*)malloc(sizeof(Node));
        tail->item = x;
        tail->next = 0;

        if (head == 0)
        {
            head = tail;            
        }
        else t->next = tail;        

        cnt++;

        if (cnt > max && max > 0) get();
    }


    Item get()
    {
        Item v = head->item;
        Node *t = head->next;

        delete head;
        //free(head);
        head = t;
        --cnt;

        return v;
    }

    Item at(int n)
    {
        if (n >= count()) n = count() - 1;

        Item v = head->item;
        if (n == 0) return v;

        Node *index = head->next;
        for (int i = 1; i <= n; i++)
        {
            v = index->item;
            index = index->next;
        }

        return v;
    }

	Item last()
	{
		Item v = tail->item;

		return v;
	}

	Item first()
	{
		Item v = head->item;

		return v;
	}

    void add(Item *a, int size)
    {
        for (int i = 0; i < size; i++)
        {
            Item t = a[i];
            put(t);
        }
    }

    /*void add(QUEUE<Item> *queue, int num)
    {
        int size = queue->count();
        for (int i = num; i < size; i++)
        {
            Item t = queue->at(i);
            put(t);
        }
    }*/

    int count() const
    {
        return cnt;
    }

    int max_size()
    {
        return max;
    }

    void set_max_size(int size)
    {
        if (size > 0) max = size;
    }

    void remove(int size)
    {
        if (size > cnt) size = cnt;
        //Item v;

        for (int i = 0; i < size; i++)
        {
            get();
        }
    }

    void clear()
    {
        int size = cnt;
        /*for (int i = 0; i < size; i++)
        {
            get();
        }*/
        remove(size);
    }

	QUEUE<Item> &QUEUE<Item>::operator = (const QUEUE<Item> &queue)
	{
		clear();
		int size = queue.count();
		max = queue.max_size();
		for (int i = 0; i < size; i++) 
		{
			Item item = queue.get();
			put(item);
		}

		return this;
	}

private:
    struct Node
    {        
        Item item;
        Node *next;

        Node(Item x)
        {
            item = x;
            next = 0;
        }  		
    };

    Node *head, *tail;
    int max;
    int cnt;
};



/*template <class Item>
class QUEUE_PTR
{
public:
    QUEUE_PTR()
    {
        head = 0;
        cnt = 0;
        max = 0;
    }

    QUEUE_PTR(int size)
    {
        head = 0;
        cnt = 0;
        max = size;
    }

    ~QUEUE_PTR()
    {
        remove(cnt);
    }
    
    void put(Item *x)
    {
        Node **t = tail;
        tail = new Node(x);        
        tail->item = x;
        tail->next = 0;
		tail->prev = t;

        if (head == 0)
        {
            head = tail;            
        }
        else t->next = tail;        

        cnt++;

        if (cnt > max && max > 0) get();
    }

	Item* at(int n)
	{
		if (n >= count()) n = count() - 1;

		Item *v = head->item;
		if (n == 0) return v;

		Node **index = head->next;
		for (int i = 1; i <= n; i++)
		{
			v = index->item;
			index = index->next;
		}

		return v;
	}

    Item* get()
    {
        Item *v = head->item;
        Node **t = head->next;		

        delete head;
		t->prev = 0;
        head = t;
        --cnt;

        return v;
    }

	Item* getAt(int n)
	{
		if (count() == 0) return 0;

		if (n >= count()) n = count() - 1;		
		if (n == 0) return get();

		Item *v = head->item;
		Node **index = head->next;
		for (int i = 1; i < n; i++)
		{
			v = index->item;
			index = index->next;				
		}

		Node **t2 = index;
		index = index->prev;
		Node **t1 = index->prev;
		--cnt;
		
		delete index;
		t1->next = t2;
		t2->prev = t1;

		return v;
	}
        
    void remove(int size)
    {
        if (size > cnt) size = cnt;
        
        for (int i = 0; i < size; i++)
        {
            Item *v = get();
			delete v;
        }
    }

    void clear()
    {
        int size = cnt;
        remove(size);
    }

	void set_max_size(int size)
	{
		if (size > 0) max = size;

		int sz = count();
		if (sz < max)
		{
			remove(max-sz);
		}		
	}

	int count() const
	{
		return cnt;
	}

	int max_size()
	{
		return max;
	}

	
private:
    struct Node
    {        
        Item *item;
        Node **next;
		Node **prev;

        Node(Item *x)
        {
            item = x;
            next = 0;
			prev = 0;
        }  		
    };

    Node **head, **tail;
    int max;
    int cnt;
};*/

#endif // IO_CONTAINERS_H

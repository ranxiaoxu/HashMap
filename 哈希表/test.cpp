#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;

enum Status
{
	EXITS,
	DELETE,
	EMPTY,
};

size_t BKDRHash(const char *str)
{
	unsigned int seed = 131;
	unsigned int hash = 0;
	while(*str)
	{
		hash = hash*seed + (*str++);  //保证"abcd"和"bcda"算出来的值不一样,减少冲突 
	}
	return hash & 0x7fffffff;
}

template<class T>   //一般类型
struct HashFuner
{
	size_t operator()(const T &value)
	{
		return value;
	}
};

template<>
struct HashFuner<string>  //字符型
{
	size_t operator()(const string &value)
	{
		return BKDRHash(value.c_str());
	}
};

template<class T,class HashFun = HashFuner<T>>
class HashTable
{
public:
	HashTable()
		:_tables(NULL)
		,_size(0)
		,_capacity(0)
		,_status(0)
	{}
	HashTable(const size_t capacity)
		:_tables(new T[capacity])
		,_capacity(capacity)
		,_size(0)
		,_status(new Status[capacity])
	{
		for(size_t i = 0;i < _capacity;i++)
		{
			_status[i] = EMPTY;
		}
	}
	~HashTable()
	{
		if(_tables != NULL)
		{
			delete [] _tables;
			delete [] _status;
		}
	}
	int GetKey(const T &value)   //算出应该存放的位置
	{
		return HashFun()(value)%_capacity;
	}
	void Swap(HashTable<T> &newTable)
	{
		swap(_tables,newTable._tables);
		swap(_size,newTable._size);
		swap(_capacity,newTable._capacity);
		swap(_status,newTable._status);

	}
	bool Insert(const T& value)
	{
		if(_size*10 / _capacity >= 8)
		{
			HashTable<T> newTable(2*_capacity);
			for(size_t i = 0;i < _capacity;i++)
			{
				if(_status[i] == EXITS)
				{
					newTable.Insert(_tables[i]);
				}
			}
			Swap(newTable);
		}
		size_t index = GetKey(value);
		while(_status[index] == EXITS)
		{
			++index;
			if(index == _capacity)
			{
				index = 0;
			}
		}
		_tables[index] = value;
		++_size;
		_status[index] = EXITS;
		return true;
	} 
	bool Remove(const T& value)
	{
		size_t index = GetKey(value);
		while(_tables[index] != EMPTY)
		{
			if(_tables[index] == value)
			{
				_status[index] = DELETE;
				--_size;
				return true;
			}
			++index;
			if(_size == _capacity)
			{
				index = 0;
			}
		}
		return false;
	}
	/*void Display()
	{
		for(size_t i = 0;i < _capacity;i++)
		{
			cout<<_status[i]<<":"<<_tables[i]<<endl;
		}
	}*/
	bool Find(const T& value)
	{
		size_t index = GetKey(value);
		while(_tables[index] != EMPTY)
		{
			if(_tables[index] == value)
			{
				if(_status[index] == EXITS)  //因为前面用的是懒删除法
				{
					return true;
				}
			}
			++index;
			if(_size == _capacity)
			{
				index = 0;
			}
		}
		return false;
	}
private:
	T *_tables;
	size_t _size;
	size_t _capacity;
	Status *_status;

};

void test()
{
	HashTable<int,HashFuner<int>> ht(10);
	ht.Insert(67);
	ht.Insert(58);
	ht.Insert(27);
	ht.Remove(27);
	//ht.Display();
	cout<<ht.Find(58);
}
void test1()
{
	HashTable<string> ht(20);
	ht.Insert("chf");
	ht.Insert("hcf");
	ht.Remove("chf");
//	ht.Display();
}
int main()
{
	//test();
	test1();
	system("pause");
	return 0;
}
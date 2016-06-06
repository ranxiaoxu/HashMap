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

template<class K,class V>
struct KeyValueNode
{
	K _key;
	V _value;

	KeyValueNode()
	{}

	KeyValueNode(const K &key,const V &value)
		:_key(key)
		,_value(value)
	{}

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

template<class K>   //一般类型
struct HashFuner
{
	size_t operator()(const K &value)
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

template<class K,class V,class HashFun = HashFuner<K>>
class HashTable
{
	typedef KeyValueNode<K,V> KVNode;
public:
	HashTable()
		:_tables(NULL)
		,_size(0)
		,_capacity(0)
		,_status(0)
	{}
	HashTable(const size_t capacity)
		:_tables(new KVNode[capacity])
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
	int GetKey0(const K &key)   //算出第0次应该存放的位置
	{
		return HashFun()(key)%_capacity;
	}
	int GetKeyI(size_t &PrevKeyVal,int i)   //算出第i次应该存放的位置
	{
		return (PrevKeyVal + 2*i-1)% _capacity;
	}
	void Swap(HashTable<K,V> &newTable)
	{
		swap(_tables,newTable._tables);
		swap(_size,newTable._size);
		swap(_capacity,newTable._capacity);
		swap(_status,newTable._status);

	}
	bool Insert(const K &key,const V& value)
	{
		if(_size*10 / _capacity >= 8)
		{
			HashTable<K,V,HashFun> newTable(2*_capacity);
			for(size_t i = 0;i < _capacity;i++)
			{
				if(_status[i] == EXITS)
				{
					newTable.Insert(_tables[i]._key,_tables[i]._value);
				}
			}
			Swap(newTable);
		}
		size_t index = GetKey0(key);
		int i = 0;
		while(_status[index] == EXITS)
		{
			index = GetKeyI(index,++i);
			if(index == _capacity)
			{
				index = 0;
			}
		}
		_tables[index] = KVNode(key,value);
		++_size;
		_status[index] = EXITS;
		return true;
	} 
	/*bool Remove(const K& key)
	{
		size_t index = GetKey0(key);
		int i = 0;
		while(_status[index] != EMPTY)
		{
			if(_tables[index]._key == key)
			{
				_status[index] = DELETE;
				--_size;
				return true;
			}
			index =  GetKeyI(index,++i);
			if(_size == _capacity)
			{
				index = 0;
			}
		}
		return false;
	}*/
	bool Find(const K& key)
	{
		size_t index = GetKey0(key);
		while(_tables[index] != EMPTY)
		{
			if(_tables[index]._key == key)
			{
				if(_status[index] == EXITS)  //因为前面用的是懒删除法
				{
					return true;
				}
			}
			index =  GetKeyI(index,++i);
			if(_size == _capacity)
			{
				index = 0;
			}
		}
		return false;
	}
private:
	KVNode *_tables;
	size_t _size;
	size_t _capacity;
	Status *_status;

};

void test()
{
	HashTable<string,string> ht(10);
	ht.Insert("dictionary","字典");
	ht.Insert("book","书");
	ht.Insert("cup","杯子");
	//ht.Remove("cup");
	//ht.Display();
}

int main()
{
	test();
	system("pause");
	return 0;
}
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<string>
using namespace std;

template<class K,class V>
struct KeyValueNode
{
	K _key;
	V _value;
	KeyValueNode *_next;

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
class HashTableBucket
{
	typedef KeyValueNode<K,V> KVNode;
public:
	HashTableBucket()
		:_size(0)
	{}

	HashTableBucket(const HashTableBucket<K,V,HashFun> &ht)
	{
		_tables.resize(ht._tables.size());
		for(size_t i = 0;i < ht._tables.size();++i)
		{
			KVNode *cur = ht._tables[i];
			while(cur)
			{
				KVNode *newnode = new KVNode(cur->_key,cur->_value);
				newnode->_next = _tables[i];
				_tables[i] = newnode;
				cur = cur->_next;
			}
			_tables[i] = NULL;
		}
		_size = ht._size;
	}

	HashTableBucket& operator = (HashTableBucket<K,V,HashFun> &ht)
	{
		if(this != &ht)
		{
			HashTableBucket<K,V,HashFun> tmp(ht);
			this->Swap(tmp);
		}
		return *this;
	}


	~HashTableBucket()
	{
		for(size_t i = 0;i < _tables.size();++i)
		{
			KVNode *cur = _tables[i];
			while(cur)
			{
				KVNode *del = cur;
				cur = cur->_next;
				delete del;
			}
			_tables[i] =NULL;
		}
		_size = 0;
	}

	bool Insert(const K &key,const V &value)
	{
		_CheckCapacity();
		size_t index = GetIndex(key);
		KVNode *cur = _tables[index];
		while(cur)                //检查要插入的数据是否已经存在
		{
			if(cur->_key == key && cur->_value == value)
			{
				return false;
			}
			cur = cur->_next;
		}
		KVNode *tmp = new KVNode(key,value);
		tmp->_next = _tables[index];
		_tables[index] = tmp;
		return true;
	}
	bool Remove(const K & key)
	{
		bool ret = false;
		size_t index = GetIndex(key);
		KVNode *cur = _tables[index];
		KVNode *prev = cur;
		while(cur != NULL)
		{
			if(cur->_key == key)
			{
				ret = true;
				KVNode *del = cur;
				prev = cur->_next;
				delete del;
				if(del == _tables[index])
				{
					_tables[index] = NULL;
				}
				cur = prev->_next;
			}
			else
			{
				prev = cur;
				cur = cur->_next;
			}
		}
		return ret;
	}
	KVNode * Find(const K & key)
	{
		size_t index = GetIndex(key);
		KVNode *cur = _tables[index];
		while(cur )
		{
			if(cur->_key == key)
			{
				return cur;
			}
			cur = cur->_next;
		}
		return NULL;
	}

protected:
	size_t GetIndex(const K &key)
	{
		return HashFun()(key)%_tables.size();
	}
	size_t GetNextPrime(const unsigned long *PrimeList,const size_t PrimeSize,size_t capacity)
	{
		for(size_t i = 0;i < PrimeSize;i++)
		{
			if(PrimeList[i] > capacity)
			{
				return PrimeList[i];
			}
		}
	}
	void _CheckCapacity()
	{
		unsigned long _PrimeList[5] = {53,97,193,389,769};
		if(_size == _tables.size())
		{
			size_t newcapacity = GetNextPrime(_PrimeList,5,_tables.size());
			vector<KVNode *> newtables;
			newtables.resize(newcapacity);
			for(size_t i = 0;i < _tables.size();i++)
			{
				KVNode *cur = _tables[i];
				while(cur)
				{
					KVNode *tmp = cur;
					cur = cur->_next;
					size_t newindex = GetIndex(cur->_key);
					tmp->_next = newtables[newindex]; //头插
					newtables[newindex] = tmp;
				}
				_tables[i] = NULL;
			}
			_tables.swap(newtables);
		}
	}
	void Swap(HashTableBucket<K,V,HashFun> &ht)
	{
		_tables.swap(ht._tables);
		swap(_size,ht._size);
	}
	
private:
	vector<KeyValueNode<K,V> *> _tables;
	size_t _size;   //记录数据的个数
};

void test()
{
	HashTableBucket<int,int,HashFuner<int>> ht;
	ht.Insert(1,31);
	ht.Insert(2,32);
	ht.Insert(2,42);
	ht.Insert(3,43);
	ht.Remove(2);
	ht.Find(1);
	HashTableBucket<int,int,HashFuner<int>> ht1(ht);
	HashTableBucket<int,int,HashFuner<int>> ht2;
	ht2.Insert(5,65);
	ht2 = ht1;


}

void test2()
{
	HashTableBucket<string,string> ht;
	ht.Insert("字典","dictionary");
	ht.Insert("书","book");
	HashTableBucket<string,string> ht1(ht);
}
int main()
{
	test();
	//test2();
	system("pause");
	return 0;
}
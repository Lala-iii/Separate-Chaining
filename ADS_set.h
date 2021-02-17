#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

//MAMEDOVA LALA HAMID 01652485
//SEPARATE CHAINING SS19

template <typename Key, size_t N = 7>
class ADS_set {
public:
	class Iterator;
	using value_type = Key;
	using key_type = Key;
	using reference = key_type & ;
	using const_reference = const key_type&;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using iterator = Iterator;
	using const_iterator = Iterator;
	using key_compare = std::less<key_type>;   // B+-Tree
	using key_equal = std::equal_to<key_type>; // Hashing
	using hasher = std::hash<key_type>;        // Hashing

											   
private:
	struct bucket {
		key_type key;
		bucket *next = nullptr;
		~bucket(){}
	 
	};

	bucket *table {nullptr};
    size_type table_sz {0}, current_size {0};
    float max_lf {0.6};
	
	size_type h(const key_type & key)const { return hasher{} (key) % table_sz; }

	
	void insert_einzel(const key_type &key);
    bucket * find_(const key_type& key) const;
    void reserve(size_type n);
	
	void rehash(size_type n) ;

public:

	
	ADS_set() { rehash(N);}
 
	
	ADS_set(std::initializer_list<key_type> ilist) : ADS_set{} { insert(ilist); }

	
    void insert(std::initializer_list<key_type> ilist) { 
	
	 insert(std::begin(ilist), std::end(ilist)); 
	}
	
	template<typename InputIt> ADS_set(InputIt first, InputIt last) : ADS_set{} {
		
		insert(first, last);
	}
	
	//Kopierkontruktor
	ADS_set(const ADS_set &other): ADS_set{} {
    reserve(other.current_size);
    for (const auto &k: other) {
      insert_einzel(k);
    }
  }
   //Kopierzuweisungsoperator
   ADS_set &operator=(const ADS_set &other) {
    if (this == &other) return *this;
    ADS_set tmp{other};
    swap(tmp);
    return *this;
  }
   
   ADS_set &operator=(std::initializer_list<key_type> ilist) {
    ADS_set tmp{ilist};
    swap(tmp);
    return *this;
  } 
  
  
   ~ADS_set()
    {
		for(size_type index{0}; index<table_sz; index++){
			 if(table[index].next!=nullptr){
			 while(table[index].next!=nullptr) {
					bucket* temp = table[index].next;
					table[index].next=table[index].next -> next;
					delete temp;
				 }
			 }
		 }
        delete[] table;
		
    }

	
	template<typename InputIt> void insert(InputIt first, InputIt last); 
	size_type size() const {
		return current_size;
	}
   
	
	bool empty() const {
		return !current_size;
	}

	
	size_type count(const key_type& key) const { return !!find_(key); }
	
	
	iterator find(const key_type &key) const {
		
		if(auto position { find_(key)})
		{
			
			size_type nummer=h(key);
			return iterator{position,nummer,table,table_sz};
			
		}
		else
			return end();
		
		
		
   }
   
    void clear() {
    ADS_set tmp;
    swap(tmp);
   }
   
    void swap(ADS_set &other) {
    using std::swap;
    swap(table, other.table);
    swap(current_size, other.current_size);
    swap(table_sz, other.table_sz);
    swap(max_lf, other.max_lf);
   }
    
	std::pair<iterator,bool> insert(const key_type &key) {
		if(find_(key)) 
		return {find(key),false};
	else 
		reserve(current_size+1);
	    insert_einzel(key);
		return {find(key),true};
  }
	 
	size_type erase(const key_type &key) {
	size_type anzahl = 1;
	
    size_type index = { h(key) };
    if(count(key)) {
		bucket *help=table[index].next;
		if(key_equal{}(help->key,key)){
			table[index].next=help->next;
				help->next=nullptr;
				delete help;
				--current_size;
				return 1;
		}
		else
			for(bucket * i=table[index].next; i!=nullptr ; i=i->next){
				if (key_equal{}(i->key,key)){
				    
					if(anzahl>1){
						bucket * temp = table[index].next;
						for(size_type loop=0 ; loop < anzahl-2 ; loop++){
							temp=temp->next;
						}
						temp->next=i->next;
						i->next=nullptr;
						delete i;
						--current_size;
						return 1;
					}
				}
				anzahl++;
			}
	
		}
	
		return 0;
  }
   
size_t step(const key_type &k1, const key_type &k2){
	bool found=false;
	size_t counter=0;
	for(size_t i=0; i<table_sz; i++){
		for(bucket *element=table[i].next; element!=nullptr; element=element->next){
			if( key_equal{}(element->key,k1)){
				found=true;
				bucket *k1_elem=element;
			}
			if(found==true){
				for(bucket *k2_elem=k1_elem; k2_elem!=nullptr; k2_elem=k2_elem->next){
					if(key_equal{}(k2_elem->key,k2)){
						return counter;
					}
					counter++;
				}
			}
		}
	}
	return counter;
}   
  const_iterator begin() const { 
  
	  for(size_type nummer=0 ; nummer < table_sz ; nummer++){
		  if(table[nummer].next!=nullptr) {
			  return const_iterator{table[nummer].next,nummer,table,table_sz};
		  }
	  }
  
	  return end();
  
  
  }
  const_iterator end() const { return const_iterator{nullptr}; }

	const_iterator w(const size_t *a) const {}
	

	
	void dump(std::ostream& o = std::cerr) const;
	
	friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {
    if (lhs.current_size != rhs.current_size) return false;
    for (const auto &k: rhs) if (!lhs.count(k)) return false;
    return true;
    }
   
    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) { return !(lhs == rhs); }
  

};
template <typename Key, size_t N>
 	void ADS_set<Key,N>::insert_einzel(const key_type &key) {
         size_type index = { h(key)};
	  if (table[index].next!=nullptr) {
		  
		  bucket *dobavlayemiy_element = new bucket;
	      dobavlayemiy_element->key=key;
		  (*dobavlayemiy_element).next=table[index].next;
		  table[index].next=dobavlayemiy_element;
		  current_size++;
	  } 
	  else {
		  
		  bucket *dobavlayemiy_element = new bucket;
	      dobavlayemiy_element->key=key;
		  table[index].next=dobavlayemiy_element;
		  dobavlayemiy_element->next=nullptr;
		  current_size++;
	  }
	
	 if (current_size > table_sz * max_lf) {
			
			rehash(7);
		}

   } 
    
	template <typename Key, size_t N>
  typename ADS_set<Key,N>::bucket*ADS_set<Key,N>::find_(const key_type &key) const {
         size_type position=h(key);
		 bucket * temp= table[position].next;
		  while(temp!=nullptr){
			  if(key_equal{} (temp->key,key)){
					  
					  return temp;
				}
                         temp=temp->next;
		  }
			return nullptr; 
	}

   
 
	template <typename Key, size_t N>
  template<typename InputIt> void ADS_set<Key,N>::insert(InputIt first, InputIt last) {
	  for (auto it {first}; it != last; ++it) {
		if (!count(*it)) {
		  reserve(current_size+1);
		  insert_einzel(*it);
		}
	  }
	}
	

	template <typename Key, size_t N>
  void ADS_set<Key,N>::reserve(size_type n) {
  if (n > table_sz * max_lf) {
    size_type new_table_size {table_sz};
    do {
      new_table_size = new_table_size * 2 + 1;
    } while (n > new_table_size * max_lf);
    rehash(new_table_size);
  }
}	


	template <typename Key, size_t N>
  void ADS_set<Key,N>::rehash(size_type n) {
	 size_type new_table_size {std::max(N,std::max(n,size_type(current_size/max_lf)))};
	  bucket *new_table = new bucket[new_table_size];
	  size_type old_table_size = table_sz;
	  bucket *old_table {table};
	  current_size=0;
	  table = new_table;
	  table_sz = new_table_size;
	
	     for(size_type index{0}; index<old_table_size; index++){
			 if(old_table[index].next!=nullptr){
			 while(old_table[index].next!=nullptr) {
			        insert_einzel(old_table[index].next->key);
					bucket* temp = old_table[index].next;
					old_table[index].next=old_table[index].next -> next;
					delete temp;
				 }
			 }
		 }
		delete[] old_table;
    


  }
template <typename Key, size_t N>
  void ADS_set<Key,N>::dump(std::ostream &o) const {
	for (size_type index{0}; index < table_sz; index++){
		o<< index <<" :===> ";
		if (table[index].next==nullptr){
			
			o<< std::endl;
			
		}
		else {
			for (bucket *it = table[index].next; it != nullptr; it = (*it).next) {
				o<< it -> key<<" ===> ";
			}
			o<< std::endl;
		}
		
	 }
}
  
  
  template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
  using value_type = Key;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type &;
  using pointer = const value_type *;
  using iterator_category = std::forward_iterator_tag;
private:
  bucket *pos;
  size_type anzahl;
  bucket * table; 
  size_type table_sz;
  
  void weiter () { if(pos->next!=nullptr) {pos=pos->next;} }
  
public:
  explicit Iterator(bucket *pos=nullptr	, size_type anzahl = 0 , bucket* table = nullptr ,size_type table_sz = 0): pos{pos} , anzahl {anzahl}, table{table}, table_sz{table_sz} {}
  reference operator*() const { return pos->key; }
  pointer operator->() const { return &pos->key; }
  Iterator &operator++() {
	  if (pos->next!=nullptr && anzahl < table_sz) {
		 weiter();
		  return *this;
		
	  } 
	  else {
             anzahl++;
			 while(table[anzahl].next==nullptr && anzahl < table_sz) {
				 anzahl++;
				}
				if(anzahl==table_sz){
					pos=nullptr;
					return *this;
				}
		  pos=table[anzahl].next;
		  return *this;
	  }

	 
	  }
  
  Iterator operator++(int) { auto rc {*this}; ++*this; return rc; }
  friend bool operator==(const Iterator &lhs, const Iterator &rhs) { return lhs.pos == rhs.pos; }
  friend bool operator!=(const Iterator &lhs, const Iterator &rhs) { return !(lhs == rhs); }
};

template <typename Key, size_t N> void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }



#endif
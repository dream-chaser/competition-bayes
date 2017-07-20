#pragma once
#include <unordered_map>
#include <vector>

// K must have the implementation of '<' and hash struct
template<class K, class V, class H = std::hash<K> >
class fast_map
{
public:
	fast_map() { _size = 0; };
	~fast_map() { };

	inline int size() { return _size; };
	inline int count(K key) { return index_map.count(key); };
	inline K& getKey(int i) { return keys[i]; };
	inline V& getValue(int i) { return values[i]; };
	inline V& operator [] (K key) { return values[index_map[key]]; };
	inline void add(K key, V value) {
		index_map[key] = _size;
		keys.push_back(key);
		values.push_back(value);
		_size++;
	};

private:
	std::unordered_map<K, int, H > index_map; // fast search
	std::vector<V> values; // fast iteration
	std::vector<K> keys;
	int _size;
};
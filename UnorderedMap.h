#include <cstddef>    // size_t
#include <functional> // std::hash
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"

template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode *next = nullptr) : next{next} {}
        HashNode(const value_type & val, HashNode * next = nullptr) : next { next }, val { val } { }
        HashNode(value_type && val, HashNode * next = nullptr) : next { next }, val { std::move(val) } { }
    };

    HashNode **_buckets;
    size_type _size;
    size_type _bucket_count;

    HashNode _head;

    Hash _hash;
    key_equal _equal;

    static size_type _range_hash(size_type hash_code, size_type bucket_count) {
        return hash_code % bucket_count;
    }

    public:

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const key_type, mapped_type>;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;
    
    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        HashNode * _node;

        explicit iterator(HashNode *ptr) noexcept{ _node = ptr; /* TODO */ }

    public:
        iterator(): _node(nullptr) {};
        iterator(const iterator &) = default;
        iterator(iterator &&) = default;
        ~iterator() = default;
        iterator &operator=(const iterator &) = default;
        iterator &operator=(iterator &&) = default;
        reference operator*() const { return _node->val;}
        pointer operator->() const { return &(_node->val);}
        iterator &operator++() {
            _node = _node ->next;
            return *this;
        }
        iterator operator++(int) {
            iterator newIt = *this;
            ++(*this);
            return newIt;
        }
        bool operator==(const iterator &other) const noexcept {
            return _node == other._node;
        }
        bool operator!=(const iterator &other) const noexcept {
            return !(_node == other._node);
        }
    };

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            UnorderedMap * _map;
            HashNode * _node;
            size_type _bucket;

            explicit local_iterator(UnorderedMap * map, HashNode *ptr, size_type bucket) noexcept: _map(map), _node(ptr), _bucket(bucket) { /* TODO */ }

        public:
            local_iterator(){
                    _map = nullptr;
                    _node = nullptr;
                    _bucket = 0;/* TODO */ };
            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            reference operator*() const { return _node->val; }
            pointer operator->() const { return &(_node->val); }
            local_iterator & operator++() {
                // TODO
                if (_node->next != nullptr) {
                    if(_map-> bucket(_node ->next->val.first) != _bucket){
                        _node = nullptr;
                        return *this;
                    }
                }
                _node = _node->next;
                return *this;
            }
            local_iterator operator++(int) {
                local_iterator newIt = *this;
                ++(*this);
                return newIt;
            }
            bool operator==(const local_iterator &other) const noexcept {
                // TODO
                return _node == other._node;
            }
            bool operator!=(const local_iterator &other) const noexcept {
                // TODO
                return !(*this == other);
            }
    };

private:

    size_type _bucket(size_t code) const { /* TODO */  
        return _range_hash(code, _bucket_count);
    }
    size_type _bucket(const Key &key) const { /* TODO */
        size_type hashcode = _hash(key);
        return _range_hash(hashcode, _bucket_count);
    }

    void _insert_before(size_type bucket, HashNode *node) {
        /* TODO */
        if (_buckets[bucket] != nullptr) {
                node->next = _buckets[bucket]->next; 
                _buckets[bucket]->next = node;
        }
        else {
            node->next = _head.next;
            if (_head.next != nullptr) {
                _buckets[_bucket(_head.next->val.first)] = node;
            }
            _head.next = node;
            _buckets[bucket] = &_head;
        }
    }

    HashNode*& _bucket_begin(size_type bucket) {
        /* TODO */
        if(_buckets[bucket] == nullptr) {
            return _buckets[bucket];
        }
        else
            return _buckets[bucket]->next;
    }

    HashNode* _find_prev(size_type code, size_type bucket, const Key & key) {
        /* TODO */
        HashNode* curr = _buckets[bucket];
        if (curr == nullptr) {
            return nullptr;
        }
        else {
            while (curr->next != nullptr) {
                if(_equal(curr->next->val.first, key)) {
                    return curr;
                }
                curr = curr->next;
            } 
        }
        return nullptr;
    }

    HashNode* _find_prev(const Key & key) {
        /* TODO */
        return _find_prev(_hash(key),_bucket(key),key);
    }

    void _erase_after(HashNode * prev) {
        /* TODO */
        HashNode * curr = prev->next;
        size_type currBucket = _bucket(curr->val.first);
        if (curr->next == nullptr) {
            if(_buckets[currBucket] == prev) {
                _buckets[currBucket] = nullptr;
            }
        }
        else if(_buckets[_bucket(curr->next->val.first)] == curr) { 
            if(_buckets[currBucket] == prev) {
                _buckets[currBucket] = nullptr;
            }
            size_type nextBucket = _bucket(curr->next->val.first);
            _buckets[nextBucket] = prev;
        }
        prev->next = curr->next;
        delete curr;
        _size--;
    }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { },
                const key_equal & equal = key_equal { }): _hash{hash}, _equal{equal} {
        
        _bucket_count = next_greater_prime(bucket_count);
        _buckets = new HashNode* [_bucket_count]();
        _size = 0;     
    }

    ~UnorderedMap() {
        /* TODO */
        clear();  
    }

    UnorderedMap(const UnorderedMap & other) {
        /* TODO */
        _bucket_count = other._bucket_count;
        _buckets = new HashNode*[_bucket_count]();
        _size = 0;
        _head.next = nullptr;
        HashNode* curr = other._head.next;
        for(int i = 0; i < other._size; i++) {
            insert(curr->val);
            curr = curr->next;
            _size++;
        }
    }

    UnorderedMap(UnorderedMap && other) {
        /* TODO */
        _bucket_count = other._bucket_count;
        _size = other._size;
        _buckets = other._buckets;
        _head.next = other._head.next;

        size_t f_bucket = _bucket(other._head.next->val.first);
        _buckets[f_bucket] = &_head;
        //empty others
        other._buckets = new HashNode*[_bucket_count]();
        other._head.next = nullptr;
        other._size = 0;

    }

    UnorderedMap & operator=(const UnorderedMap & other) {
        if (this != &other) {
            clear();
            delete[] _buckets;
            _bucket_count = other._bucket_count;
            _buckets = new HashNode*[_bucket_count]();
            _size = 0;
            _head.next = nullptr;
            HashNode* curr = other._head.next;
            for(int i = 0; i < other._size; i++) {
                insert(curr->val);
                curr = curr->next;
                _size++;
            }
        }
        return *this;
    }

    UnorderedMap & operator=(UnorderedMap && other) {
        /* TODO */
        if (this != &other) {
            clear();
            delete[] _buckets;
        _bucket_count = other._bucket_count;
        _size = other._size;
        _buckets = other._buckets;
        _head.next = other._head.next;

        size_t f_bucket = _bucket(other._head.next->val.first);
        _buckets[f_bucket] = &_head;
        //empty others
        other._buckets = new HashNode*[_bucket_count]();
        other._head.next = nullptr;
        other._size = 0;
        }
        return *this;
    }

    void clear() noexcept {
        /* TODO */
        while(_size > 0) {
            iterator x = begin();
            erase(x);
        }
    }
    size_type size() const noexcept { return _size;}

    bool empty() const noexcept { return _size == 0;}

    size_type bucket_count() const noexcept { return _bucket_count;}

    iterator begin() { /* TODO */ return iterator(_head.next); }

    iterator end() { /* TODO */return iterator(); }

    local_iterator begin(size_type n) { /* TODO */  return local_iterator(this, _bucket_begin(n), n);}

    local_iterator end(size_type n) { /* TODO */ return local_iterator();}

    size_type bucket_size(size_type n) { /* TODO */ return std::distance(begin(n), end(n));}

    float load_factor() const { /* TODO */ return float(_size)/float(_bucket_count);}

    size_type bucket(const Key & key) const { /* TODO */ return _bucket(_hash(key)); }

    std::pair<iterator, bool> insert(value_type && value) {
        size_type hashcode = _hash(value.first);
        size_type currbucket = _bucket(hashcode);
        HashNode* prev =  _find_prev(hashcode, currbucket, value.first);

        //TWo cases: true
        if(prev == nullptr) {
            HashNode* curr = new HashNode(std::move(value));
            _insert_before(currbucket, curr);
            iterator newIt = iterator(curr);
            std::pair<iterator,bool> pair = std::make_pair(newIt, true);
            _size++;
            return pair;
        }

        //False
        iterator newIt = iterator(prev);
        std::pair<iterator,bool> pair = std::make_pair(iterator(prev), false);
        return pair;
        
    }

    std::pair<iterator, bool> insert(const value_type & value) {
        size_type hashcode = _hash(value.first);
        size_type currbucket = _bucket(hashcode);
        HashNode* prev =  _find_prev(hashcode, currbucket, value.first);

        //TWo cases: true
        if(prev == nullptr) {
            HashNode* curr = new HashNode((value));
            _insert_before(currbucket, curr);
            iterator newIt = iterator(curr);
            std::pair<iterator,bool> pair = std::make_pair(newIt, true);
            return pair;
        }

        //False
        iterator newIt = iterator(prev);
        std::pair<iterator,bool> pair = std::make_pair(iterator(prev), false);
        return pair;
    }

    iterator find(const Key & key) {
        /* TODO */
        HashNode* curr = _find_prev(key);
        if (curr == nullptr) {
            return iterator();
        }
        iterator temp = iterator(curr->next);
        return temp;
    }

    T& operator[](const Key & key) {
        /* TODO */

        HashNode *prev = _find_prev(key);
        if(prev == nullptr){
            value_type pair = std::make_pair(key,T());
            insert(pair);
            HashNode* curr = _find_prev(key);
            return curr->next->val.second;
        }
        return prev->next->val.second;
    }

    iterator erase(iterator pos) {
        /* TODO */
        HashNode * curr = _find_prev(pos._node->val.first);
        if(curr == nullptr) {
            return pos;
        }
        _erase_after(curr);
        pos++;
        return pos;
    }

    size_type erase(const Key & key) {
        /* TODO */
        HashNode * curr = _find_prev(key);
        if(curr == nullptr) {
            return 0;
        }
        _erase_after(curr);
        return 1;
    }


    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    HashNode const * node = map._head.next;
    os << "List: ";
    do {
        if(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
        } else {
            os << "(nullptr)";
            break;
        }

        node = node->next;
    } while(true);
    os << std::endl;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        if(!node) {
            os << "(nullptr)";
        } else {
            while((node = node->next) && map.bucket(node->val.first) == bucket) {
                os << "(" << node->val.first << ", " << node->val.second << ") ";
            }
        }
        os << std::endl;
    }
}
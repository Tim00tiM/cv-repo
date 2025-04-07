#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename T, typename Alloc>
class List;

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;

 private:
  using allocator_type = Alloc;
  using AllocTraits = std::allocator_traits<Alloc>;

  struct MapNode {
    NodeType value;
    size_t hash;

    MapNode(const Key& key, const Value& value, const Hash& hash)
        : value(key, value), hash(hash(key)) {}

    MapNode(Key&& key, const Value& value_, const Hash& hash)
        : value(std::move(key), value_), hash(hash(value.first)) {}

    MapNode(const Key& key, const Hash& hash)
        : value(key, Value()), hash(hash(key)) {}

    MapNode(Key&& key, const Hash& hash)
        : value(std::move(key), Value()), hash(hash(value.first)) {}

    MapNode(const Key& key, Value&& value, const Hash& hash)
        : value(key, std::move(value)), hash(hash(key)) {}

    MapNode(Key&& key, Value&& value_, const Hash& hash)
        : value(std::move(key), std::move(value_)), hash(hash(value.first)) {}

    MapNode(const NodeType& pair, const Hash& hash)
        : value(pair), hash(hash(value.first)){};

    MapNode(NodeType&& pair, const Hash& hash)
        : value(const_cast<Key&&>(pair.first), std::move(pair.second)),
          hash(hash(value.first)){};

    MapNode(MapNode&& to_move)
        : value(const_cast<Key&&>(to_move.value.first),
                std::move(to_move.value.second)),
          hash(to_move.hash){};

    MapNode(const MapNode& to_move)
        : value(to_move.value), hash(to_move.hash){};
  };

  using InternalList =
      List<MapNode, typename AllocTraits::template rebind_alloc<MapNode>>;
  using ListBaseNode = typename InternalList::BaseNode;
  using ListNode = typename InternalList::Node;

  size_t capacity = 7;
  double max_load_factor_ = 1;
  [[no_unique_address]] Hash hash;
  [[no_unique_address]] Equal equal;
  [[no_unique_address]] Alloc alloc;
  InternalList list;
  std::vector<ListBaseNode*,
              typename AllocTraits::template rebind_alloc<ListBaseNode*>>
      container;

  template <bool isConst>
  struct base_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type =
        typename std::conditional<isConst, const NodeType, NodeType>::type;
    using node_type =
        typename std::conditional<isConst, const ListNode*, ListNode*>::type;
    using base_node_type =
        typename std::conditional<isConst, const ListBaseNode*,
                                  ListBaseNode*>::type;
    using difference_type = long;
    using pointer =
        typename std::conditional<isConst, const NodeType*, NodeType*>::type;
    using reference =
        typename std::conditional<isConst, const NodeType&, NodeType&>::type;

   private:
    base_node_type current_element = nullptr;

   public:
    base_iterator() = default;

    base_iterator(base_node_type init) : current_element(init) {}

    template <bool listConstness>
    base_iterator(typename InternalList::template Iter<listConstness> init)
        : current_element(const_cast<base_node_type>(init.current_element)) {}

    template <bool anotherIsConst>
    base_iterator(const base_iterator<anotherIsConst>& another)
        : current_element(another.current_element) {}

    reference operator*() {
      return static_cast<node_type>(current_element)->value.value;
    }

    pointer operator->() {
      return &static_cast<node_type>(current_element)->value.value;
    }

    base_iterator& operator++() {
      current_element = current_element->next;
      return *this;
    }

    template <bool anotherConst>
    bool operator!=(const base_iterator<anotherConst>& other) {
      return current_element != other.current_element;
    }

    template <bool anotherConst>
    bool operator==(const base_iterator<anotherConst>& other) {
      return current_element == other.current_element;
    }

    base_iterator operator++(int) {
      auto to_return = *this;
      ++(*this);
      return to_return;
    }

    base_iterator& operator--() {
      current_element = current_element->prev;
      return current_element;
    }

    base_iterator operator--(int) {
      auto to_return = *this;
      ++(*this);
      return to_return;
    }

    operator base_iterator<false>() = delete;

    template <bool constness>
    friend struct base_iterator;

    friend class UnorderedMap;
  };

  void rehash(size_t count) {
    if (capacity > count) {
      return;
    }
    while (count > capacity) {
      capacity *= 2;
      capacity += 5;
    }
    container.assign(capacity, nullptr);
    ListBaseNode* left = list.fake_node.next;
    ListBaseNode* next_left = left->next;
    size_t list_size = list.sz;
    list.fake_node.next = &list.fake_node;
    list.fake_node.prev = &list.fake_node;
    size_t element_hash;
    for (size_t i = 0; i < list_size; ++i) {
      element_hash = static_cast<ListNode*>(left)->value.hash;
      if (container[element_hash % capacity] == nullptr) {
        container[element_hash % capacity] = left;
        left->next = list.fake_node.next;
        left->prev = &list.fake_node;
        list.fake_node.next->prev = left;
        list.fake_node.next = left;
      } else {
        left->prev = container[element_hash % capacity]->prev;
        left->prev->next = left;
        left->next = container[element_hash % capacity];
        container[element_hash % capacity]->prev = left;
        container[element_hash % capacity] = left;
      }
      left = next_left;
      next_left = next_left->next;
    }
  }

 public:
  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

  iterator begin() { return iterator(list.begin()); }

  iterator end() { return iterator(list.end()); }

  const_iterator begin() const { return const_iterator(list.cbegin()); }

  const_iterator end() const { return const_iterator(list.cend()); }

  const_iterator cbegin() { return const_iterator(list.cbegin()); }

  const_iterator cend() { return const_iterator(list.cend()); }

  Alloc get_allocator() const { return alloc; }

 private:
  iterator find_impl(const Key& key) const {
    size_t key_hash = hash(key);
    auto bucket_elem = container[key_hash % capacity];
    if (bucket_elem == nullptr) {
      return iterator(const_cast<ListBaseNode*>(&list.fake_node));
    }
    while (bucket_elem != &list.fake_node &&
           (static_cast<ListNode*>(bucket_elem)->value.hash % capacity) ==
               (key_hash % capacity)) {
      if (equal(key, static_cast<ListNode*>(bucket_elem)->value.value.first)) {
        return iterator(bucket_elem);
      }
      bucket_elem = bucket_elem->next;
    }
    return iterator(const_cast<ListBaseNode*>(&list.fake_node));
  }

 public:
  iterator find(const Key& key) { return find_impl(key); };

  const_iterator find(const Key& key) const { return find_impl(key); };

  size_t size() { return list.size(); }

  double load_factor() {
    if (capacity == 0) {
      return 0;
    }
    return (double)size() / (double)capacity;
  };

  double max_load_factor() { return max_load_factor_; }

  void max_load_factor(double new_max_load_factor) {
    max_load_factor_ = new_max_load_factor;
    if (load_factor() > max_load_factor_) {
      rehash(std::ceil(((double)list.sz / (double)max_load_factor_)));
    }
  }

  void reserve(size_t count) {
    auto new_cap = std::ceil(count / max_load_factor());
    if (new_cap <= capacity) {
      return;
    }
    rehash(new_cap);
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    ListNode* place =
        InternalList::NodeTraits::allocate(list.node_allocator, 1);
    try {
      using maptraits =
          typename InternalList::NodeTraits::template rebind_traits<MapNode>;
      using mapalloc =
          typename InternalList::NodeTraits::template rebind_alloc<MapNode>;
      MapNode ListNode::*list_value = &ListNode::value;
      mapalloc ma = list.node_allocator;
      maptraits::construct(ma, &(place->*list_value),
                           std::forward<Args>(args)..., hash);
    } catch (...) {
      InternalList::NodeTraits::deallocate(list.node_allocator, place, 1);
      throw;
    }
    size_t element_hash = place->value.hash;
    if (container[element_hash % capacity] == nullptr) {
      container[element_hash % capacity] = place;
      place->next = list.fake_node.next;
      place->prev = &list.fake_node;
      list.fake_node.next->prev = place;
      list.fake_node.next = place;
      ++list.sz;
    } else {
      auto bucket_elem = container[element_hash % capacity];
      while (bucket_elem != &list.fake_node &&
             (static_cast<ListNode*>(bucket_elem)->value.hash % capacity) ==
                 (element_hash % capacity)) {
        if (equal(place->value.value.first,
                  static_cast<ListNode*>(bucket_elem)->value.value.first)) {
          return std::make_pair(bucket_elem, false);
        }
        bucket_elem = bucket_elem->next;
      }
      bucket_elem->prev->next = place;
      place->prev = bucket_elem->prev;
      place->next = bucket_elem;
      bucket_elem->prev = place;
      ++list.sz;
    }
    if (load_factor() > max_load_factor_) {
      rehash(std::ceil(((double)list.sz / (double)max_load_factor_)));
    }
    return std::make_pair(iterator(place), true);
  }

  Value& operator[](const Key& key) {
    auto iter = find(key);
    if (iter == end()) {
      auto ret = emplace(key);
      return (*(ret.first)).second;
    }
    return iter->second;
  }

  Value& operator[](Key&& key) {
    auto iter = find(key);
    if (iter == end()) {
      auto ret = emplace(std::forward<Key>(key));
      return (*(ret.first)).second;
    }
    return iter->second;
  }

 private:
  Value& at_impl(const Key& key) const {
    auto iter = find_impl(key);
    if (iter == end()) {
      throw std::out_of_range("out of range: operator at");
    }
    return iter->second;
  }

 public:
  Value& at(const Key& key) { return at_impl(key); }

  const Value& at(const Key& key) const { return at_impl(key); }

  void swap(UnorderedMap& other) {
    std::swap(container, other.container);
    std::swap(capacity, other.capacity);
    std::swap(max_load_factor_, other.max_load_factor_);
    std::swap(list, other.list);
    std::swap(hash, other.hash);
    std::swap(equal, other.equal);
    if constexpr (std::derived_from<
                      typename AllocTraits::propagate_on_container_swap,
                      std::true_type>) {
      std::swap(alloc, other.alloc);
    }
  }

 private:
  iterator erase_node(const_iterator to_erase) {
    size_t erase_hash =
        static_cast<const ListNode*>(to_erase.current_element)->value.hash;
    if (to_erase.current_element != container[erase_hash % capacity]) {
      return list.erase(to_erase.current_element);
    }
    auto next_base_node = container[erase_hash % capacity]->next;
    auto next_node = static_cast<ListNode*>(next_base_node);
    if (next_base_node == &list.fake_node ||
        next_node->value.hash % capacity != erase_hash % capacity) {
      container[erase_hash % capacity] = nullptr;
      return list.erase(to_erase.current_element);
    }
    container[erase_hash % capacity] = next_node;
    return list.erase(to_erase.current_element);
  }

 public:
  iterator erase(iterator pos) { return erase_node(pos); }

  iterator erase(const_iterator pos) { return erase_node(pos); }

  iterator erase(const_iterator start, const_iterator end) {
    iterator ret;
    while (start != end) {
      ret = erase_node(start++);
    }
    return ret;
  }

 private:
  //  std::pair<iterator, bool> insert(const NodeType& value) {
  //    size_t value_hash = hash(value);

  //  }

 public:
  std::pair<iterator, bool> insert(const NodeType& value) {
    return emplace(value);
  }

  std::pair<iterator, bool> insert(NodeType&& value) {
    return emplace(std::forward<NodeType>(value));
  }

  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    for (; first != last; ++first) {
      insert(*first);
    }
  };

  UnorderedMap(const Hash& hash_outer = Hash(),
               const Equal& equal_outer = Equal(),
               const Alloc& alloc_outer = Alloc())
      : hash(hash_outer),
        equal(equal_outer),
        alloc(alloc_outer),
        list(alloc),
        container(capacity, nullptr, alloc) {}

  UnorderedMap(const UnorderedMap& other)
      : capacity(other.capacity),
        max_load_factor_(other.max_load_factor_),
        hash(other.hash),
        equal(other.equal),
        alloc(AllocTraits::select_on_container_copy_construction(
            other.get_allocator())),
        list(other.list),
        container(capacity, nullptr, alloc) {
    rehash(std::ceil(capacity / max_load_factor_));
  }

  UnorderedMap(UnorderedMap&& other)
      : capacity(other.capacity),
        max_load_factor_(other.max_load_factor_),
        hash(other.hash),
        equal(other.equal),
        alloc(std::move(other.get_allocator())),
        list(std::move(other.list)),
        container(std::move(other.container)) {
    other.capacity = 0;
  }

  UnorderedMap& operator=(const UnorderedMap& to_copy) {
    if (this == &to_copy) {
      return *this;
    }
    if constexpr (AllocTraits::propagate_on_container_copy_assignment::value) {
      alloc = to_copy.get_allocator();
    }
    capacity = to_copy.capacity;
    max_load_factor_ = to_copy.max_load_factor_;
    hash = to_copy.hash;
    equal = to_copy.equal;
    list = to_copy.list;
    container = to_copy.container;
    std::fill(container.begin(), container.end(), nullptr);
    rehash(std::ceil(capacity / max_load_factor_));
    return *this;
  }

  UnorderedMap& operator=(UnorderedMap&& to_move) {
    if (this == &to_move) {
      return *this;
    }
    if constexpr (AllocTraits::propagate_on_container_move_assignment::value) {
      alloc = to_move.get_allocator();
    }
    capacity = to_move.capacity;
    max_load_factor_ = to_move.max_load_factor_;
    hash = to_move.hash;
    equal = to_move.equal;
    list = std::move(to_move.list);
    container = std::move(to_move.container);
    to_move.capacity = 0;
    return *this;
  }
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    mutable BaseNode* next = nullptr;
    mutable BaseNode* prev = nullptr;
  };

  struct Node : BaseNode {
    T value;

    template <typename... Args>
    Node(Args&&... args) : BaseNode(), value(std::forward<Args>(args)...) {}

    Node() = default;
  };

  using value_type = T;
  using allocator_type = Alloc;
  using AllocTraits = std::allocator_traits<allocator_type>;
  using NodeAlloc = AllocTraits::template rebind_alloc<Node>;
  using NodeTraits = AllocTraits::template rebind_traits<Node>;

  BaseNode fake_node;
  size_t sz = 0;
  [[no_unique_address]] NodeAlloc node_allocator;

  void copy_list(const List& copy) {
    long long i;
    const_iterator elem = copy.cbegin();
    try {
      for (i = 0; i < static_cast<long long>(copy.size()); ++i) {
        push_back(*elem);
        ++elem;
      }
      sz = copy.size();
    } catch (...) {
      for (; i > 0; --i) {
        pop_back();
      }
      throw;
    }
  }

  void destroy_fake_node(BaseNode fake_node_d, size_t sz_d,
                         NodeAlloc& node_allocator_d) {
    Node* start = reinterpret_cast<Node*>(fake_node_d.next);
    Node* next = reinterpret_cast<Node*>(start->next);
    for (size_t i = 0; i < sz_d; ++i) {
      NodeTraits::destroy(node_allocator_d, reinterpret_cast<Node*>(start));
      NodeTraits::deallocate(node_allocator_d, start, 1);
      start = next;
      next = reinterpret_cast<Node*>(start->next);
    }
  }

 public:
  List(const Alloc& alloc = Alloc()) : node_allocator(alloc) {
    fake_node.prev = &fake_node;
    fake_node.next = &fake_node;
  }

  List(size_t n, const Alloc& alloc = Alloc()) : List(alloc) {
    BaseNode* last_node = &fake_node;
    BaseNode* prev_node = &fake_node;
    long long i;
    bool last_constructed = false;
    bool last_allocated = false;
    try {
      for (i = 0; i < static_cast<long long>(n); ++i) {
        last_allocated = false;
        last_constructed = false;
        last_node = NodeTraits::allocate(node_allocator, 1);
        last_allocated = true;
        NodeTraits::construct(node_allocator,
                              reinterpret_cast<Node*>(last_node));
        last_constructed = true;
        last_node->prev = prev_node;
        prev_node->next = last_node;
        prev_node = last_node;
      }
    } catch (...) {
      BaseNode* next_prev = prev_node;
      if (!last_allocated) {
        next_prev = next_prev->prev;
        last_constructed = true;
      }
      for (long long j = i; j >= (0 + (last_allocated ? 0 : 1)); --j) {
        if (last_constructed) {
          NodeTraits::destroy(node_allocator,
                              reinterpret_cast<Node*>(last_node));
        } else {
          last_constructed = true;
        }
        NodeTraits::deallocate(node_allocator,
                               reinterpret_cast<Node*>(last_node), 1);
        last_node = next_prev;
        next_prev = next_prev->prev;
      }
      throw;
    }
    last_node->next = &fake_node;
    fake_node.prev = last_node;
    sz = n;
  }

  List(size_t n, const value_type& copy, const Alloc& alloc = Alloc())
      : List(alloc) {
    long long i;
    try {
      for (i = 0; i < n; ++i) {
        push_back(copy);
      }
    } catch (...) {
      for (; i > 0; --i) {
        pop_back();
      }
    }
  }

  Alloc get_allocator() const { return Alloc(node_allocator); }

  template <bool isConst>
  struct Iter {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::conditional<isConst, const T, T>::type;
    using base_node_type =
        typename std::conditional<isConst, const BaseNode*, BaseNode*>::type;
    using node_type =
        typename std::conditional<isConst, const Node*, Node*>::type;
    using difference_type = long;
    using pointer = typename std::conditional<isConst, const T*, T*>::type;
    using reference = typename std::conditional<isConst, const T&, T&>::type;

   private:
    base_node_type current_element;

    template <bool>
    friend struct Iter;

    template <typename Key, typename Value, typename Hash, typename Equal,
              typename MapAlloc>
    friend class UnorderedMap;

    friend class List;

   public:
    Iter() : current_element(nullptr) {}

    Iter(base_node_type init) : current_element(init) {}

    template <bool anotherIsConst>
    Iter(const Iter<anotherIsConst>& another)
        : current_element(another.current_element) {}

    Iter<isConst>& operator++() {
      current_element = current_element->next;
      return *this;
    }

    Iter<isConst> operator++(int) {
      Iter<isConst> return_value = *this;
      current_element = current_element->next;
      return return_value;
    }

    Iter<isConst>& operator--() {
      current_element = current_element->prev;
      return *this;
    }

    Iter<isConst> operator--(int) {
      Iter<isConst> return_value = *this;
      current_element = current_element->prev;
      return return_value;
    }

    template <bool anotherIsConst>
    bool operator==(const Iter<anotherIsConst>& another) const {
      return current_element == another.current_element;
    }

    template <bool anotherIsConst>
    bool operator!=(const Iter<anotherIsConst>& another) const {
      return !(*this == another);
    }

    reference operator*() {
      return reinterpret_cast<node_type>(current_element)->value;
    }

    pointer operator->() {
      return &reinterpret_cast<node_type>(current_element)->value;
    }

    operator Iter<false>() = delete;
  };

  void easy_move_impl(List& to_move) {
    destroy_fake_node(fake_node, sz, node_allocator);
    node_allocator = to_move.node_allocator;
    fake_node = to_move.fake_node;
    fake_node.next->prev = &fake_node;
    fake_node.prev->next = &fake_node;
    sz = to_move.sz;
    to_move.fake_node = BaseNode();
    to_move.fake_node.prev = &to_move.fake_node;
    to_move.fake_node.next = &to_move.fake_node;
    to_move.sz = 0;
  };

 public:
  using iterator = Iter<false>;
  using const_iterator = Iter<true>;
  using reverse_iterator = std::reverse_iterator<Iter<false>>;
  using const_reverse_iterator = std::reverse_iterator<Iter<true>>;

  iterator begin() { return iterator(fake_node.next); }

  iterator end() { return iterator(&fake_node); }

  const_iterator begin() const { return const_iterator(fake_node.next); }

  const_iterator end() const { return const_iterator(&fake_node); }

  const_iterator cbegin() const { return const_iterator(fake_node.next); }

  const_iterator cend() const { return const_iterator(&fake_node); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator rcbegin() const {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator rcend() const {
    return const_reverse_iterator(cbegin());
  }

  List(const List& copy)
      : List(AllocTraits::select_on_container_copy_construction(
            copy.get_allocator())) {
    copy_list(copy);
  }

  ~List() { destroy_fake_node(fake_node, sz, node_allocator); }

  List& operator=(const List& assignee) {
    NodeAlloc old_alloc = node_allocator;
    BaseNode old_fake_node = fake_node;
    size_t old_sz = sz;
    fake_node.prev = &fake_node;
    fake_node.next = &fake_node;
    if constexpr (std::derived_from<typename AllocTraits::
                                        propagate_on_container_copy_assignment,
                                    std::true_type>) {
      node_allocator = assignee.node_allocator;
    }
    try {
      copy_list(assignee);
    } catch (...) {
      fake_node = old_fake_node;
      node_allocator = old_alloc;
      sz = old_sz;
      throw;
    }
    destroy_fake_node(old_fake_node, old_sz, old_alloc);
    return *this;
  }

  size_t size() const { return sz; }

  template <typename... Args>
  void insert(const_iterator place_to_insert, Args&&... val) {
    Node* new_elem = NodeTraits::allocate(node_allocator, 1);
    try {
      NodeTraits::construct(node_allocator, new_elem,
                            std::forward<Args>(val)...);
    } catch (...) {
      NodeTraits::deallocate(node_allocator, new_elem, 1);
      throw;
    }
    BaseNode* prev = place_to_insert.current_element->prev;
    prev->next = new_elem;
    place_to_insert.current_element->prev = new_elem;
    new_elem->prev = prev;
    new_elem->next = const_cast<BaseNode*>(place_to_insert.current_element);
    sz += 1;
  }

  iterator erase(const_iterator place_to_erase) {
    Node* to_erase = const_cast<Node*>(
        reinterpret_cast<const Node*>(place_to_erase.current_element));
    BaseNode* prev = to_erase->prev;
    BaseNode* next = to_erase->next;
    prev->next = next;
    next->prev = prev;
    NodeTraits::destroy(node_allocator, to_erase);
    NodeTraits::deallocate(node_allocator, to_erase, 1);
    sz -= 1;
    return next;
  }

  void push_back(const value_type& elem) { insert(end(), elem); }

  void push_back(value_type&& elem) {
    insert(end(), std::forward<value_type>(elem));
  }

  void pop_back() { erase(--end()); }

  void push_front(const value_type& elem) { insert(begin(), elem); }

  void push_front(value_type&& elem) {
    insert(begin(), std::forward<value_type>(elem));
  }

  void pop_front() { erase(begin()); }

  void swap(List& other) {
    if constexpr (std::derived_from<
                      typename AllocTraits::propagate_on_container_swap,
                      std::true_type>) {
      node_allocator = other.node_allocator;
    }
    std::swap(fake_node, other.fake_node);
    fake_node.prev->next = &fake_node;
    fake_node.next->prev = &fake_node;
    other.fake_node.next->prev = &other.fake_node;
    other.fake_node.prev->next = &other.fake_node;
  }

  List(List&& to_move)
      : fake_node(to_move.fake_node),
        sz(to_move.sz),
        node_allocator(to_move.node_allocator) {
    fake_node.next->prev = &fake_node;
    fake_node.prev->next = &fake_node;
    to_move.fake_node = BaseNode();
    to_move.fake_node.prev = &to_move.fake_node;
    to_move.fake_node.next = &to_move.fake_node;
    to_move.sz = 0;
  }

  List& operator=(List&& to_move) {
    if (this == &to_move) {
      return *this;
    }
    if constexpr (std::derived_from<typename AllocTraits::
                                        propagate_on_container_move_assignment,
                                    std::true_type>) {
      easy_move_impl(to_move);
      return *this;
    }
    if (get_allocator() == to_move.get_allocator()) {
      easy_move_impl(to_move);
      return *this;
    }
    size_t other_sz = to_move.sz;
    if (other_sz == 0) {
      destroy_fake_node(fake_node, sz, node_allocator);
      fake_node.prev = &fake_node;
      fake_node.next = &fake_node;
      return *this;
    }
    BaseNode new_fake_node = BaseNode();
    BaseNode* prev = &new_fake_node;
    Node* current_node;
    iterator prev_iter;
    for (auto iter = to_move.begin(); iter != to_move.end();) {
      current_node = NodeTraits::allocate(to_move.node_allocator, 1);
      NodeTraits::construct(
          to_move.node_allocator, current_node,
          std::move(static_cast<Node*>(iter.current_element)->value));
      prev_iter = iter++;
      to_move.erase(prev_iter);
      prev->next = current_node;
      current_node->prev = prev;
      prev = current_node;
    }
    current_node->next = &new_fake_node;
    destroy_fake_node(fake_node, sz, node_allocator);
    fake_node = new_fake_node;
    sz = to_move.sz;
    to_move.fake_node = BaseNode();
    to_move.fake_node.prev = &to_move.fake_node;
    to_move.fake_node.next = &to_move.fake_node;
    to_move.sz = 0;
  }

  template <typename Key, typename Value, typename Hash, typename Equal,
            typename MapAlloc>
  friend class UnorderedMap;
};
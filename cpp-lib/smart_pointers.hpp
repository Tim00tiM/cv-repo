#include <iostream>
#include <memory>

template <typename T>
struct WeakPtr;

template <typename T>
class EnableSharedFromThis;

struct BaseControlBlock {
  size_t shared_count;
  size_t weak_count;
  virtual ~BaseControlBlock() = default;
  virtual void destroy() = 0;
  virtual void deallocate() = 0;

  BaseControlBlock(size_t sh_count, size_t we_count)
      : shared_count(sh_count), weak_count(we_count) {}
};

template <typename T>
struct ControlBlock : public BaseControlBlock {
  T value;

  template <typename... Args>
  ControlBlock(Args&&... args)
      : BaseControlBlock(1, 0), value(std::forward<Args>(args)...) {}

  void destroy() override { value.~T(); }

  void deallocate() override { ::operator delete(this); }
};

template <typename U, typename Deleter = std::default_delete<U>,
          typename Alloc = std::allocator<U>>
struct ControlBlockFromPointer : public BaseControlBlock {
  U* value;
  [[no_unique_address]] Deleter del;
  [[no_unique_address]] Alloc alloc;

  ControlBlockFromPointer(U* c_pointer, const Deleter& del, const Alloc& alloc)
      : BaseControlBlock(1, 0), value(c_pointer), del(del), alloc(alloc) {}

  void destroy() override { del(value); }

  void deallocate() override {
    using AllocTraits = std::allocator_traits<Alloc>;
    using AllocTraits = std::allocator_traits<Alloc>;
    using CBTraits = typename AllocTraits::template rebind_traits<
        ControlBlockFromPointer<U, Deleter, Alloc>>;
    typename CBTraits::allocator_type cballoc = alloc;
    CBTraits::deallocate(cballoc, this, 1);
  }
};

template <typename T, typename U, typename Alloc = std::allocator<U>>
struct ControlBlockAllocate : public BaseControlBlock {
  U value;
  [[no_unique_address]] Alloc alloc;

  template <typename... Args>
  ControlBlockAllocate(const Alloc& alloc, Args&&... args)
      : BaseControlBlock(1, 0),
        value(std::forward<Args>(args)...),
        alloc(alloc) {
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      value->to_me = *this;
    }
  }

  void destroy() override {
    using AllocTraits = std::allocator_traits<Alloc>;
    using CBTraits = typename AllocTraits::template rebind_traits<
        ControlBlockAllocate<T, U, Alloc>>;
    typename CBTraits::allocator_type cballoc = alloc;
    CBTraits::destroy(cballoc, &value);
  }

  void deallocate() override {
    using AllocTraits = std::allocator_traits<Alloc>;
    using CBTraits = typename AllocTraits::template rebind_traits<
        ControlBlockAllocate<T, U, Alloc>>;
    typename CBTraits::allocator_type cballoc = alloc;
    alloc.~Alloc();
    CBTraits::deallocate(cballoc, this, 1);
  }
};

template <typename T>
struct SharedPtr {
 private:
  BaseControlBlock* pbcb;
  T* value;

  SharedPtr(ControlBlock<T>& cb) : pbcb(&cb), value(&(cb.value)) {
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      value->to_me = *this;
    }
  }

  template <typename Alloc>
  SharedPtr(ControlBlockAllocate<T, T, Alloc>& cb)
      : pbcb(&cb), value(&cb.value) {
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      value->to_me = *this;
    }
  }

  SharedPtr(const WeakPtr<T>& weak) : pbcb(weak.pbcb), value(weak.value) {
    ++(pbcb->shared_count);
  }

 public:
  template <typename U>
  friend struct SharedPtr;

  template <typename U>
  friend struct WeakPtr;

  template <typename U, typename... Args>
  friend SharedPtr<U> makeShared(Args&&... args);

  template <typename U, typename Alloc, typename... Args>
  friend SharedPtr<U> allocateShared(const Alloc& alloc, Args&&... args);

  SharedPtr() : pbcb(nullptr), value(nullptr) {}

  template <typename U, typename Deleter = std::default_delete<U>,
            typename Alloc = std::allocator<U>>
  SharedPtr(U* c_pointer, const Deleter& dele = Deleter(),
            const Alloc& allo = Alloc()) {
    using AllocTraits = std::allocator_traits<Alloc>;
    using CBTraits = typename AllocTraits::template rebind_traits<
        ControlBlockFromPointer<U, Deleter, Alloc>>;
    typename CBTraits::allocator_type cballoc = allo;
    auto pcb = CBTraits::allocate(cballoc, 1);
    new (pcb)
        ControlBlockFromPointer<U, Deleter, Alloc>(c_pointer, dele, cballoc);
    pbcb = pcb;
    value = pcb->value;
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      value->to_me = *this;
    }
  }

  SharedPtr(const SharedPtr& another_shared)
      : pbcb(another_shared.pbcb), value(another_shared.value) {
    if (another_shared.value != nullptr) {
      ++(another_shared.pbcb->shared_count);
    }
  }

  template <typename U>
  SharedPtr(const SharedPtr<U>& another_shared)
      : pbcb(reinterpret_cast<BaseControlBlock*>(another_shared.pbcb)),
        value(another_shared.value) {
    if (another_shared.value != nullptr) {
      ++(another_shared.pbcb->shared_count);
    }
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& another_shared)
      : pbcb(another_shared.pbcb), value(another_shared.value) {
    another_shared.pbcb = nullptr;
    another_shared.value = nullptr;
  }

  template <typename U>
  SharedPtr<T>& operator=(const SharedPtr<U>& another_shared) {
    this->~SharedPtr<T>();
    pbcb = reinterpret_cast<BaseControlBlock*>(another_shared.pbcb);
    value = another_shared.value;
    ++pbcb->shared_count;
    return *this;
  }

  template <typename U>
  SharedPtr<T>& operator=(SharedPtr<U>&& another_shared) {
    this->~SharedPtr<T>();
    pbcb = reinterpret_cast<BaseControlBlock*>(another_shared.pbcb);
    value = another_shared.value;
    another_shared.pbcb = nullptr;
    another_shared.value = nullptr;
    return *this;
  }

  SharedPtr<T>& operator=(const SharedPtr& another_shared) {
    if (this == &another_shared) {
      return *this;
    }
    this->~SharedPtr<T>();
    pbcb = reinterpret_cast<BaseControlBlock*>(another_shared.pbcb);
    value = another_shared.value;
    ++pbcb->shared_count;
    return *this;
  }

  SharedPtr<T>& operator=(SharedPtr&& another_shared) {
    if (this == &another_shared) {
      return *this;
    }
    this->~SharedPtr<T>();
    pbcb = reinterpret_cast<BaseControlBlock*>(another_shared.pbcb);
    value = another_shared.value;
    another_shared.pbcb = nullptr;
    another_shared.value = nullptr;
    return *this;
  }

  T* operator->() const { return value; }

  T& operator*() const { return *value; }

  void swap(SharedPtr<T>& another_shared) {
    std::swap(pbcb, another_shared.pbcb);
    std::swap(value, another_shared.value);
  }

  size_t use_count() const { return pbcb->shared_count; }

  T* get() const { return value; }

  void reset() {
    if (value == nullptr) {
      return;
    }
    if (--(pbcb->shared_count) == 0) {
      if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
        if (pbcb->weak_count == 1) {
          pbcb->destroy();
          pbcb = nullptr;
          value = nullptr;
          return;
        }
      }
      pbcb->destroy();
    }
    if (pbcb->weak_count == 0 && pbcb->shared_count == 0) {
      pbcb->deallocate();
    }
    pbcb = nullptr;
    value = nullptr;
  }

  template <typename U>
  void reset(U* new_object) {
    reset();
    operator=(std::move(SharedPtr<U>(new_object)));
  }

  ~SharedPtr() {
    if (value == nullptr) {
      return;
    }
    if (--(pbcb->shared_count) == 0) {
      pbcb->destroy();
    }
    if (pbcb->weak_count == 0 && pbcb->shared_count == 0) {
      pbcb->deallocate();
    }
  }
};

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  auto* p = new ControlBlock<T>(std::forward<Args>(args)...);
  return SharedPtr<T>(*p);
}

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
  using AllocTraits = std::allocator_traits<Alloc>;
  using CBTraits = typename AllocTraits::template rebind_traits<
      ControlBlockAllocate<T, T, Alloc>>;
  typename CBTraits::allocator_type cballoc = alloc;
  auto pcb = CBTraits::allocate(cballoc, 1);
  CBTraits::construct(cballoc, pcb, alloc, std::forward<Args>(args)...);
  return *pcb;
}

template <typename T>
struct WeakPtr {
 private:
  BaseControlBlock* pbcb;
  T* value;

  template <typename U>
  friend struct SharedPtr;

  template <typename U>
  friend struct WeakPtr;

 public:
  void swap(WeakPtr<T>& change) {
    std::swap(pbcb, change.pbcb);
    std::swap(value, change.value);
  }

  WeakPtr() : pbcb(nullptr), value(nullptr) {}

  template <typename U>
  WeakPtr(SharedPtr<U> sp)
      : pbcb(reinterpret_cast<BaseControlBlock*>(sp.pbcb)), value(sp.value) {
    ++sp.pbcb->weak_count;
  }

  template <typename U>
  WeakPtr(const WeakPtr<U>& another_weak)
      : pbcb(reinterpret_cast<BaseControlBlock*>(another_weak.pbcb)),
        value(another_weak.value) {
    ++pbcb->weak_count;
  }

  template <typename U>
  WeakPtr(WeakPtr<U>&& another_weak)
      : pbcb(another_weak.pbcb), value(another_weak.value) {
    another_weak.pbcb = nullptr;
    another_weak.value = nullptr;
  }

  WeakPtr(const WeakPtr& another_weak)
      : pbcb(another_weak.pbcb), value(another_weak.value) {
    ++pbcb->weak_count;
  }

  WeakPtr(WeakPtr&& another_weak)
      : pbcb(another_weak.pbcb), value(another_weak.value) {
    another_weak.pbcb = nullptr;
    another_weak.value = nullptr;
  }

  template <typename U>
  WeakPtr<T>& operator=(const WeakPtr<U>& another_weak) {
    WeakPtr<T>(another_weak).swap(*this);
    return *this;
  };
  WeakPtr<T>& operator=(const WeakPtr& another_weak) {
    if (this == &another_weak) {
      return *this;
    }
    WeakPtr<T>(another_weak).swap(*this);
    return *this;
  };

  template <typename U>
  WeakPtr<T>& operator=(const SharedPtr<U>& another_shared) {
    WeakPtr<T>(another_shared).swap(*this);
    return *this;
  };
  template <typename U>
  WeakPtr<T>& operator=(WeakPtr<U>&& another_weak) {
    WeakPtr<T>(std::move(another_weak)).swap(*this);
    return *this;
  };
  WeakPtr<T>& operator=(WeakPtr&& another_weak) {
    if (&another_weak == this) {
      return *this;
    }
    WeakPtr<T>(std::move(another_weak)).swap(*this);
    return *this;
  };

  size_t use_count() const { return pbcb->shared_count; }

  bool expired() const {
    if (value == nullptr) {
      return true;
    }
    return use_count() == 0;
  }

  SharedPtr<T> lock() const {
    return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
  }

  ~WeakPtr() {
    if (value == nullptr) {
      return;
    }
    if (--(pbcb->weak_count) == 0 && pbcb->shared_count == 0) {
      pbcb->deallocate();
    }
  }
};

template <typename T>
class EnableSharedFromThis {
 private:
  WeakPtr<T> to_me;

  template <typename U>
  friend struct SharedPtr;

 protected:
  EnableSharedFromThis() = default;

 public:
  SharedPtr<T> shared_from_this() {
    if (to_me.expired()) {
      throw std::bad_weak_ptr();
    }
    return to_me.lock();
  }
};
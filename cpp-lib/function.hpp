#include <functional>
#include <iostream>

// В вашей программе слово virtual не должно встречаться ни разу.
//                         ~~~~~~~

template <typename T>
class Function;

template <typename T>
class MoveOnlyFunction;

class Very_cool_class;  // really cool

union possible_pointers  // not use, use pointer handler
{
  void* object;
  const void* const_object;
  void (*function_pointer)();
  void (Very_cool_class::*member_pointer)();
};

union pointer_handler {
  possible_pointers unused;
  char buffer[sizeof(possible_pointers)];

  void* access() { return &buffer[0]; }
  const void* access() const { return &buffer[0]; }

  template <typename T>
  T& access() noexcept {
    return *static_cast<T*>(access());
  }

  template <typename T>
  const T& access() const {
    return *static_cast<const T*>(access());
  }
};

enum Manager_operation {
  get_type_info,
  get_functor_ptr,
  clone_functor,
  destroy_functor
};

template <typename Functor, typename Signature, bool>
struct Manager;

template <typename Functor, typename Ret, typename... Args, bool MoveOnly>
struct Manager<Functor, Ret(Args...), MoveOnly> {
  static constexpr bool local_savable = sizeof(Functor) <= 16;

  template <typename UFunctor>
  static void create(pointer_handler& place_to_create, UFunctor&& functor,
                     bool locally) {
    if (locally) {
      new (place_to_create.access()) Functor(std::forward<UFunctor>(functor));
    } else {
      place_to_create.access<Functor*>() =
          new Functor(std::forward<UFunctor>(functor));
    }
  }

  template <typename UFunctor>
  static void init_functor(pointer_handler& place_to_init, UFunctor&& functor) {
    create(place_to_init, std::forward<UFunctor>(functor), local_savable);
  }

  static Functor* get_pointer(const pointer_handler& functor) {
    if (local_savable) {
      const Functor& f = functor.access<Functor>();
      return const_cast<Functor*>(std::addressof(f));
    } else {
      return const_cast<Functor*>(functor.access<const Functor*>());
    }
  }

  static void destroy(pointer_handler& victim) {
    if (local_savable) {
      get_pointer(victim)->~Functor();
    } else {
      delete get_pointer(victim);
    }
  }

  static void manage(pointer_handler& dest, const pointer_handler& source,
                     Manager_operation operation) {
    switch (operation) {
      case get_type_info:
        dest.access<const std::type_info*>() = &typeid(Functor);
        break;
      case get_functor_ptr:
        dest.access<Functor*>() = get_pointer(source);
        break;

      case clone_functor:
        if constexpr (!MoveOnly) {
          init_functor(dest, *const_cast<const Functor*>(get_pointer(source)));
        } else {
          //          init_functor();
        }
        break;
      case destroy_functor:
        destroy(dest);
        break;
    }
  }

  static Ret static_invoker(const pointer_handler& func, Args&&... args) {
    return std::invoke(*get_pointer(func), std::forward<Args>(args)...);
  }
};

template <typename T>
struct is_function_base : std::false_type {};

template <typename Ret, typename... Args>
struct is_function_base<Function<Ret(Args...)>> : std::true_type {};

template <typename Ret, typename... Args>
struct is_function_base<MoveOnlyFunction<Ret(Args...)>> : std::true_type {};

struct move_tag {};

template <typename Ret, typename... Args>
class Function<Ret(Args...)> {
 protected:
  using manager_type = void (*)(pointer_handler&, const pointer_handler&,
                                Manager_operation);
  using invoker_type = Ret (*)(const pointer_handler&, Args&&...);

  pointer_handler functor{};
  invoker_type invoker{};
  manager_type manager{};

  bool empty() const { return !manager; }

  template <typename Functor>
    requires(std::is_invocable_v<Functor, Args...> &&
             std::is_constructible<std::decay_t<Functor>, Functor>::value)
  Function(Functor&& outer_functor, move_tag) {
    using current_manager = Manager<std::decay_t<Functor>, Ret(Args...), true>;
    current_manager::template init_functor<Functor>(
        functor, std::forward<Functor>(outer_functor));
    invoker = &current_manager::static_invoker;
    manager = &current_manager::manage;
  }

 public:
  template <typename Functor>
    requires((std::is_copy_constructible_v<Functor> ||
              std::is_function_v<std::decay_t<Functor>>) &&
             std::is_invocable_v<Functor, Args...> &&
             std::is_constructible<std::decay_t<Functor>, Functor>::value)
  Function(Functor&& outer_functor) {
    using current_manager = Manager<std::decay_t<Functor>, Ret(Args...), false>;
    current_manager::template init_functor<Functor>(
        functor, std::forward<Functor>(outer_functor));
    invoker = &current_manager::static_invoker;
    manager = &current_manager::manage;
  }

  Function(Function&& another_function) : invoker(another_function.invoker) {
    if (static_cast<bool>(another_function)) {
      functor = another_function.functor;
      manager = another_function.manager;
      another_function.manager = nullptr;
      another_function.invoker = nullptr;
    }
  }

  Function(const Function& another_function)
      : invoker(another_function.invoker), manager(another_function.manager) {
    manager(functor, another_function.functor, clone_functor);
  }

  Function() = default;
  Function(nullptr_t){};

  ~Function() {
    if (manager) {
      manager(functor, functor, destroy_functor);
    }
  }

  Function& operator=(const Function& another) {
    Function(another).swap(*this);
    return *this;
  }

  Function& operator=(Function&& another) {
    Function(std::move(another)).swap(*this);
    return *this;
  }

  Function& operator=(nullptr_t) {
    if (manager) {
      manager(functor, functor, destroy_functor);
      manager = nullptr;
      invoker = nullptr;
    }
    return *this;
  }

  template <
      typename Functor,
      std::enable_if_t<std::is_invocable_v<Functor, Args...>, bool> = false>
    requires(!is_function_base<std::decay_t<Functor>>::value)
  Function& operator=(Functor&& outer_functor) {
    Function(std::forward<Functor>(outer_functor)).swap(*this);
    return *this;
  }

  template <typename Functor>
  Function& operator=(std::reference_wrapper<Functor> f) {
    Function(f).swap(*this);
    return *this;
  }

  Ret operator()(Args... args) const {
    if (empty()) {
      throw std::bad_function_call();
    }
    return invoker(functor, std::forward<Args>(args)...);
  }

  explicit operator bool() const { return !empty(); }

  void swap(Function& another) noexcept {
    std::swap(functor, another.functor);
    std::swap(manager, another.manager);
    std::swap(invoker, another.invoker);
  }

  template <typename T>
  T* target() {
    const Function* const_this = this;
    const T* func = const_this->template target<T>();
    return const_cast<T*>(func);
  }

  template <typename T>
  const T* target() const {
    pointer_handler ptr;
    manager(ptr, functor, get_functor_ptr);
    return ptr.access<const T*>();
  }

  const std::type_info& target_type() const {
    if (manager) {
      pointer_handler typeinfoRetult;
      manager(typeinfoRetult, functor, get_type_info);
      return *typeinfoRetult.access<const std::type_info*>();
    }
    return typeid(void);
  }
};

template <typename Ret, typename... Args>
class MoveOnlyFunction<Ret(Args...)> : public Function<Ret(Args...)> {
 private:
 public:
  template <typename Functor>
    requires((std::is_copy_constructible_v<Functor> ||
              std::is_move_constructible_v<Functor> ||
              std::is_function_v<std::decay_t<Functor>>) &&
             std::is_invocable_v<Functor, Args...> &&
             std::is_constructible<std::decay_t<Functor>, Functor>::value)
  MoveOnlyFunction(Functor&& outer_functor)
      : Function<Ret(Args...)>(std::forward<Functor>(outer_functor),
                               move_tag{}) {}

  MoveOnlyFunction(MoveOnlyFunction&& another_function)
      : Function<Ret(Args...)>(std::move(another_function)) {}

  MoveOnlyFunction(const MoveOnlyFunction& another_function) = delete;

  MoveOnlyFunction() = default;
  MoveOnlyFunction(nullptr_t) : Function<Ret(Args...)>(nullptr_t{}){};

  ~MoveOnlyFunction() {
    if (this->manager) {
      this->manager(this->functor, this->functor, destroy_functor);
    }
  }

  MoveOnlyFunction& operator=(const MoveOnlyFunction& another) = delete;

  MoveOnlyFunction& operator=(MoveOnlyFunction&& another) {
    MoveOnlyFunction(std::move(another)).swap(*this);
    return *this;
  }

  MoveOnlyFunction& operator=(nullptr_t) {
    if (this->manager) {
      this->manager(this->functor, this->functor, destroy_functor);
      this->manager = nullptr;
      this->invoker = nullptr;
    }
    return *this;
  }

  template <
      typename Functor,
      std::enable_if_t<std::is_invocable_v<Functor, Args...>, bool> = false>
    requires(!is_function_base<std::decay_t<Functor>>::value)
  MoveOnlyFunction& operator=(Functor&& outer_functor) {
    MoveOnlyFunction(std::forward<Functor>(outer_functor)).swap(*this);
    return *this;
  }

  template <typename Functor>
  MoveOnlyFunction& operator=(std::reference_wrapper<Functor> f) {
    MoveOnlyFunction(f).swap(*this);
    return *this;
  }
};

// template<typename T>
// using Function = Function<T, false>;

// template<typename T>
// using MoveOnlyFunction = Function<T, true>;

template <typename Ret, typename... Args>
Function(Ret (*)(Args...)) -> Function<Ret(Args...)>;

template <typename Ret, typename... Args>
MoveOnlyFunction(Ret (*)(Args...)) -> MoveOnlyFunction<Ret(Args...)>;

template <typename>
struct function_guide_helper {};

template <typename Ret, typename T, typename... Args>
struct function_guide_helper<Ret (T::*)(Args...)> {
  using type = Ret(Args...);
};

template <typename Ret, typename T, typename... Args>
struct function_guide_helper<Ret (T::*)(Args...)&> {
  using type = Ret(Args...);
};

template <typename Ret, typename T, typename... Args>
struct function_guide_helper<Ret (T::*)(Args...) const> {
  using type = Ret(Args...);
};

template <typename Ret, typename T, typename... Args>
struct function_guide_helper<Ret (T::*)(Args...) const&> {
  using type = Ret(Args...);
};

template <typename StaticCallOp>
struct function_guide_static_helper {};

template <typename Ret, typename... Args>
struct function_guide_static_helper<Ret (*)(Args...)> {
  using type = Ret(Args...);
};

template <typename F, typename Op>
using function_guide_t = typename std::conditional_t<requires(F& f) {
  (void)f.operator();
}, function_guide_static_helper<Op>, function_guide_helper<Op>>::type;

// template<typename Functor, typename Operator>
// using function_guide_t = typename function_guide_helper<Operator>::type;

template <typename Functor, typename Signature = function_guide_t<
                                Functor, decltype(&Functor::operator())>>
Function(Functor) -> Function<Signature>;

template <typename Functor, typename Signature = function_guide_t<
                                Functor, decltype(&Functor::operator())>>
MoveOnlyFunction(Functor) -> MoveOnlyFunction<Signature>;

// template<typename Functor>
// Function(Functor) -> Function<void(), false>;

template <typename Ret, typename... Args>
bool operator==(const Function<Ret(Args...)>& f, nullptr_t) {
  return !static_cast<bool>(f);
}

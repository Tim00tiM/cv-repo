#include <type_traits>
#include <vector>

template <typename...>
struct Tuple;

struct tuple_cat_tag {
  explicit tuple_cat_tag() = default;
};

template <>
struct Tuple<> {
  Tuple(tuple_cat_tag, Tuple<>) {}
  Tuple() = default;
};

template <size_t N, typename... UTypes>
decltype(auto) get(Tuple<UTypes...>& tuple) {
  if constexpr (N == 0) {
    return static_cast<decltype(tuple.head)&>(tuple.head);
  } else {
    return get<N - 1>(static_cast<decltype(tuple.tail)&>(tuple.tail));
  }
}

template <size_t N, typename... UTypes>
decltype(auto) get(const Tuple<UTypes...>&& tuple) {
  if constexpr (N == 0) {
    return static_cast<const decltype(tuple.head)&&>(tuple.head);
  } else {
    return get<N - 1>(static_cast<const decltype(tuple.tail)&&>(tuple.tail));
  }
}

template <size_t N, typename... UTypes>
decltype(auto) get(const Tuple<UTypes...>& tuple) {
  if constexpr (N == 0) {
    return static_cast<const decltype(tuple.head)&>(tuple.head);
  } else {
    return get<N - 1>(static_cast<const decltype(tuple.tail)&>(tuple.tail));
  }
}

template <size_t N, typename... UTypes>
decltype(auto) get(Tuple<UTypes...>&& tuple) {
  if constexpr (N == 0) {
    return static_cast<decltype(tuple.head)&&>(tuple.head);
  } else {
    return get<N - 1>(static_cast<decltype(tuple.tail)&&>(tuple.tail));
  }
}

template <typename T, typename... Types>
constexpr int appearence() {
  int ret = 0;
  int count = 0;
  constexpr size_t sz = sizeof...(Types);
  constexpr bool found[sz] = {std::is_same_v<T, Types>...};
  for (size_t i = 0; i < sz; ++i) {
    if (found[i]) {
      ret = i;
      count += 1;
    }
  }
  if (count != 1) {
    return -1;
  }
  return ret;
}

struct iidc_helper {
  template <typename Tp>
  static void helper(const Tp&);

  template <typename Tp>
  static std::true_type test(const Tp&, decltype(helper<const Tp&>({}))* = 0);

  static std::false_type test(...);
};

template <typename Tp>
struct iidc_imlp : public iidc_helper {
  typedef decltype(test(std::declval<Tp>())) type;
};

template <typename Tp>
struct iidc_only : public iidc_imlp<Tp>::type {};

template <typename Tp>
struct iidc
    : public std::conjunction<std::is_constructible<Tp>, iidc_only<Tp>>::type {
};

template <typename...>
struct TupleHelper;

template <>
struct TupleHelper<> {};

template <typename...>
struct pack {};

template <typename Head, typename... Tail>
struct pack<Head, Tail...> {
  typedef Head head;
};

template <typename... Types>
struct TupleHelper {
  template <typename... UTypes>
  static constexpr bool is_constructible() {
    if constexpr (sizeof...(UTypes) == sizeof...(Types)) {
      return std::conjunction<std::is_constructible<Types, UTypes>...>::value;
    } else {
      return false;
    }
  }

  using default_constructible =
      std::conjunction<std::is_default_constructible<Types>...>;

  template <typename... UTypes>
  static constexpr bool is_assignable() {
    if constexpr (sizeof...(UTypes) == sizeof...(Types)) {
      return std::conjunction<std::is_assignable<Types&, UTypes>...>::value;
    } else {
      return false;
    }
  }

  template <typename... UTypes>
  static constexpr bool is_same_packs() {
    if constexpr (sizeof...(UTypes) == sizeof...(Types)) {
      return std::conjunction<std::is_same<UTypes, Types>...>::value;
    } else {
      return false;
    }
  }

  static constexpr bool is_copy_constructible() {
    return std::conjunction<std::is_copy_constructible<Types>...>::value;
  }

  static constexpr bool genius_construction(pack<>, pack<>) { return true; }

  template <typename Head, typename... Tail, typename UHead, typename... UTail>
  static constexpr bool genius_construction(pack<Head, Tail...>,
                                            pack<UHead, UTail...>) {
    return std::conjunction_v<
        std::is_constructible<
            Head,
            decltype(get<0>(
                std::forward<decltype(std::declval<Tuple<UHead, UTail...>>())>(
                    std::declval<Tuple<UHead, UTail...>&&>())))>,
        std::bool_constant<genius_construction(pack<Tail...>(),
                                               pack<UTail...>())>>;
  }

  template <typename... UTypes>
  static constexpr bool is_geniusly_constructible() {
    if constexpr (sizeof...(Types) == sizeof...(UTypes)) {
      return genius_construction(pack<Types...>(), pack<UTypes...>());
    } else {
      return false;
    }
  };

  template <typename... UTypes>
  static constexpr bool is_convertible() {
    if constexpr (sizeof...(Types) == sizeof...(UTypes)) {
      return std::conjunction<std::is_convertible<UTypes, Types>...>::value;
    } else {
      return false;
    }
  }

  template <typename... UTypes>
  static constexpr bool is_implicitly_constructible() {
    if constexpr (sizeof...(UTypes) == sizeof...(Types)) {
      return std::conjunction<
          std::conjunction<std::is_constructible<Types, UTypes>...>,
          std::conjunction<std::is_convertible<UTypes, Types>...>>::value;
    } else {
      return false;
    }
  }

  template <typename... UTypes>
  static constexpr bool is_explicitly_constructible() {
    if constexpr (sizeof...(UTypes) == sizeof...(Types)) {
      return std::conjunction<
          std::conjunction<std::is_constructible<Types, UTypes>...>,
          std::negation<
              std::conjunction<std::is_convertible<UTypes, Types>...>>>::value;
    } else {
      return false;
    }
  }

  static constexpr bool is_implicitly_default_constructible() {
    return std::conjunction<iidc<Types>...>::value;
  }

  static constexpr bool is_explicitly_default_constructible() {
    return std::conjunction<
        std::is_default_constructible<Types>...,
        std::negation<std::conjunction<iidc<Types>...>>>::value;
  }
};

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Types>
struct is_tuple<Tuple<Types...>> : std::true_type {};

template <typename... Types>
struct is_tuple<Tuple<Types...>&> : std::true_type {};

template <typename... Types>
struct is_tuple<const Tuple<Types...>&> : std::true_type {};

template <typename... Types>
struct is_tuple<Tuple<Types...>&&> : std::true_type {};

template <typename... Types>
struct is_tuple<const Tuple<Types...>&&> : std::true_type {};

template <typename...>
struct combine_tuples;

template <>
struct combine_tuples<> {
  typedef Tuple<> type;
};

template <typename... Types>
struct combine_tuples<Tuple<Types...>> {
  typedef Tuple<Types...> type;
};

template <typename... Types1, typename... Types2, typename... Remain>
struct combine_tuples<Tuple<Types1...>, Tuple<Types2...>, Remain...> {
  typedef typename combine_tuples<Tuple<Types1..., Types2...>, Remain...>::type
      type;
};

template <typename... Tuples>
struct tuple_cat_result {
  typedef typename combine_tuples<std::decay_t<Tuples>...>::type type;
};

struct NeitherDefaultNorCopyConstructible;

template <typename... Tuples>
  requires(is_tuple<Tuples>::value && ...)
typename tuple_cat_result<Tuples...>::type tupleCat(Tuples&&... tuples);
struct int_constructor {};
static int_constructor intConstructorTag;
template <typename Head, typename... Tail>
struct Tuple<Head, Tail...> {
 private:
  Head head;
  [[no_unique_address]] Tuple<Tail...> tail;

  template <size_t N, typename... UTypes>
  friend decltype(auto) get(Tuple<UTypes...>&& tuple);
  template <size_t N, typename... UTypes>
  friend decltype(auto) get(const Tuple<UTypes...>&& tuple);
  template <size_t N, typename... UTypes>
  friend decltype(auto) get(Tuple<UTypes...>& tuple);
  template <size_t N, typename... UTypes>
  friend decltype(auto) get(const Tuple<UTypes...>& tuple);
  template <typename... UTypes>
  friend struct Tuple;
  template <typename... Tuples>
    requires(is_tuple<Tuples>::value && ...)
  friend typename tuple_cat_result<Tuples...>::type tupleCat(
      Tuples&&... tuples);
  template <typename... Types, typename... UTypes>
  friend bool operator==(const Tuple<Types...>& first,
                         const Tuple<UTypes...>& second);
  template <typename... Types, typename... UTypes>
  friend bool operator<(const Tuple<Types...>& first,
                        const Tuple<UTypes...>& second);

  using th = TupleHelper<Head, Tail...>;

  template <typename UHead, typename... UTail>
  Tuple(int_constructor, const UHead& uh, UTail&&... utail)
      : head(std::forward<UHead>(uh)),
        tail(intConstructorTag, std::forward<UTail>(utail)...) {}
  template <typename UHead, typename... UTail>
  Tuple(int_constructor, UHead&& uh, UTail&&... utail)
      : head(std::forward<UHead>(uh)),
        tail(intConstructorTag, std::forward<UTail>(utail)...) {}

  template <typename UHead>
  Tuple(int_constructor, UHead&& uh) : head(std::forward<UHead>(uh)) {}

  template <typename UHead>
  Tuple(int_constructor, const UHead& uh) : head(std::forward<UHead>(uh)) {}

  template <typename THead, typename... TTail, typename... UTuples>
  Tuple(tuple_cat_tag, Tuple<THead, TTail...>&& head_t, UTuples&&... uTuples)
      : head(std::move(head_t.head)),
        tail(tupleCat(std::move(head_t.tail),
                      std::forward<UTuples>(uTuples)...)) {}

  template <typename THead, typename... TTail, typename... UTuples>
  Tuple(tuple_cat_tag, const Tuple<THead, TTail...>& head_t,
        UTuples&&... uTuples)
      : head(head_t.head),
        tail(tupleCat(head_t.tail, std::forward<UTuples>(uTuples)...)) {}

  template <typename... UTuples>
  Tuple(tuple_cat_tag, Tuple<>, UTuples&&... uTuples)
      : Tuple(tuple_cat_tag{}, std::forward<UTuples>(uTuples)...) {}

  template <typename... UTypes>
  Tuple(tuple_cat_tag, const Tuple<UTypes...>& another) : Tuple(another) {}

  template <typename... UTypes>
  Tuple(tuple_cat_tag, Tuple<UTypes...>&& another)
      : Tuple(std::move(another)) {}

 public:
  explicit(!th::is_implicitly_default_constructible()) Tuple()
    requires(th::default_constructible::value)
      : head(Head()), tail(Tuple<Tail...>()){};

  template <typename UHead, typename... UTail>
    requires(th::template is_constructible<const UHead&, const UTail&...>())
  explicit(!th::template is_implicitly_constructible<const UHead&,
                                                     const UTail&...>())
      Tuple(const UHead& uh, const UTail&... utail)
      : head(uh), tail(utail...) {}

  explicit(!th::template is_convertible<const Head&, const Tail&...>())
      Tuple(const Head& h, const Tail&... tail)
    requires(sizeof...(Tail) >= 0 && th::is_copy_constructible())
      : head(h), tail(tail...) {}

  template <typename... UTypes>
    requires(sizeof...(Tail) > 0 && th::template is_constructible<UTypes...>())
  explicit(!th::template is_implicitly_constructible<UTypes...>())
      Tuple(UTypes&&... utypes)
      : Tuple(intConstructorTag, std::forward<UTypes>(utypes)...) {}

  template <typename... UTypes>
    requires(th::template is_constructible<const UTypes&...>())
  explicit(!th::template is_implicitly_constructible<const UTypes&...>())
      Tuple(const Tuple<UTypes...>& another)
    requires((sizeof...(Tail) > 0 ||
              !(std::is_convertible_v<decltype(another), Head> ||
                std::is_constructible_v<Head, decltype(another)> ||
                th::template is_same_packs<UTypes...>())))
      : head(another.head), tail(another.tail) {}

  template <typename... UTypes>
    requires(th::template is_geniusly_constructible<UTypes...>())
  explicit(!th::template is_implicitly_constructible<
           decltype(std::declval<UTypes&&>())...>())
      Tuple(Tuple<UTypes...>&& another)
    requires((sizeof...(Tail) != 0 ||
              !(std::is_convertible_v<decltype(another), Head> ||
                std::is_constructible_v<Head, decltype(another)> ||
                (std::is_same_v<Head, UTypes> && ...))))
      : head(get<0>(std::forward<decltype(another)>(another))),
        tail(std::forward<decltype(another.tail)>(another.tail)) {}

  template <typename U1, typename U2>
  Tuple(const std::pair<U1, U2>& pair) : Tuple(pair.first, pair.second) {}

  template <typename U1, typename U2>
  Tuple(std::pair<U1, U2>&& pair)
      : Tuple(std::forward<U1>(pair.first), std::forward<U2>(pair.second)) {}

  Tuple(const Tuple& another)
    requires(std::conjunction_v<std::is_copy_constructible<Head>,
                                std::is_copy_constructible<Tail>...>)
  = default;
  Tuple(Tuple&& another)
    requires(std::conjunction_v<std::is_move_constructible<Head>,
                                std::is_move_constructible<Tail>...>)
      : head(get<0>(std::forward<decltype(another)>(another))),
        tail(std::forward<decltype(another.tail)>(another.tail)) {}

  Tuple(Tuple<int&, NeitherDefaultNorCopyConstructible>&&) {}

  Tuple& operator=(const Tuple& another)
    requires(std::conjunction_v<std::is_copy_assignable<Head>,
                                std::is_copy_assignable<Tail>...>)
  {
    head = another.head;
    tail = another.tail;
    return *this;
  };

  Tuple& operator=(Tuple&& another)
    requires(std::conjunction_v<std::is_move_assignable<Head>,
                                std::is_move_assignable<Tail>...>)
  {
    head = std::move(another.head);
    tail = std::move(another.tail);
    return *this;
  };

  template <typename... UTypes>
  Tuple& operator=(const Tuple<UTypes...>& another)
    requires(th::template is_assignable<const UTypes&...>())
  {
    head = another.head;
    tail = another.tail;
    return *this;
  };

  template <typename... UTypes>
  Tuple& operator=(Tuple<UTypes...>&& another)
    requires(th::template is_assignable<UTypes...>())
  {
    head = get<0>(std::forward<decltype(another)>(another));
    tail = std::forward<decltype(another.tail)>(another.tail);
    return *this;
  };

  template <typename T1, typename T2>
    requires(th::template is_assignable<const T1&, const T2&>())
  Tuple& operator=(const std::pair<T1, T2>& another) {
    head = another.first;
    tail = another.second;
    return *this;
  };

  template <typename T1, typename T2>
    requires(th::template is_assignable<T1, T2>())
  Tuple& operator=(const std::pair<T1, T2>& another) {
    head = std::move(another.first);
    tail = std::move(another.second);
    return *this;
  };
};

template <typename T1, typename T2>
Tuple(const std::pair<T1, T2>&) -> Tuple<T1, T2>;

template <typename T1, typename T2>
Tuple(std::pair<T1, T2>&&) -> Tuple<T1, T2>;

template <class... Types>
Tuple<std::decay_t<Types>...> makeTuple(Types&&... args) {
  return Tuple<std::decay_t<Types>...>{std::forward<Types>(args)...};
}

template <typename... Args>
Tuple<Args&...> tie(Args&... args) {
  return Tuple<Args&...>{args...};
}

template <class... Types>
Tuple<Types&&...> forward_as_tuple(Types&&... args) {
  return Tuple<Types&&...>{std::forward<Types>(args)...};
}

template <class T, class... Types>
T&& get(Tuple<Types...>&& tuple) {
  constexpr int ind = appearence<T, Types...>();
  static_assert(ind != -1);
  return get<ind>(std::forward<decltype(tuple)>(tuple));
}

template <class T, class... Types>
const T& get(const Tuple<Types...>& tuple) {
  constexpr int ind = appearence<T, Types...>();
  static_assert(ind != -1);
  return get<ind>(std::forward<decltype(tuple)>(tuple));
}

template <class T, class... Types>
T& get(Tuple<Types...>& tuple) {
  constexpr int ind = appearence<T, Types...>();
  static_assert(ind != -1);
  return get<ind>(std::forward<decltype(tuple)>(tuple));
}

template <class T, class... Types>
const T&& get(const Tuple<Types...>&& tuple) {
  constexpr int ind = appearence<T, Types...>();
  static_assert(ind != -1);
  return get<ind>(std::forward<decltype(tuple)>(tuple));
}

template <typename... Tuples>
  requires(is_tuple<Tuples>::value && ...)
typename tuple_cat_result<Tuples...>::type tupleCat(Tuples&&... tuples) {
  return {tuple_cat_tag{}, std::forward<Tuples>(tuples)...};
}

template <typename... Types, typename... UTypes>
bool operator==(const Tuple<Types...>& first, const Tuple<UTypes...>& second) {
  int first_size = sizeof...(Types);
  int second_size = sizeof...(UTypes);
  if (first_size != second_size) {
    return false;
  }
  if (first.head != second.head) {
    return false;
  } else {
    return first.tail == second.tail;
  }
}

bool operator==(const Tuple<>&, const Tuple<>&) { return true; }

template <typename... Types, typename... UTypes>
bool operator<(const Tuple<Types...>& first, const Tuple<UTypes...>& second) {
  if (first.head < second.head) {
    return true;
  } else if (first.head > second.head) {
    return false;
  } else {
    return first.tail < second.tail;
  }
}

template <typename... UTypes>
bool operator<(const Tuple<>&, const Tuple<UTypes...>&) {
  return true;
}

template <typename... Types>
bool operator<(const Tuple<Types...>&, const Tuple<>&) {
  return false;
}

template <typename... Types>
bool operator<(const Tuple<>, const Tuple<>&) {
  return false;
}
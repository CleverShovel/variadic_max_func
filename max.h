// simple recursion
namespace variant_1 {
template <typename T, typename... Ts>
constexpr std::common_type_t<T, Ts...> max(T&& arg, Ts&&... args) {
  if constexpr (sizeof...(args) > 1)
    return max(std::forward<T>(arg), max(std::forward<Ts>(args)...));
  else return ((arg > args ? arg : args),...);
}
}  // namespace variant_1

// no recursion but with array
namespace variant_2 {

template <typename... Ts, size_t N = sizeof...(Ts)>
constexpr auto get_array(Ts... ts) {
  std::array<decltype(((std::declval<Ts>())+...)), N> result = {ts...};
  return result;
}

template <typename T>
constexpr T max(const T& t) {
  return t;
}

template <typename... Ts>
constexpr std::common_type_t<Ts...> max(const Ts&... ts) {
  auto array = get_array(ts...);
  size_t step = 1;
  do {
    step <<= 1;
    for (size_t i = 0; i < array.size(); i += step) {
      if (i + (step >> 1) < array.size())
        array[i] =
            (array[i] > array[i + (step >> 1)] ? array[i] : array[i + (step >> 1)]);
    }
  } while (step <= array.size());
  return array[0];
}
}  // namespace variant_2

// recursion using divide-and-conquer algorithm
namespace variant_3 {

template <typename IS>
struct get_impl;

template <size_t... Is>
struct get_impl<std::index_sequence<Is...>> {
  template <typename T, typename... Ts>
  static constexpr const T& value(decltype(Is, (void*)0)..., T* val, ...) {
    return *val;
  }
};

template <size_t I, typename... Ts>
constexpr auto get(Ts... ts) {
  return get_impl<std::make_index_sequence<I>>::value(&ts...);
}

template <typename T, typename U>
inline constexpr auto max_impl(const T& t, const U& u) {
  return (t > u ? t : u);
}

template <size_t L, size_t R, size_t... Is, typename... Ts>
constexpr auto max_impl(std::index_sequence<Is...> seq, const Ts&... ts) {
  if constexpr (R - L == 1) return get<L>(ts...);
  else if constexpr (R - L == 2) return max_impl(get<L>(ts...), get<L + 1>(ts...));
  else return max_impl(max_impl<L, (L + R)/2>(seq, ts...), max_impl<(L + R)/2, R>(seq, ts...)); 
}

template <typename T>
constexpr auto max(const T& t) {
  return t;
}

template <typename... Ts, typename Indexed = std::index_sequence_for<Ts...>, size_t N = sizeof...(Ts)>
constexpr auto max(const Ts&... ts) {
  //using result_type = std::common_type<const Ts&...>;
  return max_impl(max_impl<0, N/2>(Indexed{}, ts...), max_impl<N/2, N>(Indexed{}, ts...));
}
}  // namespace variant_3

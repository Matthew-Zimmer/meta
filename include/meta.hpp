#pragma once
#include <utility>
namespace Slate::Meta
{
	/*
		Args:
			t: The object to be cast
		Summary:
			Casts the object, t, to Type, using Types... in reverse order as steps
		Complexity:
			O(1)
	*/
	
	template <typename Convert, typename Type>
	auto& cast(Type& t)
	{
		return static_cast<Convert&>(t);
	}
	
	template <typename Convert, typename ... Steps, typename Type>
	auto cast(Type& t) -> std::enable_if_t<sizeof...(Steps) != 0, Convert&>
	{
		return static_cast<Convert&>(cast<Steps...>(t));
	}

	template <typename Convert, typename Type>
	auto const& cast(Type const& t)
	{
		return static_cast<Convert const&>(t);
	}
	
	template <typename Convert, typename ... Steps, typename Type>
	auto cast(Type const& t) -> std::enable_if_t<sizeof...(Steps) != 0, Convert const&>
	{
		return static_cast<Convert const&>(cast<Steps...>(t));
	}

	template <typename ... Types>
	class Wrap
	{};

	template <std::size_t value>
	class Integer
	{
	public:
		constexpr operator std::size_t() const
		{
			return value;
		}
	};

	namespace Detail::Size 
	{
		template <typename Type>
		class Size
		{};

		template <typename ... Types>
		class Size<Wrap<Types...>>
		{
		public:
			static constexpr std::size_t value = sizeof...(Types);
		};
	}

	template <typename Type>
	constexpr std::size_t size = Detail::Size::Size<Type>::value;

	namespace Detail::Eval
	{
		template <template <typename ...> typename F, typename, typename ... Ts>
		class Eval
		{
		public:
			using Type = F<Ts...>;
		};
		template <template <typename ...> typename F, typename ... Ts>
		class Eval<F, std::void_t<typename F<Ts...>::Type>, Ts...>
		{
		public:
			using Type = typename F<Ts...>::Type;
		};
	}

	template <template <typename ...> typename F, typename ... Ts>
	using Eval = typename Detail::Eval::Eval<F, void, Ts...>::Type;

	template <template <typename...> typename Func, typename ... U>
	class Adapter
	{
	public:
		template <typename ... V>
		class Function
		{
		public:
			using Type = Eval<Func, U..., V...>;
		};
		template <template <typename...> typename Func2, typename ... V>
		using Then = Adapter<Func2, Meta::Eval<Func, U..., V...>>;
	};

	template <template <typename...> typename Func, typename ... U>
	class Reverse_Adapter
	{
	public:
		template <typename ... V>
		class Function
		{
		public:
			using Type = Eval<Func, V..., U...>;
		};
		template <template <typename...> typename Func2, typename ... V>
		using Then = Adapter<Func2, Meta::Eval<Func, V..., U...>>;
	};

	namespace Detail::Convert
	{
		template <typename T, template <typename...> typename Container>
		class Convert 
		{
		public:
			using Type = Container<T>;
		};

		template <template <typename...> typename Container1, typename ... Types, template <typename...> typename Container2>
		class Convert<Container1<Types...>, Container2>
		{
		public:
			using Type = Container2<Types...>;
		};
	}

	/*
		Summary:
			Convert from/to Standard Container to/from Non Standard Containers,
			Standard referring to Meta::Wrap
	*/
	template <typename Type, template <typename...> typename Container = Slate::Meta::Wrap>
	using Convert = typename Detail::Convert::Convert<Type, Container>::Type;

	namespace Detail::Unwrap_At
	{
		template <typename Type_, std::size_t Index>
		class Unwrap_At
		{
			template <std::size_t FIndex, std::size_t ... Indexes, typename F, typename ... Types_>
			static auto imp(std::index_sequence<FIndex, Indexes...>, Slate::Meta::Wrap<F, Types_...>)
			{
				return imp(std::index_sequence<Indexes...>{}, Slate::Meta::Wrap<Types_...>{});
			}
			template <typename F, typename ... Types_>
			static auto imp(std::index_sequence<>, Slate::Meta::Wrap<F, Types_...>) -> F;
		public:
			using Type = decltype(imp(std::make_index_sequence<Index>{}, std::declval<Type_>()));
		};
	}
	
	/*
		Summary:
			Extracts types from a compile time container if and only if
			there is only one type in the container else it does nothing
	*/
	template <typename Type, std::size_t Index>
	using Unwrap_At = typename Detail::Unwrap_At::Unwrap_At<Type, Index>::Type;

	template <typename Type>
	using Unwrap = Unwrap_At<Type, 0>;

	template <typename Type>
	using Unwrap_Back = Unwrap_At<Type, size<Type> - 1>;

	namespace Detail::Append
	{
		template <typename Type1, typename Type2>
		class Append
		{
		public:
			using Type = Wrap<Type1, Type2>;
		};

		template <typename T, typename ... Types>
		class Append<T, Wrap<Types...>>
		{
		public:
			using Type = Wrap<T, Types...>;
		};

		template <typename T, typename ... Types>
		class Append<Wrap<Types...>, T>
		{
		public:
			using Type = Wrap<Types..., T>;
		};

		template <typename ... Types1, typename ... Types2>
		class Append<Wrap<Types1...>, Wrap<Types2...>>
		{
		public:
			using Type = Wrap<Types1..., Types2...>;
		};
	}

	template <typename Type1, typename Type2>
	using Append = typename Detail::Append::Append<Type1, Type2>::Type;

	namespace Detail::Join
	{
		template <typename...>
		class Join
		{};

		//Joining n > 2 Containers

		template <typename Type1, typename ... Types_Rest>
		class Join<Type1, Types_Rest...>
		{
		public:
			using Type = Meta::Append<Type1, typename Join<Types_Rest...>::Type>;
		};

		//Joining One Containers

		template <typename ... Types1>
		class Join<Wrap<Types1...>>
		{
		public:
			using Type = Wrap<Types1...>;
		};

		//Joining Zero Containers

		template <>
		class Join<>
		{
		public:
			using Type = Wrap<>;
		};
	}

	/*
		Constraits:
			Types...: Is_Compile_Time_Container<Types>... is true and 
			all compile time containers have the same degree
		Summary:
			Joins all Types together into one compile time container
	*/
	template <typename ... Types>
	using Join = typename Detail::Join::Join<Types...>::Type;

	/*
		Summary:
			If Condition is true then the type is a container of True_type 
			else it is an empty compile time container
	*/
	template <bool Condition, typename True_Type>
	using Use_If = std::conditional_t<Condition, Wrap<True_Type>, Wrap<>>;

	namespace Detail::For_Each
	{
		template <typename T, template <typename> typename F>
		class For_Each
		{
		public:
			using Type = Meta::Eval<F, T>;
		};

		template <typename ... Types, template <typename> typename Function>
		class For_Each<Wrap<Types...>, Function>
		{
		public:
			using Type = Meta::Wrap<Meta::Eval<Function, Types>...>;
		};
	}

	/*
		Constraits:
			Container: Is_Compile_Time_Container<Container> is true
			Operator: Operation<Container>::Type... exists
		Summary:
			Applies the operation, Operation on each element of Container and results in the 
			new container
	*/
	template <typename Container, template <typename> typename Function>
	using For_Each = typename Detail::For_Each::For_Each<Container, Function>::Type;

	template <typename Container, template <typename> typename Function>
	using Join_For_Each = Convert<For_Each<Container, Function>, Join>;

	template <typename Container, typename E>
	constexpr bool contains = false;

	template <typename ... Types, typename E>
	constexpr bool contains<Wrap<Types...>, E> = (std::is_same_v<E, Types> || ...);

	namespace Detail::Unique 
	{
		//Base definition
		template <typename>
		class Unique
		{};

		template <typename T, typename ... Types>
		class Unique<Wrap<T, Types...>>
		{
		public:
			using Type = Meta::Append<Meta::Use_If<!contains<Wrap<Types...>, T>, T>, typename Unique<Wrap<Types...>>::Type>;
		};

		template <>
		class Unique<Wrap<>>
		{
		public:
			using Type = Wrap<>;
		};
	}

	/*
		Constraits:
			Type: Is_Compile_Time_Container<Type>... is true
		Summary:
			Returns the unique set of types of all all containers
	*/
	template <typename Container>
	using Unique = typename Detail::Unique::Unique<Container>::Type;

	namespace Detail::Function_Types 
	{
		template <typename>
		class Function_Types {};

		template <typename Return_Type_, typename ... Args_>
		class Function_Types<Return_Type_(*)(Args_...)>
		{
		public:
			using Return_Type = Return_Type_;
			using Args = Wrap<Args_...>;
		};

		template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...)>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = Object_Type_;
			using Args = Wrap<Args_...>;
		};

		template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...) const>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = const Object_Type_;
			using Args = Wrap<Args_...>;
		};

		template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...) volatile>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = volatile Object_Type_;
			using Args = Wrap<Args_...>;
		};

		template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...) const volatile>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = const volatile Object_Type_;
			using Args = Wrap<Args_...>;
		};
	}

	template <typename Type>
	using Return_Type = typename Detail::Function_Types::Function_Types<Type>::Return_Type;

	template <typename Type>
	using Object_Type = typename Detail::Function_Types::Function_Types<Type>::Object_Type;

	//Note this loses const qualifier on non reference/pointer types
	template <typename Type>
	using Args = typename Detail::Function_Types::Function_Types<Type>::Args;

	// FIND where this stuff is used

	// namespace Detail::Tag 
	// {
	// 	template <typename Type_>
	// 	class Tag
	// 	{};

	// 	template <typename ... Types>
	// 	class Tag<Slate::Meta::Wrap<Types...>>
	// 	{
	// 		template <std::size_t ... Indexes>
	// 		static auto Imp(std::index_sequence<Indexes...>) -> Slate::Meta::Wrap<Slate::Meta::Wrap<Types, Slate::Meta::Integer<Indexes>>...>;
	// 	public:
	// 		using Type = decltype(Imp(std::index_sequence_for<Types...>{}));
	// 	};
	// }

	// template <typename Type>
	// using Tag = typename Imp::Meta::Tag<Type>::Type;

	// template <typename Type_, std::size_t Index>
	// class Extract
	// {
	// public:
	// 	using Type = Wrap<Unwrap_At<Type_, Index>>;
	// };

	namespace Detail
	{
		template <typename>
		class Unwrap_Tail
		{};

		template <typename First, typename ... Rest>
		class Unwrap_Tail<Wrap<First, Rest...>>
		{
		public:
			using Type = Wrap<Rest...>;
		};
	}

	template <typename Type>
	using Unwrap_Tail = typename Detail::Unwrap_Tail<Type>::Type;

	// namespace Detail
	// {
	// 	template <typename, typename>
	// 	class Split_I
	// 	{};

	// 	template <typename First, typename ... Rest, typename Delimiter>
	// 	class Split_I<Wrap<First, Rest...>, Delimiter>
	// 	{
	// 		using Next = typename Split_I<Wrap<Rest...>, Delimiter>::Type;
	// 	public:
	// 		using Type = std::conditional_t<
	// 			std::is_same_v<First, Delimiter>, 
	// 			Join<Wrap<Wrap<>>, Wrap<Unwrap<Next>>, Unwrap_Tail<Next>>, 
	// 			Join<Wrap<Append<First, Unwrap<Next>>>, Unwrap_Tail<Next>>>;
	// 	};

	// 	template <typename First, typename Delimiter>
	// 	class Split_I<Wrap<First>, Delimiter>
	// 	{
	// 	public:
	// 		using Type = Use_If<!std::is_same_v<First, Delimiter>, Wrap<First>>;
	// 	};
	// }

	// template <typename Container, typename Delimiter>
	// using Split = typename Detail::Split_I<Container, Delimiter>::Type;

	namespace Detail::Left_Fold
	{
		template <typename T, template <typename, typename> typename Function, typename Value>
		class Left_Fold
		{
		public:
			using Type = Meta::Eval<Function, T, Value>;
		};

		template <typename T, typename ... Types, template <typename, typename> typename Function, typename Value>
		class Left_Fold<Wrap<T, Types...>, Function, Value>
		{
		public:
			using Type = typename Left_Fold<Wrap<Types...>, Function, typename Left_Fold<T, Function, Value>::Type>::Type;
		};

		template <template <typename, typename> typename Function, typename Value>
		class Left_Fold<Wrap<>, Function, Value>
		{
		public:
			using Type = Value;
		};
	}

	template <typename Container, template <typename, typename> typename Function, typename Value = Wrap<>>
	using Left_Fold = typename Detail::Left_Fold::Left_Fold<Container, Function, Value>::Type;

	namespace Detail::Apply
	{
		template <typename>
		class Apply
		{};

		template <typename ... Types>
		class Apply<Wrap<Types...>>
		{
		public:
			template <typename Type>
			static bool imp(Type&& f)
			{
				return (f.template operator()<Types>() || ...);
			}
		};
	}

	template <typename Container, typename F>
	bool apply(F&& f)
	{
		return Detail::Apply::Apply<Container>::imp(std::forward<F>(f));
	}
}
#pragma once
#include <utility>
namespace Slate 
{
	namespace Meta
	{
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
	}

	namespace Imp::Meta
	{
		template <typename, template <typename...> typename>
		class Convert {};

		template <template <typename...> typename Container1, typename ... Types, template <typename...> typename Container2>
		class Convert<Container1<Types...>, Container2>
		{
		public:
			using Type = Container2<Types...>;
		};

		template <typename Types, template <typename...> typename Container>
		using Convert_t = typename Convert<Types, Container>::Type;

		template <typename>
		class Is_Container : public std::false_type
		{};

		template <typename ... Types>
		class Is_Container<Slate::Meta::Wrap<Types...>> : public std::true_type
		{};

		// template <template <typename...> typename Container, typename ... Types>
		// class Is_Container<std::tuple<Types...>> : public std::true_type
		// {};

		// template <template <template <typename> typename...> typename Container, template <typename> typename ... Types>
		// class Is_Container<Container<Types...>> : std::true_type
		// {};

		//template <template <template <typename, typename...> typename...> typename Container, template <typename, typename...> typename ... Types>
		//class Is_Container<Container<Types...>> : public std::true_type
		//{};

		template <typename Type>
		constexpr bool Is_Container_v = Is_Container<Type>::value;

		template <typename Type_, std::size_t Index>
        class Unwrap_At
        {
            template <std::size_t FIndex, std::size_t ... Indexes, typename F, typename ... Types_>
            static auto Imp(std::index_sequence<FIndex, Indexes...>, Slate::Meta::Wrap<F, Types_...>)
            {
                return Imp(std::index_sequence<Indexes...>{}, Slate::Meta::Wrap<Types_...>{});
            }
            template <typename F, typename ... Types_>
            static auto Imp(std::index_sequence<>, Slate::Meta::Wrap<F, Types_...>) -> F;
        public:
            using Type = decltype(Imp(std::make_index_sequence<Index>{}, std::declval<Type_>()));
        };

		template <typename Type>
		using Unwrap_t = typename Unwrap_At<Type, 0>::Type;

		template <typename Type1, typename Type2>
		class Append
		{
		public:
			using Type = Slate::Meta::Wrap<Type1, Type2>;
		};

		template <typename T, typename ... Types>
		class Append<T, Slate::Meta::Wrap<Types...>>
		{
		public:
			using Type = Slate::Meta::Wrap<T, Types...>;
		};

		template <typename T, typename ... Types>
		class Append<Slate::Meta::Wrap<Types...>, T>
		{
		public:
			using Type = Slate::Meta::Wrap<Types..., T>;
		};

		template <typename ... Types1, typename ... Types2>
		class Append<Slate::Meta::Wrap<Types1...>, Slate::Meta::Wrap<Types2...>>
		{
		public:
			using Type = Slate::Meta::Wrap<Types1..., Types2...>;
		};

		template <typename...>
		class Join
		{};

		//Joining n > 2 Containers

		template <typename Type1, typename ... Types_Rest>
		class Join<Type1, Types_Rest...>
		{
		public:
			using Type = typename Append<Type1, typename Join<Types_Rest...>::Type>::Type;
		};

		//Joining One Containers

		template <typename ... Types1>
		class Join<Slate::Meta::Wrap<Types1...>>
		{
		public:
			using Type = Slate::Meta::Wrap<Types1...>;
		};

		//Joining Zero Containers

		template <>
		class Join<>
		{
		public:
			using Type = Slate::Meta::Wrap<>;
		};

		template <typename ... Types>
		using Join_t = typename Join<Types...>::Type;

		template <typename, template <typename> typename>
		class For_Each
		{};

		template <template <typename...> typename Container, typename Type1, typename ... Types, template <typename> typename Operation>
		class For_Each<Container<Type1, Types...>, Operation>
		{
		public:
			using Type = std::conditional_t<Is_Container_v<typename Operation<Type1>::Type>, Join_t<typename Operation<Type1>::Type, typename Operation<Types>::Type...>, Slate::Meta::Wrap<typename Operation<Type1>::Type, typename Operation<Types>::Type...>>;
		};

		template <template <typename...> typename Container, template <typename> typename Operation>
		class For_Each<Container<>, Operation>
		{
		public:
			using Type = Slate::Meta::Wrap<>;
		};

		template <typename Types, template <typename> typename Meta_Functor>
		using For_Each_t = typename For_Each<Types, Meta_Functor>::Type;

		//Base definition
		template <typename...>
		class Unique
		{};

		//Unique n > 1
		template <typename Type1, typename ... Types_Rest>
		class Unique<Type1, Types_Rest...>
		{
		public:
			using Type = typename Unique<Join_t<Type1, typename Unique<Types_Rest...>::Type>>::Type;
		};

		//Unique n = 1
		template <template <typename ...> typename Container1, typename Type1, typename ... Types1>
		class Unique<Container1<Type1, Types1...>>
		{
			using Next = typename Unique<Slate::Meta::Wrap<Types1...>>::Type;
			template <typename _Type>
			class Is_Same
			{
			public:
				using Type = std::conditional_t<!std::is_same_v<Type1, _Type>, Slate::Meta::Wrap<_Type>, Slate::Meta::Wrap<>>;
			};
		public:
			using Type = Join_t<Slate::Meta::Wrap<Type1>, For_Each_t<Next, Is_Same>>;
		};

		//Unique n = 1, sizeof(c1) = 1
		template <template <typename ...> typename Container1, typename Type1>
		class Unique<Container1<Type1>>
		{
		public:
			using Type = Slate::Meta::Wrap<Type1>;
		};

		//Unique n = 0
		template <>
		class Unique<Slate::Meta::Wrap<>>
		{
		public:
			using Type = Slate::Meta::Wrap<>;
		};

		template <>
		class Unique<>
		{
		public:
			using Type = Slate::Meta::Wrap<>;
		};

		template <typename ... Types>
		using Unique_t = typename Unique<Types...>::Type;

		template <typename>
		class Function_Types {};

        template <typename Return_Type_, typename ... Args_>
		class Function_Types<Return_Type_(*)(Args_...)>
		{
		public:
			using Return_Type = Return_Type_;
			using Args = Slate::Meta::Wrap<Args_...>;
		};

        template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...)>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = Object_Type_;
			using Args = Slate::Meta::Wrap<Args_...>;
		};

		template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...) const>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = const Object_Type_;
			using Args = Slate::Meta::Wrap<Args_...>;
		};

        template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...) volatile>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = volatile Object_Type_;
			using Args = Slate::Meta::Wrap<Args_...>;
		};

        template <typename Return_Type_, typename Object_Type_, typename ... Args_>
		class Function_Types<Return_Type_(Object_Type_::*)(Args_...) const volatile>
		{
		public:
			using Return_Type = Return_Type_;
			using Object_Type = const volatile Object_Type_;
			using Args = Slate::Meta::Wrap<Args_...>;
		};

		template <typename Type>
        class Size
        {};

		template <typename ... Types>
        class Size<Slate::Meta::Wrap<Types...>>
        {
		public:
			static constexpr std::size_t value = sizeof...(Types);
		};

		template <typename Type>
        constexpr std::size_t size_v = Size<Type>::value;

		template <typename Type_>
        class Tag
        {};

		template <typename ... Types>
        class Tag<Slate::Meta::Wrap<Types...>>
        {
			template <std::size_t ... Indexes>
            static auto Imp(std::index_sequence<Indexes...>) -> Slate::Meta::Wrap<Slate::Meta::Wrap<Types, Slate::Meta::Integer<Indexes>>...>;
		public:
			using Type = decltype(Imp(std::index_sequence_for<Types...>{}));
		};
	}

	namespace Meta 
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

		/*
			Summary:
				Convert from/to Standard Container to/from Non Standard Containers,
				Standard referring to Imp::Meta::_Types
		*/
		template <typename Type, template <typename...> typename Container = Slate::Meta::Wrap>
		using Convert = typename Imp::Meta::Convert<Type, Container>::Type;

		/*
			Summary:
				Checks if a Type is a Compile Time Container
		*/
		template <typename Type>
		constexpr bool Is_Container = Imp::Meta::Is_Container<Type>::value;

		/*
			Summary:
				Extracts types from a compile time container if and only if
				there is only one type in the container else it does nothing
		*/
		template <typename Type>
		using Unwrap = typename Imp::Meta::Unwrap_At<Type, 0>::Type;

        template <typename Type, std::size_t Index>
		using Unwrap_At = typename Imp::Meta::Unwrap_At<Type, Index>::Type;

		template <typename Type1, typename Type2>
		using Append = typename Imp::Meta::Append<Type1, Type2>::Type;
		
		/*
			Constraits:
				Types...: Is_Compile_Time_Container<Types>... is true and 
				all compile time containers have the same degree
			Summary:
				Joins all Types together into one compile time container
		*/
		template <typename ... Types>
		using Join = typename Imp::Meta::Join<Types...>::Type;

		/*
			Summary:
				If Condition is true then the type is a container of True_type 
				else it is an empty compile time container
		*/
		template <bool Condition, typename True_Type>
		using Use_If = std::conditional_t<Condition, Wrap<True_Type>, Wrap<>>;

		/*
			Constraits:
				Container: Is_Compile_Time_Container<Container> is true
				Operator: Operation<Container>::Type... exists
			Summary:
				Applies the operation, Operation on each element of Container and results in the 
				new container
		*/
		template <typename Container, template <typename> typename Operation>
		using For_Each = typename Imp::Meta::For_Each<Container, Operation>::Type;

		/*
			Constraits:
				Type: Is_Compile_Time_Container<Type>... is true
			Summary:
				Returns the unique set of types of all all containers
		*/
		template <typename ... Types>
		using Unique = typename Imp::Meta::Unique<Types...>::Type;

		template <typename Type>
		using Return_Type = typename Imp::Meta::Function_Types<Type>::Return_Type;

		template <typename Type>
		using Object_Type = typename Imp::Meta::Function_Types<Type>::Object_Type;

        //Note this loses const qualifier on non reference/pointer types
		template <typename Type>
		using Args = typename Imp::Meta::Function_Types<Type>::Args;

        template <typename Type_, std::size_t Index>
        class Extract
        {
        public:
            using Type = Wrap<Unwrap_At<Type_, Index>>;
        };

		template <typename Type>
        using Tag = typename Imp::Meta::Tag<Type>::Type;

		template <typename Type>
        constexpr std::size_t Size = Imp::Meta::Size<Type>::value;

		template <typename Type>
        using Unwrap_Back = Unwrap_At<Type, Size<Type> - 1>;

		namespace Detail
        {
            template <typename>
            class Unwrap_Tail_I
            {};

            template <typename First, typename ... Rest>
            class Unwrap_Tail_I<Wrap<First, Rest...>>
            {
            public:
                using Type = Wrap<Rest...>;
            };
        }

        template <typename Type>
        using Unwrap_Tail = typename Detail::Unwrap_Tail_I<Type>::Type;

        namespace Detail
        {
            template <typename, typename>
            class Split_I
            {};

            template <typename First, typename ... Rest, typename Delimiter>
            class Split_I<Wrap<First, Rest...>, Delimiter>
            {
                using Next = typename Split_I<Wrap<Rest...>, Delimiter>::Type;
            public:
                using Type = std::conditional_t<
                    std::is_same_v<First, Delimiter>, 
                    Join<Wrap<Wrap<>>, Wrap<Unwrap<Next>>, Unwrap_Tail<Next>>, 
                    Join<Wrap<Append<First, Unwrap<Next>>>, Unwrap_Tail<Next>>>;
            };

            template <typename First, typename Delimiter>
            class Split_I<Wrap<First>, Delimiter>
            {
            public:
                using Type = Use_If<!std::is_same_v<First, Delimiter>, Wrap<First>>;
            };
        }

        template <typename Container, typename Delimiter>
        using Split = typename Detail::Split_I<Container, Delimiter>::Type;

		namespace Detail
		{
			template <typename T, template <typename, typename> typename Function, typename Value>
			class Left_Fold
			{
			public:
				using Type = typename Function<T, Value>::Type;
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
		using Left_Fold = typename Detail::Left_Fold<Container, Function, Value>::Type;
	}
}
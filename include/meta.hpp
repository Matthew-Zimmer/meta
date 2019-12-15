#pragma once
#include <type_traits>
namespace Slate 
{
	namespace Meta
	{
		template <typename ... Types>
		class Wrap
		{};
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
            template <std::size_t FIndex, std::size_t ... Indexs, typename F, typename ... Types_>
            static auto Imp(std::index_sequence<FIndex, Indexs...>, Slate::Meta::Wrap<F, Types_...>)
            {
                return Imp(std::index_sequence<Indexs...>{}, Slate::Meta::Wrap<Types_...>{});
            }
            template <typename F, typename ... Types_>
            static auto Imp(std::index_sequence<>, Slate::Meta::Wrap<F, Types_...>) -> F;
        public:
            using Type = decltype(Imp(std::make_index_sequence<Index>{}, std::declval<Type_>()));
        };

		template <typename Type>
		using Unwrap_t = typename Unwrap_At<Type, 0>::Type;

		template <typename...>
		class Join
		{};

		//Joining n > 2 Containers

		template <typename Type1, typename ... Types_Rest>
		class Join<Type1, Types_Rest...>
		{
		public:
			using Type = typename Join<Type1, typename Join<Types_Rest...>::Type>::Type;
		};

		//Joining Two Containers

		template <template <typename ...> typename Container1, template <typename ...> typename Container2, typename ... Types1, typename ... Types2>
		class Join<Container1<Types1...>, Container2<Types2...>>
		{
		public:
			using Type = Slate::Meta::Wrap<Types1..., Types2...>;
		};

		//Joining One Containers

		template <template <typename ...> typename Container1, typename ... Types1>
		class Join<Container1<Types1...>>
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
		Convert& Cast(Type& t)
		{
			return static_cast<Convert&>(t);
		}
		
		template <typename Convert, typename ... Steps, typename Type>
		auto Cast(Type& t) -> std::enable_if_t<sizeof...(Steps) != 0, Convert&>
		{
			return static_cast<Convert&>(Cast<Steps...>(t));
		}

		template <typename Convert, typename Type>
		Convert const& Cast(Type const& t)
		{
			return static_cast<Convert const&>(t);
		}

		template <typename Convert, typename ... Steps, typename Type>
		auto Cast(Type const& t) -> std::enable_if_t<sizeof...(Steps) != 0, Convert const&>
		{
			return static_cast<Convert const&>(Cast<Steps...>(t));
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
		template <typename ... Type>
		using Unique = typename Imp::Meta::Unique<Type...>::Type;

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
	}
}
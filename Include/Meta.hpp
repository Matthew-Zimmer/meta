#pragma once
#include <type_traits>

namespace COMPANY_NAME 
{
	namespace Imp::Meta
	{
		template <typename ... Types>
		class _Types
		{};

		template <typename Start_Type, typename Cast_To_Type, typename ... Intermediate_Types>
		class Cast
		{
		public:
			using Cast_Type = Cast_To_Type&;
			static Cast_Type _Cast(Start_Type& t)
			{
				return static_cast<Cast_To_Type&>(Cast<Start_Type, Intermediate_Types...>::_Cast(t));
			}
		};

		template <typename Start_Type, typename Cast_To_Type>
		class Cast<Start_Type, Cast_To_Type>
		{
		public:
			using Cast_Type = Cast_To_Type&;
			static Cast_Type _Cast(Start_Type& t)
			{
				return static_cast<Cast_To_Type&>(t);
			}
		};

		template <typename Start_Type, typename Cast_To_Type, typename ... Intermediate_Types>
		class Const_Cast
		{
		public:
			using Cast_Type = const Cast_To_Type&;
			static Cast_Type _Cast(Start_Type& t)
			{
				return static_cast<const Cast_To_Type&>(Cast<Start_Type, Intermediate_Types...>::_Cast(t));
			}
		};

		template <typename Start_Type, typename Cast_To_Type>
		class Const_Cast<Start_Type, Cast_To_Type>
		{
		public:
			using Cast_Type = const Cast_To_Type&;
			static Cast_Type _Cast(Start_Type& t)
			{
				return static_cast<const Cast_To_Type&>(t);
			}
		};

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
		class Is_Container<_Types<Types...>> : public std::true_type
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

		template <typename ... Types1>
		class Wrap
		{
		public:
			using Type = _Types<Types1...>;
		};

		template <typename ... Types>
		using Wrap_t = typename Wrap<Types...>::Type;

		template <typename Type1>
		class Unwrap
		{
		public:
			using Type = Type1;
		};

		template <typename Type1, template <typename ...> typename Container>
		class Unwrap<Container<Type1>>
		{
		public:
			using Type = Type1;
		};

		template <typename Type>
		using Unwrap_t = typename Unwrap<Type>::Type;

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
			using Type = Wrap_t<Types1..., Types2...>;
		};

		//Joining One Containers

		template <template <typename ...> typename Container1, typename ... Types1>
		class Join<Container1<Types1...>>
		{
		public:
			using Type = Wrap_t<Types1...>;
		};

		//Joining Zero Containers

		template <>
		class Join<>
		{
		public:
			using Type = Wrap_t<>;
		};

		template <typename ... Types>
		using Join_t = typename Join<Types...>::Type;

		//Base Definition
		template <typename...>
		class Distribute
		{};

		//Distribute n > 2
		template <typename Type1, typename ... Types_Rest>
		class Distribute<Type1, Types_Rest...>
		{
		public:
			using Type = typename Distribute<Type1, typename Distribute<Types_Rest...>::Type>::Type;
		};

		//Distribute n = 2
		template <template <typename ...> typename Container1, template <typename ...> typename Container2, typename ... Types1, typename ... Types2>
		class Distribute<Container1<Types1...>, Container2<Types2...>>
		{
		public:
			using Type = Join_t<typename Distribute<Wrap_t<Types1>, Wrap_t<Types2...>>::Type...>;
		};

		//Distribute n = 2, sizeof(c1)... = 1
		template <template <typename ...> typename Container1, template <typename ...> typename Container2, typename Type1, typename ... Types2>
		class Distribute<Container1<Type1>, Container2<Types2...>>
		{
		public:
			using Type = Join_t<Wrap_t<Join_t<Wrap_t<Type1>, std::conditional_t<Is_Container_v<Types2>, Types2, Wrap_t<Types2>>>>...>;
		};

		//Distribute n = 1
		template <template <typename ...> typename Container1, typename ... Types1>
		class Distribute<Container1<Types1...>>
		{
		public:
			using Type = Wrap_t<Types1...>;
		};

		//Distribute n = 0
		template <>
		class Distribute<>
		{
		public:
			using Type = Wrap_t<>;
		};

		template <typename ... Types>
		using Distribute_t = typename Distribute<Types...>::Type;

		template <typename, template <typename> typename>
		class For_Each
		{};

		template <template <typename...> typename Container, typename Type1, typename ... Types, template <typename> typename Operation>
		class For_Each<Container<Type1, Types...>, Operation>
		{
		public:
			using Type = std::conditional_t<Is_Container_v<typename Operation<Type1>::Type>, Join_t<typename Operation<Type1>::Type, typename Operation<Types>::Type...>, Wrap_t<typename Operation<Type1>::Type, typename Operation<Types>::Type...>>;
		};

		template <template <typename...> typename Container, template <typename> typename Operation>
		class For_Each<Container<>, Operation>
		{
		public:
			using Type = Wrap_t<>;
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
			using Next = typename Unique<Wrap_t<Types1...>>::Type;
			template <typename _Type>
			class Is_Same
			{
			public:
				using Type = std::conditional_t<!std::is_same_v<Type1, _Type>, Wrap_t<_Type>, Wrap_t<>>;
			};
		public:
			using Type = Join_t<Wrap_t<Type1>, For_Each_t<Next, Is_Same>>;
		};

		//Unique n = 1, sizeof(c1) = 1
		template <template <typename ...> typename Container1, typename Type1>
		class Unique<Container1<Type1>>
		{
		public:
			using Type = Wrap_t<Type1>;
		};

		//Unique n = 0
		template <>
		class Unique<Wrap_t<>>
		{
		public:
			using Type = Wrap_t<>;
		};

		template <>
		class Unique<>
		{
		public:
			using Type = Wrap_t<>;
		};

		template <typename ... Types>
		using Unique_t = typename Unique<Types...>::Type;
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
		template <typename ... Types, typename Type>
		typename Imp::Meta::Cast<Type, Types...>::Cast_Type Cast(Type& t)
		{
			return Imp::Meta::Cast<Type, Types...>::_Cast(t);
		}

		/*
			Args:
				t: The object to be cast
			Summary:
				Casts the object, t, to Type, using Types... in reverse order as steps
			Complexity:
				O(1)
		*/
		template <typename ... Types, typename Type>
		typename Imp::Meta::Cast<Type, Types...>::Cast_Type Const_Cast(const Type& t)
		{
			return Imp::Meta::Const_Cast<Type, Types...>::_Cast(t);
		}

		/*
			Summary:
				Convert from/to Standard Container to/from Non Standard Containers,
				Standard referring to Imp::Meta::_Types
		*/
		template <typename Type, template <typename...> typename Container = Imp::Meta::_Types>
		using Convert = typename Imp::Meta::Convert<Type, Container>::Type;

		/*
			Summary:
				Checks if a Type is a Compile Time Container
		*/
		template <typename Type>
		constexpr bool Is_Container = Imp::Meta::Is_Container<Type>::value;

		/*
			Summary:
				Puts all types in a compile time container
		*/
		template <typename ... Types>
		using Wrap = typename Imp::Meta::Wrap<Types...>::Type;

		/*
			Summary:
				Extracts types from a compile time container if and only if
				there is only one type in the container else it does nothing
		*/
		template <typename Type>
		using Unwrap = typename Imp::Meta::Unwrap<Type>::Type;

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
			Constraits:
				Types...: Is_Compile_Time_Container<Types...> is true
			Summary:
				Distributes from one set to another
			Example:
				Distribute<{A, B}, {C, D}> = {{A, C}, {A, D}, {B, C}, {B, D}}
				Note that the degree of the container increases by one
		*/
		template <typename ... Types>
		using Distribute = typename Imp::Meta::Distribute<Types...>::Type;

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
	}
}

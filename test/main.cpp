#include "meta.hpp"

template <typename U, typename V>
void meta_assert()
{
     static_assert(std::is_same_v<U, V>);
};

int main(int argc, char** argv)
{
    using namespace Slate::Meta;

    meta_assert<Wrap<int>, Wrap<int>>();

    meta_assert<Append<int, int>, Wrap<int, int>>();

    meta_assert<Append<Wrap<int>, Wrap<int>>, Wrap<int, int>>();

    meta_assert<Unique<Wrap<int, float, double, char>>, Wrap<int, float, double, char>>();

    meta_assert<Unique<Wrap<class A, float, double, double, int, char, char, float>>, Wrap<class A, double, int, char, float>>();

    meta_assert<For_Each<Wrap<class A, class B>, Reverse_Adapter<Append, class C>::Function>, Wrap<Wrap<class A, class C>, Wrap<class B, class C>>>();
}
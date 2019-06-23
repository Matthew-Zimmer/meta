#include "Meta.hpp"

using namespace COMPANY_NAME::Meta;

int main()
{
	static_assert(std::is_same_v<Unique<Wrap<int>, Wrap<double>>, Wrap<int, double>>);
	return 0;
}
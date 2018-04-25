#include <iostream>

#include "application.hpp"

int main(int argc, char** argv)
{
	auto app = cg::Application("Test");
	app.execute();
	return 0;
}

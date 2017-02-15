#include "main.h"

int main()
{

	std::string strEndPoint("");
	std::cout<< "\n Enter End Point (e.g. 192.168.243.129/api/): ";
	std::cin >> strEndPoint;
	std::unique_ptr<RestApp> rest(new RestApp(strEndPoint));

	rest->Initialize();

	rest->Run();

	std::cout << " Done..... :) " << std::endl;

	return 0;
}

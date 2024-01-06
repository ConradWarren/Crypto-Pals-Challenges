#include <iostream>
#include <string>

std::string Find_XOR_Product(const std::string& str_1, const std::string& str_2){

	std::string result;
	unsigned char num;
	
	for(int i = 0; i<str_1.length(); i++){
		num = (str_1[i] >= '0' && str_1[i] <= '9') ? str_1[i] - '0' : str_1[i] - 'a' + 10;
		num ^= (str_2[i] >= '0' && str_2[i] <= '9') ? str_2[i] - '0' : str_2[i] - 'a' + 10;
		result += (num < 10) ? '0' + num : 'a' + num - 10;
	}

	return result;
}

int main(int argc, char** argv){
	
	if(argc > 2){
		std::cout<<Find_XOR_Product(argv[1], argv[2])<<'\n';
	}

	return 0;
}

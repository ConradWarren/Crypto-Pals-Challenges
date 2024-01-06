#include <iostream>
#include <string>

std::string Multi_Bit_XOR_Encryption(const std::string& input_str, const std::string& key){
	
	char key_byte = 0;
	int num = 0;
	std::string result;

	for(int i = 0; i<input_str.size(); i++){
		num = (input_str[i] ^ key[i%key.size()]) >> 4;
		result += (num > 9) ? 'a' + num - 10 : '0' + num;
		num = (input_str[i] ^ key[i%key.size()]) & 0xf;
		result += (num > 9) ? 'a' + num - 10 : '0' + num;	
	}
	
	return result;
}


int main(int argc, char** argv){
	
	if(argc > 2){
		std::cout<<Multi_Bit_XOR_Encryption(argv[1], argv[2])<<'\n';
	}
	
	return 0;
}
